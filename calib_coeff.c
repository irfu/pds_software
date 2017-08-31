/* Module for functionality relating to CALIB_COEFF files and extracting interpolated coefficients for a specified time.
 * 
 * The basic concept is that the main program (pds_3.09.c) uses the functions here to create, access, indirectly update,
 * and deallocate a "CALIB_COEFF data struct" of type calib_coeff_data_type. No other code should need to work with the actual insides of
 * that struct type.
 * 
 * IMPLEMENTATION NOTE: Uses specified (hard-coded) mission beginning and end to guess which CALIB_COEFF files that might exist
 * and then creates a data stucture with one "slot" (index value in arrays) per such hypothetical file. The advantage with this
 * is that the code can then (and does) construct an array of (sorted) time boundaries between files and can (and does) quickly
 * look up indices with a "clever" search algorithm (FindNearestInSortedArray) without having to search through it one-by-one.
 * 
 * IMPLEMENTATION NOTE: Organizes the loaded coefficients as a 1D array (instead of a struct; instead of a 2D array
 * 2 probes*N values/probe) so that it is easy to:
 * (1) iterate over values (e.g. for interpolating),
 * (2) change the number of CALIB_COEFF coefficients (per probe),
 * (3) add extra non-probe-specific coefficients (which could be unrelated to bias-dependent offsets).
 * 
 * NOTE: The module does include some generic functions which were needed for the implementation.
 * These generic functions could be moved to some other module.
 * 
 * Created 2017-08-17 by Erik P G Johansson, IRF Uppsala, Sweden.
 */

/* Things to test
 * ==============
 * -- Legitimately empty CALIB_COEFF files.
 * -- CALIB_COEFF at end of mission.
 */



#include <limits.h>        // Standard limits of integer types, PATH_MAX
#include <math.h>          // isnan function
#include <stdio.h>         // Standard Input/output 
#include <stdlib.h>        // Standard General utilities
#include <string.h>        // String handling
#include <time.h>          // Standard date and time
#include <unistd.h>        // Required for "access" function

#include "pds.h"           // PDS & LAP definitions and structures
#include "calib_coeff.h"



/* Headers for pds_3.09.c functions used by this module.
 * 
 * TEMPORARY SOLUTION
 * 
 * NOTE: These are manual copies of these headers for these functions!
 * NOTE: Doubtful if this is the recommended way to handle the situation, but it works.
 * Maybe some functions in pds_3.09.c should be moved out it into its own module which could be imported from here.
 * 
 * PROPOSAL: Move these (and related functions) to a separate module.
 *      CON: Might use global variables (global in pds_3.09.c).
 *          Ex: YPrintf: pds.ylog_fd, protect_log.
 *          Ex: ConvertSccd2Utc_nonSPICE (indirect): tcp (contains coefficients).
 *      PROPOSAL: Move out only those which are easy to move.
 *          NOTE: Might only be CallocDoubleMatrix.
 *          PRO: Still improves the code.
 *              CON: Not if it is only one function.
 * PROPOSAL: Create separate header file for these pds_3.09.c functions: ~pds_global.h, ~pds_export.h, ~pds_exposed
 */
int  YPrintf(const char *fmt, ...);                                         // Prints to pds system log

int ConvertSccd2Utc         (double sccd, char *utc_3decimals, char *utc_6decimals);         // Decodes raw S/C time (calibration included) 
int ConvertUtc2Timet(char *sdate,time_t *t);                                // Returns UTC time in seconds (since 1970) for a PDS date
void ConvertUtc2Sccd_SPICE(char *utc, int *reset_counter, double *sccd);
int ConvertSccs2Sccd(char *sccs, int *reset_counter, double *sccd);         // Convert OBT string to raw time.
int ConvertTimet2Utc(double raw, char *utc, int use_6_decimals);            // Decodes SCET (Spacecraft event time, Calibrated OBT) into a date

double **CallocDoubleMatrix (int rows, int cols);                           // Dynamically allocate two dimensional array of doubles
void FreeDoubleMatrix(double ** C, int rows, int cols);                     // Free two dim. array of doubles








/* Modify existing UTC string by setting hour, minute, second to zero.
 * 
 * NOTE: Sets number of second decimals to three.
 * NOTE: Indirectly useful for rounding SCCD to beginning of day (for time_t one can also use C library functions).
 */
void ModifyUtc2BeginDay(char *utc)
{
    // CCYY-MM-DDThh:mm:ss.dddddd
    // 01234567890123456789012345
    // Set hours, minutes, seconds to zero, i.e. beginning of day.
    utc[11] = '0';   // h
    utc[12] = '0';   // h
    utc[14] = '0';   // m
    utc[15] = '0';   // m
    utc[17] = '0';   // s
    utc[18] = '0';   // s
    
    utc[19] = '.';
    
    utc[20] = '0';
    utc[21] = '0';
    utc[22] = '0';
    utc[23] = '\0';   // Set to NULL to end string. Effectively deletes second decimals.
}



/* Extract YYMMDD string from UTC string.
 */
void GetUtcYYMMDD(char *utc, char *date_str)
{
    // Copy year-month-day from UTC string: CCYY-MM-DD... --> YYMMDD
    //                                      0123456789        012345
    date_str[0] = utc[2];   // Y
    date_str[1] = utc[3];   // Y
    date_str[2] = utc[5];   // M
    date_str[3] = utc[6];   // M
    date_str[4] = utc[8];   // D
    date_str[5] = utc[9];   // D
    date_str[6] = '\0';
}



/* Prepare the reading of a datafile by
 * (1) opening generic text file (for reading),
 * (2) counting the number of rows with data, 
 * (3) rewinding file, 
 * (4) returning the file descriptor to the opened file.
 * This function is useful when reading the data file requires first allocating memory depending
 * on an unknown amount of data in the file. The read data is likely cached by the OS (if the file
 * is not closed and then reopened), and should therefor not slow down the application too much.
 * 
 * NOTE: Assumes that certain types of rows should be ignored.
 * 
 *
 * ARGUMENTS AND RETURN VALUE
 * ==========================
 * INPUT  : file_path    : Path to file that should be opened.
 * OUTPUT : file_descr   : File descriptor (pointer to ~object) returned by fopen(path,"r").
 * OUTPUT : N_rows       : Number of data rows. Ignores rows beginning with LF (works with rows ending with CR+LF?), "#", whitespace.
 * RETURN VALUE : Error code: 0=No error.
 */
int OpenFileCountDataRows(char* file_path, FILE **file_descr, int *N_rows)
{
    char row_str[MAX_STR]; // Line buffer
    int N_rows_p = 0;

    if((*file_descr=fopen(file_path, "r"))==NULL)
    {
        YPrintf("Can not open file \"%s\".\n", file_path);
        return -1;
    }

    // Count exclude table lines.
    while(fgets(row_str, MAX_STR-1, *file_descr) != NULL)
    {
        if (row_str[0] == '\n') continue;   // Empty line.
        if (row_str[0] == '#')  continue;   // Remove comments.
        if (row_str[0] == ' ')  continue;   // Ignore whitespace line.
        N_rows_p++;
    }
    *N_rows = N_rows_p;

    rewind(*file_descr);   // Rewind index label to beginning of file.
    return 0;
}//*/



/* Generic function for finding the closest lower and higher values in a sorted array.
 * This is useful for finding relevant indices into CALIB_COEFF data using SCCD values. Function is designed to be "fast".
 * 
 * 
 * ARGUMENTS AND RETURN VALUE
 * ==========================
 * INPUT  : x_array : Array of strictly monotonically increasing values. Only finite values (no NaN, +/-Inf).
 *          N       : Length of x_array.
 *          x       : Finite value to "search for".
 * OUTPUT : i_le    : (1) index to the closest value in x_array which is LESS    THAN OR EQUAL TO x.
 *                    (2) -1, iff x < x_array[0]
 * OUTPUT : i_ge    : (1) index to the closest value in x_array which is GREATER THAN OR EQUAL TO x.
 *                    (2) N, iff x > x_array[N-1]
 *          Return value :  0 = Found i_le and i_ge as defined above.
 *                         -1 = x is NaN or N < 0. 
 *          
 * 
 * IMPLEMENTATION NOTE: Always returns well defined i_le, i_ge so that can apply automated test code to the function.
 */
int FindNearestInSortedArray(double *x_array, int N, double x, int *i_le, int *i_ge)
{
    if (isnan(x) || (N < 0)) {    // Overkill?
        *i_le = INT_MIN;
        *i_ge = INT_MIN;
        return -1;
    }
    
    //============================================================================
    // Handle various special cases which the main algorithm/loop can not handle.
    //============================================================================
    if (N < 1) {
        *i_le = -1;
        *i_ge = N;
        return 0;
    }
    if (x < x_array[0]) {
        *i_le = (0) - 1;
        *i_ge = (0);
        return 0;
    }
    if (x_array[N-1] < x) {
        *i_le = (N-1);
        *i_ge = (N-1) + 1;
        return 0;
    }
    if (N == 1) {
        *i_le = 0;
        *i_ge = 0;
        return 0;
    }
    // CASE: N>=2
    if (x == x_array[0]) {
        *i_le = 0;
        *i_ge = 0;
        return 0;
    }
    if (x == x_array[N-1]) {
        *i_le = N-1;
        *i_ge = N-1;
        return 0;
    }

    // CASE: x not NaN
    // CASE: x1_array[0] < x < x_array[N-1]   (requires N>=2)

    //================
    // Main algorithm
    //================
    int i1 = 0;
    int i2 = N-1;
    while (TRUE) {
        const double x1 = x_array[i1];
        const double x2 = x_array[i2];
        
        // CASE: x1 < x < x2
        
        if (i2 - i1 == 1) {
            *i_le = i1;
            *i_ge = i2;
            return 0;
        }
        
        // CASE: i2 - i1 >= 2
        
        // Select index location between i1 and i2
        // =======================================
        // To speed up the algorithm, x_array[is] should be as close to
        // the final x_array[i_le] as possible.
        // SIMPLE  IMPLEMENTATION: Select the middle point between i1 and i2 (arbitrary rounding).
        // CURRENT IMPLEMENTATION: Assume that i is an approximately linear function of x_array[i].
        // NOTE: Important to avoid rounding errors. Expression contains both integers and doubles.
//         int is = (i1 + i2) / 2 ;   // Middle point.
        int is = i1 + ((double) i2-i1) / (x2-x1) * (x-x1);    // Linear interpolation of x_array as a function of index (or vice versa).
        
        // Make sure that i1 < is < i2, just in case the above formula fails at it, e.g. due to rounding.
        // NOTE: Requires i2 - i1 >= 2.
        is = is<=i1 ? i1+1 : is;
        is = is>=i2 ? i2-1 : is;
        
        // CASE: i1 < is < i2
        
        if (x == x_array[is]) {
            *i_le = is;
            *i_ge = is;
            return 0;
        } else if (x < x_array[is]) {        
            i2 = is;
        } else {
            // CASE: x_array[is] < x
            i1 = is;
        }
        // CASE: x_array[i1] < x < x_array[i2]

    }   // while(TRUE)
    
    // This point is never reached.
}



/* Automated test code for FindClosestInSortedArray.
 */
void FindNearestInSortedArray_TEST()
{
    typedef struct test_def {
        double x;
        int    i_le;
        int    i_ge;
        int    error_code;
        int    N;
        double *xa;    // Last in struct so that columns line up easily when using literals for assignments.
    } test_type;

    test_type test_array[99];

    int N_tests = 0;
    test_array[N_tests++] = (test_type) {2.0,   -1,    0,      0,  0, (double[]) {}};
    test_array[N_tests++] = (test_type) {2.0,   -1,    0,      0,  1, (double[]) {9.0}};
    test_array[N_tests++] = (test_type) {0.5,   -1,    0,      0,  2, (double[]) {1.0, 2.0}};
    test_array[N_tests++] = (test_type) {2.5,    1,    2,      0,  2, (double[]) {1.0, 2.0}};
    test_array[N_tests++] = (test_type) {1.5,    0,    1,      0,  2, (double[]) {1.0, 2.0}};
    test_array[N_tests++] = (test_type) {NAN, INT_MIN,INT_MIN,-1,  2, (double[]) {1.0, 2.0}};
    test_array[N_tests++] = (test_type) {4.0,    0,    0,      0,  5, (double[]) {4, 4.1, 4.2, 4.3, 5}};   // Equality with lower bound.
    test_array[N_tests++] = (test_type) {5.0,    4,    4,      0,  5, (double[]) {4, 4.1, 4.2, 4.3, 5}};   // Equality with upper bound.
    test_array[N_tests++] = (test_type) {4.3,    3,    3,      0,  5, (double[]) {4, 4.1, 4.2, 4.3, 5}};   // Equality with non-bound.
    test_array[N_tests++] = (test_type) {4.5,    3,    4,      0,  5, (double[]) {4, 4.1, 4.2, 4.3, 5}};   // Approximate match in non-linear distribution.
    test_array[N_tests++] = (test_type) {4.5,    0,    1,      0,  5, (double[]) {4, 4.7, 4.8, 4.9, 5}};   // Approximate match in non-linear distribution.

    printf("Beginning tests\n");
    int i;
    for (i=0; i<N_tests; i++) {
        test_type test = test_array[i];
        
        int i_le, i_ge;
        int error_code = FindNearestInSortedArray(test.xa, test.N, test.x, &i_le, &i_ge);
        
        if ((error_code == test.error_code) && (i_le == test.i_le) && (i_ge == test.i_ge)) {
            printf("TEST OK; i=%i\n", i);
        } else {
            printf("TEST ERROR; i=%i\n", i);
            printf("   test.x          = %f\n", test.x);
            printf("   test.N          = %i\n", test.N);
            printf("   test.i_le       = %i\n", test.i_le);
            printf("        i_le       = %i\n",      i_le);
            printf("   test.i_ge       = %i\n", test.i_ge);
            printf("        i_ge       = %i\n",      i_ge);
            printf("   test.error_code = %i\n", test.error_code);
            printf("        error_code = %i\n",      error_code);
            exit(1);
        }
    }
    printf("Finished tests\n");
}



// Print subset of the contents of a calib_coeff_data_type data structure.
// Only for debugging.
void PrintCalibCoeff(calib_coeff_data_type *cc_data)
{
    printf("cc_data->N = %i\n", cc_data->N);
    printf("cc_data->ccf_sccd_begin_array = %p\n",                cc_data->ccf_sccd_begin_array);
    printf("cc_data->ccf_sccd_begin_array[%i] = %f\n", 0,            cc_data->ccf_sccd_begin_array[0]);
    printf("cc_data->ccf_sccd_begin_array[%i] = %f\n", cc_data->N-1, cc_data->ccf_sccd_begin_array[cc_data->N-1]);
    
    printf("cc_data->ccf_data_array[%i].N = %i\n", 0,            cc_data->ccf_data_array[0].N);
    printf("cc_data->ccf_data_array[%i].N = %i\n", cc_data->N-1, cc_data->ccf_data_array[cc_data->N-1].N);
//     printf("\n");
//     printf("\n");
}



/* Derives "metadata" for a CALIB_COEFF file given an sccd that should be covered by the file.
 * 
 * This function returns, and hence effectively defines,
 * (1) the locations of CALIB_COEFF files (TAB+LBL),
 * (2) the filenaming convention,
 * (3) what SCCD ranges the files should cover (how files divide time into periods of time).
 * This function is also useful for 
 * (1) iterating over (potential) CALIB_COEFF files,
 * (2) checking that the files contain the right data.
 * 
 * ARGUMENTS AND RETURN VALUE
 * ==========================
 * INPUT  : cc_dir       : Directory where CALIB_COEFF files reside.
 * INPUT  : sccd         : SCCD that should be covered by the file.
 * OUTPUT : tab_path     : Path to TAB file.
 * OUTPUT : lbl_path     : Path to LBL file. Will be ignored if NULL.
 * OUTPUT : sccd1, sccd2 : Beginning and end of the time period that should be covered by the file.
 *                         The file should cover the SCCD range sccd1 <= sccd < sccd2.
 * RETURN VALUE :  0 = No error
 *                -1 = Time conversion error.
 */
int GetCalibCoeffFileMetadata(char *cc_dir, double sccd, char *tab_path, char *lbl_path, double *sccd1, double *sccd2)
{
    char utc[MAX_STR];
    char file_date_str[MAX_STR];
    
//     printf("GetCalibCoeffFileMetadata 1\n");    // DEBUG        
    
    // Derive sccd1 and file_date_str.
    if (ConvertSccd2Utc(sccd, utc, NULL) != 0) {
        return -1;
    }
    GetUtcYYMMDD(utc, file_date_str);
    ModifyUtc2BeginDay(utc);
    ConvertUtc2Sccd_SPICE(utc, NULL, sccd1);
    
    // Derive sccd2 = sccd1 + 1 day.
    // ASSUMES: SCCD approximately counts seconds.
    // 
    // IMPLEMENTATION NOTE: Could have been implemented with time_t+24*3600 (which appears to count seconds without leap seconds),
    // but we avoid that to minimize the usage of time_t, an in particular minimize the usage of that interpretation of time_t.
    // NOTE: Could probably have been implemented with time_t+gmtime+localtime.
    if (ConvertSccd2Utc(*sccd1+24*3600*1.5, utc, NULL) != 0) {
        return -1;
    }
    ModifyUtc2BeginDay(utc);
    ConvertUtc2Sccd_SPICE(utc, NULL, sccd2);
    
    sprintf(tab_path, "%s/RPCLAP%s_CALIB_COEFF.TAB", cc_dir, file_date_str);
    if (lbl_path != NULL) {
        sprintf(lbl_path, "%s/RPCLAP%s_CALIB_COEFF.LBL", cc_dir, file_date_str);
    }
    
//     printf("GetCalibCoeffFileMetadata 2\n");    // DEBUG
    return 0;
}



/* Functions for reading CALIB_COEFF TAB file and putting the result in an empty data structure.
 * 
 * ARGUMENTS AND RETURN VALUE
 * ==========================
 * INPUT  tab_file_path : Path to CALIB_COEFF TAB file.
 * INPUT  sccd1, sccd2  : The file should cover SCCD range, sccd1 <= sccd < sccd2 (assertion).
 * OUTPUT ccf_data      : Data structure which the functions fills with the content of the file.
 *                        Preallocated struct, but with its field pointers unused.
 * RETURN VALUE         : Error code.
 * 
 * 
 * NOTE: Assumes (asserts) that SCCD values in file increase monotonically.
 * NOTE: Sets ccf_data.data_used        = FALSE;    // Even if reloaded data, but why would one do that...
 *            ccf_data.loaded_with_data = TRUE;
 * NOTE: Does not deallocate allocated memory on return with non-zero error code.
 */
// TODO: Handle empty files (zero rows).
//      PROPOSAL: Simply allocate one extra array element, so that code never allocates for zero elements.
//      QUESTION: Already does handle empty files due to how empty arrays are handled by calloc?
// TODO: Use arrays for shortening code.
int ReadCalibCoeffFile(char *tab_file_path, double sccd1, double sccd2, calib_coeff_file_type *ccf_data)
{
    calib_coeff_file_type ccf_data_temp;

//     printf("ReadCalibCoeffFile 1\n");    // DEBUG

    YPrintf("Reading CALIB_COEFF file: %s\n", tab_file_path);
    int  N_data_rows;
    FILE *file_descr = NULL;
    if (OpenFileCountDataRows(tab_file_path, &file_descr, &N_data_rows) != 0) {
        return -1;
    }
//     printf("ReadCalibCoeffFile 2\n");    // DEBUG
    
    //=======================================================================================
    // Initialize data structure "data" representing the content of the CALIB_COEFF TAB file
    //=======================================================================================
    ccf_data_temp.data_used        = FALSE;
    ccf_data_temp.loaded_with_data = TRUE;
    ccf_data_temp.sccd_array = calloc(N_data_rows, sizeof *ccf_data_temp.sccd_array);
    ccf_data_temp.coeffs     = CallocDoubleMatrix(2*N_CALIB_COEFFS, N_data_rows);
    ccf_data_temp.N = 0;

    
    //=======================
    // Read CALIB_COEFF file
    //=======================
    double sccd_prev = -1;    // Initial value which represents there being no previous value.
    char row_str[MAX_STR];
//     printf("ReadCalibCoeffFile 3\n");    // DEBUG
//     printf("N_data_rows = %i\n", N_data_rows);
    while(fgets(row_str, MAX_STR-1, file_descr) != NULL)
    {
//         printf("ReadCalibCoeffFile 3.5\n");    // DEBUG
        if (row_str[0] == '\n') continue;    // Empty line.
        if (row_str[0] == '#' ) continue;    // Remove comments.
        if (row_str[0] == ' ' ) continue;    // Ignore whitespace line.

        //=======================
        // Interpret row in file
        //=======================
        double row_coeffs[2*N_CALIB_COEFFS];    // Temporary storage for coefficients. Using it makes the code cleaner.
        char utc[MAX_STR], sccs[MAX_STR];
//         printf("ReadCalibCoeffFile 4\n");    // DEBUG
//         printf("1: row_str = %s\n", row_str);
        sscanf(row_str, " %[^,], %[^,], %le, %le, %le, %le, %le, %le, %le, %le",
               utc, sccs,               
               &row_coeffs[CALIB_COEFF_P_P1],
               &row_coeffs[CALIB_COEFF_Q_P1],
               &row_coeffs[CALIB_COEFF_R_P1],
               &row_coeffs[CALIB_COEFF_S_P1],

               &row_coeffs[CALIB_COEFF_P_P2],
               &row_coeffs[CALIB_COEFF_Q_P2],
               &row_coeffs[CALIB_COEFF_R_P2],
               &row_coeffs[CALIB_COEFF_S_P2]
        );
//         printf("ReadCalibCoeffFile 5\n");    // DEBUG
        double sccd;
        ConvertSccs2Sccd(sccs, NULL, &sccd);

        //============
        // ASSERTIONS
        //============
        if ((sccd < sccd1) || (sccd2 <= sccd)) {
            YPrintf("%s: Corresponding SCCD values are outside of the expected range.\n", tab_file_path);   return -3;
        }
        if ((sccd_prev != -1) && !(sccd_prev < sccd)) {
            YPrintf("%s: Corresponding SCCD values are not strictly monotonically increasing.\n", tab_file_path);   return -4;
        }
//         printf("ReadCalibCoeffFile 6\n");    // DEBUG
        int i_coeff;
        for (i_coeff=0; i_coeff<2*N_CALIB_COEFFS; i_coeff++) {
            if (isnan(row_coeffs[i_coeff])) {
                YPrintf("%s: Either (1) found NaN, or (2) something could not be interpreted.\n", tab_file_path);
                YPrintf("    row_str = \"%s\".\n", row_str);
                return -2;
            }
        }
        
//         printf("ReadCalibCoeffFile 7\n");    // DEBUG
        //=========================================================
        // Copy the (relevant) parsed values to the data structure
        //=========================================================
        ccf_data_temp.sccd_array[ccf_data_temp.N] = sccd;
        for (i_coeff=0; i_coeff<2*N_CALIB_COEFFS; i_coeff++) {
            ccf_data_temp.coeffs[i_coeff][ccf_data_temp.N] = row_coeffs[i_coeff];
        }

        
        
        sccd_prev = sccd;
        ccf_data_temp.N++;
//         printf("ReadCalibCoeffFile 8\n");    // DEBUG
//         printf("2: row_str = %s\n", row_str);
//         printf("ReadCalibCoeffFile 9\n");    // DEBUG
    }
    
    //===========
    // ASSERTION
    //===========
    if (N_data_rows != ccf_data_temp.N) {
        YPrintf("Ambiguous number of data rows in file \"%s\". N_data_rows=%i, ccf_data_temp.N=%i\n",
                tab_file_path, N_data_rows, ccf_data_temp.N);
        return -4;
    }

    fclose(file_descr);
    
    *ccf_data = ccf_data_temp;
    return 0;
}



/* Initialize an empty "calib_coeff_data_type" data structure.
 * 
 * 
 * ARGUMENTS AND RETURN VALUE
 * ==========================
 * INPUT  : t_dataset_begin, t_dataset_end : Beginning and end of dataset. Used for pre-loading CALIB_COEFF files, if enabled.
 * OUTPUT : cc_data
 * RETURN VALUE : 0 = No error; -1 = Could not allocate memory; -2 = Can not obtain file metadata; -3 = Error reading file.
 * 
 * ASSUMPTION: Relies on time_t being interpreted as number of seconds (without leap seconds).
 * IMPLEMENTATION NOTE: Does not re-adjust the allocated array size downwards at the end. ==> Allocates too much, but should work.
 * NOTE: Pre-loading files assumes that there must be CALIB_COEFF files for every day in the dataset, even if there is no LAP data.
 */
int InitCalibCoeff(char *cc_dir, time_t t_dataset_begin, time_t t_dataset_end, calib_coeff_data_type *cc_data)
{
    calib_coeff_data_type ccd;   // Temporary struct being built up internally before begin assigned to the return argument.
    
//     YPrintf("Initializing CALIB_COEFF data structure. Using directory: %s\n", cc_dir);

    //===============================================================
    // Derive sccd_dataset1, sccd_dataset2
    // These define which CALIB_COEFF files to pre-load, if enabled.
    //===============================================================
    char utc[MAX_STR];
    double sccd_dataset1, sccd_dataset2;
    ConvertTimet2Utc(t_dataset_begin - CALIB_COEFF_PRELOAD_DATASET_TIME_MARGIN_S, utc, FALSE);   // Ignores error code.
    ConvertUtc2Sccd_SPICE(utc, NULL, &sccd_dataset1);
    ConvertTimet2Utc(t_dataset_end   + CALIB_COEFF_PRELOAD_DATASET_TIME_MARGIN_S, utc, FALSE);   // Ignores error code.
    ConvertUtc2Sccd_SPICE(utc, NULL, &sccd_dataset2);

    //=====================================
    // Derive sccd_mission1, sccd_mission2
    //=====================================
    double sccd_mission1, sccd_mission2;
    ConvertUtc2Sccd_SPICE(MISSION_START_UTC, NULL, &sccd_mission1);
    ConvertUtc2Sccd_SPICE(MISSION_END_UTC,   NULL, &sccd_mission2);

    //==============================
    // Initial allocation of arrays
    //==============================
    ccd.N       = 0;
    int N_alloc = ccd.N+1 +10;   // Initial array length to allocate. NOTE: Requires at least ccd.N+1 elements.
    if ((ccd.ccf_sccd_begin_array = calloc(N_alloc, sizeof *ccd.ccf_sccd_begin_array))==NULL) {
        perror("InitCalibCoeff: Can not allocate memory.");   return -1;
    }
    if ((ccd.ccf_data_array       = calloc(N_alloc, sizeof *ccd.ccf_data_array       ))==NULL) {
        perror("InitCalibCoeff: Can not allocate memory.");   return -1;
    }

//     printf("sccd_dataset1/2 = %f, %f\n", sccd_dataset1, sccd_dataset2);    // DEBUG
//     printf("sccd_mission1/2 = %f, %f\n", sccd_mission1, sccd_mission2);    // DEBUG
//     printf("InitCalibCoeff 1\n");   // DEBUG

    //==================================================================================
    // Iterate over all days for which there might be CALIB_COEFF files (whole mission)
    // Create "record" for every such day. May optionally also load these "records" with data at
    // this stage or wait until data is requested.
    //==================================================================================
    double sccd;
    for (sccd=sccd_mission1; sccd<sccd_mission2;) {   // NOTE: sccd is not updated/incremented here.
        
//         printf("InitCalibCoeff 2\n");   // DEBUG
        char tab_file_path[PATH_MAX];
        double sccd_file1, sccd_file2;
        if (GetCalibCoeffFileMetadata(cc_dir, sccd, tab_file_path, NULL, &sccd_file1, &sccd_file2)) {
//             printf("InitCalibCoeff 3\n");   // DEBUG
            YPrintf("Can not derive CALIB_COEFF file metadata.\n");
            return -2;
        }
//         printf("InitCalibCoeff 4\n");   // DEBUG
//         printf("InitCalibCoeff tab_file_path = %s\n", tab_file_path);   // DEBUG
//         printf("sccd_file1/2 = %f, %f\n", sccd_file1, sccd_file2);    // DEBUG
        

        
        calib_coeff_file_type ccf_data;
        ccf_data.data_used        = FALSE;
        ccf_data.loaded_with_data = FALSE;
        ccf_data.N                = 0;
        ccf_data.sccd_array       = NULL;
        ccf_data.coeffs           = NULL;

        //-------------------------------------------------------------------------------------------
        // OPTIONAL: Pre-load CALIB_COEFF file into ccf_data.
        // NOTE: Assumes that there are files for every day in the time interval. Must be no missing files.
        if (CALIB_COEFF_PRELOAD) {
            if ((sccd_dataset1 <= sccd_file2) && (sccd_file1 < sccd_dataset2)) {
//                 printf("InitCalibCoeff 9\n");   // DEBUG
                // CASE: The current CALIB_COEFF file overlaps with the current dataset (incl. margin).
                if (ReadCalibCoeffFile(tab_file_path, sccd_file1, sccd_file2, &ccf_data) != 0) {
                    return -3;
                }
                ccf_data.loaded_with_data = TRUE;   // Unnecessary since set by ReadCalibCoeffFile. Just there to be explicit.
            }
        }
        //-------------------------------------------------------------------------------------------

//         printf("InitCalibCoeff 5\n");   // DEBUG
        //=====================================================================
        // Replace the array allocations with larger allocations if necessary.
        //=====================================================================
        if (ccd.N+1 > N_alloc) {
            N_alloc = 2 * N_alloc;
//             printf("ccd.N   = %i\n", ccd.N);      // DEBUG
//             printf("N_alloc = %i\n", N_alloc);    // DEBUG
//             printf("InitCalibCoeff 6\n");   // DEBUG
//             printf("N_alloc*sizeof *ccd.ccf_sccd_begin_array = %i\n", N_alloc*sizeof *ccd.ccf_sccd_begin_array);   // DEBUG
            ccd.ccf_sccd_begin_array = realloc(                ccd.ccf_sccd_begin_array,
                                               N_alloc*sizeof *ccd.ccf_sccd_begin_array);   // Ignoring error.
            ccd.ccf_data_array       = realloc(                ccd.ccf_data_array,
                                               N_alloc*sizeof *ccd.ccf_data_array       );   // Ignoring error.
        }
        
        // Add ccf_data to ccd.
        ccd.ccf_sccd_begin_array[ccd.N] = sccd_file1;
        ccd.ccf_data_array      [ccd.N] = ccf_data;
        ccd.N++;
        
        sccd = sccd_file2;   // Prepare for next iteration.
    }

    // Add extra SCCD value which marks the end of the time range covered by the last ccf_data (the struct with index N-1).
    ccd.ccf_sccd_begin_array[ccd.N] = sccd;    // sccd == sccd_file2
    
    *cc_data = ccd;
    return 0;
}



/* To be run when pds is finished using CALIB_COEFF.
 * 
 * The function:
 * (1) deletes unused CALIB_COEFF files
 * (2) deallocates the CALIB_COEFF data structure
 * In other words, the content of cc_data should not be used again after being used as an argument to this function.
 * 
 * ASSUMES: Assumes that the function can still call the log function YPrintf.
 * (This is important information for when to call this function during the shutdown process.)
 */
// PROPOSAL: Better name, not "Destroy".
//      Ex: Destruct, Destructor (cf Constructor), Done
int DestroyCalibCoeff(char *cc_dir, calib_coeff_data_type *cc_data)
{
    int i_ccf;
    for (i_ccf=0; i_ccf<cc_data->N; i_ccf++) {
        calib_coeff_file_type *ccf_data = &(cc_data->ccf_data_array[i_ccf]);
        
        double sccd1, sccd2;   // Not used.
        char tab_path[PATH_MAX], lbl_path[PATH_MAX];        
        if (GetCalibCoeffFileMetadata(cc_dir, cc_data->ccf_sccd_begin_array[i_ccf], tab_path, lbl_path, &sccd1, &sccd2)) {
            return -1;
        }
            
        //=========================================
        // Deleting files unused CALIB_COEFF files
        //=========================================
        if (!ccf_data->data_used) {

            // IMPLEMENTATION NOTE: Wise too look for existence of files before deleting. Will otherwise try
            // to delete files for the entire mission's dates (since there are likely not CALIB_COEFF files for all those dates).
            if (0==access(tab_path, W_OK)) {
                YPrintf("Deleting unused calibration file: %s\n", tab_path);
                if (remove(tab_path)) {
                    YPrintf("Error when deleting file \"%s\"\n", tab_path);
                    return -1;
                }
            }
            if (0==access(lbl_path, W_OK)) {
                YPrintf("Deleting unused calibration file: %s\n", lbl_path);
                if (remove(lbl_path)) {
                    YPrintf("Error when deleting file \"%s\"\n", lbl_path);
                    return -2;
                }
            }
        } else {
            YPrintf("Keeping  used   calibration file: %s\n", tab_path);
            if (0==access(lbl_path, W_OK)) {
                YPrintf("                                  %s\n", lbl_path);
            }
        }

        //======================================================
        // Deallocate file data structure/calib_coeff_file_type
        //======================================================
        free(ccf_data->sccd_array);
        if (ccf_data->coeffs != NULL) {  
            // NOTE: Must test for NULL since FreeDoubleMatrix does not handle that special case (unless rows=0).
            FreeDoubleMatrix(ccf_data->coeffs, 2*N_CALIB_COEFFS, ccf_data->N);
        }
        // NOTE: Can not free(ccf_data) since it points to an element
        // in an array. It does not represent one allocation.
    }
    
    //==================================
    // Deallocate "root" data structure
    //==================================
    free(cc_data->ccf_sccd_begin_array);
    free(cc_data->ccf_data_array);
    cc_data->ccf_sccd_begin_array = NULL;    // For "safety", in case the caller uses the struct.
    cc_data->ccf_data_array       = NULL;    // For "safety", in case the caller uses the struct.
    
    return 0;
}//*/



/* Retrieve CALIB_COEFF file data/calib_coeff_file_type. Load it into memory it if not already loaded.
 * 
 * ARGUMENTS
 * =========
 * INPUT  : i_ccf    : Index to the calib_coeff_file_type to load.
 * OUTPUT : ccf_data : Pointer to the original data calib_coeff_file_type.
 */
int GetCalibCoeffFileData(
    char *cc_dir, calib_coeff_data_type *cc_data,
    int i_ccf, calib_coeff_file_type **ccf_data)
{
//     printf("GetCalibCoeffFileData 1\n");    // DEBUG
    //===============================
    // ASSERTION: i_ccf within range
    //===============================
    // ASSERTION;
    if ((i_ccf < 0) || (cc_data->N-1 < i_ccf)) {
        YPrintf("GetCalibCoeffFileData: Can not find CALIB_COEFF data struct for index i_ccf=%i (cc_data->N=%i). Out of range.\n", i_ccf, cc_data->N);
        return -1;
    }

//     printf("GetCalibCoeffFileData 2\n");    // DEBUG
    //==================================================================
    // Pick out the file data structure as it is at the specified index
    //==================================================================
    calib_coeff_file_type *ccf_data_ptr = &(cc_data->ccf_data_array[i_ccf]);
    
    //=======================================================
    // Load data from file if it has not already been loaded.
    //=======================================================
    if (!ccf_data_ptr->loaded_with_data) {
        // CASE: Data has not been loaded from file.
        
        // Load data from file.
        char tab_file_path[PATH_MAX];
        double sccd_file1, sccd_file2;
        
        if (GetCalibCoeffFileMetadata(cc_dir, cc_data->ccf_sccd_begin_array[i_ccf], tab_file_path, NULL,
            &sccd_file1, &sccd_file2) != 0) {
            YPrintf("GetCalibCoeffFileData: Can not derive CALIB_COEFF file metadata.\n");
            return -2;
        }
        if (ReadCalibCoeffFile(tab_file_path, sccd_file1, sccd_file2, ccf_data_ptr)) {
            YPrintf("GetCalibCoeffFileData: Can not load CALIB_COEFF file.");
            return -3;
        }
    }
    
    *ccf_data = ccf_data_ptr;
    return 0;
}



/* Find indices specifying the location of the CALIB_COEFF coefficients nearest in time.
 * 
 * ARGUMENTS
 * =========
 * OUTPUT : i_ccf_1,  i_ccf_2  : Indices to the nearest EARLIER and LATER file data structures.
 * OUTPUT : i_sccd_1, i_sccd_2 : Indices to the nearest EARLIER and LATER coefficients in
 *                               the file data structures referred to by i_ccf_1/2.
 * 
 * RETURN VALUE :  0 = No error.
 *                -1 = Can not search for CALIB_COEFF file data.
 *                -2 = sccd outside interval covered by cc_data (all files).
 *                -3 = Can not load data from file.
 *                -4 = 
 * 
 * IMPLEMENTATION NOTE: The scope of this function is chose to be relatively well suited for manual standalone tests.
 */
// PROPOSAL: Change pds_data --> Function pointer to ~GetCalibCoeffFileData-like function.
//      NOTE: Still need to submit cc_data.
//      PRO: Easier to apply automatic test code.
int FindNearestCalibCoeffTimes(
    char *cc_dir, calib_coeff_data_type *cc_data, double sccd,
    int *i_ccf_1, int *i_ccf_2, int *i_sccd_1, int *i_sccd_2)
{
    // NOTE: Effectively having two sets of identical variables
    // (1) one set of variables as "assignable arguments" (pointers), and
    // (2) one set of variables for the actual code/body.
    // Must therefore have different variable naming conventions for these two sets.
    // This also has the advantage of not assigning any arguments in the event of error.
    //      
    // Variable naming convention:
    //      *_1 <==> *_min refers to nearest earlier time.
    //      *_2 <==> *_max refers to nearest later   time.
    
    //========================================================================================================
    // Find file struct which covers "sccd".
    // <=> Set preliminary values for (i_ccf_1p, i_ccf_2p) that should be valid for the vast majority of calls.
    //========================================================================================================
//     printf("FindNearestCalibCoeffTimes 1\n");    // DEBUG
    int i_ccf_1p, i_ccf_2p;    // Indices into ccf_sccd_begin_array and ccf_data_array.
    if (FindNearestInSortedArray(cc_data->ccf_sccd_begin_array, cc_data->N, sccd, &i_ccf_1p, &i_ccf_2p)) {
        YPrintf("FindNearestCalibCoeffTimes: Can not identify which CALIB_COEFF file data to use for time sccd=%f.\n", sccd);
        return -1;
    }
    // i_ccf_1p/2 hereafter refer to the file data structures which (presumably) CONTAINS the nearest earlier
    // and nearest later time coefficients. If not, then they will be updated to do this below.
//     printf("FindNearestCalibCoeffTimes i_ccf_1p=%i, i_ccf_2p=%i\n", i_ccf_1p, i_ccf_2p);   // DEBUG
    i_ccf_2p = i_ccf_1p;
    
    //===============================================
    // Search file struct which should cover "sccd".
    //===============================================
//     printf("FindNearestCalibCoeffTimes 2\n");    // DEBUG
    int i_sccd_1p, i_sccd_2p;    // Indices into sccd_array.
    calib_coeff_file_type *ccf_data;
    // NOTE: Must use i_ccf_1p/nearest-earlier-time since each file
    // includes (covers) the stated starting time, but excludes the stated end time.
    if (GetCalibCoeffFileData(cc_dir, cc_data, i_ccf_1p, &ccf_data) != 0) {
        YPrintf("FindNearestCalibCoeffTimes: Can not obtain already CALIB_COEFF file table to use for time sccd=%f.\n", sccd);
        return -3;
    }
//     printf("FindNearestCalibCoeffTimes 2.5\n");    // DEBUG
    if (FindNearestInSortedArray(
        ccf_data->sccd_array,
        ccf_data->N,
        sccd, &i_sccd_1p, &i_sccd_2p))
    {
        YPrintf("FindNearestCalibCoeffTimes: Can not identify which time (inside CALIB_COEFF file table) to use for time sccd=%f.\n", sccd);
        return -4;    // Should be very unlikely error.
    }

    //=====================================================================
    // If found no nearest earlier value in the initial file,
    // search through earlier-time files and modify (i_ccf_1p, i_sccd_1p).
    //=====================================================================
//     printf("FindNearestCalibCoeffTimes 3\n");    // DEBUG
    if (i_sccd_1p < 0) {
        do {
            i_ccf_1p--;
            if (GetCalibCoeffFileData(cc_dir, cc_data, i_ccf_1p, &ccf_data) != 0) {   return -3;   }
        } while (ccf_data->N == 0);
        i_sccd_1p = ccf_data->N-1;
    }

    //===================================================================
    // If found no nearest later value in the initial file,
    // search through later-time files and modify (i_ccf_2p, i_sccd_2p).
    //===================================================================
//     printf("FindNearestCalibCoeffTimes 4\n");    // DEBUG
    if (ccf_data->N-1 < i_sccd_2p) {
        do {
            i_ccf_2p++;
            if (GetCalibCoeffFileData(cc_dir, cc_data, i_ccf_2p, &ccf_data) != 0) {   return -3;   }
        } while (ccf_data->N == 0);
        i_sccd_2p = 0;
    }

    *i_ccf_1  = i_ccf_1p;
    *i_ccf_2  = i_ccf_2p;
    *i_sccd_1 = i_sccd_1p;
    *i_sccd_2 = i_sccd_2p;
    return 0;
}



/* Derive the CALIB_COEFF coefficients for the time sccd, interpolated from the values stored
 * at the positions indicated by indices.
 * 
 * ARGUMENTS
 * =========
 * INPUT  : i_ccf_1,  i_ccf_2  : Indices to earlier and later file data structures.
 * INPUT  : i_sccd_1, i_sccd_2 : Indices to earlier and later coefficients in
 *                               the file data structures referred to by i_ccf_1/2.
 * OUTPUT : coeff_array        : Array of length 2*N_CALIB_COEFFS.
 * RETURN VALUE : 0 = No error; -1 = Can not obtain file data.
 * 
 * NOTE: Sets the calib_coeff_file_type.data_used=TRUE.
 * NOTE: ESOTERIC: Only sets the calib_coeff_file_type.data_used=TRUE if coefficients are actually read from that file,
 * but NOT empty CALIB_COEFF files which are "interpolated over".
 * Example:
 *      RPCLAP160101_CALIB_COEFF.TAB contains data point 1
 *      RPCLAP160102_CALIB_COEFF.TAB is empty
 *      RPCLAP160103_CALIB_COEFF.TAB contains data point 2
 *      If data points 1 & 2 are used to interpolat to a point 2016-01-02, then 
 *      RPCLAP160102_CALIB_COEFF.TAB will still be data_used=FALSE (not set to TRUE).
 * 
 * IMPLEMENTATION NOTE: The scope of this function is chose to be relatively well suited for manual standalone tests.
 */
int GetInterpolatedCalibCoeff(
    char *cc_dir, calib_coeff_data_type *cc_data,
    double sccd,
    int i_ccf_1, int i_ccf_2, int i_sccd_1, int i_sccd_2,
    double *coeff_array)
{
    //================================================================
    // Obtain the relevant file data structures/calib_coeff_file_type.
    //================================================================
    calib_coeff_file_type *ccf_data_1, *ccf_data_2;
    if (GetCalibCoeffFileData(cc_dir, cc_data, i_ccf_1, &ccf_data_1)) {   return -1;   }
    if (GetCalibCoeffFileData(cc_dir, cc_data, i_ccf_2, &ccf_data_2)) {   return -1;   }


//     { // DEBUG    
//         int i;
//         printf("GetCalibCoeffFileData coeff_array=");
//         for (i=0; i<2*N_CALIB_COEFFS; i++) {
//             printf("%.7g, ", ccf_data_1->coeffs[i][i_sccd_1]);
//         }
//         printf("\n");
//         printf("GetCalibCoeffFileData coeff_array=");
//         for (i=0; i<2*N_CALIB_COEFFS; i++) {
//             printf("%.7g, ", ccf_data_2->coeffs[i][i_sccd_2]);
//         }
//         printf("\n");
//     }

    //=====================================
    // Calculate interpolated coefficients
    //=====================================
    if ((i_ccf_1 == i_ccf_2) && (i_sccd_1 == i_sccd_2)) {
        // CASE: Nearest time before and after are identical.
        int i_coeff;
        for (i_coeff = 0; i_coeff<2*N_CALIB_COEFFS; i_coeff++) {
            // Unimportant if uses *_1 or *_2.
            coeff_array[i_coeff] = ccf_data_1->coeffs[i_coeff][i_sccd_1];
        }
    } else {    
        const double sccd_1 = ccf_data_1->sccd_array[i_sccd_1];
        const double sccd_2 = ccf_data_2->sccd_array[i_sccd_2];
        const double C = (sccd - sccd_1) / (sccd_2 - sccd_1);
        int i_coeff;
        for (i_coeff = 0; i_coeff<2*N_CALIB_COEFFS; i_coeff++) {
            const double coeff_1 = ccf_data_1->coeffs[i_coeff][i_sccd_1];
            const double coeff_2 = ccf_data_2->coeffs[i_coeff][i_sccd_2];
//             printf("(coeff_1, coeff_2) = %e, %e\n", coeff_1, coeff_2);    // DEBUG
            coeff_array[i_coeff] = coeff_1 + C * (coeff_2 - coeff_1);
        }
    }



    ccf_data_1->data_used = TRUE;
    ccf_data_2->data_used = TRUE;
    return 0;
}



/* Get the CALIB_COEFF coefficients that apply for a given time.
 * 
 * ARGUMENTS AND RETURN VALUE
 * ==========================
 * OUTPUT : coeff_array : Array of length 2*N_CALIB_COEFFS.
 * RETURN VALUE : 0=No error; -1,-2=Error
 * 
 * IMPLEMENTATION NOTE: The scope of this function is chose to be relatively well suited for manual standalone tests.
 */
int GetCalibCoeff(char *cc_dir, calib_coeff_data_type *cc_data, double sccd, double *coeff_array)
{
//     PrintCalibCoeff(cc_data);    // DEBUG
    
    //=================================================================
    // Find the two nearest data points (moments in time), as indices.
    //=================================================================
    int i_ccf_1, i_ccf_2, i_sccd_1, i_sccd_2;
    if (FindNearestCalibCoeffTimes(
        cc_dir, cc_data, sccd,
        &i_ccf_1, &i_ccf_2, &i_sccd_1, &i_sccd_2) != 0)
    {
        YPrintf("GetCalibCoeff: Can not obtain the location of CALIB_COEFF coefficients nearest to the specified time sccd=%f.\n", sccd);
        return -1;
    }
    
    //==================================
    // Interpolate the two data points.
    //==================================
    if (GetInterpolatedCalibCoeff(
        cc_dir, cc_data, sccd,
        i_ccf_1, i_ccf_2, i_sccd_1, i_sccd_2, coeff_array)) {
        return -2;
    }

    return 0;
}



// "Manual" test cod of CALIB_COEFF functionality.
void CalibCoeff_TEST()
{
    int junk;
    
    time_t t_dataset_begin, t_dataset_end;
    ConvertUtc2Timet("2016-01-01T00:00:00", &t_dataset_begin);
    ConvertUtc2Timet("2016-01-03T00:00:00", &t_dataset_end);
    
    char cc_dir[PATH_MAX];
// //     strcpy(cc_dir, "/misc/rosetta/LAP_ARCHIVE/test_CALIB_COEFF");   // NOTE: No ending slash.
    strcpy(cc_dir, "/home/erjo/pds_new_datasets/RO-C-RPCLAP-3-PRL-CALIB-V0.1/CALIB");   // NOTE: No ending slash.
    
    calib_coeff_data_type cc_data;
    
//     printf("CalibCoeff_TEST 1\n");    // DEBUG
    if (InitCalibCoeff(cc_dir, t_dataset_begin, t_dataset_end, &cc_data)) {
        printf("CalibCoeff_TEST InitCalibCoeff ERROR\n");
        return;
    }
    printf("CalibCoeff_TEST 2\n");    // DEBUG

    double sccd;
    double coeff_array[2*N_CALIB_COEFFS];
    
//     char *utc = "2016-01-01T12:00:00";
    char *utc = "2016-01-02T00:00:00";
    ConvertUtc2Sccd_SPICE(utc, &junk, &sccd);
    if (GetCalibCoeff(cc_dir, &cc_data, sccd, coeff_array)) {
        printf("CalibCoeff_TEST GetCalibCoeff ERROR\n");
        return;
    }
    printf("CalibCoeff_TEST 3\n");    // DEBUG
    printf("Coeffs: %s : %.7g, %.6g, %.6g, %.6g : %.7g, %.6g, %.6g, %.6g\n",
           utc, 
           coeff_array[0], coeff_array[1], coeff_array[2], coeff_array[3],
           coeff_array[4], coeff_array[5], coeff_array[6], coeff_array[7]);
    
    if (DestroyCalibCoeff(cc_dir, &cc_data)) {    // NOTE: Should remove files. Important to experiment on COPIES.
        printf("CalibCoeff_TEST DestroyCalibCoeff ERROR\n");
        return;
    }
    printf("CalibCoeff_TEST 5\n");    // DEBUG
}
