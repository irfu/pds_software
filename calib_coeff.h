/*
 * Header file for functions and definitions used by calib_coeff.c
 *
 * Not all functions. Only functions which need to be exposed to the outside.
 */


// Constants defining the meaning and range of the coefficient index in calib_coeff_file_type->coeffs[i_coeff][i_sccd].
//
// PROPOSAL: Move to pds.h?
//      CON: Not needed outside of this module.
//      CON: Esoteric constants.
#define CALIB_COEFF_P_P1   0
#define CALIB_COEFF_Q_P1   1
#define CALIB_COEFF_R_P1   2
#define CALIB_COEFF_S_P1   3
#define CALIB_COEFF_P_P2   (CALIB_COEFF_P_P1 + N_CALIB_COEFFS)
#define CALIB_COEFF_Q_P2   (CALIB_COEFF_Q_P1 + N_CALIB_COEFFS)
#define CALIB_COEFF_R_P2   (CALIB_COEFF_R_P1 + N_CALIB_COEFFS)
#define CALIB_COEFF_S_P2   (CALIB_COEFF_S_P1 + N_CALIB_COEFFS)
#define N_CALIB_COEFFS     4      // Number of CALIB_COEFF coefficients per probe (per time; e.g. 4 for p,q,r,s)



//==================================================================================
// Data structure corresponding to the content of one CALIB_COEFF TAB file.
//==================================================================================
typedef struct calib_coeff_file_type_def
{
  int    data_used;             // Boolean. TRUE=Iff the data has actually been used.
  int    loaded_with_data;      // Boolean. TRUE=Iff the data structure has been filled with data.
  int    N;                     // Length of the arrays in this struct.
  double *sccd_array;           // Timestamps for the corresponding p1, q1 etc arrays. Length N.
  double **coeffs;              // 2D array [i_coeff][i_time] of coefficients, size [2*N_CALIB_COEFFS][N].
} calib_coeff_file_type;

//==================================================================================
// Data structure that represents the calibration contents of all CALIB_COEFF files
// via calib_coeff_file_type structs.
//==================================================================================
typedef struct calib_coeff_data_type_def
{
  // Array of sorted SCCD values (strictly monotonically increasing).
  // For records loaded with contents of a CALIB_COEFF file: Beginning of the day (midnight).
  // For records not loaded: Beginning of day of time period that is not represented.
  // The extra last element represents only the end of the last file.
  // Length N+1.
  double *ccf_sccd_begin_array;

  // Array where each component contains data for one CALIB_COEFF file pair. Length N.
  calib_coeff_file_type  *ccf_data_array;
    
  int N;         // Number of (potential) files, number of calib_coeff_file_type structs.
//   int N_alloc;   // Size of the space actually allocated for arrays, in units of their components.
//                  // Unnecessary unless one wants to change the size of arrays in the future.
} calib_coeff_data_type;



// Functions needed outside the module.
void FindNearestInSortedArray_TEST();
void CalibCoeff_TEST();
int InitCalibCoeff(char *cc_dir, time_t t_dataset_begin, time_t t_dataset_end, calib_coeff_data_type *cc_data);
int DestroyCalibCoeff(char *cc_dir, calib_coeff_data_type *cc_data);
int GetCalibCoeff(char *cc_dir, calib_coeff_data_type *cc_data, double sccd, double *coeff_array);

// Not presently used by any other module, but could be useful elsewhere.
int OpenFileCountDataRows(char *file_path, FILE **file_descr, int *N_rows);
void ModifyUtc2BeginDay(char *utc);
void GetUtcYYMMDD(char *utc, char *date_str);
