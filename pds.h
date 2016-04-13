#define EPS 1E-10           // Just a small number
#define MLEN_DSID 1024      // Max length of data set ID. (Officially max 40 char string, according to "Rosetta Archiving Conventions".)
#define MLEN_DSNA 1024      // Max length of data set name. (Officially max 60 char string, according to "Rosetta Archiving Conventions".)
#define DDS_TM_FILE_LEN 37  // DDS TM File length in the sorted directories
#define SC_SIZE    131072   // Length of circular science input buffer 2^17
#define MC_SIZE    32768    // Length of mirror buffer
#define HK_SIZE    65536    // Length of circular HK input buffer 2^16
#define T_SIZE     8192     // Temporary input buffer size ~2 max S/C packets
#define RIDICULUS  65536    // Length of science data longer than this is ridiculus.
#define MAX_STR    1024    
#define LAP_HK_LEN 12


// Manually set values for determining when to cancel the science (SC) thread.
// ---------------------------------------------------------------------------
// NOTE: The optimal time (timeout & delay) values depend on the speed of execution, i.e. on (1) the speed
// of the host computer itself (the specification) and (2) whether other processor-intensive applications are running at the same time.
// PROPOSAL: Move these definitions to nice.h.
// 
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

#define NO_PARAMS    0 // Parameter type indicator, no parameters
#define SWEEP_PARAMS 1 // Parameter type indicator, sweep parameters
#define ADC20_PARAMS 2 // Parameter type indicator, ADC 20 Bit parameters

// Used by "curr_type_def#sensor".
#define SENS_NONE    0
#define SENS_P1      1
#define SENS_P2      2
#define SENS_P1P2    3

// ADC sampling frequencies.
#define SAMP_FREQ_ADC16 18750.0
#define SAMP_FREQ_ADC20 60.0

#define DENSITY 1
#define E_FIELD 2

// D16=ADC16, D20=ADC20
// D20x(T), x=probe
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
// Theese constants give the temperature 
// of an OP amp on the analog board.
// Accuracy is not that good!

#define T_OFFSET -260    
#define T_SCALE  0.0215  


#define HK_NUM_LINES       16         // Maximum number of lines (HK packets) per HK TAB file.
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
#define S01_GET_MAINH         1
#define S02_TEST_SYNC         2
#define S03_GET_TIME_CODE     3
#define S04_GET_ID_CODE       4
#define S05_GET_MACRO_ID      5
#define S06_GET_MACRO_DESC    6
#define S07_GET_PARAMS        7
#define S08_DECODE_PARAMS     8
#define S09_COMPARE_PARAMS    9
#define S10_NOT_USED          10
#define S11_GET_LENGTH        11
#define S12_GET_DATA          12
#define S13_RECONNECT         13
#define S14_RESOLVE_MACRO_PARAMETERS 14
#define S15_WRITE_PDS_FILES   15


#define FINITE_YIELDS         5


// Constants used for calibration of ADC20 data ("relative" to ADC16).
// --------------------------------------------------------------------
// To be multiplied with
//    ROSETTA:LAP_VOLTAGE_CAL_16B, or
//    ROSETTA:LAP_CURRENT_CAL_16B_G1 or
//    ROSETTA:LAP_CURRENT_CAL_16B_G0_05,
// depending on mode and high/low-gain to
// produce "Delta", the constant difference between ADC20 and ADC16 for the same physical signal.
// Values have been derived through a one-time calibration using LAP data from 2015-05-28.
// See Chapter 4, "LAP Offset Determination and Calibration", Anders Eriksson 2015-06-02.
// Values have been taken from Table 4.
// 
// NOTE: Current (2015-06-04) calibration values were obtained using HIGH-GAIN ("G1") so values are strictly speaking
//       ONLY VALID FOR HIGH-GAIN data.
//       Lacking low-gain values, the high-gain values might or might not be used also for low-gain data in the actual
//       implementation that uses these values. See the actual use of these constants in pds_x.xx.c:WritePTABFile.
//
// NOTE: DELTA values (as defined here) are to be multipled with the ADC16 conversion factors (LAP_CURRENT_CAL_16B_G1), not ADC20 factors.
//
// NOTE: DELTA values refer to ADC20 data after truncation. ==> Must be multiplied by 16 for non-truncated ADC20 data(?)
// 
// /Erik P G Johansson 2015-06-11
#define CALIB_ADC_G1_TM_DELTA_P1     77.9601
#define CALIB_ADC_G1_TM_DELTA_P2     84.8991

// Time correlation structure
//
// 1) UTC_TIME=OBT_TIME*gradient(n)+offset(n) 
// 2) Valid correction if SCET(n) <= UTC_TIME <= SCET(n+1)
// 3) If no SCET(n+1) exists yet, it is considered to be infinite.
//
//
typedef struct tc_type_def
{
  int netries;        // Number of entries
  double *SCET;       // Time correlated OBT from which correlation below is valid
  double *offset;     // Offset
  double *gradient;   // Gradient
} tc_type;



// Calibration data structure
// NOTE: This structure is also used outside of/independently of m_type. Notably course/fine bias voltages and current bias calibrations.
typedef struct c_type_def
{
  char validt[32];        // Data is taken/valid at this time (UTC string).
  int rows;               // Rows
  int cols;               // Columns
  double **C;             // Conversion matrix
} c_type;

// Calibration Factors for measured data TM to Physical units
typedef struct cf_type_def
{
  double v_cal_16b;
  double v_cal_20b;
  double c_cal_16b_hg1; 
  double c_cal_20b_hg1;
  double c_cal_16b_lg; 
  double c_cal_20b_lg;  
} cf_type;

typedef struct m_type_def
{
  int n;         // Lengt of arrays below
  cf_type *CF;   // Array of calibration factor structures 
  c_type  *CD;   // Array of calibration data structures
} m_type;



typedef struct tid_type_def
{
  int id;
  char code[3];
} tid_type;

typedef struct sweep_type_def
{
  char format[10];             // LAP_SWEEP_FORMAT           = UP:DOWN UP:DOWN:UP DOWN
  unsigned char formatv;       // LAP_SWEEP_FORMAT           = RAW UNDECODED VALUE..same information as above string
  char resolution[9];          // LAP_SWEEP_RESOLUTION       = COARSE:FINE
  char p1[4];                  // LAP_SWEEPING_P1            = NO:YES
  char p2[4];                  // LAP_SWEEPING_P2            = NO:YES
  unsigned int p1_fine_offs;   // LAP_P1_FINE_SWEEP_OFFSET   = VALUE
  unsigned int p2_fine_offs;   // LAP_P2_FINE_SWEEP_OFFSET   = VALUE
  unsigned int plateau_dur;    // LAP_SWEEP_PLATEAU_DURATION = VALUE (to get seconds => plateau_dur/SAMP_FREQ_ADC16)
  unsigned int steps;          // LAP_SWEEP_STEPS            = VALUE
  unsigned int height;         // LAP_SWEEP_STEP_HEIGHT      = VALUE
  unsigned int start_bias;     // LAP_SWEEP_START_BIAS       = VALUE

  unsigned int sweep_dur_s;    // Derived duration of sweep in samples. Using plateu_dur*(steps+3), plateaues = steps+1+2
} sweep_type;

//
// Ex: sweep below has thus:
//
// 4 steps (not counting intial and final edges)
// 5 plateaues
// 2 initial plateues
// 
// Total: 4+1+2=7 plateaues
//
//
//   |-|     |-|
//   |  �   -  |
// --|   |_|   |
//
//
//

typedef struct adc20_type_def
{
  unsigned int moving_average_length;
  unsigned int adc20_control;
  unsigned int adc20_data_length;
  unsigned int resampling_factor;
} adc20_type;


// Current settings for various parameters
typedef struct curr_type_def
{
  unsigned int sensor;      // LAP sensor currently in use
  unsigned int transmitter; // LAP currently transmitting sensor
  double       seq_time;    // Current time of a mesurment sequence in a macro cycle
  double       stop_time;   // Current stop time of a mesurment sequence in a macro cycle
  double       offset_time; // Time since raw start time
  double       old_time;    // Used to test if we have extra bias settings
  unsigned int old_macro;   // Used to test if we have a new macro
  double       factor;      // Current conversion factor (Not physical current..but the factor currently used for time computations)
  unsigned int afilter;     // Current analog filter (if data from both sensors(i.e. correlation) we assume the same filter on both)
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
typedef struct pds_type_def
{
  int  SCResetCounter;         // Number of times the spacecraft clock has been reset
  char SCResetClock[20];       // Date and time of last spacecraft clock reset ex. 2003-01-01T00:00:00
  char templp[PATH_MAX];       // Path to PDS archive template
  char macrop[PATH_MAX];       // Path to lap macro descriptions
  int  DPLNumber;              // DPL number
  float DataSetVersion;        // Data Set Version, first version is 1.0
  char LabelRevNote[PATH_MAX]; // Label revision note, unlimited length in PDS, but we limit it to same as PATH_MAX characters!
  char ReleaseDate[11];        // Release date
  char cpath[PATH_MAX];        // Path to configuration file
  char apath[PATH_MAX];        // Path to anomaly file
  char bpath[PATH_MAX];        // Path to bias settings file
  char epath[PATH_MAX];        // Path to (CALIB macro) exclude file
  char depath[PATH_MAX];       // Path to data exclude file   // Erik P G Johansson 2015-03-25: Added
  char mpath[PATH_MAX];        // Path to macro description file
  char mcpath[PATH_MAX];       // Mission calendar path and file name
  char apathpds[PATH_MAX];     // Archive path PDS (Out data)
  char apathdds[PATH_MAX];     // Archive path DDS (In data)
  char tpath[PATH_MAX];        // Path to time correlation packets
  char lpath[PATH_MAX];        // Log path
  char dpathse[PATH_MAX];      // Data path PDS science edited
  char dpathsc[PATH_MAX];      // Data path PDS science calibrated
  char cpathd[PATH_MAX];       // Root path to calibration data directory (d)
  char cpathf[PATH_MAX];       // Path to fine bias calibration data
  char cpathc[PATH_MAX];       // Path to coarse bias calibration data
  char cpathi[PATH_MAX];       // Path to current bias calibration data
  char cpathm[PATH_MAX];       // Path to offset calibration data. The filename part is a filename pattern.
  char cpathdfp1[PATH_MAX];    // Path to density frequency response probe 1
  char cpathdfp2[PATH_MAX];    // Path to density frequency response probe 2
  char cpathefp1[PATH_MAX];    // Path to e-field frequency response probe 1
  char cpathefp2[PATH_MAX];    // Path to e-field frequency response probe 2
  char spaths[PATH_MAX];       // Data subdirectory path for PDS science
  char dpathh[PATH_MAX];       // Data path PDS HK
  char spathh[PATH_MAX];       // Data subdirectory path for PDS HK
  char uapath[PATH_MAX];       // Path to data that has not been accepted
  char ipath[PATH_MAX];        // Index table file path.
  FILE *ylog_fd;               // Log file descriptor LAP PDS System log
  FILE *plog_fd;               // S/C packet filtering log
  FILE *clog_fd;               // Log file descriptor Science Decoding log
  FILE *hlog_fd;               // Log file descriptor HK Decoding log
  FILE *dlog_fd;               // Log file descriptor dds packet filter log
  FILE *uaccpt_fd;             // File descriptor to recoverfile
  FILE *slabel_fd;             // Science archive PDS data file descriptor
  FILE *stable_fd;             // Science data table file descriptor
  FILE *hlabel_fd;             // HK archive PDS data file descriptor
  FILE *htable_fd;             // HK data table file descriptor
  FILE *ilabel_fd;             // Index label file descriptor
  FILE *itable_fd;             // Index table file descriptor
  FILE *ddsr_fd;               // DDS Read file descriptor
  FILE *ddsp_fd;               // DDS progress file descriptor
} pds_type;

//Argument structure type for threads.
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


gstype gstations[NGSTATIONS]=
  {
    {0x000D,"ESA Villafranca 2              "},
    {0x0015,"ESA Kourou                     "},
    {0x0016,"NDIULite (for SVTs)            "},
    {0x0017,"ESA New Norcia                 "},
    {0x0022,"NASA Goldstone                 "},
    {0x0023,"NASA Canberra                  "},
    {0x0024,"NASA Madrid                    "},
    {0x007F,"ESA/ESOC Test station          "},
    {0x0082,"NDIU classic (SVTs)            "},
  };

// Contains information about the current mission phase.
// Array lengths are high estimates.
// NOTE: Not necessarily strictly mission phases, since mission phases can be split up for data deliveries.
typedef struct mission_phase_struct 
{

  char data_set_id[MLEN_DSID];
  char data_set_name[MLEN_DSNA];
  char phase_name[64];
  char abbrev[5];

  char target_name_did[64];      // did = ?!  (Not DATA_SET_ID) Value is used for TARGET_NAME.
  char target_name_dsn[32];      // dsn = DATA_SET_NAME
  char target_id[8];             // Used in DATA_SET_ID.
  char target_type[32];
  
  time_t start;
  time_t stop;
} mp_type;

typedef struct hk_lbl_info_struct
{
  char utc_time_str[256];         // Stop/start UTC time string
  char obt_time_str[256];         // Stop/start OBT time string
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
  int      N_intervals;       // Length of arrays = Number of time intervals.
  int     *SCResetCounter_begin_list;
  double  *t_begin_list;    // Spacecraft clock counter as double (true decimals). Pointer to array.
  double  *t_end_list;      // Spacecraft clock counter as double (true decimals). Pointer to array.
} data_exclude_times_type;
