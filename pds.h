
// NOTE: TRUE mostly useful for assignment. NOTE: If all non-zero values represent true, then comparing with
// this does not work for equality checks ("true==true", but still 1!=2).
#define FALSE 0
#define TRUE 1              

#define EPS                1E-10   // Just a small number
#define MLEN_DSID           1024   // Max length of data set ID. (Officially max 40 char string, according to "Rosetta Archiving Conventions".)
#define MLEN_DSNA           1024   // Max length of data set name. (Officially max 60 char string, according to "Rosetta Archiving Conventions".)
#define DDS_TM_FILE_LEN       37   // DDS TM File length in the sorted directories
#define SC_SIZE           131072   // Length of circular science input buffer 2^17
#define MC_SIZE            32768   // Length of mirror buffer
#define HK_SIZE            65536   // Length of circular HK input buffer 2^16
#define T_SIZE              8192   // Temporary input buffer size ~2 max S/C packets
#define RIDICULUS          65536   // Length of science data longer than this is ridiculus.
#define MAX_STR             1024   // Intended (likely) as allocation length for all temporary & generic strings. Not applied everywhere though.
#define LAP_HK_LEN            12 



/**=====================================================================================================================================
 * Manually set values for determining when to cancel the science (SC) thread.
 * ---------------------------------------------------------------------------
 * NOTE: The optimal time (timeout & delay) values depend on the speed of execution, i.e. on (1) the speed
 * of the host computer itself (the specification) and (2) whether other processor-intensive applications are running at the same time.
 * 
 * PROPOSAL: Move these definitions to nice.h.
 =====================================================================================================================================*/
// When exiting, try to terminate the science thread only after the science buffer fill value goes below this value
// (or when exceeding timeout).
#define SC_THREAD_CANCEL_BUFF_SIZE_THRESHOLD        3000   // [bytes]
// When exiting, only wait for the science buffer to shrink below the threshold for no more than this period of time before
// terminating the science thread anyway. This is a default value which can optionally be overridden.
#define SC_THREAD_CANCEL_THRESHOLD_TIMEOUT_DEFAULT   600   // [seconds], non-negative integer
// When exiting, after the science buffer is below the threshold (or timeout has passed), wait this long for science buffer to be emptied.
#define SC_THREAD_CANCEL_POSTTHRESHOLD_DELAY          12   // [seconds]  



#define M_HEAD     0xAA   // Main header
#define S_HEAD     0xCC   // Sub header. pds_x.xx.c contains many 0xCC which one can suspect should really be replaced with S_HEAD.

#define MAX_MACRO_BLCKS 10
#define MAX_MACROS_INBL 256 // This is 8 physically in instrument..but to accomodate for extra versions it is set higher.
#define MAX_LINES       1024

// Used by pds_x.xx.c: DecodeScience: param_type
#define NO_PARAMS    0 // Parameter type indicator, no parameters. Implicitly ADC16, non-sweep (?). Change name to ADC16_LF_PARAMS?
#define SWEEP_PARAMS 1 // Parameter type indicator, sweep parameters
#define ADC20_PARAMS 2 // Parameter type indicator, ADC 20 Bit parameters

// Used by "curr_type_def#sensor".
// NONE: Can not be set to arbitrary values since curr_type#sensor is incremented in DecodeScience.
#define SENS_NONE    0
#define SENS_P1      1
#define SENS_P2      2
#define SENS_P1P2    3    // = P3

// ADC sampling frequencies.
#define SAMP_FREQ_ADC16   18750.0
#define SAMP_FREQ_ADC20      57.8

#define DENSITY 1
#define E_FIELD 2

// Prefix "D16" = ADC16 (NOT the same as 16-bit data!)
// Prefix "D20" = ADC20
// D20(x)(T), x=probe
// T = Truncated (i.e. 16 bit data from ADC20)
#define D16     16
#define D20     20
#define D20T    203
#define D201    201
#define D201T   204
#define D202    202
#define D202T   205

#define TIME_CORR_PTOT_SIZE 48 // Size of time correlation packet + DDS packet header
#define TIME_CORR_P_SIZE 30    // Size of time correlation packet itself

// HK temperature calibration.
// These constants give the temperature 
// of an OP amp on the analog board.
// Accuracy is not that good!

#define T_OFFSET  -260    
#define T_SCALE   0.0215  


#define HK_NUM_LINES     2702         // Maximum number of lines (HK packets) per HK TAB file. 24*60*60/32 = 2700. Set high to minimize number of HK files.
#define HK_LINE_SIZE      208
#define HK_LINE_SIZE_STR "208"
#define HK_LENGTH 14

//DDS Definitions
#define MAX_NAME 64
#define MIN_NAME 32

#define PDS_LOG0        "0pds_system.log"
#define PDS_LOG1        "1pds_dds_packet_filter.log"
//#define PDS_LOG2        "2pds_dds_last_state.log"
#define PDS_LOG3        "3pds_dds_progress.log"
#define PDS_LOG4        "4pds_rpc_packet_filter.log"
#define PDS_LOG5        "5pds_science_decode.log"
#define PDS_LOG6        "6pds_hk_decode.log"

// Number of ground stations
#define NGSTATIONS 9

//States
#define S01_GET_MAINH                  1
#define S02_TEST_SYNC                  2
#define S03_GET_TIME_CODE              3
#define S04_GET_ID_CODE                4
#define S05_GET_MACRO_ID               5
#define S06_GET_MACRO_DESC             6
#define S07_GET_PARAMS                 7
#define S08_DECODE_PARAMS              8
#define S09_COMPARE_PARAMS             9
#define S10_NOT_USED                  10
#define S11_GET_LENGTH                11
#define S12_GET_DATA                  12
#define S13_RECONNECT                 13
#define S14_RESOLVE_MACRO_PARAMETERS  14
#define S15_WRITE_PDS_FILES           15

#define FINITE_YIELDS         5



// Values that are used for the corresponding PDS keywords.
// NOTE: The string value of PRODUCER_FULL_NAME must be quoted. In addition to that, C syntax requires quotes around the string.
#define PDS_PRODUCER_ID          "EJ"
#define PDS_PRODUCER_FULL_NAME   "\"ERIK P G JOHANSSON\""



#define ROSETTA_SPICE_ID   -226    // ID number used to represent the Rosetta spacecraft in SPICE functions. Defined by NASA NAIF.



/*========================================================================================================
 * TRUE=Use SPICE-based time conversion function(s)
 * FALSE=Use older, non-SPICE-based time conversion function(s).
 * This functionality is implemented as TRUE/FALSE which can be read by C code, rather than #ifdef-#else
 * to deactivate code. This way, test code can have access to both SPICE and non-SPICE code simultaneously
 * so that it can compare them in the same run.
 ========================================================================================================*/
#define USE_SPICE          TRUE
// #define USE_SPICE          FALSE

/*========================================================================================================
 * Rosetta spacecraft clock reset counter (used in SCCS). Used for those parts of the code where
 * it is not/can not be properly obtained from a proper source. The reset counter was 1 for the entire
 * mission (at least after launch) so it should not matter that it is set from a constant sometimes.
 ========================================================================================================*/
#define ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER   1

/*========================================================================================================
 * TRUE=Run informal "test code", which normally deactivates the normal functioning of pds.
 * FALSE=Run pds the way it is supposed to run.
 * Should always be set to FALSE, except during development.
 ========================================================================================================*/
#define RUN_TEST_CODE      FALSE
// #define RUN_TEST_CODE      TRUE



/**=============================================================================================
 * Values which are SUBTRACTED from all non-negative ADC16 EDITED and CALIB data respectively.
 * 
 * CONTEXT: The ADC16s have a flaw that makes them jump betweeen negative and non-negative values.
 * Therefore one needs to subtract/add a value to non-negative ADC16 values for at least CALIB.
 * The value should ideally be 2.5.
 * NOTE: According to LAP team agreement, EDITED should NOT have this jump (i.e. offset=0), but
 * the functionality is kept to make it possible to emulate the old behaviour. To emulate pds'
 * old behaviour, set both constants to -2.
 * NOTE: The value of ADC16_EDITED_NONNEGATIVE_OFFSET_ADC16TM affects the generation of sweep
 * offsets (bias-dependent; macro 0x104). write_CALIB_MEAS_files contains a similar constant
 * which must be compatible with the value here.
 ==============================================================================================*/
#define ADC16_EDITED_NONNEGATIVE_OFFSET_ADC16TM    -0      // [ADC16 TM units]. Must be an integer.
#define ADC16_CALIB_NONNEGATIVE_OFFSET_ADC16TM     -2.5    // [ADC16 TM units]. Does not have to an integer.



/**========================================================================================================================
 * Constants that describe the difference between ADC16 and ADC20 measurements as a linear function
 * ------------------------------------------------------------------------------------------------
 * NOTE: The values used here could be used to derive the ADC20 calibration factors from the ADC16 calibration factors.
 * They would be slightly different for the different probes.
 * 
 * CALIB_ADC20_Px_OFFSET_ADC16TM = "Adjusted difference" = A_16/A_20*B_20 - B_16 =
 * = Values which should be SUBTRACTED from all ADC20 data. x = probe 1 or 2.
 * Values are expressed in ADC16 TM units (NOT ADC20) and should thus be multiplied with appropriate ADC16
 * conversion factor (never ADC20 factor) depending on density/E-field and high/low-gain.
 * 
 * ADC_RATIO_Px = Ratio between the slope constants A_16/A_20 (!) for probe x=1 or 2 (linear fit slope constants),
 *                i.e. calibration/conversion factors relate as C_ADC20 = C_ADC16 * ADC_RATIO_Px * constant(!).
 * 
 * NOTE: Since these offsets refer to a systematic difference between two internal analogue signals, they correspond
 * neither to offsets between analogue input signals, nor between output signals.
 * 
 * NOTE: CALIB_ADC20_P1/P2_OFFSET_ADC16TM & ADC_RATIO_P1/P2 have been derived from a one-time in-flight calibration on
 * 2015-05-28.
 * See Chapter 4, "LAP Offset Determination and Calibration", Anders Eriksson 2015-06-02. It defines A_16, A_20, B_16, B_20.
 * Values have been taken from Table 4.
 * ADC_RATIO_P1/P2 have been adjusted for the moving-average bug which was not know at the time of the calibration
 * measurement, and which is not considered in the report (v2015-06-02).
 *
 * /Erik P G Johansson 2015-06-11, 2016-09-27, 2017-04-24
 ========================================================================================================================*/
#define CALIB_ADC20_P1_OFFSET_ADC16TM   77.9601
#define CALIB_ADC20_P2_OFFSET_ADC16TM   84.8991
#define ADC_RATIO_P1                     1.0030
#define ADC_RATIO_P2                     1.0046



/*=================================================================================================
 * ADC16 calibration constants
 * ---------------------------
 * Corresponds to old, mission-specific PDS keywords which are no longer read from LBL files.
 * Converts TM units --> volt, and
 * Converts TM units --> ampere respectively.
 =================================================================================================*/
#define CALIB_ADC16_FACTOR_VOLTAGE        1.22072175E-3   // ROSETTA:LAP_VOLTAGE_CAL_16B = "1.22072175E-3"
#define CALIB_ADC16_FACTOR_CURRENT_G1     3.05180438E-10  // ROSETTA:LAP_CURRENT_CAL_16B_G1 = "3.05180438E-10"    High-gain
#define CALIB_ADC16_FACTOR_CURRENT_G0_05  6.10360876E-9   // ROSETTA:LAP_CURRENT_CAL_16B_G0_05 = "6.10360876E-9"  Low-gain



/**========================================================================================================================
 * Offsets between 8 kHz and 4 kHz data (i.e. only ADC16 data, density+E field)
 * ----------------------------------------------------------------------------
 * See Chapter 4.4, "LAP Offset Determination and Calibration", v2015-09-01, Anders Eriksson.
 * This may have to be changed to time-varying values some day.
 * 
 * The values should be SUBTRACTED from all 8 kHz ADC16 data.
 * Values are expressed in ADC16 TM units and should thus be multiplied with appropriate ADC16
 * conversion factor depending on density/E-field and high/low-gain:
 * 
 * NOTE: The sign is not clearly expressed in the report. The report refers to sweep offsets b_pq (not signals) to use for
 * 4 kHz and 8 kHz data and which are _subtracted_ from signal, i.e. their difference have the opposite sign.
 *
 * NOTE: Since these offsets refer to a systematic difference between two internal analogue signals, they correspond
 * neither to fixed offsets between physical input signals, nor between TM output signals.
 * 
 * AE mail 2016-09-27: EFIELD_P1 - EFIELD_P2 = DENSITY_P1 - DENSITY_P2 = 25.35-1.4 = 23.95.
 ========================================================================================================================*/
#define CALIB_8KHZ_P1_OFFSET_ADC16TM    -1.4
#define CALIB_8KHZ_P2_OFFSET_ADC16TM   -25.35



/*===========================================================================================================================
 * ADC20 data is timestamped by the RPCLAP electronics some time after the actual measurement. Therefore, the below number
 * is SUBTRACTED from the TM timestamp for CALIB datasets.
 * NOTE: The subtraction takes place in SCCD/spacecraft clock for performance reasons, not e.g. "et" (ephemeris time;
 * proper second counter), and is therefore slightly approximate (effectively: the actually subtracted value varies
 * slightly over time).
 * Unit: seconds
 * 
 * The value 20 ms can be found in the paper "RPC-LAP: The Rosetta Langmuir Probe Instrument", A. I. Eriksson et.al., 2006,
 * Figure 6, caption.
 ==========================================================================================================================*/
#define ADC20_DELAY_S    0.020
// #define ADC20_DELAY_S    0.000    // For testing



/*========================================================================================================
 * Flag for whether to use CALIB_COEFF or not.
 * TRUE  = Use CALIB_COEFF
 * FALSE = Use CALIB_MEAS
 * 
 * NOTE: Must call InitCalibMeas and DestroyCalibMeas to remove copied, but unused CALIB_MEAS files even
 * if the data is not used.
 ========================================================================================================*/
#define CALIB_COEFF_ENABLED   TRUE
// #define CALIB_COEFF_ENABLED   FALSE



/*======================================================================================================
 * Define one CALIB_MEAS file pair (LBL+TAB) which will not be deleted, even if CALIB_MEAS is not used.
 * In practice, this is intended to prevent RPCLAP030101_CALIB_MEAS.LBL/TAB from being automatically
 * deleted since that is a special ground-calibration data product that is not really part of the other
 * CALIB_MEAS files.
 * 
 * NOTE: Must be a specified using a LBL file because of how the CALIB_MEAS automatic removal algorithm
 * works.
 ======================================================================================================*/
#define CALIB_MEAS_FILE_EXCEPTION_LBL   ("RPCLAP030101_CALIB_MEAS.LBL")



/*========================================================================================================
 * Determine whether to attempt to load CALIB_COEFF files before the actual processing of data.
 * 
 * If pre-loading is on
 * ====================
 * pds will preemptively load all CALIB_COEFF files for the dataset time period
 * (plus some margin, see CALIB_COEFF_PRELOAD_DATASET_TIME_MARGIN_S).
 * NOTE: ADVANTAGE: This is useful for immediately making sure that all presumably needed CALIB_COEFF
 * files can be loaded early in the execution of pds (files exist and are non-corrupt; loading code
 * works). This is meant to avoid that pds crashes late in the processing of large datasets
 * just because something is wrong with the CALIB_COEFF files, or the code that loads them.
 * NOTE: DISADVANTAGE: This means that there has to be CALIB_COEFF files for every single day of
 * the dataset, including for days without CALIB_COEFF data and for which CALIB_COEFF data is not needed (because of
 * LAP data gaps). One can create empty CALIB_COEFF files for this case.
 * NOTE: If pds needs more CALIB_COEFF files (because of interpolation over empty files at beginning
 * or end of dataset; should ideally never happen), then pds will load files on demand.
 * 
 * If pre-loading is off
 * =====================
 * pds will only load CALIB_COEFF files on-demand. If there is truly CALIB_COEFF data for all times for
 * which there is RPCLAP data (plus some minor time margins), then no extra (empty) CALIB_COEFF files
 * should be needed.
 * 
 * NOTE: FALSE is recommended, unless debugging CALIB_COEFF (data files or code).
 *========================================================================================================*/
// #define CALIB_COEFF_PRELOAD                        TRUE
#define CALIB_COEFF_PRELOAD                        FALSE

/*========================================================================================================
 * The pre-loaded CALIB_COEFF files covers the official dataset time interval plus an extra time margin
 * before and after. This variable is that time margin. Should sensibly (but most not be) at least zero.
 * Unit: Seconds (hence "_S" in the name).
 * 
 * NOTE: Only relevant when pre-loading of CALIB_COEFF files is activated.
 ========================================================================================================*/
#define CALIB_COEFF_PRELOAD_DATASET_TIME_MARGIN_S  1000

/*=======================================================================================================
 * Approximate begin and end of Rosetta mission.
 * Indirectly defines the range of time for which pds will assume that there MIGHT be CALIB_COEFF files
 * which it needs to have to create a data structure with "slots"/array indices for every potential
 * CALIB_COEFF file. Should therefore contain some extra margins.
 * 
 * NOTE: Approximately these times will be converted to SCCS, and they can therefore only be set to
 * times for which there are valid SCCS times.
 =======================================================================================================*/
#define MISSION_START_UTC                          "2004-03-03T00:00:00"
#define MISSION_END_UTC                            "2016-09-30T23:59:59"



/*===============================================================================================================
 * Constants controlling how saturation is detected in CALIB data sets using
 * (1) min-max limits (in calibrated values; voltage, ampere)
 * (2) special values (in TM units) that ADC:s generate when they are saturated.
 * When a saturated sample is detected, it is replaced by a special value.
 *
 * NOTE: This saturation detection algorithm does not work for P3 and is not used for P3, since:
 * (1) There are no (calibrated) min-max limits that separate saturated from non-saturated values
 *  min1 < x1 < max1
 *  min2 < x2 < max2
 *  ==> min1-max2 < x1-x2 < max1-min2
 *  which is what is needed to avoid false negatives, but which also produces many false positives
 *  (positive=saturation).
 * (2) The subtraction between the probe signals is done digitally onboard and there is (presumably)
 *  no checking for saturation in the two probe signals before the subtraction
 * ==> There is no special value in the difference that represents saturation.
 *
 * NOTE: For saturated truncated ADC20 data, the same TM value may correspond to both saturation, and
 * non-saturation, since the (onboard) 4-bit truncation reduces sets of 16 different values to one.
 * Saturation detection on truncated ADC20 data via TM special value will therefore produce some false positives
 * (positive=saturation).
 * NOTE: Saturation can not be detected in onboard averaged data, since only a subset of underlying
 * samples may be saturated, or all are saturated but both at min and max (analog saturation), giving a seemingly
 * non-saturated average.
 * NOTE: Min-max limits are inclusive, i.e. only values outside of the range (e.g. x < x_min) count as saturated.
 * NOTE: This functionality is unlikely to detect saturation for diffs since diffs are calculate (onboard) from
 * digital values. If, for example, both probes are saturated, then the diff will still appear to be
 * non-saturated.
 * 
 * NOTE: Lapdog uses the special value to represent the absence of data for many types of causes, whereas in pds
 * this special value ONLY REPRESENTS SATURATION.
 ===============================================================================================================*/
#define USE_SATURATION_LIMITS                   TRUE   // Enable/disable saturation
//#define USE_SATURATION_LIMITS                   FALSE

// Special ADC values that are interpreted as saturation.
// 20-bit data: ADC20 non-truncated data (2^19=524288)
#define SATURATION_ADC20_NONTRUNC_TM_VALUE_1      -524288
#define SATURATION_ADC20_NONTRUNC_TM_VALUE_2       524287
// 16-bit data: (1) ADC16 data and (2) truncated ADC20 data (2^15=32768)
#define SATURATION_ADC16_ADC20_TRUNC_TM_VALUE_1    -32768
#define SATURATION_ADC16_ADC20_TRUNC_TM_VALUE_2     32767

#define SATURATION_EFIELD_MIN                   -1e99
#define SATURATION_EFIELD_MAX                    1e99
// #define SATURATION_EFIELD_MIN                   1.7e0    // Test value
// #define SATURATION_EFIELD_MAX                   1.8e0    // Test value

#define SATURATION_DENSITY_LG_MIN               -1e99
#define SATURATION_DENSITY_LG_MAX                1e99
// #define SATURATION_DENSITY_LG_MIN                 -1e-8    // Test value
// #define SATURATION_DENSITY_LG_MAX                  1e-8    // Test value

#define SATURATION_DENSITY_HG_MIN               -1e99
#define SATURATION_DENSITY_HG_MAX                9.7e-6    // Official saturation limit: 9.7 uA.
// #define SATURATION_DENSITY_HG_MIN              -1e-8    // Test value
// #define SATURATION_DENSITY_HG_MAX               1e-8    // Test value

#define SATURATION_TAB_CONSTANT                 -1000    // Special value used to represent saturation in sample in TAB files.



/*===============================================================
 * FALSE = Keep fine sweeps in all datasets.
 * TRUE  = Exclude fine sweeps from CALIB datasets.
 * 
 * NOTE: Variable only meant to be temporary.
 * Fine sweeps should always be excluded (from CALIB) eventually.
 *===============================================================*/
// #define EXCLUDE_FINE_SWEEPS_FROM_CALIB     FALSE
#define EXCLUDE_FINE_SWEEPS_FROM_CALIB     TRUE

/*==========================================================================================================
 * Enable/disable old bugfix which disables manually commanded bias for a selected list of macros.
 * This bugfix is/was meant as a mitigation for the problem that bias was manually commanded for macros
 * which set the bias as part of its macro loop, and therefore re-set (restored) the bias. Unfortunately,
 * pds does not similarily re-set the bias as part of the macro loop.
 * TRUE = Use old bugfix
 * 
 * 2019-01-23: Experiments are underway with replacing this bugfix with fake manually commanded bias
 * commands which restore the bias the same way the macros do (in the flight s/w).
 * 2019-01-29: Change to default FALSE since fake manually commanded bias commands have been added to
 * pds.bias to solve the original problem.
 ==========================================================================================================*/
#define IGNORE_MANUALLY_COMMANDED_BIAS_FOR_SELECTED_MACROS   FALSE

/*====================================================================================================================
 * How to set the timestamp for ADC20 moving average TM samples
 * 
 * FALSE: (The original implementation) Use the timestamp of the first internal sample of all the internal samples
 *        averaged over.
 * TRUE:  (Bugfix) Uses the middle point between the first and the last internal sample averaged over.
 * 
 * NOTE: This bugfix does not consider the known (2019-02-15) flight s/w bug which means that moving
 *       average also averages over one additional internal sample.
 ====================================================================================================================*/
#define ADC20_MA_TIMESTAMP_CENTER_OF_INTERNAL_SAMPLES   TRUE



// Time correlation data structure (for non-SPICE time conversion)
//
// 1) UTC_TIME=OBT_TIME*gradient(n)+offset(n) 
// 2) Valid correction if SCET(n) <= UTC_TIME <= SCET(n+1)
// 3) If no SCET(n+1) exists yet, it is considered to be infinite.
//
typedef struct tc_type_def
{
  int     n_entries;   // Number of entries
  double  *SCET;       // Time correlated OBT from which correlation below is valid
  double  *offset;     // Offset
  double  *gradient;   // Gradient
} tc_type;



// Data structure for storing "generic" table data.
//
// NOTE: ReadTableFile reads file and puts the data in this data structure.
// NOTE: This structure is used inside m_type (CALIB_MEAS data), but is ALSO used outside of and independently
// of m_type, notably course/fine bias voltages and current bias calibrations.
//      NOTE: CALIB_MEAS calibration data should be obsolete.
typedef struct c_type_def
{
  char    valid_utc[32];   // Data is taken/valid at this time (UTC string).
  int     rows;            // Rows
  int     cols;            // Columns
  double  **C;             // Conversion matrix
} c_type;

// Calibration Factors for measured data TM to Physical units
// ADC20 values not used since new functionality requires different values for P1 and P2.
/*typedef struct cf_type_def
{
  double  v_cal_16b;
//   double  v_cal_20b;
  double  c_cal_16b_hg1; 
//   double  c_cal_20b_hg1;
  double  c_cal_16b_lg; 
//   double  c_cal_20b_lg;
} cf_type;//*/



// Linked list. Last item has next==NULL.
typedef struct calib_meas_interval_type_def
{
  struct calib_meas_interval_type_def  *next;
  time_t                               t_begin;
  time_t                               t_end;
} calib_meas_interval_type;

// Represents one CALIB_MEAS file pair (TAB+LBL) plus relevant time intervals found CALIB_MEAS_EXCEPT file,
// but not calibration factors in LBL files (should be abolished).
// Could probably be merged with cf_type but then the name (cf_type) is bad and I want to avoid renaming
// variable m_type->CF (it us used in many places).
typedef struct calib_meas_file_type_def
{
  // Needed for (1) deleting unused calibration files and (2) matching CALIB_MEAS_EXCEPT exceptions time intervals (requires filename, not path).
  char                      *LBL_filename;
  // (Boolean flag.) Determine whether the corresponding calibrations (files) were actually used (true=used).
  // Can be used upon exit to delete files that were never used.
  int                       calibration_file_used;
  // Linked list of (probably) time intervals within which the corresponding CALB_MEAS data should be used.
  calib_meas_interval_type  *intervals;
} calib_meas_file_type;



// Structure containing :
// (1) Data read from the offset calibration files (CALIB_MEAS LBL+TAB files)
//     and information on when the information should be used
// (2) Calibration factors (CF) -- REMOVED
typedef struct m_type_def
{
  int                    N_calib_meas;      // Length of arrays below, i.e. number of CALIB_MEAS files.
//   cf_type                *CF;               // Array of calibration factor (CF) structures. Length N_calib_meas.
  c_type                 *CD;               // Array of calibration data   (CD) structures. Length N_calib_meas.
  calib_meas_file_type   *calib_meas_data;  // Array of CALIB_MEAS calibration data structures. Length N_calib_meas.
} m_type;



typedef struct tid_type_def
{
  int   id;
  char  code[3];
} tid_type;

typedef struct sweep_type_def
{
  char format[10];                  // LAP_SWEEP_FORMAT           = UP:DOWN UP:DOWN:UP DOWN
  unsigned char formatv;            // LAP_SWEEP_FORMAT           = RAW UNDECODED VALUE..same information as above string
  char resolution[9];               // LAP_SWEEP_RESOLUTION       = COARSE:FINE
  char p1[4];                       // LAP_SWEEPING_P1            = NO:YES
  char p2[4];                       // LAP_SWEEPING_P2            = NO:YES
  unsigned int p1_fine_offs;        // LAP_P1_FINE_SWEEP_OFFSET   = VALUE
  unsigned int p2_fine_offs;        // LAP_P2_FINE_SWEEP_OFFSET   = VALUE
  unsigned int N_plateau_insmp;     // LAP_SWEEP_PLATEAU_DURATION = VALUE (to get seconds => N_plateau_insmp/SAMP_FREQ_ADC16)
  unsigned int N_tsweep_bias_steps; // LAP_SWEEP_STEPS            = VALUE. Number of true sweep bias STEPS, i.e. number of times the bias CHANGES within true sweep, i.e. number of true sweep plateaus MINUS ONE.
  unsigned int height;              // LAP_SWEEP_STEP_HEIGHT      = VALUE
  unsigned int start_bias;          // LAP_SWEEP_START_BIAS       = VALUE

  unsigned int N_rsweep_insmp;      // Duration of raw sweep in internal samples.
} sweep_type;

//
// Ex: Sweep below has thus:
//
// 4 steps (not counting intial and final edges)
// 5 plateaues
// 2 initial plateues
// 
// Total: 4+1+2=7 plateaues
//
//
//   |-|     |-|
//   |  -   -  |
// --|   |_|   |
//
//

typedef struct adc20_type_def
{
  unsigned int N_MA_length_insmp;
  unsigned int adc20_control;
  unsigned int adc20_data_length;
  unsigned int insmp_per_tmsmp;
} adc20_type;


// Current settings for various parameters
typedef struct curr_type_def
{
  unsigned int sensor;              // LAP sensor(s) currently in use. See constants SENS_P1P2 etc.
  unsigned int transmitter;         // LAP currently transmitting sensor
  double       seq_time_TM;         // Current time (SCCD) of a measurement sequence in a macro cycle, as it occurs in the TM.
  double       stop_time_TM;        // Current stop time (SCCD) of a measurement sequence in a macro cycle, as it occurs in the TM.
  double       seq_time_corrected;  // Like seq_time_TM  (SCCD), but (possibly) adjusted for analog signal delays, ADC20_DELAY_S.
  double       stop_time_corrected; // Like stop_time_TM (SCCD), but (possibly) adjusted for analog signal delays, ADC20_DELAY_S.
  double       offset_time;         // Time since raw start time
  double       old_time;            // Used to test if we have extra bias settings
  unsigned int old_macro;           // Used to test if we have a new macro
  double       sec_per_tmsmp;       // Time [seconds] between TM samples.
  unsigned int afilter;     // Current analog filter (if data from both sensors (i.e. correlation) we assume the same filter on both)
  int ibias1;               // Fix current bias p1
  int vbias1;               // Fix voltage bias p1
  int ibias2;               // Fix current bias p2
  int vbias2;               // Fix voltage bias p2
  char bias_mode1;          // Current bias mode for P1 (according to ID code), or P1 & P2 if difference is measured.
  char bias_mode2;          // Current bias mode for P2 (according to ID code)
  char gain1[16];           // Current gain p1
  char gain2[16];           // Current gain p2
} curr_type;


// PDS structure containing PDS archive information and path, file descriptors.
// Note that in structure below we often use path to refer to both path and filename.
// Prefix c = calibration(?)
typedef struct pds_type_def
{
  int  SCResetCounter;              // Number of times the spacecraft clock has been reset
  char SCResetClock[20];            // Date and time of last spacecraft clock reset ex. 2003-01-01T00:00:00
  char templp[PATH_MAX];            // Path to PDS archive template
  char macrop[PATH_MAX];            // Path to LAP macro descriptions
  int  DPLNumber;                   // DPL number
  float DataSetVersion;             // Data Set Version, first version is 1.0
  char LabelRevNote[PATH_MAX];      // Label revision note, unlimited length in PDS, but we limit it to same as PATH_MAX characters!
  char ReleaseDate[11];             // Release date
  char cpath[PATH_MAX];             // Path to configuration file
  char apath[PATH_MAX];             // Path to anomaly file
  char bpath[PATH_MAX];             // Path to bias settings file
  char epath[PATH_MAX];             // Path to (CALIB macro) exclude file
  char depath[PATH_MAX];            // Path to data exclude file   // Erik P G Johansson 2015-03-25: Added
  char mpath[PATH_MAX];             // Path to macro description file
  char mcpath[PATH_MAX];            // Mission calendar path and file name
  char apathpds_parent[PATH_MAX];   // Dataset parent directory
  char apathpds[PATH_MAX];          // Archive path PDS (Out dataset)
  char apathdds[PATH_MAX];          // Archive path DDS (In data/TM)
  char tpath[PATH_MAX];             // Path to time correlation packets (TCORR)
  char lpath[PATH_MAX];             // Log path
  char dpathse[PATH_MAX];           // Data path PDS science edited
  char dpathsc[PATH_MAX];           // Data path PDS science calibrated
  char cpathd[PATH_MAX];            // Root path to calibration (c) data directory (d), i.e. CALIB/.
  char cpathf[PATH_MAX];            // Path to fine bias calibration data
  char cpathc[PATH_MAX];            // Path to coarse bias calibration data
  char cpathi[PATH_MAX];            // Path to current bias calibration data
  char cpathm[PATH_MAX];            // Path and filename pattern for CALIB_MEAS calibration data files.
  char cpathdfp1[PATH_MAX];         // Path to density (d) frequency (f) response probe 1 (p1)
  char cpathdfp2[PATH_MAX];         // Path to density (d) frequency (f) response probe 2 (p2)
  char cpathefp1[PATH_MAX];         // Path to E-field (e) frequency (f) response probe 1 (p1)
  char cpathefp2[PATH_MAX];         // Path to E-field (e) frequency (f) response probe 2 (p2)
  char cpathocel[PATH_MAX];         // Path to offset calibration exceptions (OCE) data (LBL file)
  char cpathocet[PATH_MAX];         // Path to offset calibration exceptions (OCE) data (TAB file)
  char spaths[PATH_MAX];            // Data subdirectory path for PDS science
  char dpathh[PATH_MAX];            // Data path PDS HK
  char spathh[PATH_MAX];            // Data subdirectory path for PDS HK
  char uapath[PATH_MAX];            // Path to data that has not been accepted
  char ipath[PATH_MAX];             // Index table file path.
  char pathmk[PATH_MAX];            // Path to SPICE metakernel.
  FILE *ylog_fd;                    // Log file descriptor LAP PDS System log
  FILE *plog_fd;                    // S/C packet filtering log
  FILE *clog_fd;                    // Log file descriptor Science Decoding log
  FILE *hlog_fd;                    // Log file descriptor HK Decoding log
  FILE *dlog_fd;                    // Log file descriptor dds packet filter log
  FILE *uaccpt_fd;                  // File descriptor to recoverfile
  FILE *slabel_fd;                  // Science archive PDS data file descriptor
  FILE *stable_fd;                  // Science data table file descriptor
  FILE *hlabel_fd;                  // HK archive PDS data file descriptor
  FILE *htable_fd;                  // HK data table file descriptor
  FILE *ilabel_fd;                  // Index label file descriptor
  FILE *itable_fd;                  // Index table file descriptor
  FILE *ddsr_fd;                    // DDS Read file descriptor
  FILE *ddsp_fd;                    // DDS progress file descriptor
} pds_type;

// Argument structure type for submitting arguments to threads when launching them.
typedef struct arg_type_def 
{
  void *arg1;
  void *arg2;
} arg_type;


typedef struct ground_stations
{
  unsigned short int ID;
  char gname[32];
} gstype;



// Contains information about the current mission phase.
// Array lengths are high estimates.
// NOTE: Not necessarily strictly mission phases, since mission phases can be split up for data deliveries.
typedef struct mission_phase_struct 
{
  char data_set_id[MLEN_DSID];
  char data_set_name[MLEN_DSNA];
  char mission_phase_name[64];   // From mission calendar.
  char mission_phase_abbrev[5];  // Mission phase abbreviation in PDS keyword DATA_SET_ID. From mission calendar

  char target_name[64];          // PDS keyword TARGET_NAME. 5th column in mission calendar.
  char target_name_dsn[32];      // Target in PDS keyword DATA_SET_NAME (dsn). 8th column in mission calendar.
  char target_name_dsi[8];       // Target in PDS keyword DATA_SET_ID   (dsi). 6th column in mission calendar.
  char target_type[32];          // Used for PDS keyword TARGET_TYPE.          7th column in mission calendar.
  
    /*
    * Beginning and end of dataset as specified by the CALL to pds, i.e.
    * (1) the mission calender, or (2) CLI arguments.
    * This is the time interval which will be searched for data.
    */
  time_t t_start;
  time_t t_stop;
  
    /**
     * Official beginning and end of dataset as will be specified in DATASET.CAT.
     *
     * This may be different from t_start, t_end (besides the difference in way of representing time) due to 
     * not splitting time series (TAB files) when they cross the specified dataset time boundaries.
     * Empirically, it is known that often, but not always, there are TAB files which begin before the dataset start time (t_start).
     * Hence, t_start should not be used for setting DATASET.CAT:START_TIME.
     * 
     * Therefore, these variables are updated based on the files actually written to disk.
     * 
     * NOTE: In principle, the dataset may also contain data for a time interval much shorter than (t_start, t_time)
     * due to data gaps but the current implementation (2017-11-17) only allows (sccd_start_data, sccd_stop_data)
     * to specify a superset of (t_start, t_stop), i.e. they always specify a larger time interval.
     */
  double sccd_start_data;
  double sccd_stop_data;
} mp_type;

typedef struct hk_lbl_info_struct
{
  char utc_time_str[256];         // Stop/start UTC time string
  char obt_time_str[256];         // Stop/start OBT time string (SCCS).
  int hk_cnt;                     // House keeping packet counter

} hk_info_type;

//------------------------------------------------------------------------
// Erik P G Johansson 2015-03-25: Created struct.
// Struct for storing intervals of time for which data is to be excluded.
// NOTE: Only uses the reset counter value at beginning of interval.
// Assumes the same reset counter value for start and stop time.
//------------------------------------------------------------------------
typedef struct data_exclude_times_struct
{
  int     N_intervals;                 // Length of arrays = Number of time intervals.
  int    *scrc_begin_list;             // Pointer to array. SCRC = Spacecraft reset counter
  double *sccd_begin_list;             // Pointer to array.
  double *sccd_end_list;               // Pointer to array.
  int    *probe_constraint_list;       // Pointer to array. Takes on values PROBE_CONSTRAINT_NONE, SENS_P1, SENS_P2, SENS_P1P2.
  int    *data_type_constraint_list;   // Pointer to array. Takes on values DATA_TYPE_CONSTRAINT_*.
} data_exclude_times_type;

#define PROBE_CONSTRAINT_NONE   0   // Must have value different from SENS_P1, SENS_P1, SENS_P1P2.

#define DATA_TYPE_CONSTRAINT_NONE    0
#define DATA_TYPE_CONSTRAINT_SWEEP   1
#define DATA_TYPE_CONSTRAINT_LF      2
#define DATA_TYPE_CONSTRAINT_HF      3
