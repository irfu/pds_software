//====================================================================================================================
// LAP PDS SOFTWARE
// ----------------
//
// See 00_README.TXT for more information.
//
//
// VERSION: 3.09
// 
// AUTHOR: Reine Gill
// Lines 4603-4604 modified from 2.98 by aie@irfu.se,
// Lines 6661 & 6806-6815 edited by fj@irfu.se
// DATE: 140710
// ...
// CHANGES FROM VERSION 3.07 TO 3.08
//  * Fixed bug that assigns command-line argument specified configuration file paths to the wrong string variables.
//       Erik P G Johansson 2015-02-xx
//  * Changed the name of three PDS keywords to being probe-specific (in addition to four previously renamed PDS keywords).
//       Erik P G Johansson 2015-03-xx
//  * Implemented functionality for ignoring data within specific time intervals (pds.dataexcludetimes).
//       Erik P G Johansson 2015-03-31
//  * Corrected typos in PDS DESCRIPTION fields in LBL files.
//       Erik P G Johansson 2015-03-30
//  * Fixed bug that sometimes terminates the DecodeScience thread prematurely thus omitting the last hour(s) of data.
//       Erik P G Johansson 2015-03-31
//  * Specifically permit the faulty macro 515 to set new vbias2 in every macro loop (not just the first loop) thus
//    permitting vbias2 to change value in every macro loop cycle, as described in the .mds file.
//       Erik P G Johansson 2015-04-10
//  * Modified and simplified code that updates certain PDS keyword values in LBL/CAT files copied from
//    the template directory so that it now updates more PDS keyword values.
//    Thus modified WriteLabelFile and renamed it WriteUpdatedLabelFile.
//       Erik P G Johansson 2015-05-04
//  * Fixed bug that made INDEX.LBL not adjust to the DATA_SET_ID column changing width.
//       Erik P G Johansson 2015-05-12
//
// CHANGES FROM VERSION 3.08 TO 3.09
//  * Fixed bug that made ADC20 always choose high-gain.
//       Erik P G Johansson 2015-06-03
//  * Added code for calibrating ADC20 data relative to ADC16
//    See Chapter 4, "LAP Offset Determination and Calibration", Anders Eriksson 2015-06-02.
//    Also see added constants in "pds.h".
//       Erik P G Johansson 2015-06-10
//  * Fixed bug that multiplied ccalf_ADC16 by 16 for non-truncated ADC20 data, density mode, P1/P2.
//    This led to calibration offsets being multiplied by 16.
//    (Braces surrounding the statements after two if-then were missing.)
//       Erik P G Johansson 2015-08-31
//  * Fixed potential bug that interpreted macro ID strings as decimal numbers rather than hexadecimal numbers.
//    Code still worked since the result was only used to compare with decimal macro ID numbers
//    and the code (seemed to) fail well for hexadecimal string representations.
//       Erik P G Johansson 2015-12-07
//  * Added extra flags for overriding the MISSION_PHASE_NAME, period start date & duration, and description
//    string used in DATA_SET_ID and DATA_SET_NAME.
//       Erik P G Johansson 2015-12-09
//  * Start time & duration can now be specified with higher accuracy than days using CLI parameters.
//  * All logs now display wall time the same way ("3pds_dds_progress.log" was previously different).
//  * Bug fix: pds.modes can now handle colons in the description string.
//  * Raised permitted string length for code interpreting "pds.modes". Can now (probably) handle all rows up
//    to 1024 characters, including CR & LF.
//       Erik P G Johansson 2015-12-17
//
//
// "BUG": INDEX.LBL contains keywords RELEASE_ID and REVISION_ID, and INDEX.TAB and INDEX.LBL contain columns
//        RELEASE_ID and REVISION_ID which should all be omitted for Rosetta.
//     NOTE: Changing this will/might cause backward-compatibility issues with lapdog and write_calib_meas.
// 
// "BUG": Code requires output directory path in pds.conf to end with slash.
// "BUG": The code still does not update the LBL files in the DOCUMENT/ directory.
//
// BUG: Appears to use a single line feed (LF) without carriage return (CR) for one single line when updating
//    CALIB/RPCLAP030101_CALIB_FRQ_D_P1.LBL
//    CALIB/RPCLAP030101_CALIB_FRQ_E_P1.LBL
//    CALIB/RPCLAP030101_CALIB_FRQ_D_P2.LBL
//    CALIB/RPCLAP030101_CALIB_FRQ_E_P2.LBL
//    NOTE: These files are specified in pds.conf.
//    NOTE: This bug has probably been fixed now. /Erik P G Johansson 2015-12-10
//    
// NOTE: Indentation is largely OK, but with some exceptions. Some switch cases use different indentations.
//       This is due to dysfunctional automatic indentation in the Kate editor.
// NOTE: Contains many 0xCC which one can suspect should really be replaced with S_HEAD.
// 
//====================================================================================================================

// *************************************************************
// -= "Emancipate yourself from mental slavery..", Bob Marley =-
// *************************************************************

#include <string.h>       // String handling
#include <ctype.h>        // Character types
#include <sys/stat.h>     // POSIX Standard file charateristics, fstat()..
#include <errno.h>        // Used for error handling, perror()..
#include <sys/types.h>    // POSIX Primitive System Data Types 
#include <dirent.h>       // POSIX Directory operations
#include <sys/wait.h>     // POSIX Wait for process termination
#include <sys/time.h>     // Time definitions, nanosleep()..0
#include <stdio.h>        // Standard Input/output 
#include <stdlib.h>       // Standard General utilities
#include <limits.h>       // Standard limits of integer types
#include <fcntl.h>        // POSIX Standard File Control Operations
#include <fnmatch.h>      // Filename matching types
#include <unistd.h>       // POSIX Standard Symbolic Constants
#include <linux/unistd.h> // POSIX Standard Symbolic Constants
#include <signal.h>       // Standard Signal handling
#include <stdarg.h>       // Handle variable argument list
#include <time.h>         // Standard date and time
#include <pthread.h>      // Linux implementation of POSIX threads
#include <fts.h>          // BSD library for traversing UNIX file hierachies
#include <libgen.h>       // For basename and dirname
#include <sched.h>        // For RT version
#include <stdint.h>       // Standard types
#include "id.h"           // LAP Data ID codes
#include "esatm.h"        // S/C TM Definitions
#include "pds.h"          // PDS & LAP definitions and structures
#include "plnk.h"         // Code for linked lists of property/value pairs
#include "plnkdec.h"      // plnk declarations
#include "cirb.h"         // Code for simple power of two circular buffers 
#include "cirbdec.h"      // cirb declarations
#include "nice.h"         // Sleep definitions etc
#include <math.h>         //floor function



// We skipped using OASWlib and ESA provided time cal. approach it produced weird results.
// We still use time calibration packets though.
//
//     FORTRAN ROUTINES PROVIDED BY ESOC FROM DDS SYSTEM
//extern void dj2000_(double *DAY,int *I,int *J,int *K,int *JHR,int *MI,double *SEC);


void printUserHelpInfo(FILE *stream, char *executable_name);    // Print user help info.

// Thread functions
//----------------------------------------------------------------------------------------------------------------------------------
void *SCDecodeTM(void *);					// Decode S/C TM thread
void *DecodeHK(void *);						// Decode HK data thread
void *DecodeScience(void *);					// Decode Science data thread

// Signal handler
//----------------------------------------------------------------------------------------------------------------------------------
static void ExitWithGrace(int signo);				// Gracefull exit

// Logging and exit functions
//----------------------------------------------------------------------------------------------------------------------------------
void ExitPDS(int status);					// Closes logging and exits with status
int  YPrintf(const char *fmt, ...);				// Prints to pds system log
int  DPrintf(const char *fmt, ...);				// Prints to DDS packet filter log 
int  PPrintf(const char *fmt, ...);				// Prints to S/C packet filter log
int  CPrintf(const char *fmt, ...);				// Prints to Science decoding log
int  HPrintf(const char *fmt, ...);				// Prints to HK decoding log 
int  OpenLogFile(FILE **pfd,char *name,FILE *fderr);		// Opens log file with name "name"
int  AddPathsToSystemLog(pds_type *pds);			// Adds paths to system log

// Program option functions
//----------------------------------------------------------------------------------------------------------------------------------
int  GetOption(char *opt,int argc, char *argv[],char *arg);        // Get an input option
// Pointer to file desc pointer pfd is needed and an error stream
// Functions to load and test external information 

int HasMoreArguments(int argc, char *argv[]);    // Return true if-and-only-if argv[i] contains non-null components for i >= 1.

//----------------------------------------------------------------------------------------------------------------------------------
int  LoadConfig1(pds_type *p);                          // Loads configuration information first part
int  LoadConfig2(pds_type *p,char *data_set_id);        // Loads configuration information second part
int  LoadAnomalies(prp_type *p,char *path);             // Load anomaly file
int  LoadModeDesc(prp_type *p,char *path);              // Load human description of macro modes into a linked list of properties.
int  LoadBias(unsigned int ***bias_s,unsigned int ***mode_s,int *bias_cnt_s,int *mode_cnt,char *path);		// Load bias settings file
int  LoadExclude(unsigned int **exclude,char *path);                                   // Load exclude file
int  LoadDataExcludeTimes(data_exclude_times_type **dataExcludeTimes, char *depath);   // Load data exclude times file.
int  DecideWhetherToExcludeData(data_exclude_times_type *dataExcludeTimes, prp_type *file_properties, int *excludeData);
int  LoadTimeCorr(pds_type *pds,tc_type *tcp);                                  // Load time correlation packets
int  LoadMacroDesc(prp_type macs[][MAX_MACROS_INBL],char *);                    // Loads all macro descriptions
int  GetMCFiles(char *rpath,char *fpath,m_type *m);                             // Get measured data calibration files 
int  InitMissionPhaseStructFromMissionCalendar(mp_type *mp,pds_type *pds);      // Given a path, data set version and mission abbreviation (in mp) 

// Derive DATA_SET_ID and DATA_SET_NAME keyword values, INCLUDING QUOTES!
void DeriveDSIandDSN(
    char* DATA_SET_ID, char* DATA_SET_NAME,
    char* targetID, int DPLNumber, char* mpAbbreviation, char* descr, float dataSetVersion, char* targetName_dsn);

// returns updated mission phase structure.
void TestDumpMacs();						// Test dump of macro descriptions

// Label and Table functions
//----------------------------------------------------------------------------------------------------------------------------------
int WriteUpdatedLabelFile(prp_type *pds,char *name);		// Write label file
int ReadLabelFile(prp_type *pds,char *name);			// Read a label file 
int ReadTableFile(prp_type *lbl_data,c_type *cal,char *path);	// Read table file

char getBiasMode(curr_type *curr, int dop);

// Write data to data product table file .tab
int WritePTAB_File(unsigned char *buff,char *fname,int data_type,int samples,int id_code,int length,sweep_type *sw_info,curr_type *curr,int param_type,int dsa16_p1,int dsa16_p2,int dop,m_type *m_conv,unsigned int **bias,int nbias,unsigned int **mode,int nmode,int ini_samples,int samp_plateau);

// Write to data product label file .lbl
int WritePLBL_File(char *path,char *fname,curr_type *curr,int samples,int id_code,int dop,int ini_samples,int param_type);

// Buffer and TM functions
//----------------------------------------------------------------------------------------------------------------------------------
void FreeBuffs(buffer_struct_type *b0,buffer_struct_type *b1,buffer_struct_type *b2,buffer_struct_type *b3); // Free buffer memory
int  GetBuffer(buffer_struct_type *cs,unsigned char *buff,int len);			// Get data from circular buffer
int  LookBuffer(buffer_struct_type *bs,unsigned char *buff,int len);			// Look ahead in circular buffer
int  GetHKPacket(buffer_struct_type *,unsigned char *,double *);			// Get one packet of HK data
void DumpTMPacket(buffer_struct_type *cs,unsigned char packet_id);			// Dump the non interesting SC TM packets
int  SyncAhead(buffer_struct_type *cb,int len);						// Test data syncronisation ahead..

// Functions handling/working with linked lists of property/value pairs
//----------------------------------------------------------------------------------------------------------------------------------
int  ClearCommonPDS(prp_type *p);							// Clear common PDS parameters
int  ClearDictPDS(prp_type *p);								// Clear dictionary PDS LAP parameters
int  SetupHK(prp_type *p);								// Setup HK label
int  SetupIndex(prp_type *p);								// Setup index label parameters
void WriteIndexLBL(prp_type *p,mp_type *m);						// Write index label file
void WriteToIndexTAB(char* relative_LBL_file_path, char* product_ID, char* prod_creation_time); // Write one line in index label file.
int  TotAQPs(prp_type *p,int n);							// Return tot num of aqps since start for sequence n
int  FindIDCode(prp_type *p,int n);							// Find ID code in macro overriding ID code in data 
// if anomaly correction is applicable

// Program state handler functions
//----------------------------------------------------------------------------------------------------------------------------------
void DispState(int,char *);								// Display state changes for debugging

// String and alpha numeric handling functions
//----------------------------------------------------------------------------------------------------------------------------------
int  Separate(char *,char *,char *,char,int);                                   // Separate into left & right token out of many.
int  SeparateOnce(char* str, char* strLeft, char* strRight, char separator);    // Separate into left & right tokens.
int  TrimWN(char *);									// Trims initial and trailing whitespace and all newlines away
int  TrimQN(char *);									// Trims initial and trailing quotes and all newlines away
int  ExtendStr(char *dest,char *src,int elen,char ch);					// Make a new string dest using src extended with ch to elen length. 
void ReplCh(char *str,char ch1,char ch2);						// Replace all characters ch1 in str with characters ch2
int  IncAlphaNum(char *n);								// Increments the alphanumeric number stored in string n 
// returns <0 on error
int  GetAlphaNum(char *n,char *path,char *pattern);					// Get largest alphanumeric value stored in file names matching 
// a specific pattern
// In directory path
int  Alpha2Num(char *n);								// Convert a positive alpha numeric value to a number 
// returns negative value on error
int  IsNumber(char *str);								// Check if string is a number


// File handling functions
//----------------------------------------------------------------------------------------------------------------------------------
int  FileLen(FILE *);									// Get length of file
int  FileStatus(FILE *fd,struct stat *sp);						// Return FileStatus
int  SetupPath(char *error_txt,char *path);						// Resolves,tests and returns a usable path 
int  TestDir(char *name);								// Test if directory exists
int  TestFile(char *name);								// Test if file exists and if we can read it.
int  MakeDir(char *,char *,char *);							// Make a directory for current YYMMDDD, if it's not already there!
int  StrucDir(char *,char *,char *);							// Test and create directory structure for data
void DumpDir(char *path);								// Dump a directory..mostly for debugging
int  GetUnacceptedFName(char *name);							// Get new filename for manual unaccepted file
void FTSDump(FTSENT *fe);								// Dump FTSENT structure for debbuging
int  Match(char *,char *);								// Match filename to pattern

// HK Functions
//----------------------------------------------------------------------------------------------------------------------------------
void AssembleHKLine(unsigned char *,char *,double,char *);				// Assemble a HK line entry

// Low level data functions
//----------------------------------------------------------------------------------------------------------------------------------
void         SignExt20(int *);								// Sign extend 20 bit to 32 bit
double       GetDBigE(unsigned char *buff);						// Get bigendian double assuming we are on a little endian machine
unsigned int GetBitF(unsigned int word,int nb,int sb);					// Returns nb number of bits starting at bit sb

// Time related functions
//----------------------------------------------------------------------------------------------------------------------------------
unsigned int E2Epoch(char *rtime);							// Get seconds from 1970 epoch to epoch "epoch"
double DecodeSCTime(unsigned char *buff);						// Decoding S/C time, returns raw S/C time in seconds as a double
double DecodeLAPTime(unsigned char *buff);						// Decoding lap time, returns raw S/C time in seconds as a double
int DecodeRawTimeEst(double raw,char *stime);						// Decodes raw S/C time (estimates UTC no calibration) and returns 
// a PDS compliant time string
int DecodeRawTime(double raw,char *stime,int lfrac);					// Decodes raw S/C time (calibration included) 
// also returns a PDS compliant time string, lfrac is long or 
// short fractions of seconds.
// This function calls Scet2Date_2.

int Raw2OBT_Str(double raw,int rcount,char *stime);					// Convert raw time to an OBT string (On Board Time)
int OBT_Str2Raw(char *stime, int *resetCounter, double *rawTime);                       // Convert OBT string to raw time.

//int Scet2Date(double raw,char *stime,int lfrac);     // Decodes SCET (Spacecraft event time, Calibrated OBT) into a date
// lfrac is long or short fractions of seconds.

// Replacement for Scet2Date ESA approach with OASWlib give dubious results. (dj2000 returns 60s instead of 59s etc.)
int Scet2Date_2(double raw,char *stime,int lfrac);					// Decodes SCET (Spacecraft event time, Calibrated OBT) into a date
// lfrac is long or short fractions of seconds.

int TimeOfDatePDS(char *sdate,time_t *t);						// Returns UTC time in seconds (since 1970...) for a PDS date string 
// NOTE: This is not the inverse of Scet2Date!

int GetUTime(char *);									// Returns current UTC date and time as string CCYY-MM-DDThh:mm:ss

// LAP Logarithmic decompression functions
//----------------------------------------------------------------------------------------------------------------------------------
void DoILogTable(unsigned int *);
int  LogDeComp(unsigned char *buff,int ilen,unsigned int *ilog);
int  HighestBit(unsigned int value);


// DDS Archive functions (input archive)
//----------------------------------------------------------------------------------------------------------------------------------
void   TraverseDDSArchive(pds_type *p);							// Traverse DDS archive path
int    Compare(const FTSENT **af, const FTSENT **bf);					// Used for traversal of DDS archives using fts functions
// also used in DumpDir() ( Can be seen as adding a metric to
// a mathematical file space :) )

void   ProcessDDSFile(unsigned char * ibuff,int len,struct stat *sp,FTSENT *fe); // Process DDS file
int    DDSFileDuration(char *str);							// Returns DDS file duration in seconds, computed on filename!
time_t DDSFileStartTime(FTSENT *f);							// Returns the start time of entries in a DDS archive for sorting purposes 
double DDSTime(unsigned char *ibuff);							// Return DDS packet time
int    DDSVirtualCh(unsigned char *ibuff);						// Returns DDS packet virtual channel
void   DDSGroundSN(unsigned short int gsid,char *str);					// Get ground station name

// Dynamic allocation functions
//----------------------------------------------------------------------------------------------------------------------------------
void *CallocArray(int n,int s);                                                         // Allocate array of n entries each n bytes in size and clear
double **CallocDoubleMatrix (int rows, int cols);                                       // Dynamically allocate two dimensional array of doubles
unsigned int **CallocIntMatrix (int rows, int cols);                                    // Dynamically allocate two dimensional array of ints
void FreeIntMatrix(unsigned int **C, int rows, int cols);                               // Free two dim. array of integers
void FreeDoubleMatrix(double ** C, int rows, int cols);                                 // Free two dim. array of doubles

// RT Version functions
//----------------------------------------------------------------------------------------------------------------------------------
// Set thread priority and scheduling policy, for RT version of PDS (not needed anymore, keep anyway)
int SetPRandSched(pthread_t thread,int priority,int policy); 


//-=SOME GLOBAL VARIABLES AND STRUCTURES=- 
//----------------------------------------------------------------------------------------------------------------------------------

static volatile sig_atomic_t exit_gracefully=0; // Used to cleanly exit then Ctrl-C is pressed


unsigned int sec_epoch;    // Seconds from 1970 epoch to epoch "epoch"
int debug=0;               // Turn on/off debugging

int macro_priority=0;      // Priority of macros (Trust more or less than info in data). 0=Trust data, 1=Trust macro info

int calib=0;               // Indicate if we are doing a calibrated (1) or edited (0) archive.

extern char IDList[][33];  // ID array, string with short name of ID code. Defined in "id.c".

pthread_t sctmthread = 0;  // S/C TM thread
pthread_t scithread  = 0;  // Science thread
pthread_t hkthread   = 0;  // HK thread

prp_type comm;            // Linked property/value list for PDS Common Parameters
prp_type dict;            // Linked property/value list for PDS LAP Dictionary
prp_type hkl;             // Linked property/value list for PDS LAP HK
prp_type anom;            // Linked property/value list for Anomalies
prp_type ind;             // Linked property/value list for PDS INDEX Label
prp_type mdesc;           // Linked property/value list for Macro Descriptions
prp_type cc_lbl;          // Linked property/value list for Coarse Bias Voltage Calibration Data Label
prp_type ic_lbl;          // Linked property/value list for Bias Current Calibration Data Label
prp_type fc_lbl;          // Linked property/value list for Fine Bias Voltage Calibration Data Label
prp_type tmp_lbl;         // Linked property/value list for Temporary use
prp_type cat;             // Linked property/value list for Catalog Files 


hk_info_type hk_info;     // Some HK info needed to dump last HK label at exit

// Matrix of linked property/value lists containing macros
prp_type macros[MAX_MACRO_BLCKS][MAX_MACROS_INBL]; 

mp_type mp;     // Mission phase data

unsigned int volume_id;


// Initialize PDS configuration info, paths and file descriptors
pds_type pds =
{
    0,          // Number of times the spacecraft clock has been reset
    "",         // Date and time of last spacecraft clock reset example: 2003-01-01T00:00:00
    "",         // Path to LAP template PDS archive
    "",         // Path to LAP macro descriptions
    0,          // DPL Number
    0.0,        // Data Set Version
    "",         // Label revision note
    "",         // Release date       
    "",         // Path to configuration file	    
    "",         // Path to anomaly file		    
    "",         // Path to bias settings file	    
    "",         // Path to (CALIB macro) exclude file
    "",         // Path to data exclude file    // Erik P G Johansson 2015-03-25: Added
    "",         // Path to macro description file	    
    "",         // Mission calendar path and file name
    "",         // Archive path PDS (Out data)
    "",         // Archive path DDS (In data)
    "",         // Path to time correlation packets
    "",         // Log path
    "",         // Data path PDS science edited		
    "",         // Data path PDS science calibrated		
    "",         // Root path to calibration data.		
    "",         // Path to fine bias calibration data	
    "",         // Path to coarse bias calibration data	
    "",         // Path to current bias calibration data	
    "",         // Path to offset calibration data		
    "",         // Path to density frequency response probe 1
    "",         // Path to density frequency response probe 2
    "",         // Path to e-field frequency response probe 1
    "",         // Path to e-field frequency response probe 2
    "",         // Data subdirectory path for PDS science   
    "",         // Data path PDS HK			       
    "",         // Data subdirectory path for PDS HK	       
    "",         // Path to data that has not been accepted	       
    "",         // Index table file path.
    NULL,       // Log file descriptor LAP PDS System log   
    NULL,       // S/C packet filtering log
    NULL,       // Log file descriptor Science Decoding log 
    NULL,       // Log file descriptor HK Decoding log
    NULL,       // Log file descriptor dds packet filter log
    NULL,       // File descriptor to recoverfile
    NULL,       // Science archive PDS data file descriptor	
    NULL,       // Science data table file descriptor
    NULL,       // HK archive PDS data file descriptor
    NULL,       // HK data table file descriptor
    NULL,       // Index label file descriptor
    NULL,       // Index table file descriptor
    NULL,       // DDS Read file descriptor
    NULL,       // DDS progress file descriptor
};


// Circular Buffers
//--------------------------------------------------------------------------
// Originally intended to support realtime stream processing.
// Now they are usefull to spread processing load on multicore machines.
//
buffer_struct_type cbtm;        // Circular S/C TM buffer
buffer_struct_type cbs;         // Circular in Science buffer
buffer_struct_type cbh;         // Circular in HK buffer
buffer_struct_type cmb;         // Circular mirror buffer

c_type v_conv;                  // Coarse voltage conversion to TM data structure.
c_type f_conv;                  // Fine voltage bias conversion to TM data structure.
c_type i_conv;                  // Current conversion to TM data structure.
m_type m_conv;                  // Calibration of measured data offset and conversion to volts/ampere

tc_type tcp={0,NULL,NULL,NULL}; // Time correlation packets structure

unsigned int ilogtab[256];      // Inverse logarithmic table for decoding of logarithmic sweeps

extern FILE *stdout;            // Keep track of standard output 

unsigned int macro_id = 0;      // Macro ID tag.

int sc_thread_cancel_threshold_timeout;
    
// Added mutex protector for logging functions
static pthread_mutex_t protect_log=PTHREAD_MUTEX_INITIALIZER; 



// -=MAIN FUNCTION=-
//----------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    struct sigaction act;
    int status;           // Just a temporary status/error code variable
    char tstr1[1024];     // Temporary string
    char tstr2[1024];     // Temporary string
    char tstr3[1024];     // Temporary string
    
    arg_type scarg;       // Argument structure S/C TM thread
    arg_type sarg;        // Argument structure Science thread
    arg_type harg;        // Argument structure HK thread
    
    mp.start=0;           // Mission start time, small dummy
    mp.stop=INT32_MAX;    // Mission stop time, big dummy ~68 years
    
    
    if(GetOption("-h",argc,argv,NULL))
    {
        printUserHelpInfo(stdout, argv[0]);
        exit(0);
    }
    
    // Basic check on number of input arguments.
    if (argc>23 || argc<7) 
    {
        fprintf(stderr, "Called with too few or too many parameters.\n\n");
        printUserHelpInfo(stderr, argv[0]);    // NOTE: Prints to stderr.
        exit(1);
    }
    
    printf("\n");
    
    
    // Get options, note that we do no syntactic checks of the input!
    // We assume it is correct, things like: pds -c  or pds -c pds.conf -debug
    // Will not work, since required parameter values -mp xx -vid yy -dsv zz are missing!!
    // ----------------------------------------------------------------------------------------------------
    
    //========================================
    // Get path option for configuration file
    //========================================
    if(!GetOption("-c",argc,argv,pds.cpath)) {
        sprintf(pds.cpath,"%s/%s",getenv("HOME"),"pds.conf");
    }
    
    //========================================
    // Get path option for anomaly file
    //========================================
    if(!GetOption("-a",argc,argv,pds.apath)) {
        sprintf(pds.apath,"%s/%s",getenv("HOME"),"pds.anomalies");
    }
    
    //========================================
    // Get path option for bias file
    //========================================
    if(!GetOption("-b",argc,argv,pds.bpath)) {
        sprintf(pds.bpath,"%s/%s",getenv("HOME"),"pds.bias");
    }
    
    //========================================
    // Get path option for exclude file
    //========================================
    if(!GetOption("-e",argc,argv,pds.epath)) {   // Erik P G Johansson 2015-03-24: Fixed bug. Store path in correct variable.
        sprintf(pds.epath,"%s/%s",getenv("HOME"),"pds.exclude");
    }
    
    //===========================================
    // Get path option for mode description file
    //===========================================
    if(!GetOption("-m",argc,argv,pds.mpath)) {    // Erik P G Johansson 2015-03-24: Fixed bug. Store path in correct variable.
        sprintf(pds.mpath,"%s/%s",getenv("HOME"),"pds.modes");
    }
    
    //=============================================
    // Get path option for data exclude times file
    //=============================================
    // Erik P G Johansson 2015-03-25: Added
    if(!GetOption("-d",argc,argv,pds.depath)) {
        sprintf(pds.depath,"%s/%s",getenv("HOME"),"pds.dataexcludetimes");
    }
    
    //=============================================
    // Get calibration option
    //=============================================
    if(GetOption("-calib",argc,argv,NULL))
    {
        printf("Producing calibrated archive\n");
        calib=1;
    }
    
    //=============================================
    // Get mission phase option
    //=============================================
    // Only process mission phase abbreviated as second column in mission calendar file.
    if(GetOption("-mp",argc,argv,tstr1))
    {
        strncpy(mp.abbrev,tstr1,4);
        mp.abbrev[4]='\0';
        printf("Processing mission phase with ID %s\n",mp.abbrev);
    }
    else
    {
        fprintf(stderr,"Mandatory mission phase abbreviation argument is missing.\n");
        exit(1);
    }
    
    
    //=============================================
    // Get volume ID option
    //=============================================
    if(GetOption("-vid",argc,argv,tstr1))
    {
        sscanf(tstr1,"%d\n",&volume_id);
        printf("Volume ID: %d\n",volume_id);
        
        if(volume_id==0 || volume_id>999)
        {
            fprintf(stderr,"check -vid volume_id input\n");
            exit(1);
        }
    }
    else
    {
        fprintf(stderr,"Mandatory volume ID argument is missing (Unique for each data set)\n");
        exit(1);
    }
    
    
    //=======================
    // Get data set version
    //=======================
    if(GetOption("-dsv",argc,argv,tstr1))
    {
        sscanf(tstr1,"%f\n",&pds.DataSetVersion);
        printf("Data set version: %2.1f\n",pds.DataSetVersion);
        
        if(pds.DataSetVersion<=0.0 || pds.DataSetVersion>9.9)
        {
            fprintf(stderr,"check -dsv data_set_version input\n");
            exit(1);
        }
    }
    else
    {
        fprintf(stderr,"Mandatory data set version argument is missing (incremental for new versions of a data set)\n");
        exit(1);
    }
    
    
    //==================
    // Get debug option
    //==================
    if(GetOption("-debug",argc,argv,tstr1))
    {
        sscanf(tstr1,"%d",&debug);
        printf("Debug level %d\n",debug);
    }
    printf("\n");
    
    
    
    ProtectPlnkInit();
    
    // Loads first part of configuration information into the PDS structure
    if((status=LoadConfig1(&pds))<0) 
    {
        fprintf(stderr,"Mangled configuration file (part 1): %d\n",status);
        exit(1);
    }
    
    if(calib) {
        pds.DPLNumber=3;			// Calibrated data has DPL number 3
    } else {
        pds.DPLNumber=2;			// Edited data has DPL number 2
    }
    
    sec_epoch=E2Epoch(pds.SCResetClock);	// Compute seconds from epoch 1970 to S/C Clock reset. 
    
    // Get mission phase data 
    if(InitMissionPhaseStructFromMissionCalendar(&mp,&pds) < 0) {
        exit(1);
    }
    
    
    /*===================================================
     * Get options for altering mission phase parameters
     *================================================================================================
     * Overwrite mission phase values if values can be found among (optional) command-line arguments.
     * NOTE: Current implementation requires all or none of these extra options.
     * NOTE: These options have to be read AFTER reading and interpreting the mission calendar
     *       so that those values are available if they are not overwritten here.
     *================================================================================================*/
    if (GetOption("-ds", argc, argv, tstr1))
    {
        DeriveDSIandDSN(
            mp.data_set_id, mp.data_set_name,
            mp.target_id, pds.DPLNumber, mp.abbrev, tstr1, pds.DataSetVersion, mp.target_name_dsn);        
        
        
        if (GetOption("-mpn", argc, argv, tstr1)) {
            sprintf(mp.phase_name, "\"%s\"", tstr1);    // NOTE: Surround with quotes since the archiving standard requires it.
        } else {
            fprintf(stderr, "Can not find option -mpn.\n");
            exit(1);
        }
        
        
        if (GetOption("-ps", argc, argv, tstr1)) {
            if((status=TimeOfDatePDS(tstr1,&(mp.start))) < 0) {
                fprintf(stderr, "Can not convert argument \"%s\" to a time: error code %i\n", tstr1, status);
                exit(1);
            }
        } else {
            fprintf(stderr, "Can not find option -ps.\n");
            exit(1);
        }
        
        
        if (GetOption("-pd", argc, argv, tstr1))
        {
            float dur;
            if (!sscanf(tstr1, "%e", &dur)) {
                fprintf(stderr, "Can not interpret argument \"%s\".\n", tstr1);    // NOTE: Periods shorter than one day are useful for debugging. Therefore permit decimal numbers.
                exit(1);
            }
            mp.stop = mp.start + dur*24*3600;  // Compute end time. NOTE: Does not take leap seconds (e.g. 2015-06-30, 23:59.60) into account.            
        }
        else
        {
            fprintf(stderr, "Can not find option -pd.\n");
            exit(1);
        }
    }

    YPrintf("DATA_SET_ID                 : %s\n",mp.data_set_id);
    printf("DATA_SET_ID                 : %s\n",mp.data_set_id);
    
    // Create unquoted data set ID
    strcpy(tstr1,mp.data_set_id);		// Make temporary copy
    TrimQN(tstr1);			// Remove quotes in temporary copy
    
    // Loads second part of configuration information into the PDS structure and opens some log/status files
    if((status=LoadConfig2(&pds,tstr1))<0) 
    {
        fprintf(stderr,"Mangled configuration file (part 2): %d\n",status); // Check arguments
        exit(1);
    }
    
    if(LoadTimeCorr(&pds,&tcp)<0)         // Load the time correlation packets, once and for all!
    {
        fprintf(stderr,"Warning: No time correlation packets found.\n"); 
        fprintf(stderr,"All UTC times from this point are estimates.\n");
    }
    
    // Open VOLDESC.CAT and change some keywords
    //-----------------------------------------------------------------------------------------------------------
    sprintf(tstr1,"%sVOLDESC.CAT",pds.apathpds);          // Get full path
    status=ReadLabelFile(&cat,tstr1);                     // Read catalog keywords into property value pair list
    
    sprintf(tstr2,"ROLAP_1%03d",volume_id);
    SetP(&cat,"VOLUME_ID",tstr2,1);                       // Set VOLUME_ID
    
    
    // Create unquoted mission phase name
    strcpy(tstr3,mp.phase_name); // Make temporary copy
    TrimQN(tstr3);               // Remove quotes in temporary copy
    
    // Set the VOLUME_NAME
    if(calib) {
        sprintf(tstr2,"\"RPCLAP CALIBRATED DATA FOR %s\"",tstr3);
    } else {
        sprintf(tstr2,"\"RPCLAP EDITED RAW DATA FOR %s\"",tstr3);
    }
    
    SetP(&cat,"VOLUME_NAME",tstr2,1); // Set VOLUME_NAME
    
    WriteUpdatedLabelFile(&cat,tstr1);                           // Write back label file with new info
    FreePrp(&cat);                                        // Free property value list
    
    sc_thread_cancel_threshold_timeout = SC_THREAD_CANCEL_THRESHOLD_TIMEOUT_DEFAULT;
    if (GetOption("-stctt", argc, argv, tstr1)) {
        // This flag is good to have while testing on small data sets.
        // Temporarily Reducing the value can speed up the execution.
        
        // NOTE: The error test does not catch very much.
        if ((status=sscanf(tstr1, "%d", &sc_thread_cancel_threshold_timeout))<=0) {
            fprintf(stderr, "Can not interpret argument \"%s\"\n", tstr1);
            exit(1);
        }
        YPrintf("Using %d as \"Science thread cancel timeout\" value instead of the default (%d).\n", sc_thread_cancel_threshold_timeout, SC_THREAD_CANCEL_THRESHOLD_TIMEOUT_DEFAULT);
    }
    
    //========================================================================================
    // CHECK ASSERTION that there are no "unused" arguments left.
    // This implicitly checks for misspelled options/flags and options/flags occurring twice.
    //========================================================================================
    if (HasMoreArguments(argc, argv))
    {
        // Extra newline since preceeding log messages (not stderr) make it difficult to visually spot the error message.
        fprintf(stderr, "\nCould not interpret all command-line options, or some command-line options occurred multiple times.\n\n");
        //printUserHelpInfo(stderr, argv[0]);    // NOTE: Prints to stderr.
        exit(1);
    }



    // Open DATASET.CAT and change some keywords
    //-----------------------------------------------------------------------------------------------------------
    sprintf(tstr1,"%sCATALOG/DATASET.CAT",pds.apathpds);  // Get full path
    status=ReadLabelFile(&cat,tstr1);                     // Read catalog keywords into property value pair list
    
    Scet2Date_2((double)mp.start,tstr2,0);                // Decode raw time into PDS compliant UTC time
    YPrintf("Mission phase start         : %s\n",tstr2);
    printf("Mission phase start         : %s\n",tstr2);
    SetP(&cat,"START_TIME",tstr2,1);                      // Set START_TIME
    
    Scet2Date_2((double)mp.stop,tstr2,0);                 // Decode raw time into PDS compliant UTC time
    YPrintf("Mission phase stop          : %s\n\n",tstr2);
    printf("Mission phase stop          : %s\n\n",tstr2);
    SetP(&cat,"STOP_TIME",tstr2,1);                       // Set STOP_TIME
    
    SetP(&cat,"DATA_SET_RELEASE_DATE",pds.ReleaseDate,1); // Set DATA_SET_RELEASE_DATE
    
    WriteUpdatedLabelFile(&cat,tstr1);                           // Write back label file with new info
    FreePrp(&cat);                                        // Free property value list
    
    // Write initial message to system log
    YPrintf("LAP PDS SYSTEM STARTED     \n");
    YPrintf("========================================================================\n");
    
    if(calib) {
        YPrintf("Generating calibrated PDS data archive\n");
    }
    
    AddPathsToSystemLog(&pds); // Add paths to system log file
    
    // Update keywords in the calibration files and get calibration data.
    //---------------------------------------------------------------------------------------------------------------------------------
    // Calibration files are supposed to reside in the CALIB directory, both for EDITED and CALIBRATED archives
    // however for EDITED archives the files are not used.
    
    // pds->cpathd   Path to calib data
    // pds->cpathf   Path to fine bias calibration data
    // pds->cpathc   Path to coarse bias calibration data
    // pds->cpathi   Path to current bias calibration data
    // pds->cpathm   Path to offset calibration data
    
    InitP(&cc_lbl);                                      // Initialize property value pair list
    status=ReadLabelFile(&cc_lbl,pds.cpathc);            // Read coarse bias voltage calibration label into property value pair list
    if(status>=0) {
        status+=ReadTableFile(&cc_lbl,&v_conv,pds.cpathd); // Read coarse bias voltage calibration data into v_conv structure
    }
    
    WriteUpdatedLabelFile(&cc_lbl,pds.cpathc);                  // Write back label file with new info
    
    InitP(&fc_lbl);                                      // Initialize property value pair list
    
    // Read fine bias voltage calibration label into property value pair list
    status=+ReadLabelFile(&fc_lbl,pds.cpathf);           
    
    // Read fine bias voltage calibration data into f_conv structure
    if(status>=0) {
        status+=ReadTableFile(&fc_lbl,&f_conv,pds.cpathd); 
    }
    
    WriteUpdatedLabelFile(&fc_lbl,pds.cpathf);                  // Write back label file with new info
    
    InitP(&ic_lbl);                                      // Initialize property value pair list
    
    // Read current bias voltage calibration label into property value pair list
    status=+ReadLabelFile(&ic_lbl,pds.cpathi);           
    
    // Read current bias calibration data into i_conv structure
    if(status>=0) {
        status+=ReadTableFile(&ic_lbl,&i_conv,pds.cpathd);  
    }
    
    WriteUpdatedLabelFile(&ic_lbl,pds.cpathi);                  // Write back label file with new info
    
    InitP(&tmp_lbl);                                     // Initialize property value pair list
    status=+ReadLabelFile(&tmp_lbl,pds.cpathdfp1);       // Read density frequency response calibration file probe 1
    WriteUpdatedLabelFile(&tmp_lbl,pds.cpathdfp1);              // Write back label file with new info
    
    FreePrp(&tmp_lbl);                                   // Initialize property value pair list
    status=+ReadLabelFile(&tmp_lbl,pds.cpathdfp2);       // Read density frequency response calibration file probe 2
    WriteUpdatedLabelFile(&tmp_lbl,pds.cpathdfp2);              // Write back label file with new info
    
    FreePrp(&tmp_lbl);                                   // Initialize property value pair list
    status=+ReadLabelFile(&tmp_lbl,pds.cpathefp1);       // Read e-field frequency response calibration file probe 1
    WriteUpdatedLabelFile(&tmp_lbl,pds.cpathefp1);              // Write back label file with new info
    
    FreePrp(&tmp_lbl);                                   // Initialize property value pair list
    status=+ReadLabelFile(&tmp_lbl,pds.cpathefp2);       // Read e-field frequency response calibration file probe 2
    WriteUpdatedLabelFile(&tmp_lbl,pds.cpathefp2);              // Write back label file with new info
    
    status=+GetMCFiles(pds.cpathd,pds.cpathm,&m_conv);   // Get measurement calibration files
    
    if(status<0)
    {
        YPrintf("Can not get or interpret the calibration files. Exiting.\n");
        ExitPDS(1);
    }
    
    //Open INDEX table
    sprintf(tstr1,"%sINDEX.TAB",pds.ipath); 
    if((pds.itable_fd=fopen(tstr1,"a+"))==NULL) // Open index table for appending and reading
    {
        YPrintf("Couldn't open PDS index table file\n");
        ExitPDS(1);
    }
    
    // Open INDEX label
    sprintf(tstr1,"%sINDEX.LBL",pds.ipath);
    if((pds.ilabel_fd=fopen(tstr1,"w"))==NULL) // Open index label for writing, (file remade every time)
    {
        YPrintf("Couldn't open PDS index label file\n");
        ExitPDS(1);
    }
    
    SetupIndex(&ind);
    
    // Load all macro descriptions
    // ----------------------------------------------------------------------------------------------------
    if(!(status=LoadMacroDesc(macros,pds.macrop)))
    {
        YPrintf("Error loading macros\n");
        ExitPDS(1);
    }
    else
        YPrintf("Loaded %d macro descriptions\n",status);
    
    
    // Initialize inverse log table to decode logarithm sweeps
    DoILogTable(ilogtab); 
    
    // Initialize circular input buffers
    // ----------------------------------------------------------------------------------------------------
    YPrintf("Initialize circular buffers\n");
    
    if(InitBuffer(&cbtm,SC_SIZE)<0) 
    {
        YPrintf("Initialization of circular S/C TM buffer failed\n");
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1);
    }
    
    if(InitBuffer(&cbs,SC_SIZE)<0) 
    {
        YPrintf("Initialization of circular Science buffer failed\n");
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1);
    }
    
    // Circular Mirror/Error buffer
    if(InitBuffer(&cmb,MC_SIZE)<0) 
    {
        YPrintf("Initialization of circular Error buffer failed\n");
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1);
    }
    
    if(InitBuffer(&cbh,HK_SIZE)<0) 
    {
        YPrintf("Initialization of circular HK buffer failed\n");
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1);
    }
    
    
    // Setup a gracefull exit if Ctrl-c is pressed! Exits at a convenient starting point!..may take a long time!
    act.sa_handler=ExitWithGrace;
    act.sa_flags=0;
    if((sigemptyset(&act.sa_mask)==-1) || (sigaction(SIGINT,&act,NULL)==-1))
    {
        perror("Failed to set SIGINT handler");
        exit(1);
    }
    
    // For real time version of PDS, (not needed)
    /*
     * minp=sched_get_priority_min(SCHEDULING); // [max - min]>=32 guaranteed..
     * SetPRandSched(pthread_self(),minp+3,SCHEDULING); // Set priority and scheduling of main thread
     */
    
    // Starting data threads
    // ----------------------------------------------------------------------------------------------------
    YPrintf("Starting data threads\n");
    
    scarg.arg1=&cbtm; // Pass circular TM buffer pointer as an argument
    
    if(pthread_create(&sctmthread,NULL,SCDecodeTM,(void *)&scarg)!=0)
    { 
        YPrintf("Error starting tm thread");   
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1); 
    }
    
    // For real time version of PDS, (not needed)
    //SetPRandSched(sctmthread,minp+2,SCHEDULING); // Set priority and scheduling
    
    sarg.arg1=&cbs; // Pass circular science buffer pointer as an argument
    sarg.arg2=&cmb; // Pass circular mirror buffer pointer as an argument
    
    if(pthread_create(&scithread,NULL,DecodeScience,(void *)&sarg)!=0)
    { 
        YPrintf("Error starting science thread");   
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1); 
    }
    
    // For real time version of PDS, (not needed)
    //SetPRandSched(scithread,minp+1,SCHEDULING); // Set priority and scheduling
    
    if(!calib) // If not a calibrated archive then HK exists.
    {
        harg.arg1=&cbh; // Pass circular house keeping buffer pointer as an argument
        if(pthread_create(&hkthread,NULL,DecodeHK,(void *)&harg)!=0)
        { 
            YPrintf("Error starting HK thread");
            FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
            ExitPDS(1); 
        }  
        // For real time version of PDS, (not needed)
        // SetPRandSched(hkthread,minp,SCHEDULING); // Set priority and scheduling
    }
    
    YPrintf("\n");
    YPrintf("DATA FILTERING STARTED          \n");
    YPrintf("--------------------------------\n");
    
    // Go through all DDS archive files, if no new ones can be found we wait 10s and try again.
    TraverseDDSArchive(&pds);
    
    return 0;
}



// executable_name : <String to be displayed as command name>.
void printUserHelpInfo(FILE *stream, char *executable_name) {
    fprintf(stream, "Usage: %s  [-h] [-debug <Level>]\n", executable_name);
    fprintf(stream, "            [-c pds.conf] [-a pds.anomalies] [-b pds.bias] [-e pds.exclude] [-m pds.modes] [-d pds.dataexcludetimes]\n");
    fprintf(stream, "            [-calib] -mp <Mission phase abbreviation> -vid <Volume ID> -dsv <Data set version>\n");        
    fprintf(stream, "\n");
    fprintf(stream, "            [-stctt <seconds>]              Science thread cancel threeshold timeout (STCTT), i.e. the time the program\n");
    fprintf(stream, "                                            waits for the science thread to empty the science buffer to below a certain\n");
    fprintf(stream, "                                            threshold before exiting.\n");
    fprintf(stream, "\n");
    fprintf(stream, "   Alter default values and values in the mission calendar.\n");
    fprintf(stream, "            [-ds <Description string>       The free-form component of DATA_SET_ID, DATA_SET_NAME. E.g. EDITED, CALIB, MTP014.\n");
    
    // Values normally obtained from the mission calendar.
    // NOTE: Start and duration and  MISSION_PHASE_NAME(!) are not necessarily those of an entire mission phase,
    // since deliveries may split up mission phases.
    fprintf(stream, "             -mpn <MISSION_PHASE_NAME>      E.g. \"COMET ESCORT 2\", \"COMET ESCORT 2 MTP014\"\n");
    fprintf(stream, "             -ps <Period starting date>     Specific day or day+time, e.g. \"2015-12-13\", or \"2015-12-17 12:34:56\".\n");
    fprintf(stream, "                                            (Characters between field values are not important, only their absolute positions.)\n");
    fprintf(stream, "             -pd <Period duration>]         Positive decimal number. Unit: days. E.g. \"28\", \"0.0416666\"\n");    
    fprintf(stream, "\n");
    fprintf(stream, "NOTE: The caller should NOT submit parameter values surrounded by quotes (more than what is required by the command shell.\n");
}



// Thread functions
//----------------------------------------------------------------------------------------------------------------------------------

// Decodes S/C TM (RPC TM) thread
void *SCDecodeTM(void *arg)
{
    char tstr[64];
    unsigned int length;           // Length of data
    unsigned char packet_id;       // S/C packet ID, low byte
    double rawt;                   // Raw time
    buffer_struct_type *cs;        // Pointer to circular buffer structure type, for S/C TM decoding
    unsigned char buff[RIDICULUS];  // Temporary in buffer
    
    cs=(buffer_struct_type *)((arg_type *)arg)->arg1; 
    
    int i;
    
    int oldstate;
    int oldtype;
    int status;
    
    
    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate);
    status+= pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&oldtype);
    
    if(status>0) {
        PPrintf("SCDecodeTM thread might not work properly.");
    }
    
    while(1)
    {
        pthread_testcancel();
        
        GetBuffer(cs,buff,1); // Get 1 bytes from circular buffer
        
        if(buff[0]==HBYTE)
        {
            GetBuffer(cs,buff,1); // Get 1 bytes from circular buffer
            
            packet_id=buff[0];    // Get packet ID
            
            switch(packet_id)
            {
                case HK: 
                    
                    if(calib) // No HK in calibration mode
                    {
                        DumpTMPacket(cs,packet_id);
                        break;
                    }
                    
                    GetBuffer(cs,buff,14); // Get 14 bytes from circular buffer ESA S/C TM packet header
                    
                    length=((buff[2]<<8) | buff[3])-9;           // Get data length
                    PPrintf("LAP HK packet found\n");
                    PPrintf("    Packet ID: 0x0d%02x , Data length: %d\n",packet_id,length);
                    if(length!=HK_LENGTH)
                    {
                        PPrintf("    Weird HK length discarding!\n");
                        continue;
                    }
                    rawt=DecodeSCTime(&buff[4]);      // Decode S/C time into raw time
                    DecodeRawTime(rawt,tstr,0);       // Decode raw time to PDS compliant date format
                    PPrintf("    SCET Time: %s OBT Raw Time: %014.3f\n",tstr,rawt);
                    
                    In(&cbh,HBYTE);         // Keep packet ID high byte
                    In(&cbh,HK);            // Keep packet ID low  byte
                    InB(&cbh,buff,14);      // Store full HK S/C header, because we want to keep the time code
                    
                    GetBuffer(cs,buff,length); // Get length bytes from circular buffer 
                    InB(&cbh,buff,length);     // Store data in cicular HK input buffer
                    
                    PPrintf("    HK FIFO: %4.1f%\n",100.0*cbh.fill/cbh.max); 
                    
                    while(FullBuffer(&cbh,0.85))   // If the circular HK buffer is filled to more than 85 % then yield until it's less
                    {
                        pthread_testcancel();
                        sched_yield();
                    }
                    
                    for(i=0;i<FINITE_YIELDS;i++)
                    {
                        if(FullBuffer(&cbh,0.70))  // If the circular HK buffer is filled to more than 70% then yield, test finite times
                        {
                            pthread_testcancel();
                            sched_yield();
                        }
                        else
                            break;
                    }
                    
                    break;
                    // Discard theese packets
                case ACK:   // LAP ACK
                    PPrintf("LAP ACK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;  
                case EVENT: // LAP EVENT
                    PPrintf("LAP EVENT packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case DUMP:  // LAP DUMP
                    PPrintf("LAP DUMP packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break; 
                case PIU_ACK:
                    PPrintf("PIU ACK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case PIU_HK:
                    PPrintf("PIU HK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case PIU_EVENT:
                    PPrintf("PIU EVENT packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case PIU_DUMP:
                    PPrintf("PIU DUMP packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case PIU_SCIENCE:
                    PPrintf("PIU SCIENCE packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case IES_ACK:
                    PPrintf("IES ACK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case IES_HK:
                    PPrintf("IES HK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case IES_EVENT:
                    PPrintf("IES EVENT packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case IES_DUMP:
                    PPrintf("IES DUMP packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case IES_SCIENCE:
                    PPrintf("IES SCIENCE packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case ICA_ACK:
                    PPrintf("ICA ACK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case ICA_HK:
                    PPrintf("ICA HK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case ICA_EVENT:
                    PPrintf("ICA EVENT packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case ICA_DUMP:
                    PPrintf("ICA DUMP packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case ICA_SCIENCE:
                    PPrintf("ICA SCIENCE packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case MIP_ACK:
                    PPrintf("MIP ACK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case MIP_HK:
                    PPrintf("MIP HK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case MIP_SCIENCE:
                    PPrintf("MIP SCIENCE packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case MAG_ACK:
                    PPrintf("MAG ACK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case MAG_HK:
                    PPrintf("MAG HK packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case MAG_SCIENCE:
                    PPrintf("MAG SCIENCE packet found\n");
                    DumpTMPacket(cs,packet_id);
                    break;
                case SCIENCE:
                    
                    GetBuffer(cs,buff,14); // Get 14 bytes from circular buffer
                    
                    // Store data in the mirror buffer, 
                    // keep S/C packet structure in mirror buffer 
                    In(&cmb,HBYTE);      // Keep packet ID high byte	      
                    In(&cmb,SCIENCE);    // Keep packet ID low  byte
                    InB(&cmb,buff,14);   // Store data in the mirror buffer
                    
                    length=((buff[2]<<8) | buff[3])-9;           // Get data length
                    PPrintf("LAP Science packet found\n");
                    
                    if(length<0) {
                        continue;
                    }
                    
                    rawt=DecodeSCTime(&buff[4]);      // Decode S/C time into raw time
                    DecodeRawTime(rawt,tstr,0);       // Decode raw time to PDS compliant date format
                    PPrintf("    Packet ID: 0x0d%02x , Data length: %d\n",packet_id,length);
                    PPrintf("    SCET Time: %s OBT Raw Time: %014.3f\n",tstr,rawt);
                    GetBuffer(cs,buff,length); 
                    
                    
                    InB(&cbs,buff,length); // Store data in circular science input buffer
                    InB(&cmb,buff,length); // Store data in circular mirror buffer
                    
                    PPrintf("    Science FIFO: %4.1f% Mirror FIFO: %4.1f\n",100.0*cbs.fill/cbs.max,100.0*cmb.fill/cmb.max);
                    
                    while(FullBuffer(&cbs,0.85)) // If the circular Science buffer is filled to more than 85 % then yield until it's less
                    {
                        pthread_testcancel();
                        //nanosleep(&dose,NULL);
                        sched_yield();
                    }
                    for(i=0;i<FINITE_YIELDS;i++)
                    {
                        if(FullBuffer(&cbs,0.70)) // If the circular Science buffer filled to more than 70% then yield test finite times
                        {
                            pthread_testcancel();		    
                            sched_yield();
                        }
                        else
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }   // while
}   // SCDecodeTM







// -= THREAD TO DECODE HK =-
// 
// IMPORTANT NOTE: It appears that HK TAB/LBL file pairs group together up to HK_NUM_LINES subsequent HK packets.
// Exactly which HK packets that are grouped together with which depend depends on when the previous group
// of HK packets ended, and hence it depends on the exact start time of the whole data set. Data
// for the same day from two different data sets with different start times will thus (likely) have different
// HK files with different groups of HK packets.
// /Erik P G Johansson 2016-03-21
void *DecodeHK(void *arg)
{   
    char alphanum_h[4]="000";       // Default starting alpha numeric value for unique HK file name
    unsigned char buff[16];         // Temporary in buffer
    
    char lbl_fname[256];            // Data HK label file name and path (complies to 27.3 file name standard)
    char stub_fname[256];           // Stub file name and path
    char tab_fname[256];            // Data HK table file name and path (complies to 27.3 file name standard)
    
    char inst_mode_id[16]; 
    char prod_creat_time[32];       // Product Creation time
    char line[256];                 // HK line in PDS file
    
    
    char tstr1[256];                // Temporary string
    char tstr2[256];                // Temporary string
    char tstr3[256];                // Temporary string
    
    double time;                    // Raw time
    
    unsigned ti;                    // Temporary integer
    buffer_struct_type *ch;         // Pointer to circular buffer structure type, for HK decoding
    
    int oldstate;
    int oldtype;
    int status;
    
    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate);
    status+= pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&oldtype);
    
    if(status>0) {
        HPrintf("DecodeHK thread might not work properly.");
    }
    
    
    
    InitP(&hkl);                            // Initialize linked property/value list for PDS LAP HK
    ClearCommonPDS(&hkl);                   // Set the common PDS header keywords 
    SetupHK(&hkl);                          // Setup HK keywords
    SetP(&hkl,"RECORD_BYTES",HK_LINE_SIZE_STR,1);       // Set number of bytes in a column of a record
    SetP(&hkl,"DESCRIPTION","\"LAP HK Data, Each line is a separate HK packet sent every 32s\"",1);
    SetP(&hkl,"MISSION_PHASE_NAME",mp.phase_name,1);    // Set mission phase name in HK parameters
    SetP(&hkl,"TARGET_TYPE",mp.target_type,1);          // Set target type in  HK parameters 
    SetP(&hkl,"TARGET_NAME",mp.target_name_did,1);      // Set target name in  HK parameters 
    
    SetP(&hkl,"DATA_SET_ID",mp.data_set_id,1);          // Set DATA SET ID in HK parameters
    SetP(&hkl,"DATA_SET_NAME",mp.data_set_name,1);      // Set DATA SET NAME in HK parameters
    
    if(calib) {// Set product type
        SetP(&hkl,"PRODUCT_TYPE","\"RDR\"",1);
    } else {
        SetP(&hkl,"PRODUCT_TYPE","\"EDR\"",1);
    }
    
    ch=(buffer_struct_type *)((arg_type *)arg)->arg1;   // Get circular house keeping buffer pointer
    
    while(1)
    {
        pthread_testcancel();                             // Test if we are to cancel
        
        //=======================================================================
        // Assemble first line of TAB file, and gather information for LBL file.
        //=======================================================================
        
        // Get first HK packet data and raw time (HK_NUM_LINES per TAB file)
        GetHKPacket(ch,buff,&time);
        
        DecodeRawTime(time,hk_info.utc_time_str,0);			// First convert space craft time to UTC to get time calibration right
        Raw2OBT_Str(time,pds.SCResetCounter,hk_info.obt_time_str);	// Compile OBT string and add reset number of S/C clock
        
        SetP(&hkl,"SPACECRAFT_CLOCK_START_COUNT",hk_info.obt_time_str,1); // Set OBT start time
        SetP(&hkl,"START_TIME",hk_info.utc_time_str,1);                   // Update START_TIME in common PDS parameters
        
        HPrintf("S/C time PDS Format: %s Raw Time: %014.3f\n",hk_info.utc_time_str,time);
        HPrintf("Mission ID: %s Phase: %s\n",mp.abbrev,mp.phase_name);
        
        GetUTime(tstr1);						// Get current UTC time     
        sprintf(tstr2, "\"%s\"", pds.LabelRevNote);               // Assemble label revison note    // Modified 2015-04-10 /Erik P G Johansson
        SetP(&hkl,"LABEL_REVISION_NOTE",tstr2,1);                 // Set LABEL Revision note        // Removed 2015-02-27 /Erik P G Johansson
        
        
        AssembleHKLine(buff,line,time,inst_mode_id);		// Assemble first HK line (HK_NUM_LINES per TAB file)
        SetP(&hkl,"INSTRUMENT_MODE_ID",inst_mode_id,1);		// Set mode ID to macro ID
        
        HPrintf("LINE=%s",line);
        
        strncpy(tstr2,hk_info.utc_time_str,20);			// Truncate raw time and store fractions of a seconds  
        tstr2[19]='\0';						// Add null terminator
        
        // Create data path for current day
        hk_info.utc_time_str[10]='\0';                            // Truncate hh:mm:ss away and keep CCYY-MM-DD
        StrucDir(hk_info.utc_time_str,pds.dpathh,pds.spathh);     // Test if RPCLAPCCYY/MONTH exists for current time if not it create them.
        
        // Replace - in CCYY-MM-DD by null terminations so we can convert date from CCYY-MM-DD into YYMMDD
        tstr2[4]='\0'; 
        tstr2[7]='\0';
        tstr2[10]='\0';
        
        // Get highest alphanumeric value in filenames in dpathh matching pattern "RPCLAPYYMM*_*_H.LBL",
        // This causes the alphanum value to restart at zero every new day.
        // (Any matching days from previous runs are not overwritten until alphanum wraps.)
        //
        sprintf(tstr3,"RPCLAP%s%s*_*_*H.LBL",&tstr2[2],&tstr2[5]);
        GetAlphaNum(alphanum_h,pds.spathh,tstr3); 
        IncAlphaNum(alphanum_h); // Increment alphanumeric value
        
        sprintf(stub_fname,"RPCLAP%s%s%s_%s_H",&tstr2[2],&tstr2[5],&tstr2[8],alphanum_h);
        
        sprintf(tab_fname,"%s.TAB",stub_fname);                     // Compile HK data TAB path+filename
        sprintf(lbl_fname,"%s.LBL",stub_fname);                     // Compile HK data LBL path+filename
        
        SetP(&hkl,"PRODUCT_ID",stub_fname,1);                       // Add PRODUCT_ID HK LBL
        
        GetUTime(prod_creat_time);                                  // Get current UTC time     
        HPrintf("    UTC Creation Time: %s\n",prod_creat_time,1);
        SetP(&hkl,"PRODUCT_CREATION_TIME",prod_creat_time,1);       // Set creation time in common PDS parameters, no quotes!
        
        sprintf(tstr3,"\"%s\"",lbl_fname);          // Add PDS quotes ".."
        SetP(&hkl,"FILE_NAME",tstr3,1);             // Add file name to HK LBL
        sprintf(tstr3,"\"%s\"",tab_fname);          // Add PDS quotes ".." 
        SetP(&hkl,"^TABLE",tstr3,1);                // Add pointer to HK TAB file
        
        // Write to the index file (We have at least one HK line otherwise GetHKPacket would not have returned.
        //----------------------------------------------------------------------------------------------------------------------------------------------------
        ti=strlen(pds.apathpds);                            // Find position there the root of the PDS archive starts
        sprintf(tstr2,"%s%s",&pds.spathh[ti],lbl_fname);    // Set path and file name together
        ExtendStr(tstr3,tstr2,58,' ');                      // Make a new string extended with whitespace to 58 characters
        ExtendStr(tstr2,stub_fname,25,' ');                 // Make a new string extended with whitespace to 25 characters
        WriteToIndexTAB(tstr3, tstr2, prod_creat_time);
        
        // Open label file and tab file for writing
        //----------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy(tstr2,pds.spathh);                       // Copy data path
        strcat(tstr2,lbl_fname);                        // Add lbl filename to data path 
        
        //===============================================================================================
        // Write all lines of TAB file, and gather remaining information for LBL file and write LBL file.
        //===============================================================================================
        if ((pds.hlabel_fd=fopen(tstr2,"w"))==NULL)     // Open HK label file
        {
            HPrintf("Couldn't open PDS HK LBL data file: %s!!\n",tstr2);
        }
        else
        {
            strcpy(tstr2,pds.spathh);               // Copy data path
            strcat(tstr2,tab_fname);                // Add tab filename to data path 
            
            if((pds.htable_fd=fopen(tstr2,"w"))==NULL)          // Open HK table file
            {
                HPrintf("Couldn't open PDS HK TAB data file: %s!!\n",tab_fname);
            }
            else
            {
                // CASE: Successfully opened both LBL and TAB file. First TAB line (variable "line") has already been assembled.
                
                fwrite(line,HK_LINE_SIZE,1,pds.htable_fd);      // Add first HK line from above into the present table file
                
                //===========================================
                // Iterate over remaining lines in TAB file.                
                //===========================================
                // (Start at hk_cnt = 1 to skip counting the line already written above.)
                for (hk_info.hk_cnt=1; hk_info.hk_cnt<HK_NUM_LINES; hk_info.hk_cnt++)
                {
                    GetHKPacket(ch,buff,&time);
                    
                    DecodeRawTime(time,hk_info.utc_time_str,0);  // Decode raw time to PDS compliant date format
                    
                    HPrintf("S/C time PDS Format: %s Raw Time: %014.3f\n",hk_info.utc_time_str,time);
                    AssembleHKLine(buff,line,time,inst_mode_id); // Assemble a HK line     
                    HPrintf("%s",line);
                    fwrite(line,HK_LINE_SIZE,1,pds.htable_fd);   // Add HK line
                    fflush(pds.htable_fd);	
                }
                fclose(pds.htable_fd);
            }
            
            Raw2OBT_Str(time,pds.SCResetCounter,hk_info.obt_time_str);		// Compile OBT string and add reset number of S/C clock
            SetP(&hkl,"SPACECRAFT_CLOCK_STOP_COUNT",hk_info.obt_time_str,1);	// Set OBT stop time
            SetP(&hkl,"STOP_TIME",hk_info.utc_time_str,1);			// Update STOP_TIME in common PDS parameters
            sprintf(tstr1,"%d",hk_info.hk_cnt);
            SetP(&hkl, "FILE_RECORDS", tstr1, 1);               // Set number of records
            SetP(&hkl, "ROWS",         tstr1, 1);               // Set number of rows
            
            // Write to the HK Label file
            //------------------------------------------------------------------------------------------------------------------------------------------------
            FDumpPrp(&hkl,pds.hlabel_fd); // Dump hkl to HK label file
            fprintf(pds.hlabel_fd,"END\r\n");
            fclose(pds.hlabel_fd);
            pds.hlabel_fd=NULL;
            
            //HPrintf("(DecodeHK:) Closed HK LBL file.");   // Useful for determining whether the above code closed the LBL file, or if ExitPDS did.
        }   // if ((pds.hlabel_fd=fopen(tstr2,"w"))==NULL) ... else ...
    }
}   // DecodeHK







// -=THREAD TO DECODE SCIENCE STREAM=-
// Programmed in state machine style.
// 

void *DecodeScience(void *arg)
{ 
    char alphanum_s[4]="000";       // Default starting alpha numeric value for unique sience data file name and product ID
    
    unsigned int state;             // Current state of state machine
    unsigned char buff[RIDICULUS];  // Temporary in buffer using a ridiculously large size...thus whole buffer shall never be needed.
    unsigned char tbuff[8];         // Temporary buffer
    unsigned int main_h_sum=0;      // Main header sum
    unsigned int byte_sum=0;        // Byte sum scanning for header
    
    int in_sync;                    // Indicates if we are in sync or not
    int id_code=0;                  // Temporary ID code variable
    int params=0;                   // Parameter flag, indicates if parameters exists in science stream
    int param_type=0;               // Indicate type of parameters
    unsigned int length=0;          // Length of science data,
    unsigned int hb=0;              // high
    unsigned int lb=0;              // and low byte.
    unsigned int samples  = 0;      // Number of samples in science data (Not same as length!)
    int macro_descr_NOT_found=0;    // Indicates that we have NOT found a matching macro description for the macro ID. 0==Found, 1==Not found (!).
    
    // Measurement sequence.
    // Specifies one of the LAP_SET_SUBHEADER/LAP_TRANSFER_DATA_TO_OUT_FROM in macro description (.mds file).
    // Value is only changed in case S01_GET_MAINH (set to zero) and case S04_GET_ID_CODE (incremented by one)
    unsigned int meas_seq = 0;
    
    int aqps_seq = 0;               // Number of aqps to the start of a sequence
    unsigned int ma=0;              // Macro number in block
    unsigned int mb=0;              // Macro block number
    int dsa16_p1;                   // Down sample p1 adc 16
    int dsa16_p2;                   // Down sample p2 adc 16
    int samp_plateau=0;             // Samples on a plateau
    int ini_samples=0;              // Number of initial plateau samples in a sweep, not the true number due to a well known bug..
    // that we compensate for in this code.
    
    // Structure with current settings for various parameters
    curr_type curr={0,0,0,0,0,0,0,0,0,0x7f,0x7f,0}; 
    
    int          finger_printing=0; // Are we doing fingerprinting ?
    sweep_type   sw_info;           // Sweep info structure steps, step height, duration, ....
    adc20_type   a20_info;          // ADC 20 info structure resampling, moving average, length, ...
    
    unsigned int W0;                // Temporary word 0
    unsigned int W1;                // Temporary word 1
    unsigned int W2;                // Temporary word 2
    
    char tstr1[256];                // Temporary string
    char tstr2[256];                // Temporary string
    char tstr3[256];                // Temporary string 
    //char tstr4[256];                // Temporary string
    char tstr5[256];                // Temporary string 
    
    int i;                          // Temporary counters
    int val,ti1;                    // Temporary integers
    
    char *tp;                       // Temporary string pointer
    char stime[30];                 // S/C time string in PDS format
    char tm_rate='Z';               // Telemetry rate (Z)ero,(M)inimum,(N)ormal and (B)urst
    double rstime=0.0;              // Raw S/C time in seconds
    
    // Status string for 20 Bit ADC:s
    char a20status[16][10]={"EMPTY","P2T","P1T","P1T & P2T","","P2F","","P1T P2F","","","P1F","P1F P2T","","","","P1F & P2F"};
    
    unsigned int **bias=NULL;       // Extra bias settings
    unsigned int **mode=NULL;       // Mode changes from command files
    
    unsigned int *exclude=NULL;     // Exclude macros
    int nbias=0;                    // Number of extra bias settings
    int nmode=0;                    // Number of mode settings
    int nexcl=0;                    // Number of macros to exclude
    data_exclude_times_type *dataExcludeTimes;  // Data on which time intervals for which to exclude data.
    char lbl_fname[32];             // Data Science label file name complies to 27.3 file name standard
    char tab_fname[32];             // Data Science table file name complies to 27.3 file name standard
    
    char prod_id[32];               // Product ID
    
    int  data_type=0;               // Type of data 16 or 20 bit ADC:s
    
    property_type *property1;       // Temporary property pointer
    property_type *property2;       // Temporary property pointer
    
    buffer_struct_type *cb;         // Pointer to circular buffer structure type, for science decoding
    buffer_struct_type *ct;         // Pointer to circular buffer structure type, for temporary science storage
    
    int oldstate;
    int oldtype;
    int status;
    
    
    
    /* Function to remove repetition and shorten the code that writes TABL/LFL file pairs.
     * It represents the writing of one LBL/TAB file pair for one probe (P1,P2,P3).
     * 
     * NOTE: The function uses MANY variables defined in the enclosing outer function
     * (DecodeScience) to avoid a very long and awkward argument list, but none of these are temporary variables.
     * 
     * dop : Defined in analogy with in "WritePTAB_File".
     */
    void WriteTABLBL_FilePair(int dop, unsigned int probeNbr) {
        char tempChar;
        char tstr10[256];
        char indexStr[256];
        
        // Modify filenames and product ID.
        if(getBiasMode(&curr, dop)==E_FIELD) {
            tempChar = 'E';
        } else {
            tempChar = 'D';
        }
        lbl_fname[19]=tempChar;
        tab_fname[19]=tempChar;
        prod_id[19+1]=tempChar;
        
        sprintf(tstr10, "%1d", probeNbr);
        lbl_fname[21]=tstr10[0];
        tab_fname[21]=tstr10[0];
        prod_id[21+1]=tstr10[0];
        
        sprintf(tstr10,"%s%s",&pds.spaths[ti1],lbl_fname); // Put together file name without base path.
        ExtendStr(indexStr,tstr10,58,' ');                    // Make a new string extended with whitespace to 58 characters.
        
        // Name changed
        SetP(&comm,"PRODUCT_ID",prod_id,1);    // Change PRODUCT ID in common PDS parameters.
        sprintf(tstr10,"\"%s\"",lbl_fname);    // Add PDS quotes ".." 
        SetP(&comm,"FILE_NAME",tstr10,1);      // Set filename in common PDS parameters
        sprintf(tstr10,"\"%s\"",tab_fname);    // Add PDS quotes ".." 
        SetP(&comm,"^TABLE",tstr10,1);         // Set link to table in common PDS parameters
        
        if(WritePLBL_File(pds.spaths,lbl_fname,&curr,samples,id_code, dop, ini_samples,param_type)>=0)
        {
            WritePTAB_File(
                buff,tab_fname,data_type,samples,id_code,length,&sw_info,&curr,param_type,dsa16_p1,dsa16_p2, dop,
                &m_conv,bias,nbias,mode,nmode,ini_samples,samp_plateau);
            
            strncpy(tstr10,lbl_fname,29);
            tstr10[25]='\0';   // Remove the file type ".LBL".
            
            WriteToIndexTAB(indexStr, tstr10, property2->value);
        }
    }
    
    
    
    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate);
    status+= pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&oldtype);
    
    if(status>0) {
        YPrintf("Decode Science thread might not work properly.");
    }
    
    
    // Get pointer to circular buffer structures containing indata
    cb=(buffer_struct_type *)((arg_type *)arg)->arg1; 
    ct=(buffer_struct_type *)((arg_type *)arg)->arg2; 
    
    InitP(&comm);  // Initialize linked property/value list for PDS common parameters 
    InitP(&dict);  // Initialize linked property/value list for PDS LAP dictionary
    InitP(&anom);  // Initialize linked property/value list for anomalies
    InitP(&mdesc); // Initialize linked property/value list for macro descriptions
    
    // Load anomalies from anomaly file
    if(LoadAnomalies(&anom,pds.apath)<0)
    {
        YPrintf("Warning: anomaly correction can not be done\n");
        printf("Warning: anomaly correction can not be done\n");
    }
    
    // Load bias settings from bias file
    if((LoadBias(&bias,&mode,&nbias,&nmode,pds.bpath))<0) 
    {
        YPrintf("Warning: Extra bias settings can not be done\n");
        printf("Warning: Extra bias settings can not be done\n");
    }
    
    if(nbias>0 && bias!=NULL && debug>1) {
        for(i=0;i<nbias;i++) {
            printf("Time %d DBIAS %x EBIAS %x\n",bias[i][0],bias[i][1],bias[i][2]);
        }
    }
    
    if(nmode>0 && mode!=NULL && debug>1)
    {
        for(i=0;i<nmode;i++)
        {
            printf("Time %d Mode 0x%02x\n",mode[i][0],mode[i][1]);
        }
    }
    
    // Load Exclude file
    if(calib) {
        if((nexcl=LoadExclude(&exclude,pds.epath))<0) 
        {
            YPrintf("Warning: Calibration macros will not be excluded\n");
            printf("Warning: Calibration macros will not be excluded\n");
        }
    }
    
    // Erik P G Johansson 2015-03-25: Added reading data exclude list file.
    if((LoadDataExcludeTimes(&dataExcludeTimes, pds.depath)))
    {
        YPrintf("Warning: Can not load data exclude times.\n");   // Write to "pds system log".
        printf( "Warning: Can not load data exclude times.\n");
    }
    
    // Load Macro descriptions
    if(LoadModeDesc(&mdesc,pds.mpath)<0)
    {
        YPrintf("Warning: No macro descriptions INST_MODE_DESC cant be set.\n");
        printf("Warning: No macro descriptions INST_MODE_DESC cant be set.\n");
    }
    
    //DumpPrp(&mdesc);
    //####################
    // STATE MACHINE LOOP
    //####################
    in_sync=1;           // Assume we are in sync!
    state=S02_TEST_SYNC; // Set starting state
    while(1)
    { 
        pthread_testcancel();
        switch(state)
        {
            case S01_GET_MAINH: 
                DispState(state,"STATE = S01_GET_MAINH\n");
                
                macro_descr_NOT_found=1;   // Assume initially that no macro description is found
                macro_priority=0;          // Assume we do not trust macro info more than parameters inside the data
                
                GetBuffer(cb,buff,1);      // Read one byte from circular science buffer.
                
                byte_sum+=buff[0];         // Increase byte sum
                
                if(buff[0]==M_HEAD) 
                {
                    if(in_sync) {            // Are we nicely in sync ?
                        main_h_sum=byte_sum; // Set main header sum to byte sum
                    } else {
                        main_h_sum=M_HEAD;   // Nope force main_h_sum to header byte
                    }
                    
                    CPrintf("Found start of LAP science block\n"); 
                    
                    in_sync=1;             // Assume we are in sync
                    byte_sum=0;
                    meas_seq=0;            // Set number of measurement sequences to none
                    state=S02_TEST_SYNC;   // Found header change state
                }
                
                // Do not change value of "state". ==> Try again.
                break;
                
            case S02_TEST_SYNC:
                DispState(state,"STATE = S02_TEST_SYNC\n");
                ClearCommonPDS(&comm);       // Clear common PDS parameters
                ClearDictPDS(&dict);         // Clear dictionary PDS LAP parameters
                UnCheckAll(&macros[mb][ma]); // Uncheck all in current macro description 
                
                if(main_h_sum==M_HEAD) {      // Test if we are in sync
                    state=S03_GET_TIME_CODE;   // In sync pds has been written go get time code
                } else {
                    in_sync=0;               // Indicate not nicely in sync
                    state=S01_GET_MAINH;     // Not in sync! try to get in sync!
                }
                break;
                
            case S03_GET_TIME_CODE:
                DispState(state,"STATE = S03_GET_TIME_CODE\n");
                
                GetBuffer(cb,buff,5);        // Get 5 bytes from circular science buffer
                
                CPrintf("============================================================================\n");
                rstime=DecodeLAPTime(buff);    // Get raw time, decode LAP S/C time in Science data
                DecodeRawTime(rstime,stime,0); // Decode raw time into PDS compliant UTC time
                CPrintf("    SCET time: %s OBT time: %016.6f\n",stime,rstime);
                CPrintf("Mission ID: %s Phase: %s\n",mp.abbrev,mp.phase_name);
                SetP(&comm,"MISSION_PHASE_NAME",mp.phase_name,1);  // Set mission phase name in common PDS parameters
                SetP(&comm,"TARGET_TYPE",mp.target_type,1);        // Set target type in common PDS parameters
                SetP(&comm,"TARGET_NAME",mp.target_name_did,1);    // Set target name in common PDS parameters
                
                if(calib) {
                    SetP(&comm,"PRODUCT_TYPE","\"RDR\"",1);
                } else {
                    SetP(&comm,"PRODUCT_TYPE","\"EDR\"",1);
                }
                
                GetUTime(tstr1);                                   // Get current UTC time     
                CPrintf("    UTC Creation Time: %s\n",tstr1,1);
                SetP(&comm,"PRODUCT_CREATION_TIME",tstr1,1);       // Set creation time in common PDS parameters, no quotes!
                
                TrimWN(tstr1);                                     // Trim whitespace
                //sprintf(tstr2,"\"%s, %s\"",tstr1,pds.LabelRevNote);
                sprintf(tstr2, "\"%s\"", pds.LabelRevNote);        // Modified to not include current time. /Erik P G Johansson 2015-04-10
                SetP(&comm,"LABEL_REVISION_NOTE",tstr2,1);         // Set LABEL Revision note
                
                curr.old_macro=macro_id; // Remember old macro ID at this point
                
                // Do we correct for anomalies ?
                if(FindP(&anom,&property1,stime,1,DNTCARE)>0)
                {
                    //printf("TIME: %s SELECT: %s\n",property1->name,property1->value);
                    macro_priority=1; // Set higher priority on macro description, thus trust it above information in data
                    if(!strcmp(property1->value,"FROM_DATA"))
                    {
                        state=S04_GET_ID_CODE; // Get ID code
                    }
                    else
                    {
                        sscanf(property1->value,"%x",&macro_id); // Set macro ID
                        state=S06_GET_MACRO_DESC;  // Find macro description
                    }
                }
                else
                {
                    state=S04_GET_ID_CODE;
                }
                break;
                
                //##############################################
                // NOTE: BAD INDENTATION, CHANGE IN INDENTATION
                //##############################################                
                case S04_GET_ID_CODE:
                    DispState(state,"STATE = S04_GET_ID_CODE\n");
                    GetBuffer(cb,buff,1); // Get a byte from circular science buffer
                    
                    if(buff[0]==0xCC) // Found Sub Header
                    {
                        GetBuffer(cb,buff,1); // Get a byte from circular science buffer
                        
                        id_code=buff[0];        // Remember ID code
                        
                        params=0;               // Assume no parameters in science stream
                        param_type=NO_PARAMS;   // Default type of parameters, none
                        state=S11_GET_LENGTH;   // Assume next state is this one
                        
                        curr.sensor=SENS_NONE;      // Set current sensor to 0=none. (1=Sens 1, 2=Sens 2, and 3=Sens 1 & 2)
                        curr.transmitter=SENS_NONE; // Set current transmitter to none.
                        CPrintf("----------------------------------------------------------------------------\n");
                        
                        // !macro_descr_NOT_found = macro desc found
                        // At this point:
                        // If anomaly found in pds.anomalies
                        // 1) If macro ID was provided in pds.anomalies it is already set at this point!
                        // 2) if it wasn't it will be determined by:
                        // 2.1) Finding a macro ID tag below and returning here!
                        // 2.2) Finding it at a later stage by fingerprinting and not posssible to return here
                        //
                        // Thus we can not fix ID code problems in data, if we do not directly provide a macro ID
                        // in the anomaly file..this we can easily do!
                        
                        // Macro information has high priority and a macro description has been found!
                        // Thus we want to overide ID code in data!
                        if(macro_priority && !macro_descr_NOT_found) 
                        {
                            if((val=FindIDCode(&macros[mb][ma],meas_seq+1))<0)
                            {
                                CPrintf("Can't override ID code with information from macro: 0x%04x\n",macro_id);
                            }
                            else
                            {
                                CPrintf("Anomaly correction overrides ID Code %d with %d for sequence %d in current data set\n",id_code,val,meas_seq+1);
                                id_code=val;
                            }
                        }
                        
                        switch(id_code) // Test ID Code
                        {
                            case MACRO_ID_TAG:
                                state=S05_GET_MACRO_ID;
                                break;
                            case SECOND_ID_01: // Second sub ID to be expected
                                state=S04_GET_ID_CODE; // Get second sub ID 
                                // Set a flag indicating second sub ID list 
                                // currently not existing!, but for contingency!
                                break;
                            case P2_TRANSMITTER: // Transmitting on P2! no data on P2 but probably on P1
                                
                                curr.sensor=SENS_P1;
                                curr.transmitter=SENS_P2; // Set current transmitter to P2
                                state=S04_GET_ID_CODE;
                                break;
                            case P1_TRANSMITTER: // Transmitting on P1! no data on P1 but probably on P2
                                curr.sensor=SENS_P2;
                                curr.transmitter=SENS_P1; // Set current transmitter on P1
                                state=S04_GET_ID_CODE;
                                break;
                                
                                // This is a dirty workaround for the generic macros.
                                // We can accept a low probability 0.002 % for the 
                                // workaround to do harm. Currently we do not use generic
                                // macros.                                
                            case GENERIC_ID_P1:
                                LookBuffer(cb,buff,2);     // Look ahead two bytes
                                
                                if(buff[0]==0xCC && buff[1]==GENERIC_ID_P2)
                                {
                                    Forward(cb,1); // Go forward skip ID code
                                    state=S04_GET_ID_CODE; // Look for next ID code
                                }
                                else
                                {
                                    curr.sensor=SENS_P1; // Set current sensor to sensor 1/probe 1
                                    state=S11_GET_LENGTH; // Ok! No params in this..go get length
                                    meas_seq++; // Increase number of measurement sequences
                                    CPrintf("    Found science data, from generic macro, ID CODE:   0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                }
                                break;
                                
                                // This is a dirty workaround for the generic macros.
                                // We can accept a low probability 0.002 % for the 
                                // workaround to do harm. Currently we do not use generic
                                // macros.
                            case GENERIC_ID_P2:
                                LookBuffer(cb,buff,2);     // Look ahead two bytes
                                
                                if(buff[0]==0xCC && buff[1]==GENERIC_ID_20BIT)
                                {
                                    Forward(cb,1); // Go forward skip ID code
                                    state=S04_GET_ID_CODE; // Look for next ID code
                                }
                                else
                                {  
                                    curr.sensor=SENS_P2; // Set current sensor to sensor 2/probe 2
                                    state=S11_GET_LENGTH; // Ok! No params in this..go get length
                                    meas_seq++; // Increase number of measurement sequences
                                    CPrintf("    Found science data, from generic macro, ID CODE: 0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                } 
                                break;
                                
                                // This is a dirty workaround for the generic macros.
                                // We can accept a low probability 0.002 % for the 
                                // workaround to do harm. Currently we do not use generic
                                // macros.
                            case GENERIC_LDL_P1:
                                LookBuffer(cb,buff,2);     // Look ahead two bytes
                                
                                if(buff[0]==0xCC && buff[1]==GENERIC_LDL_P2)
                                {
                                    Forward(cb,1);         // Go forward skip ID code
                                    state=S04_GET_ID_CODE; // Look for next ID code
                                }
                                else
                                {	  
                                    curr.sensor=SENS_P1; // Set current sensor to sensor 1/probe 1
                                    state=S11_GET_LENGTH; // Ok! No params in this..go get length
                                    meas_seq++; // Increase number of measurement sequences
                                    CPrintf("    Found science data, from generic LDL macro, ID CODE: 0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                }
                                break;
                                
                            case GENERIC_LDL_P2:
                                LookBuffer(cb,buff,2);     // Look ahead two bytes
                                
                                if(buff[0]==0xCC && buff[1]==GENERIC_LDL_20BIT)
                                {
                                    Forward(cb,1);         // Go forward skip ID code
                                    state=S04_GET_ID_CODE; // Look for next ID code
                                }
                                else
                                {    
                                    curr.sensor=SENS_P2;  // Set current sensor to sensor 2/probe 2
                                    state=S11_GET_LENGTH; // Ok! No params in this..go get length
                                    meas_seq++; // Increase number of measurement sequences
                                    CPrintf("    Found science data, from generic LDL macro, ID CODE: 0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                }
                                break;
                                
                            case D_SWEEP_P2_RAW_16BIT_BIP:
                            case D_SWEEP_P2_LC_16BIT_BIP: // LOG COMPRESSION I HAVE TO DO SEPARATE PROCESSING FOR THIS!!! 
                                curr.sensor++;              // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)    
                            case D_SWEEP_P1_RAW_16BIT_BIP:
                            case D_SWEEP_P1_LC_16BIT_BIP: // LOG COMPRESSION I HAVE TO DO SEPARATE PROCESSING FOR THIS!!!
                                curr.sensor++;              // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                                params=6;
                                meas_seq++;                 // Increase number of measurement sequences
                                CPrintf("    Found sweep science data, ID CODE: 0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                param_type=SWEEP_PARAMS;
                                state=S07_GET_PARAMS;
                                break;

                            /* Case (macro variable) E_P1_D_P2_INTRL_20_BIT_RAW_BIP is defined in id.h.
                             * 
                             * The exact desired response/behaviour/code for this case is presently unknown.
                             * The case should probably be inserted somewhere around here according to Anders Eriksson.
                             * /Erik P G Johansson 2016-03-03
                             */
                            case E_P1_D_P2_INTRL_20_BIT_RAW_BIP:
                                
                            case D_P1P2INTRL_TRNC_20BIT_RAW_BIP:
                            case D_P1P2INTRL_20BIT_RAW_BIP:
                            case E_P1P2INTRL_TRNC_20BIT_RAW_BIP:
                            case E_P1P2INTRL_20BIT_RAW_BIP:
                                // Effectively curr.sensor = 3 (SENS_P1P2)
                                curr.sensor++; // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                                // NOTE: No break!
                                
                            case D_P2_TRNC_20_BIT_RAW_BIP:
                            case E_P2_TRNC_20_BIT_RAW_BIP:
                            case D_P2_20_BIT_RAW_BIP:
                            case E_P2_20_BIT_RAW_BIP:    
                                // Effectively curr.sensor = 2 (SENS_P2)
                                curr.sensor++; // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                                // NOTE: No break!
                                
                            case D_P1_TRNC_20_BIT_RAW_BIP:
                            case D_P1_20_BIT_RAW_BIP:
                            case E_P1_TRNC_20_BIT_RAW_BIP:
                            case E_P1_20_BIT_RAW_BIP:
                                // Effectively curr.sensor = 1 (SENS_P1)
                                curr.sensor++; // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                                
                                params=2;
                                meas_seq++; // Increase number of measurement sequences
                                CPrintf("    Found science data, ID CODE: 0x%.2x Sequence %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                param_type=ADC20_PARAMS;
                                state=S07_GET_PARAMS;
                                break;
                                
                            case D_DIFF_P1P2:
                            case E_DIFF_P1P2:
                                curr.sensor++; // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                            case D_P2_RAW_16BIT:
                            case E_P2_16BIT_RAW:
                            case D_P2_RAW_16BIT_D2:
                            case D_P2_RAW_16BIT_D4:
                            case E_P2_RAW_16BIT_D2:
                            case E_P2_RAW_16BIT_D16:
                            case E_P2_RAW_16BIT_D4:
                            case E_P2_RAW_16BIT_D8:
                            case D_P2_RAW_16BIT_D16:
                            case D_P2_RAW_16BIT_D8:
                                curr.sensor++; // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                            case D_P1_RAW_16BIT:
                            case E_P1_16BIT_RAW:
                            case D_P1_RAW_16BIT_D4:
                            case D_P1_RAW_16BIT_D2:
                            case D_P1_RAW_16BIT_D8:
                            case D_P1_RAW_16BIT_D16:
                            case E_P1_RAW_16BIT_D2:
                            case E_P1_RAW_16BIT_D4:
                            case E_P1_RAW_16BIT_D8:
                            case E_P1_RAW_16BIT_D16:
                                curr.sensor++; // Increment current sensor. (Always initially set to 0=SENS_NONE before switch().)
                                meas_seq++;    // Increase number of measurement sequences
                                CPrintf("    Found science data, no parameters, ID CODE: 0x%.2x Sequence: %d Sensor: %d \n",id_code,meas_seq,curr.sensor);
                                state=S11_GET_LENGTH;
                                break; 
                                
                            default:
                                CPrintf("    Found undefined, ID CODE:   0x%.2x\n",id_code);
                                break;
                        }
                    }   // if(buff[0]==0xCC) // Found Sub Header
                    else
                    {
                        in_sync=0; // Indicate out of sync.
                        state=S01_GET_MAINH; // No sub ID, End or out of sync
                    }
                    break;
                    
                            //##############################################
                            // NOTE: BAD INDENTATION, CHANGE IN INDENTATION
                            //##############################################                
                            case S05_GET_MACRO_ID:
                                DispState(state,"STATE = S05_GET_MACRO_ID\n");
                                
                                GetBuffer(cb,buff,2); // Get 2 bytes from circular science buffer
                                
                                hb=buff[0]<<8;      // Extract hi byte
                                lb=buff[1];         // Extract lo byte
                                macro_id=(hb | lb); // Put together hi and lo byte
                                CPrintf("    Executed macro was, MACRO ID: 0x%.4x\n",macro_id);	
                                state=S06_GET_MACRO_DESC;
                                break;
                                
                            case S06_GET_MACRO_DESC:
                                DispState(state,"STATE = S06_GET_MACRO_DESC\n");
                                
                                sprintf(tstr1,"MCID0X%04x",macro_id);
                                SetP(&comm,"INSTRUMENT_MODE_ID",tstr1,1);         // Set mode ID in common PDS parameter
                                SetP(&comm,"DATA_SET_ID",mp.data_set_id,1);       // Set DATA SET ID in common PDS parameters
                                SetP(&comm,"DATA_SET_NAME",mp.data_set_name,1);   // Set DATA SET NAME in common PDS parameters
                                
                                // Find human description of macro in macro mode descriptions
                                sprintf(tstr1,"0x%04x",macro_id); // Create search variable..to search for in linked list of property name value pairs.
                                if(FindP(&mdesc,&property1,tstr1,1,0)>0)
                                {
                                    SetP(&comm,"INSTRUMENT_MODE_DESC",property1->value,1); // Set human description of mode
                                }
                                
                                sprintf(tstr1,"\"%d\"",pds.DPLNumber);
                                
                                SetP(&comm,"PROCESSING_LEVEL_ID",tstr1,1);  // Set processing level ID...
                                
                                // Search for macro with right macro ID in macs matrix
                                macro_descr_NOT_found=1; // Indicate that we haven't found the macro 
                                for(mb=0;macro_descr_NOT_found && mb<MAX_MACRO_BLCKS;mb++)
                                    for(ma=0;macro_descr_NOT_found && ma<MAX_MACROS_INBL;ma++)
                                    {
                                        if(FindP(&macros[mb][ma],&property1,"ROSETTA:LAP_MACRO_ID_TAG",1,0)>0)
                                        {
                                            //TrimQN(property1->value);
                                            if(!sscanf(property1->value,"\"%x\"",&val)) 
                                            {
                                                CPrintf("    MACRO ID corrupt in macro description\n");
                                                
                                                if(finger_printing) // Are we fingerprinting ?
                                                {
                                                    finger_printing=0;
                                                    state=S09_COMPARE_PARAMS;
                                                }
                                                else
                                                {
                                                    state=S04_GET_ID_CODE;
                                                }
                                            }
                                            else
                                            {
                                                if(val==macro_id)
                                                {
                                                    // Move temporary buffer to it's end, This buffer will be used to dump
                                                    // science data that doesn't contain macro ID's to special files that
                                                    // should be manually checked and then resubmitted to this pds program
                                                    // using the correct macro ID on the command line.
                                                    Forward(ct,ct->fill); 
                                                    CPrintf("    Macro description found, Block %d, Macro num %d\n",mb,ma);
                                                    macro_descr_NOT_found=0; // We found macro description!
                                                    if(pds.uaccpt_fd!=NULL) 
                                                    {
                                                        fclose(pds.uaccpt_fd);
                                                        pds.uaccpt_fd=NULL;
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }      
                                    
                                    if(macro_descr_NOT_found) 
                                    {
                                        CPrintf("    Error: Could not find a matching macro description for MACRO ID: 0x%.4x\n",macro_id);
                                    }
                                    else
                                        mb--; // mb is one step to much here since break above only breaks out of inner loop!
                                        
                                        if(finger_printing) // Are we fingerprinting ?
                                        {
                                            finger_printing=0;
                                            state=S09_COMPARE_PARAMS;
                                        }
                                        else
                                            state=S04_GET_ID_CODE;
                                        break;
                                    
                                    //##############################################
                                    // NOTE: BAD INDENTATION, CHANGE IN INDENTATION
                                    //##############################################
                                    case S07_GET_PARAMS:
                                        DispState(state,"STATE = S07_GET_PARAMS\n");
                                        
                                        // Get params bytes from circular science buffer
                                        GetBuffer(cb,buff,params); // Get 2 bytes from circular science buffer
                                        
                                        state=S08_DECODE_PARAMS;
                                        break;
                                        
                                    case S08_DECODE_PARAMS:
                                        DispState(state,"STATE = S08_DECODE_PARAMS\n");
                                        // DECODE PARAMS TO PDS FORMAT
                                        CPrintf("    Decoding parameters %d Bytes Type %d Sensor %d\n",params,param_type,curr.sensor);
                                        
                                        if(param_type==SWEEP_PARAMS) // DECODE SWEEP PARAMETERS
                                        {
                                            W0=buff[1];                // Put together words since
                                            W1=(buff[2]<<8 | buff[3]); // we sometimes have parameters
                                            W2=(buff[4]<<8 | buff[5]); // crossing byte boundaries.
                                            
                                            sw_info.formatv=GetBitF(W2,2,0); // Remember raw format value
                                            switch(sw_info.formatv)          // Decode LAP_SWEEP_FORMAT 
                                            {
                                                case 0:
                                                    strcpy(sw_info.format,"UP");
                                                    break;
                                                case 1:
                                                    strcpy(sw_info.format,"DOWN UP");
                                                    break;
                                                case 2:
                                                    strcpy(sw_info.format,"DOWN");
                                                    break;
                                                case 3:
                                                    strcpy(sw_info.format,"UP DOWN");
                                                    break;
                                                default:
                                                    strcpy(sw_info.format,"UNKNOWN");
                                            }
                                            
                                            if(GetBitF(W2,1,2)) {  // Decode LAP_SWEEP_RESOLUTION 
                                                strcpy(sw_info.resolution,"FINE");
                                            } else {
                                                strcpy(sw_info.resolution,"COARSE");
                                            }
                                            
                                            if(GetBitF(W2,1,3)) { // Decode LAP_SWEEPING_P1 
                                                strcpy(sw_info.p1,"YES");
                                            } else {
                                                strcpy(sw_info.p1,"NO");
                                            }
                                            
                                            if(GetBitF(W2,1,4)) {// Decode LAP_SWEEPING_P2 
                                                strcpy(sw_info.p2,"YES");
                                            } else {
                                                strcpy(sw_info.p2,"NO");
                                            }
                                            
                                            sw_info.p1_fine_offs = GetBitF(W2,4,12);       // LAP_P1_FINE_SWEEP_OFFSET
                                            sw_info.p2_fine_offs = GetBitF(W2,4,8);        // LAP_P2_FINE_SWEEP_OFFSET
                                            sw_info.plateau_dur  = 1<<(GetBitF(W1,4,0)+1); // LAP_SWEEP_PLATEAU_DURATION
                                            sw_info.steps        = (GetBitF(W1,4,4)<<4);   // LAP_SWEEP_STEPS 
                                            sw_info.height       = GetBitF(W1,4,8)+1;      // LAP_SWEEP_STEP_HEIGHT Range is from 1 to 16
                                            sw_info.start_bias   = GetBitF(W0,8,0);        // LAP_SWEEP_START_BIAS
                                            
                                            sw_info.sweep_dur_s  = (sw_info.steps+3)* sw_info.plateau_dur; // Total duration of sweep in samples (Not same as it's length!)
                                            
                                            
                                            // POPULATE PDS LAP Dictionary with sweep info.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_START_BIAS",sw_info.start_bias);         // Removed/moved 2015-02-17, Erik P G Johansson.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_STEP_HEIGHT",sw_info.height);            // Removed/moved 2015-02-23, Erik P G Johansson.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_STEPS",sw_info.steps);                   // Removed/moved 2015-02-17, Erik P G Johansson.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_PLATEAU_DURATION",sw_info.plateau_dur);  // Removed/moved 2015-02-17, Erik P G Johansson.
                                            //InsertTopQ(&dict,"ROSETTA:LAP_SWEEP_RESOLUTION",sw_info.resolution);          // Removed/moved 2015-02-23, Erik P G Johansson.
                                            //InsertTopQ(&dict,"ROSETTA:LAP_SWEEP_FORMAT",sw_info.format);                  // Removed/moved 2015-02-23, Erik P G Johansson.
                                            
                                            if(!strcmp("YES",sw_info.p1)) // If sweeping P1
                                            {
                                                // Edit 2015-02-17, 2015-02-23, Erik P G Johansson:
                                                // Changing keyword names to be probe-specific. This replaces lines of code a few lines up.
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_START_BIAS",       sw_info.start_bias);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_STEP_HEIGHT",      sw_info.height);		  
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_STEPS",            sw_info.steps);		  
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_PLATEAU_DURATION", sw_info.plateau_dur);
                                                InsertTopQ (&dict, "ROSETTA:LAP_P1_SWEEP_RESOLUTION",       sw_info.resolution);
                                                InsertTopQ (&dict, "ROSETTA:LAP_P1_SWEEP_FORMAT",           sw_info.format);
                                                
                                                // Insert fine offset for P1 into dictionary
                                                InsertTopQV(&dict,"ROSETTA:LAP_P1_FINE_SWEEP_OFFSET",sw_info.p1_fine_offs); 
                                                // Insert who's sweeping into PDS dictionary 
                                                InsertTopQ(&dict,"ROSETTA:LAP_SWEEPING_P1",sw_info.p1); 
                                            }
                                            
                                            if(!strcmp("YES",sw_info.p2)) // If sweeping P2
                                            {
                                                // Edit 2015-02-17, 2015-02-23, Erik P G Johansson:
                                                // Changing keyword names to be probe-specific. This replaces lines of code a few lines up.
                                                InsertTopQV(&dict, "ROSETTA:LAP_P2_SWEEP_START_BIAS",       sw_info.start_bias);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P2_SWEEP_STEP_HEIGHT",      sw_info.height);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P2_SWEEP_STEPS",            sw_info.steps);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P2_SWEEP_PLATEAU_DURATION", sw_info.plateau_dur);
                                                InsertTopQ (&dict, "ROSETTA:LAP_P2_SWEEP_RESOLUTION",       sw_info.resolution);
                                                InsertTopQ (&dict, "ROSETTA:LAP_P2_SWEEP_FORMAT",           sw_info.format);
                                                
                                                // Insert fine offset for P2 into dictionary
                                                InsertTopQV(&dict,"ROSETTA:LAP_P2_FINE_SWEEP_OFFSET",sw_info.p2_fine_offs); 
                                                // Insert who's sweeping into PDS dictionary
                                                InsertTopQ(&dict,"ROSETTA:LAP_SWEEPING_P2",sw_info.p2); 
                                            }
                                        }
                                        
                                        if(param_type==ADC20_PARAMS)
                                        {
                                            // ADD ADC20 parameter decoding |MALEN|DA20CTRL|.....|MARESFAC|
                                            // I use  getbitf everywhere for consistency! (could have done
                                            // things like: (buff[0] & 0xf0)>>4
                                            
                                            a20_info.moving_average_length = (1<<GetBitF(buff[0],4,4)); // Length of moving average filter
                                            a20_info.adc20_control         = GetBitF(buff[0],4,0);      // Indicate full, truncated,..and so on
                                            a20_info.resampling_factor     = (1<<GetBitF(buff[1],4,0)); // Downsampling factor..(Thus keep every n:th samp)
                                            
                                            // POPULATE PDS LAP Dictionary with 20 bit ADC info.
                                            InsertTopQV(&dict,"ROSETTA:LAP_P1P2_ADC20_DOWNSAMPLE",a20_info.resampling_factor);
                                            InsertTopQV(&dict,"ROSETTA:LAP_P1P2_ADC20_MA_LENGTH", a20_info.moving_average_length);
                                            InsertTopQ( &dict,"ROSETTA:LAP_P1P2_ADC20_STATUS",    a20status[a20_info.adc20_control & 0xf]);
                                            
                                            if((a20_info.adc20_control & 0x3)==0x02) { curr.sensor=SENS_P1; }// Modify current sensor from both P1 and P2 to P1 only
                                            if((a20_info.adc20_control & 0x3)==0x01) { curr.sensor=SENS_P2; }// Modify current sensor from both P1 and P2 to P2 only
                                            // Above: I don't treat the combination there only the lowest bits are used for the 20 bit ADC:s
                                            //        cause we never will use it..it's not scientific!
                                        }
                                        state=S11_GET_LENGTH; // Error unknown parameter type, continue to get length
                                        break;
                                        
                                        
                                        case S09_COMPARE_PARAMS:
                                            DispState(state,"STATE = S09_COMPARE_PARAMS\n");
                                            if(!macro_descr_NOT_found && !macro_priority) // If a macro description exists and macro description has lowest priority over data parameters
                                            {
                                                // Go through all property/value pairs in dictionary and compare to macro description
                                                for(i=0;i<dict.no_prop;i++) 
                                                {
                                                    GetNo(&dict,&property1,i); // Get property number i from dictionary
                                                    // Find property in macro description, in dictionary they occur only once.
                                                    if(FindP(&macros[mb][ma],&property2,property1->name,1,UNCHECKED)>0) // Look for unchecked ones 
                                                    {
                                                        if(ComparePrp(property1,property2)) // Compare properties
                                                        {
                                                            CPrintf("    Warning mismatch between parameters and macro description\n");
                                                            CPrintf("    MACRO: %s=%s\n",property2->name,property2->value);
                                                            CPrintf("    PARAM: %s=%s\n",property1->name,property1->value);
                                                            // Remove warning keyword..enough with warning in logs
                                                            //InsertTopQ(&dict,"ROSETTA:LAP_SC_VS_MACRO_MISMATCH","WARNING");
                                                            break; // Break out of loop
                                                        }
                                                        
                                                        // Below some special cases, that need to be excluded from the next 
                                                        // comparison in the current measurement cycle.
                                                        if(!strcmp(property2->name,"ROSETTA:LAP_SWEEPING_P1"))
                                                        {
                                                            // We need to exclude SWEEPING_P2 since they exist in pairs in the macro description
                                                            if(FindP(&macros[mb][ma],&property2,"ROSETTA:LAP_SWEEPING_P2",1,UNCHECKED)>0) // Look for P2 
                                                                Check(property2); // Check this one so we don't look for it the next time
                                                        }
                                                        
                                                        if(!strcmp(property2->name,"ROSETTA:LAP_SWEEPING_P2"))
                                                        {
                                                            // We need to exclude SWEEPING_P1 since they exist in pairs in the macro description
                                                            if(FindP(&macros[mb][ma],&property2,"ROSETTA:LAP_SWEEPING_P1",1,UNCHECKED)>0) // Look for P1
                                                                Check(property2); // Check this one so we don't look for it the next time
                                                        }
                                                        
                                                        // Everything that occurs more than once in the macro description need to be excluded successively
                                                        // Other properties such as TM_RATE will probably be used in more than one label file so 
                                                        // we do uncheck them again.
                                                        if(property2!=NULL)
                                                        {
                                                            Check(property2);
                                                            strcpy(tstr1,property2->name);
                                                        }
                                                        
                                                        if(FindP(&macros[mb][ma],&property2,tstr1,1,UNCHECKED)==0) // Look for unchecked ones
                                                        {
                                                            Check(property2);
                                                        }
                                                    }
                                                }
                                            }
                                            state=S14_RESOLVE_MACRO_PARAMETERS; // Resolve macro parameters
                                            break;
                                            
                                            case S11_GET_LENGTH:
                                                DispState(state,"STATE = S11_GET_LENGTH\n");
                                                
                                                GetBuffer(cb,buff,2); // Get 2 bytes from circular science buffer
                                                
                                                hb=buff[0]<<8;
                                                lb=buff[1];
                                                length=(hb | lb)*2;
                                                CPrintf("    Data length: %d Bytes\n",length);
                                                
                                                if(length>RIDICULUS) // If length is ridiculously long
                                                {
                                                    CPrintf("    Ridiculously long length in science data, trying to resync\n");
                                                    in_sync=0; // Indicate not in sync
                                                    state=S01_GET_MAINH; //Resync
                                                }
                                                else
                                                {
                                                    if(SyncAhead(cb,length)) // Extra sync test added 2004-04-13.
                                                    {
                                                        state=S12_GET_DATA;
                                                    }
                                                    else
                                                    {
                                                        CPrintf("    Broken science data packet, trying to resync\n");
                                                        in_sync=0; // Indicate not in sync
                                                        state=S01_GET_MAINH; //Resync
                                                    }
                                                }
                                                break;
                                                
                                            case S12_GET_DATA:
                                                DispState(state,"STATE = S12_GET_DATA\n");
                                                // Get length bytes from circular science buffer 
                                                GetBuffer(cb,buff,length); 
                                                
                                                if(param_type==NO_PARAMS)
                                                {
                                                    state=S14_RESOLVE_MACRO_PARAMETERS; // Resolve macro parameters
                                                }
                                                else
                                                {
                                                    state=S09_COMPARE_PARAMS;           // Check data parameters macro consistency
                                                }
                                                break;

                                            case S13_RECONNECT:
                                                DispState(state,"STATE = S13_RECONNECT\n");
                                                break;

                                            case S14_RESOLVE_MACRO_PARAMETERS:
                                                DispState(state,"STATE = S14_RESOLVE_MACRO_PARAMETERS\n");
                                                
                                                
                                                // FINGERPRINTING for the right macro description.
                                                // This is a bit dirty, but we need to do it on the prom macros (8 of them).
                                                // All macros in flash contain the macro ID in the science data stream.
                                                if(macro_descr_NOT_found) // If no macro description
                                                {
                                                    CPrintf("    WARNING: Fingerprinting macro ID.\n");
                                                    finger_printing=1;
                                                    if(length==40 && id_code==D_P1P2INTRL_TRNC_20BIT_RAW_BIP)
                                                    {
                                                        // Brute force macro settings!
                                                        macro_id=0x201; // Set macro ID
                                                        state=S06_GET_MACRO_DESC; // Break out of switch, and get macro description
                                                        break;
                                                    }
                                                    
                                                    if(400<=length && length<=512 && id_code==D_SWEEP_P1_RAW_16BIT_BIP)
                                                    {
                                                        LookBuffer(cb,tbuff,2);     // Look ahead two bytes
                                                        
                                                        if(tbuff[0]==0xCC)
                                                        {
                                                            if(tbuff[1]==D_P2_RAW_16BIT)
                                                            {
                                                                macro_id=0x202; // Set macro ID
                                                                state=S06_GET_MACRO_DESC; // Break out of switch, and get macro description
                                                            }
                                                            
                                                            if(tbuff[1]==E_P2_TRNC_20_BIT_RAW_BIP)
                                                            {
                                                                macro_id=0x203; // Set macro ID
                                                                state=S06_GET_MACRO_DESC; // Break out of switch, and get macro description
                                                            }
                                                        }
                                                        break;
                                                    }
                                                    
                                                    if(1930<=length && length<=1950 && id_code==D_SWEEP_P1_RAW_16BIT_BIP)
                                                    {
                                                        LookBuffer(cb,tbuff,2);     // Look ahead two bytes
                                                        
                                                        if(tbuff[0]==0xCC)
                                                        {
                                                            if(tbuff[1]==D_P2_RAW_16BIT)
                                                            {
                                                                macro_id=0x204; // Set macro ID
                                                                state=S06_GET_MACRO_DESC; // Break out of switch, and get macro description
                                                                break;
                                                            }
                                                        }		  
                                                    }
                                                    
                                                    if(length==8950 && id_code==D_P1P2INTRL_TRNC_20BIT_RAW_BIP)
                                                    {
                                                        // Brute force macro settings!
                                                        macro_id=0x205; // Set macro ID
                                                        state=S06_GET_MACRO_DESC;  // Break out of switch, and get macro description
                                                        break;
                                                    }
                                                    
                                                    if(length==1024 && id_code==E_P1_RAW_16BIT_D4)
                                                    {
                                                        // Brute force macro settings!
                                                        macro_id=0x207; // Set macro ID
                                                        state=S06_GET_MACRO_DESC;  // Break out of switch, and get macro description
                                                        break;
                                                    }
                                                    
                                                    finger_printing=0;
                                                }   // if (macro_descr_NOT_found) 
                                                
                                                // We are in a position of choices here!
                                                //--------------------------------------
                                                // The normal operation of the instrument is by running
                                                // a macro that returns a macro ID. But we can also run
                                                // the instrument from the old prom macros that don't
                                                // return a macro ID. In the first case we can derive a
                                                // lot of information from the macro description stored
                                                // here on earth. In the second case we have to derive
                                                // everything from the parameters in the science stream 
                                                // and the data ID tag. So far so good! But in the case
                                                // there both exist we have to choose where to derive the
                                                // information from. I'll try to derive as little as possible
                                                // and use what's in the science stream. But some things need
                                                // to be derived, the data ID code might be a bit safer to
                                                // use than the info from the macro since it's derived in 
                                                // fewer steps!
                                                
                                                
                                                CPrintf("    Parsing ID Code: %s\n",IDList[id_code]); 
                                                
                                                // To keep the information coming as close to the source as possible
                                                // WE DERIVE INFO FROM THE ID CODE STRING DIRECTLY BY SEARCHING FOR SUBSTRINGS!!
                                                // We do not compile information in a secondary file.
                                                // That would give two sources of info and consistency problems could occur.
                                                // NOTE: The ID strings are at this moment fixed (permanent) and will not change.
                                                //       The number of ID strings may however increase!
                                                
                                                dsa16_p1 = -1;   // Indicate that "down sampl 16 bit sensor 1" value is not resolved
                                                dsa16_p2 = -1;   // Indicate that "down sampl 16 bit sensor 2" value is not resolved
                                                
                                                if((tp=strstr(IDList[id_code],"16BIT_D"))!=NULL)
                                                {
                                                    if(curr.sensor==SENS_P1) {
                                                        if(!sscanf(&tp[7],"%d",&dsa16_p1)) {
                                                            dsa16_p1=-1; // Error in conversion
                                                        }
                                                    }
                                                    
                                                    if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2)
                                                    {
                                                        if(!sscanf(&tp[7],"%d",&dsa16_p2)) {
                                                            dsa16_p2=-1; // Error in conversion
                                                        }
                                                    }
                                                }
                                                
                                                curr.bias_mode1=DENSITY;   // Assume density mode unless there is a reason not to.
                                                curr.bias_mode2=DENSITY;
                                                if((tp=strstr(IDList[id_code],"E_"))!=NULL)     // Check if IDList[id_code] (human-readable string!!) indicates E-FIELD mode.
                                                {
                                                    curr.bias_mode1=E_FIELD;
                                                    curr.bias_mode2=E_FIELD;                                                    
                                                }
                                                
                                                // NOTE: Special case for id_code==E_P1_D_P2_INTRL_20_BIT_RAW_BIP since it does not fit in with any other rule,
                                                // and can not be easily fitted into the switch(id_code) segment.
                                                if (id_code==E_P1_D_P2_INTRL_20_BIT_RAW_BIP) {
                                                    curr.bias_mode1=E_FIELD;
                                                    curr.bias_mode2=DENSITY;                                                    
                                                }
                                                
                                                
                                                // 20 Bit ADC data is interleaved if both P1 and P2 are used. If full 20 bit:s are used
                                                // We have for n samples
                                                //
                                                // S1  S1  S2  S2  S3  S3       Sn  Sn  |S1S1S2S2S3S3S4S4...SnSn
                                                // ------------------------------------------------------------------------------------
                                                // PSE1PSE2PSE1PSE2PSE1PSE2.....PSE1PSE2|P1P2P1P2P1P2P1P2...P1P2
                                                //
                                                //
                                                // PSE1 = 16 Bits probe/sensor 1
                                                // PSE2 = 16 Bits probe/sensor 2
                                                //
                                                // P1   = 4 Bits probe/sensor 1
                                                // P2   = 4 Bits probe/sensor 2
                                                //
                                                // Now we have 16+4 is 20 bit, for truncated 20 bit adc:s we only keep the 16 bits!
                                                // 
                                                // And number of samples is now the byte length multiplied by a factor of:
                                                // 
                                                // Full  p1 & p2: 1/5
                                                // Trunc p1 & p2: 1/4
                                                // Full p1      : 2/5
                                                // Trunc p1     : 1/2
                                                // Full p2      : 2/5
                                                // Trunc p2     : 1/2
                                                //
                                                data_type=D16; // Set data type to default 16 bit
                                                if(param_type==ADC20_PARAMS) // Test if 20 Bit ADC:s (ADC20) are involved!
                                                {
                                                    switch(curr.sensor)
                                                    {
                                                        case SENS_P1:
                                                            if(a20_info.adc20_control & 0x08)
                                                            {
                                                                samples=length*2/5; // Sensor P1 only full 20 Bit ADC
                                                                data_type=D201;
                                                            }
                                                            else
                                                            {
                                                                // Sensor P1 only truncated 20 Bit ADC
                                                                data_type=D201T;
                                                                samples=length/2;   
                                                            }
                                                            break;
                                                        case SENS_P2:
                                                            if(a20_info.adc20_control & 0x04) 
                                                            {
                                                                samples=length*2/5; // Sensor P2 only full 20 Bit ADC
                                                                data_type=D202;
                                                            }
                                                            else
                                                            {
                                                                // Sensor P2 only truncated 20 Bit ADC
                                                                data_type=D202T;
                                                                samples=length/2;   
                                                            }
                                                            break;
                                                        case SENS_P1P2:
                                                            if((a20_info.adc20_control & 0x0C)==0x0C) 
                                                            {
                                                                samples=length/5; // Full 20 Bit ADC:s P1 and P2
                                                                data_type=D20;
                                                            }
                                                            else
                                                            {
                                                                // Truncated 20 Bit ADC:s P1 and P2
                                                                data_type=D20T; 
                                                                samples=length/4; 
                                                            }
                                                            break;
                                                        default:
                                                            break;
                                                    }
                                                }
                                                else
                                                {
                                                    samples=length/2; // Number of samples then 20 Bit ADC:s are not used and no compression. 
                                                    if(id_code==D_SWEEP_P2_LC_16BIT_BIP || id_code==D_SWEEP_P1_LC_16BIT_BIP) // If log compression is used
                                                    {
                                                        samples=length; // Log compression used
                                                    }
                                                }
                                                
                                                sprintf(tstr1,"%d",samples);
                                                SetP(&comm,"FILE_RECORDS",tstr1,1); // Set number of records (in data table file) in common PDS parameters
                                                
                                                CPrintf("    Number of samples current record: %d\n",samples);
                                                strcpy(tstr1,IDList[id_code]);      // Get ID code name
                                                TrimWN(tstr1);                      // Remove trailing whitespace
                                                sprintf(tstr2,"\"%s\"",tstr1);      // Add PDS quotes ".." 
                                                SetP(&comm,"DESCRIPTION",tstr2,1);  // Update DESCRIPTION in common PDS parameters
                                                
                                                if(!macro_descr_NOT_found)
                                                {
                                                    //===================================
                                                    // CASE: A macro description exists.
                                                    //===================================
                                                    
                                                    //Find subheader for measurement sequence meas_seq
                                                    if(FindP(&macros[mb][ma],&property1,"ROSETTA:LAP_SET_SUBHEADER",meas_seq,DNTCARE)>0)   // Set property1. Read many times afterwards with FindB(..).
                                                    {
                                                        if (debug>0) {
                                                            printf("    Uses ROSETTA:LAP_SET_SUBHEADER = %s, meas_seq=%i in macro (.mds file).\n", property1->value, meas_seq);   // DEBUG
                                                        }
                                                        
                                                        // Find downsampling value probe 1 in macro and if no ID value exists use macro desc. value
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16_DOWNSAMPLE",DNTCARE)>0)
                                                        {
                                                            sscanf(property2->value,"\"%x\"",&val);
                                                            if(dsa16_p1==-1 || macro_priority) { // Value not resolved from ID or macro has high priority use macro value instead
                                                                dsa16_p1=val;
                                                            }
                                                            
                                                            if(dsa16_p1!=val) {
                                                                CPrintf("    Warning mismatch between data ID code info. and macro description info.\n");
                                                            }
                                                            //CPrintf("%s = 0x%04x\n",property2->name,dsa16_p1);
                                                        }
                                                        
                                                        // Find downsampling value probe 2 in macro and if no ID value exists use macro desc. value
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16_DOWNSAMPLE",DNTCARE)>0)
                                                        {
                                                            sscanf(property2->value,"\"%x\"",&val);
                                                            if(dsa16_p2==-1 || macro_priority)  { // Value not resolved from ID  or macro has high priority use macro value instead
                                                                dsa16_p2=val;
                                                            }
                                                            
                                                            if(dsa16_p2!=val && !macro_priority)  { // Print mismatch warning..only if macro desc. has low priority
                                                                CPrintf("    Warning mismatch between data ID code info. and macro description info.\n");
                                                            }
                                                            //CPrintf("%s = 0x%04x\n",property2->name,dsa16_p2);
                                                        }
                                                        
                                                        // Test if digital filter was turned on for probe 1, if so add information to dictionary
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16_DIG_FILT_STATUS",DNTCARE)>0)
                                                        {
                                                            if(!strcmp(property2->value,"\"ENABLED\""))
                                                            {
                                                                if(curr.sensor==SENS_P1 || curr.sensor==SENS_P1P2)
                                                                {
                                                                    if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16_DIG_FILT_CUTOFF",DNTCARE)>0) {
                                                                        InsertTopK(&dict,property2->name,property2->value);
                                                                    }
                                                                    if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16_DIG_FILT_STATUS",DNTCARE)>0) {
                                                                        InsertTopK(&dict,property2->name,property2->value);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        
                                                        // Test if digital filter was turned on for probe 2, if so add information to dictionary
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16_DIG_FILT_STATUS",DNTCARE)>0)
                                                        {
                                                            if(!strcmp(property2->value,"\"ENABLED\""))
                                                            {
                                                                if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2)
                                                                {
                                                                    if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16_DIG_FILT_CUTOFF",DNTCARE)>0) {
                                                                        InsertTopK(&dict,property2->name,property2->value);
                                                                    }
                                                                    if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16_DIG_FILT_STATUS",DNTCARE)>0) {
                                                                        InsertTopK(&dict,property2->name,property2->value);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        curr.afilter=0; // Assume no analog filter
                                                        
                                                        
                                                        
                                                        if(curr.sensor==SENS_P1 || curr.sensor==SENS_P1P2)
                                                        {
                                                            //========================
                                                            // CASE: Dependence on P1
                                                            //========================
                                                            
                                                            // If no sweeping and current sensor is sensor 1 or sensor 1 and 2 check the bias mode for sensor 1
                                                            if(param_type!=SWEEP_PARAMS)
                                                            {
                                                                if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_BIAS_MODE",DNTCARE)>0)
                                                                {
                                                                    if(curr.bias_mode1==E_FIELD) // Current mode according to ID is E-FIELD
                                                                    {
                                                                        // Is it so according to macro description, or macro description has high priority
                                                                        if(!strcmp(property2->value,"\"E-FIELD\"") || macro_priority) {
                                                                            InsertTopK(&dict,property2->name,property2->value); // Yes, let's set it
                                                                        } else {
                                                                            InsertTopQ(&dict,property2->name,"E-FIELD"); // No! Trust the ID code more..
                                                                        }
                                                                        
                                                                        // Need to keep old bias in case of no macro change..to accomodate for extra bias
                                                                        // settings outside of macros.
                                                                        if(macro_id!=curr.old_macro) 
                                                                        {
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_IBIAS1",DNTCARE)>0) // Find current bias
                                                                            {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                                if(!sscanf(property2->value,"\"%x\"",&curr.ibias1))
                                                                                {
                                                                                    CPrintf("    Invalid fix current bias p1, using default 0x7f\n");
                                                                                    curr.ibias1=0x007f;
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            InsertTopQV(&dict,"ROSETTA:LAP_IBIAS1",curr.ibias1); // No macro change keep old bias
                                                                        }
                                                                        
                                                                        if(param_type!=ADC20_PARAMS)
                                                                        {
                                                                            // Find duration ADC16 E-Field
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_EFIELD_FIX_DURATION",DNTCARE)>0) {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                            }
                                                                        }
                                                                    }
                                                                    
                                                                    if(curr.bias_mode1==DENSITY)   // Current mode according to ID is DENSITY
                                                                    {
                                                                        // Is it so according to macro description, or macro description has high priority
                                                                        if(!strcmp(property2->value,"\"DENSITY\"") || macro_priority) {
                                                                            InsertTopK(&dict,property2->name,property2->value);// Yes, let's set it
                                                                        } else {
                                                                            InsertTopQ(&dict,property2->name,"DENSITY"); //  No! Trust the ID code more.
                                                                        }
                                                                        
                                                                        // Need to keep old bias in case of no macro change..to accomodate for extra bias
                                                                        // settings outside of macros.
                                                                        if(macro_id!=curr.old_macro)
                                                                        {
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_VBIAS1",DNTCARE)>0) // Find voltage bias
                                                                            {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                                if(!sscanf(property2->value,"\"%x\"",&curr.vbias1))
                                                                                {
                                                                                    CPrintf("    Invalid fix voltage bias p1, using default 0x7f\n");
                                                                                    curr.vbias1=0x007f;
                                                                                }
                                                                                if (debug>0) {
                                                                                    printf("    Macro: Set fix voltage bias VBIAS1 = %04x (curr.vbias1; hex TM units)\n", curr.vbias1);  // DEBUG
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            InsertTopQV(&dict,"ROSETTA:LAP_VBIAS1",curr.vbias1); // No macro change keep old bias
                                                                        }
                                                                        
                                                                        
                                                                        if(param_type!=ADC20_PARAMS)
                                                                        {
                                                                            // Find duration ADC16 Density
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_DENSITY_FIX_DURATION",DNTCARE)>0) {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            
                                                            // Find LAP_P1_ADC16_FILTER
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16_FILTER",DNTCARE)>0) 
                                                            {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                sscanf(property2->value,"\"%d\"",&curr.afilter);
                                                            }
                                                            //  Find LAP P1_RX_OR_TX
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_RX_OR_TX",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_P1_STRATEGY_OR_RANGE
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_STRATEGY_OR_RANGE",DNTCARE)>0) 
                                                            {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                strncpy(curr.gain1,property2->value,16);
                                                            }
                                                            
                                                            // Find LAP_P1_RANGE_DENS_BIAS "+-32" or "+-5"
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_RANGE_DENS_BIAS",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_P1_ADC16 mode
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_P1_ADC20 mode
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC20",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_FEEDBACK_P1
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_FEEDBACK_P1",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                        }   // if(curr.sensor==SENS_P1 || curr.sensor==SENS_P1P2)

                                                        
                                                        
                                                        if (debug>0) {   // DEBUG
                                                            printf("    property1: %s = \"%s\"\n", property1->name, property1->value);
                                                            printf("    param_type = %i\n", param_type);
                                                        }
                                                        if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2)
                                                        {
                                                            //========================
                                                            // CASE: Dependence on P2
                                                            //========================
                                                            
                                                            if(param_type!=SWEEP_PARAMS)
                                                            {
                                                                if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_BIAS_MODE",DNTCARE)>0)
                                                                {
                                                                    if(curr.bias_mode2==E_FIELD) // Current mode according to ID is E-FIELD
                                                                    {
                                                                        // Is it so according to macro description, or macro description has high priority
                                                                        if(!strcmp(property2->value,"\"E-FIELD\"") || macro_priority) {
                                                                            InsertTopK(&dict,property2->name,property2->value);   // Yes, let's set it
                                                                        }
                                                                        else
                                                                        {
                                                                            InsertTopQ(&dict,property2->name,"E-FIELD"); // No! Trust the ID code more..
                                                                        }
                                                                        
                                                                        // Need to keep old bias in case of no macro change..to accomodate for extra bias
                                                                        // settings outside of macros.
                                                                        if(macro_id!=curr.old_macro) 
                                                                        {
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_IBIAS2",DNTCARE)>0) // Find current bias
                                                                            {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                                if(!sscanf(property2->value,"\"%x\"",&curr.ibias2))
                                                                                {
                                                                                    CPrintf("    Invalid fix current bias p2, using default 0x7f\n");
                                                                                    curr.ibias2=0x007f;
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {                                                                                        
                                                                            InsertTopQV(&dict,"ROSETTA:LAP_IBIAS2",curr.ibias2); // No macro change keep old bias
                                                                        }
                                                                        
                                                                        if(param_type!=ADC20_PARAMS)
                                                                        {
                                                                            // Find duration ADC16 E-Field
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_EFIELD_FIX_DURATION",DNTCARE)>0) 
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                        }
                                                                    }
                                                                    
                                                                    if(curr.bias_mode2==DENSITY)   // Current mode according to ID is DENSITY
                                                                    {
                                                                        if (debug>0) {   // DEBUG
                                                                            printf("    property2: %s = \"%s\"\n", property2->name, property2->value);
                                                                            printf("    macro_priority = %i\n", macro_priority);
                                                                        }
                                                                        // Is it so according to macro description, or macro description has high priority
                                                                        if(!strcmp(property2->value,"\"DENSITY\"") || macro_priority) {                                                                                        
                                                                            InsertTopK(&dict,property2->name,property2->value);   // Yes, let's set it
                                                                        }
                                                                        else {
                                                                            InsertTopQ(&dict,property2->name,"DENSITY"); //  No! Trust the ID code more.
                                                                        }
                                                                        
                                                                        //----------------------------------------------------------------------------------------------------
                                                                        // Need to keep old bias in case of no macro change..to accomodate for extra bias
                                                                        // settings outside of macros. /Unknown, before 2015-04-07
                                                                        //----------------------------------------------------------------------------------------------------
                                                                        // The macro 515 that was uploaded to Rosetta is malfunctioning and appears to use two different values
                                                                        // for VBIAS2 in the macro loop despite that this should never happen.
                                                                        // For the pds code to be able to change the bias during every macro loop, the code has to specificly
                                                                        // permit macro 515 to set VBIAS2 in _every_ macro loop, not just the first one.
                                                                        // NOTE: Macro 515 is also on the list of macros that can not receive extra, external bias settings
                                                                        // ("kommenderingar"). See WritePTAB_File, "if (extra_bias_setting) ..." .
                                                                        // NOTE: One could of course make the same exception for VBIAS1 to preserve the "symmetry between
                                                                        // the probes" but that is unnecessary since VBIAS1 is constant throughout the macro loop as intended.
                                                                        // /Erik P G Johansson, 2015-04-07
                                                                        //----------------------------------------------------------------------------------------------------
                                                                        //if(macro_id!=curr.old_macro)
                                                                        if ((macro_id!=curr.old_macro) || macro_id==0x515)
                                                                        {
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_VBIAS2",DNTCARE)>0) // Find voltage bias
                                                                            {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                                if(!sscanf(property2->value,"\"%x\"",&curr.vbias2))
                                                                                {
                                                                                    CPrintf("    Invalid fix voltage bias p2, using default 0x7f\n");
                                                                                    curr.vbias2=0x007f;
                                                                                }
                                                                                if (debug>0) {
                                                                                    printf("    Macro: Set fix voltage bias VBIAS2 = %04x (curr.vbias2; hex TM units)\n", curr.vbias2);  // DEBUG
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            InsertTopQV(&dict,"ROSETTA:LAP_VBIAS2",curr.vbias2); // No macro change keep old bias
                                                                        }
                                                                        
                                                                        
                                                                        if(param_type!=ADC20_PARAMS)
                                                                        { 
                                                                            // Find duration ADC16 Density
                                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_DENSITY_FIX_DURATION",DNTCARE)>0) {
                                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            
                                                            // Find LAP_P2_ADC16_FILTER
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16_FILTER",DNTCARE)>0) 
                                                            {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                sscanf(property2->value,"\"%d\"",&curr.afilter);
                                                            }
                                                            //  Find LAP P2_RX_OR_TX
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_RX_OR_TX",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_P2_STRATEGY_OR_RANGE
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_STRATEGY_OR_RANGE",DNTCARE)>0) 
                                                            {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                                strncpy(curr.gain2,property2->value,16);
                                                            }
                                                            
                                                            // Find LAP_P2_RANGE_DENS_BIAS "+-32" or "+-5"
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_RANGE_DENS_BIAS",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_P2_ADC16 mode
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_P2_ADC20 mode
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC20",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            // Find LAP_FEEDBACK_P2
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_FEEDBACK_P2",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                        }   // if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2)
                                                        
                                                        //===============================================================================================
                                                        
                                                        // If transmitter is turned on
                                                        if(curr.transmitter==SENS_P1 || curr.transmitter==SENS_P2)
                                                        {
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_TRANSMITTER_STATUS",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_TRANSMITTER_AMPLITUDE",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                            
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_TRANSMITTER_FREQUENCY",DNTCARE)>0) {
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            }
                                                        }
                                                        
                                                        // If we are currently transmitting on P1
                                                        if(curr.transmitter==SENS_P1)
                                                        {
                                                            //  Find LAP P1_RX_OR_TX
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_RX_OR_TX",DNTCARE)>0) 
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                        }
                                                        
                                                        // If we are currently transmitting on P2
                                                        if(curr.transmitter==SENS_P2)
                                                        {
                                                            //  Find LAP P2_RX_OR_TX
                                                            if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_RX_OR_TX",DNTCARE)>0) 
                                                                InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                        }
                                                        
                                                        // Find bootstrap
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_BOOTSTRAP",DNTCARE)>0) {
                                                            InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                        }
                                                        
                                                        // Find DAC Oscillator
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_OSCILLATOR",DNTCARE)>0) {
                                                            InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                        }
                                                        
                                                        // Find first occurrence of TM_RATE from start, if found set it in dictionary
                                                        if(FindP(&macros[mb][ma],&property2,"ROSETTA:LAP_TM_RATE",1,DNTCARE)>0)   // Set property2.
                                                        {
                                                            InsertTopK(&dict,property2->name,property2->value); // Set it in dictionary
                                                            
                                                            tm_rate=property2->value[1]; // First char as telemetry rate (M)inimum,(N)ormal,(B)urst skipping quote character
                                                            if(!strcmp(property2->value,"\"NONE\""))
                                                                tm_rate='Z';            // And (Z)ero if no telemetry rate
                                                        }
                                                        
                                                        // NOTE: DECEIVING IF STATEMENT.
                                                        // The "if" condition is an assignment and only returns false in case of error.
                                                        // Therefore the if statement will basically always be executed.
                                                        // NOTE: This is the only location where "aqps_seq" is assigned.
                                                        // NOTE: This entire section could basically be moved to S15_WRITE_PDS_FILES, since it fits with creating files there.
                                                        // However, this if statement is also contained within three other if statements:
                                                        //    if((aqps_seq=TotAQPs(&macros[mb][ma],meas_seq))>=0) { ... }
                                                        //    if(FindP(&macros[mb][ma],&property1,"ROSETTA:LAP_SET_SUBHEADER",meas_seq,DNTCARE)>0) { ... }
                                                        //    if(!macro_descr_NOT_found) { ... }
                                                        // and one might not want to move the middle one to S15_WRITE_PDS_FILES(?).
                                                        // /Erik P G Johansson 2016-03-10
                                                        if((aqps_seq=TotAQPs(&macros[mb][ma],meas_seq))>=0)
                                                        {
                                                            CPrintf("    %d sequence starts %d aqps from start of sequence\n",meas_seq,aqps_seq);
                                                            curr.offset_time=aqps_seq*32.0;
                                                            curr.seq_time=rstime+curr.offset_time;      // Calculate time of current sequence
                                                            
                                                            Raw2OBT_Str(curr.seq_time,pds.SCResetCounter,tstr1); // Compile OBT string and add reset number of S/C clock
                                                            
                                                            SetP(&comm,"SPACECRAFT_CLOCK_START_COUNT",tstr1,1);
                                                            
                                                            DecodeRawTime(curr.seq_time,tstr1,0);   // Decode raw time into PDS compliant UTC time
                                                            CPrintf("    Current sequence start time is: %s\n",tstr1); 
                                                            SetP(&comm,"START_TIME",tstr1,1);
                                                            
                                                            
                                                            // Create data path for current day
                                                            tstr1[10]='\0';                 // Truncate hh:mm:ss away and keep CCYY-MM-DD

                                                            // Test if RPCLAPYYMMDD structure exists for current time, if not, create necessary directories.
                                                            StrucDir(tstr1, pds.dpathse, pds.spaths);
                                                            
                                                            // Replace "-" in CCYY-MM-DD by null terminators
                                                            // so we can convert date from CCYY-MM-DD into YYMMDD.
                                                            tstr1[4]='\0';
                                                            tstr1[7]='\0';
                                                            
                                                            // Get highest alphanumeric value in filenames in dpathse matching pattern "RPCLAPYYMM*_*_*S.LBL".
                                                            // This causes the alphanum value to restart at zero every new day.
                                                            // (Any matching days from previous runs are not overwritten until alphanum wraps
                                                            // but that means identical data are stored twice in the same data set and that should not be done.)
                                                            //
                                                            sprintf(tstr3,"RPCLAP%s%s*_*_*S.LBL",&tstr1[2],&tstr1[5]);
                                                            GetAlphaNum(alphanum_s, pds.spaths, tstr3); 
                                                            IncAlphaNum(alphanum_s);         // Increment alphanumeric value


                                                            //=======================================================
                                                            // Construct (tentative) file names (SCI) and product ID
                                                            //=======================================================
                                                            // 0000000000111111111122222222223
                                                            // 012345678901234567890123456.890
                                                            // RPCLAPYYMMDD-2Z7S-RDB14NSXX.EXT 
                                                            // 
                                                            // This is a 27.3 file name and it's accepted in PDS3.
                                                            // Currently we have 2 unused characters.
                                                            // See the EAICD (document) for details on file naming convention.
                                                            // [16] : T=20 bit, S=16 bit
                                                            // [18] : [C]alibrated or Calibrated [R]aw
                                                            // [19] : [E]-Field or [D]ensity
                                                            // [20] : [S]weep or [B]ias.
                                                            // [21] : P[1], P[2], P[3].
                                                            // Note: [i] : i=byte index in string (i=0: first character)
                                                            // NOTE: Sets probe number and density/E field to "x" since these will be overwritten later (case S15_WRITE_PDS_FILES).
                                                            sprintf(tstr2, "RPCLAP%s%s%s_%sS_RxBx%1d%cS",
                                                                    &tstr1[2], &tstr1[5], &tstr1[8],
                                                                    alphanum_s, curr.afilter, tm_rate);     // Compile product ID=base filename (filename without extension).
                                                                    
                                                            CPrintf("    Tentative basis for filename & product ID: tstr2=\"%s\"\n", tstr2);    // DEBUG

                                                            if(param_type==ADC20_PARAMS)    { tstr2[16]='T'; }  // Set to [T]wenty bit ADC:s or keep [S]ixteen bit.
                                                            if(calib)                       { tstr2[18]='C'; }  // Set to [C]alibrated or keep Calibrated [R]aw.
                                                            if(param_type==SWEEP_PARAMS)    { tstr2[20]='S'; }  // Set to [S]weep or keep [B]ias.

                                                            strcpy(lbl_fname,tstr2);              // Save product ID as label filename.
                                                            strcpy(tab_fname,tstr2);              // Save product ID as table filename.
                                                            strcat(lbl_fname,".LBL");             // Add .LBL extension
                                                            strcat(tab_fname,".TAB");             // Add .TAB extension
                                                            
                                                            // NOTE: PRODUCT_ID, FILE_NAME, ^TABLE are modified later when writing file.
                                                            sprintf(prod_id,"\"%s\"",tstr2);      // Add PDS quotes ".." 
                                                            //SetP(&comm,"PRODUCT_ID",prod_id,1);   // Set PRODUCT ID in common PDS parameters.
                                                            
                                                            //sprintf(tstr1,"\"%s\"",lbl_fname);    // Add PDS quotes ".." 
                                                            //SetP(&comm,"FILE_NAME",tstr1,1);      // Set filename in common PDS parameters

                                                            //sprintf(tstr1,"\"%s\"",tab_fname);    // Add PDS quotes ".." 
                                                            //SetP(&comm,"^TABLE",tstr1,1);         // Set link to table in common PDS parameters


                                                            curr.factor=0.0; // Init
                                                            // Compute stop time of current sequence
                                                            if(param_type==SWEEP_PARAMS)
                                                            {
                                                                // (una sensore at one time for swiping you sii!)
                                                                curr.factor=sw_info.sweep_dur_s/SAMP_FREQ_ADC16/samples; // Factor for sweeps!
                                                            }
                                                            else
                                                            {
                                                                
                                                                if(param_type!=ADC20_PARAMS)
                                                                {
                                                                    if(dsa16_p1==-1) // -1 here => Probably generic macro
                                                                    {
                                                                        dsa16_p1=1;
                                                                        CPrintf("    Warning, parameter not resolved probably generic macro, using default no resampling on ADC16 P1\n");
                                                                    }
                                                                    
                                                                    if(dsa16_p2==-1) // -1 here => Probably generic macro
                                                                    {
                                                                        dsa16_p2=1;
                                                                        CPrintf("    Warning, parameter not resolved probably generic macro, using default no resampling on ADC16 P2\n");
                                                                    }
                                                                }
                                                                
                                                                switch(curr.sensor)
                                                                {
                                                                    case SENS_P1:
                                                                        if(param_type==ADC20_PARAMS)
                                                                            curr.factor=a20_info.resampling_factor/SAMP_FREQ_ADC20; 
                                                                        else
                                                                            curr.factor=dsa16_p1/SAMP_FREQ_ADC16;
                                                                        break;
                                                                    case SENS_P2:
                                                                    case SENS_P1P2: // In this case dsa16_p1==dsa16_p2
                                                                        if(param_type==ADC20_PARAMS)
                                                                            curr.factor=a20_info.resampling_factor/SAMP_FREQ_ADC20; 
                                                                        else
                                                                            curr.factor=dsa16_p2/SAMP_FREQ_ADC16; 
                                                                        break;
                                                                    default:
                                                                        curr.factor=1/SAMP_FREQ_ADC16; 
                                                                        break;
                                                                } 
                                                            }
                                                            curr.stop_time=curr.seq_time+(samples-1)*curr.factor;   // Calculate current stop time.
                                                            
                                                            Raw2OBT_Str(curr.stop_time, pds.SCResetCounter, tstr5);   // Compile OBT string and add reset number of S/C clock.
                                                            
                                                            SetP(&comm,"SPACECRAFT_CLOCK_STOP_COUNT",tstr5,1);
                                                            
                                                            DecodeRawTime(curr.stop_time, tstr5, 0);  // Decode raw time into PDS compliant UTC time.
                                                            CPrintf("    Current sequence stop  time is: %s\n",tstr5);
                                                            SetP(&comm,"STOP_TIME",tstr5,1);         // Update STOP_TIME in common PDS parameters.
                                                        }   // if((aqps_seq=TotAQPs(&macros[mb][ma],meas_seq))>=0)
                                                    }   // if(FindP(&macros[mb][ma],&property1,"ROSETTA:LAP_SET_SUBHEADER",meas_seq,DNTCARE)>0)
                                                }   // if(!macro_descr_NOT_found)
                                                else
                                                {
                                                    //================================================================
                                                    // CASE: No macro description fits and no anomaly overide exists.
                                                    //================================================================
                                                    // Derive all that we can without anomaly override and send it to log file.
                                                    // Problematic data are stored in the UnAccepted_Data directory.
                                                    // case S15_WRITE_PDS_FILES uses "macro_descr_NOT_found" to know that the data needs a different treatment.
                                                    
                                                    CPrintf("    No macro description fits, data will be stored in the UnAccepted_Data directory\n");
                                                    if(param_type==NO_PARAMS)
                                                    {
                                                        if(dsa16_p1==-1) 
                                                        {
                                                            dsa16_p1=1; // Conversion errors use default 1
                                                            CPrintf("    Warning, parameter not resolved, using default no resampling on ADC16 P1\n");
                                                        }
                                                        
                                                        if(dsa16_p2==-1) 
                                                        {
                                                            dsa16_p2=1; // Conversion errors use default 1 
                                                            CPrintf("    Warning, parameter not resolved, using default no resampling on ADC16 P2\n");
                                                        }
                                                    }
                                                    
                                                    Raw2OBT_Str(rstime,pds.SCResetCounter,tstr1); // Compile OBT string and add reset number of S/C clock
                                                    
                                                    SetP(&comm,"SPACECRAFT_CLOCK_START_COUNT",tstr5,1);
                                                    CPrintf("    OBT time start of measurement cycle: %s \n",tstr5);
                                                    
                                                    if(param_type==NO_PARAMS) 
                                                    {
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P1 %d/[samples]\n",dsa16_p1*samples);
                                                        }
                                                        if(curr.sensor==SENS_P2) {
                                                            CPrintf("    Duration P2 %d/[samples]\n",dsa16_p2*samples);
                                                        }
                                                    }
                                                    
                                                    if(param_type==SWEEP_PARAMS)
                                                    {                                                            
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P1 %d (sweep)\n",sw_info.sweep_dur_s);
                                                        }                                                            
                                                        if(curr.sensor==SENS_P2) {
                                                            CPrintf("    Duration P2 %d (sweep)\n",sw_info.sweep_dur_s);
                                                        }
                                                    }
                                                    
                                                    if(param_type==ADC20_PARAMS)
                                                    { 
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P1\n",samples*a20_info.resampling_factor);
                                                        }                                                            
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P2\n",samples*a20_info.resampling_factor);
                                                        }                                                            
                                                        if(curr.sensor==SENS_P1P2) {
                                                            CPrintf("    Duration P1 & P2\n",samples*a20_info.resampling_factor);
                                                        }
                                                    }
                                                }   // if(!macro_descr_NOT_found) ... else ...
                                                
                                                // Downsampling values should be resolved at this point so let's put them in!
                                                // This part is always executed if we have a macro description or not.
                                                if(param_type==NO_PARAMS || param_type==SWEEP_PARAMS)
                                                {
                                                    if(curr.sensor==SENS_P1 || curr.sensor==SENS_P1P2) {
                                                        InsertTopQV(&dict,"ROSETTA:LAP_P1_ADC16_DOWNSAMPLE",dsa16_p1);
                                                    }                                                        
                                                    if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2) {
                                                        InsertTopQV(&dict,"ROSETTA:LAP_P2_ADC16_DOWNSAMPLE",dsa16_p2);
                                                    }
                                                }
                                                state=S15_WRITE_PDS_FILES; // Change state
                                                break;
                                                
                                                case S15_WRITE_PDS_FILES:
                                                    DispState(state,"STATE = S15_WRITE_PDS_FILES\n");
                                                    
                                                    //------------------------------------------------------------------------
                                                    // Erik P G Johansson 2015-03-25: Added functionality for excluding data.
                                                    // 
                                                    // Determine whether to include/exclude entire TAB&LBL file pair.
                                                    // Can be compared with the (CALIB) macro exclusion check.
                                                    // NOTE: DecideWhetherToExcludeData requires SPACECRAFT_CLOCK_START_COUNT, SPACECRAFT_CLOCK_STOP_COUNT to have been set.
                                                    //------------------------------------------------------------------------
                                                    int excludeData = 0;   // Boolean flag.
                                                    if (dataExcludeTimes != NULL) {
                                                        if (DecideWhetherToExcludeData(dataExcludeTimes, &comm, &excludeData)) {
                                                            YPrintf("DecodeScience: Error when trying to determine whether to exclude data. - Keeps data by default.\n");
                                                            printf( "DecodeScience: Error when trying to determine whether to exclude data. - Keeps data by default.\n");
                                                        } else {
                                                            if (excludeData) {
                                                                CPrintf("DecodeScience: Excluding data.\n");  // Really superfluous printout since DecideWhetherToExcludeData also prints.
                                                                ClearDictPDS(&dict);   // Clear dict     since that is what the macro CALIB macro exclusion code does.
                                                                state=S04_GET_ID_CODE; // Set this state since that is what the macro CALIB macro exclusion code does.
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    
                                                    // ------------------------------------------------------------------------------------
                                                    // Look for the existence of a macro ID before checking for (CALIB) macros to exclude.
                                                    // Early macros (before the first flight software update in space) did not have macro
                                                    // descriptions/IDs since that functionality had not been implemented yet.
                                                    // /Erik P G Johansson summarizing Reine Gill 2015-03-26.
                                                    // ------------------------------------------------------------------------------------
                                                    if(!macro_descr_NOT_found) // IF we have a macro description (macro_descr_NOT_found==0 means we have).
                                                    {
                                                        if(calib)
                                                        {
                                                            for(i=0;i<nexcl;i++)
                                                            {
                                                                if(exclude[i]==macro_id) // Exclude current macro..in case of calibration macro
                                                                {
                                                                    CPrintf("Excluding calibration macro: 0x%04x\n",macro_id);
                                                                    state=S04_GET_ID_CODE;
                                                                    break;
                                                                }
                                                            }
                                                            if(state==S04_GET_ID_CODE) {
                                                                break; // Double break :)
                                                            }
                                                        }
                                                        
                                                        // WRITE TO INDEX.TAB
                                                        //
                                                        // Example row:
                                                        //000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111111111111111111111111111111111111111111111111
                                                        //000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444444444
                                                        //123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234566667777
                                                        //"DATA/CALIBRATED/2003/JAN/D16/ ","RPCLAP030116_01NS_REB20BS.LBL","2004-04-15T12:50:22","2003-01-16T08:20:59.207","2003-01-16T08:20:59.316","MCID0x212"<CR><LF>
                                                        
                                                        FindP(&comm,&property1,"INSTRUMENT_MODE_ID",1,DNTCARE);    // Get macro ID
                                                        FindP(&comm,&property2,"PRODUCT_CREATION_TIME",1,DNTCARE); // Get product creation time
                                                        //FindP(&comm,&property3,"START_TIME",1,DNTCARE);          // Get start time
                                                        //FindP(&comm,&property4,"STOP_TIME",1,DNTCARE);           // Get stop time
                                                        
                                                        
                                                        // Find position there the root of the PDS archive starts
                                                        ti1=strlen(pds.apathpds); 
                                                        
                                                        
                                                        if(param_type==SWEEP_PARAMS)
                                                        {
                                                            // Below we compensate for a well known bug during sweeping.
                                                            // All sweeps start with two initial plateaus, with the same width as the 
                                                            // plateaus in the sweep part itself. And at the previous bias voltage.
                                                            // Now the first initial plateau has a corrupted length that is varying.
                                                            // We could simply truncate it off! But instead we figure out by how
                                                            // much it varies! Everything to get the correct current vector!
                                                            // Furthermore, one sample is always missing at the end of a sweep!
                                                            
                                                            if(curr.sensor==SENS_P1) {
                                                                samp_plateau=sw_info.plateau_dur/dsa16_p1; // Samples on one plateau
                                                            } else {
                                                                samp_plateau=sw_info.plateau_dur/dsa16_p2; // Samples on one plateau
                                                            }
                                                            
                                                            ini_samples=samples+1-(sw_info.steps+1)*samp_plateau; // Initial samples before sweep starts
                                                        }
                                                        
                                                        //##########################################################################
                                                        // WRITE TO DATA LABEL FILE (.LBL), TABLE FILE (.TAB), and add to INDEX.TAB
                                                        //##########################################################################
                                                        // NOTE: data_type!=D20 && data_type!=D20T
                                                        // <=>   (curr.sensor != SENS_P1P2 || data_type==D16)
                                                        // <=>   Not (ADC20 and both probes).
                                                        // NOTE: The if condition _APPEARS_TO_BE_ a very crude way of determining
                                                        // if-and-only-if there is data from exactly one probe (P3 counts as one probe).
                                                        // Not sure why it must should work, but it does seem to be consistent with id.h: no subheaders read ADC16 from both probes.
                                                        // Why not use (curr.sensor==SENS_P1 || curr.sensor==SENS_P2), or curr.sensor!=SENS_P1P2?
                                                        // /Erik P G Johansson 2016-03-10
                                                        if(data_type!=D20 && data_type!=D20T)
                                                        {
                                                            //=====================================================
                                                            // CASE: There is data for exactly one probe (dop==0).
                                                            // ---------------------------------------------------
                                                            // NOTE: P3 counts as one probe here.
                                                            //=====================================================
                                                            if (debug >= 1) {
                                                                CPrintf("    Creating LBL/TAB file pair (dop=0) - There is data for exactly one probe (?).\n");    // DEBUG
                                                            }                                                            

                                                            /*// Modify filenames and product ID.
                                                            {
                                                                // Set to [E]-Field or keep [D]ensity.
                                                                char tempChar;
                                                                if(getBiasMode(&curr, 0)==E_FIELD) {
                                                                    tempChar = 'E';
                                                                } else {
                                                                    tempChar = 'D';
                                                                }
                                                                lbl_fname[19]=tempChar;
                                                                tab_fname[19]=tempChar;
                                                                prod_id[19+1]=tempChar;
                                                            }                                                            
                                                            sprintf(tstr1, "%1d", curr.sensor);
                                                            lbl_fname[21]=tstr1[0];
                                                            tab_fname[21]=tstr1[0];
                                                            prod_id[21+1]=tstr1[0];

                                                            sprintf(tstr2,"%s%s",&pds.spaths[ti1],lbl_fname); // Put together file name without base path.
                                                            ExtendStr(tstr4,tstr2,58,' ');                    // Make a new string extended with whitespace to 58 characters.
                                                            
                                                            // Name changed
                                                            SetP(&comm,"PRODUCT_ID",prod_id,1);   // Change PRODUCT ID in common PDS parameters.
                                                            sprintf(tstr1,"\"%s\"",lbl_fname);    // Add PDS quotes ".." 
                                                            SetP(&comm,"FILE_NAME",tstr1,1);      // Set filename in common PDS parameters
                                                            sprintf(tstr3,"\"%s\"",tab_fname);    // Add PDS quotes ".." 
                                                            SetP(&comm,"^TABLE",tstr3,1);         // Set link to table in common PDS parameters
                                                            
                                                            if(WritePLBL_File(pds.spaths,lbl_fname,&curr,samples,id_code, 0, ini_samples,param_type)>=0)
                                                            {
                                                                WritePTAB_File(
                                                                    buff,tab_fname,data_type,samples,id_code,length,&sw_info,&curr,param_type,dsa16_p1,dsa16_p2, 0,
                                                                    &m_conv,bias,nbias,mode,nmode,ini_samples,samp_plateau);
                                                                
                                                                strncpy(tstr2,lbl_fname,29);
                                                                tstr2[25]='\0';
                                                                
                                                                WriteToIndexTAB(tstr4, tstr2, property2->value);
                                                            }*/
                                                            WriteTABLBL_FilePair(0, curr.sensor);
                                                        }
                                                        else // Split interleaved 20 Bit data into two pairs of label and tab files
                                                        {
                                                            //===========================================
                                                            // CASE: There is data for exactly 2 probes.
                                                            //===========================================

                                                            //====================
                                                            // Handle dop==1 (P1)
                                                            //====================
                                                            if (debug >= 1) {
                                                                CPrintf("    Creating LBL/TAB file pair for P1 data (dop=1) - There is data for exactly two probes.\n");    // DEBUG
                                                            }
                                                            
                                                            /*// Modify filenames and product ID.
                                                            {
                                                                // Set to [E]-Field or keep [D]ensity.
                                                                char tempChar;
                                                                if(getBiasMode(&curr, 1)==E_FIELD) {
                                                                    tempChar = 'E';
                                                                } else {
                                                                    tempChar = 'D';
                                                                }
                                                                lbl_fname[19]=tempChar;
                                                                tab_fname[19]=tempChar;
                                                                prod_id[19+1]=tempChar;
                                                            }
                                                            lbl_fname[21]='1';
                                                            tab_fname[21]='1';
                                                            prod_id[21+1]='1';
                                                            
                                                            sprintf(tstr2,"%s%s",&pds.spaths[ti1],lbl_fname); // Put together file name without base path.
                                                            ExtendStr(tstr4,tstr2,58,' ');                    // Make a new string extended with whitespace to 58 characters.
                                                            
                                                            // Name changed
                                                            SetP(&comm,"PRODUCT_ID",prod_id,1);   // Change PRODUCT ID in common PDS parameters.
                                                            sprintf(tstr1,"\"%s\"",lbl_fname);    // Add PDS quotes ".." 
                                                            SetP(&comm,"FILE_NAME",tstr1,1);      // Set filename in common PDS parameters
                                                            sprintf(tstr3,"\"%s\"",tab_fname);    // Add PDS quotes ".." 
                                                            SetP(&comm,"^TABLE",tstr3,1);         // Set link to table in common PDS parameters
                                                            
                                                            if(WritePLBL_File(pds.spaths,lbl_fname,&curr,samples,id_code, 1, ini_samples,param_type)>=0)
                                                            {
                                                                WritePTAB_File(
                                                                    buff,tab_fname,data_type,samples,id_code,length,&sw_info,&curr,param_type,dsa16_p1,dsa16_p2, 1,
                                                                    &m_conv,bias,nbias,mode,nmode,ini_samples,samp_plateau);
                                                                
                                                                strncpy(tstr2,lbl_fname,29);
                                                                tstr2[25]='\0';
                                                                
                                                                WriteToIndexTAB(tstr4, tstr2, property2->value);
                                                            }*/
                                                            WriteTABLBL_FilePair(1, 1);
                                                            

                                                            
                                                            //====================
                                                            // Handle dop==2 (P2)
                                                            //====================
                                                            if (debug >= 1) {
                                                                CPrintf("    Creating LBL/TAB file pair for P2 data (dop=2) - There is data for exactly two probes.\n");    // DEBUG
                                                            }
                                                            
                                                            /*// Modify filenames and product ID.
                                                            {
                                                                // Set to [E]-Field or keep [D]ensity.
                                                                char tempChar;
                                                                if(getBiasMode(&curr, 2)==E_FIELD) {
                                                                    tempChar = 'E';
                                                                } else {
                                                                    tempChar = 'D';
                                                                }
                                                                lbl_fname[19]=tempChar;
                                                                tab_fname[19]=tempChar;
                                                                prod_id[19+1]=tempChar;
                                                            }
                                                            lbl_fname[21]='2';
                                                            tab_fname[21]='2';
                                                            prod_id[21+1]='2';
                                                            
                                                            sprintf(tstr2,"%s%s",&pds.spaths[ti1],lbl_fname); // Put together file name without base path.
                                                            ExtendStr(tstr4,tstr2,58,' ');                    // Make a new string extended with whitespace to 58 characters.
                                                            
                                                            // Name changed
                                                            SetP(&comm,"PRODUCT_ID",prod_id,1);   // Change PRODUCT ID in common PDS parameters.
                                                            sprintf(tstr1,"\"%s\"",lbl_fname);    // Add PDS quotes ".." 
                                                            SetP(&comm,"FILE_NAME",tstr1,1);      // Set filename in common PDS parameters
                                                            sprintf(tstr3,"\"%s\"",tab_fname);    // Add PDS quotes ".." 
                                                            SetP(&comm,"^TABLE",tstr3,1);         // Set link to table in common PDS parameters
                                                            
                                                            if(WritePLBL_File(pds.spaths,lbl_fname,&curr,samples,id_code, 2, ini_samples,param_type)>=0)
                                                            {
                                                                WritePTAB_File(buff,tab_fname,data_type,samples,id_code,length,&sw_info,&curr,param_type,dsa16_p1,dsa16_p2, 2,
                                                                    &m_conv,bias,nbias,mode,nmode,ini_samples,samp_plateau);
                                                                
                                                                strncpy(tstr2,lbl_fname,29);
                                                                tstr2[25]='\0';
                                                                
                                                                WriteToIndexTAB(tstr4, tstr2, property2->value);
                                                            }*/
                                                            WriteTABLBL_FilePair(2, 2);
                                                        }
                                                        
                                                        ClearDictPDS(&dict);   // Clear dictionary PDS LAP parameters, common parameters are not cleared until a new measurement cycle beginns
                                                    }   // if(!macro_descr_NOT_found)
                                                    else
                                                    {
                                                        // CASE: A macro description could not be found.
                                                        // 
                                                        // If fingerprinting was enabled it must have failed.
                                                        // Anomaly correction must also have failed at this point.
                                                        // 
                                                        // Data will be stored in the UnAccepted_Data directory instead
                                                        // and requires manual attention.
                                                        YPrintf("Macro description missing, data stored in UnAccepted_Data\n"); // Put a note in the system log
                                                        CPrintf("    Dump data to %s\n",pds.uapath); // No macro description dump to unaccepted files
                                                        
                                                        if(pds.uaccpt_fd==NULL)
                                                        {
                                                            if(GetUnacceptedFName(tstr1)<0) {  // Get a unnaccepted filename
                                                                CPrintf("    Can't create filename for unaccepted data, uses default\n");
                                                            } else {
                                                                CPrintf("    UnAccepted file: %s\n",tstr1);
                                                            }
                                                            strcpy(tstr2,pds.uapath);
                                                            strcat(tstr2,tstr1);
                                                            if((pds.uaccpt_fd=fopen(tstr2,"w"))==NULL) {
                                                                CPrintf("    Couldn't open data file for unaccepted data?\n");
                                                            }
                                                        }
                                                        
                                                        if(pds.uaccpt_fd!=NULL)
                                                        {
                                                            val=ct->fill-1;
                                                            if(val>0)
                                                            {
                                                                GetBuffer(ct,buff,val); 
                                                                fwrite(buff,1,val,pds.uaccpt_fd);
                                                                fflush(pds.uaccpt_fd); // Flush it!
                                                            }
                                                        }
                                                    }    // if (!macro_descr_NOT_found) ... else ...
                                                    state=S04_GET_ID_CODE;
                                                    break;     
                                                    default:
                                                        DispState(state,"STATE = UNKNOWN\n");
                                                        break;
        }
    }   // while
    return 0;
    
}   // DecodeScience



// Signal handler
//----------------------------------------------------------------------------------------------------------------------------------

// Signal handler to capture Ctrl-C, to exit gracefully
// exit_gracefully is atomic to prevent it from being changed
// Then someone accesses it!
//
static void ExitWithGrace(int signo)
{
    exit_gracefully=1; // This means we should exit, When we are ready to do so!
    write(STDOUT_FILENO,"Preparing to close at convenient opportunity..\n",68); // Can not use printf in signal handler!!
}

// Logging and exit functions
//----------------------------------------------------------------------------------------------------------------------------------

// Closes logging and exits with status
void ExitPDS(int status)
{
    char tstr1[8192]; // Temporary string
    char tstr2[PATH_MAX+1]; // Temporary string
    char tstr3[64];         // Temporary string for UTC time
    
    FILE *pipe_fp;
    int len;
    int i;
    
    struct timespec dose={0,DOSE_TIME};  
    
    time_t t_start_wait, t_now, t_last_log_msg;
    unsigned int sc_buff_fill;
    double t_wait_elapsed;
    
    // Struct parameter required by nanosleep function. struct = {seconds, nanoseconds}.
    const struct timespec SC_THREAD_CANCEL_POSTTHRESHOLD_DELAY_struct = {(long int) SC_THREAD_CANCEL_POSTTHRESHOLD_DELAY, 0};
    
    const double min_time_between_log_messages = 10.0;
    
    if(pds.ylog_fd!=NULL) {
        YPrintf("Exiting pds, freeing memory, closing threads, closing file descriptors and compressing logs.\n");
    }
    
    
    if(scithread>0)  
    {
        YPrintf("Trying to cancel the science thread.\n");
        if (status == 0) {
            
            //-------------------------------------------------------------------------------------------------------------------
            // BUG FIX:
            // Erik P G Johansson 2015-03-31: Added functionality for delaying the termination of the DecodeScience thread until
            //                                it has actually finished, or almost finished.
            //
            // It has been previously observed (pds v3.07/c3.08) that the DecodeScience thread may otherwise sometimes be terminated
            // too early and thus the last hours of data in an archive are never written to disk. Even with this bug fix, small
            // amounts of data (minutes) have still been observed to be missing from the end of the last day in an archive but this
            // might have other explanations.
            // NOTE: DecodeScience does not appear to have been designed to actually quit when it runs out of data and it appears
            // difficult/dangerous to modify it and pds so that it does.
            // NOTE: It is dangerous to assume that the circular science buffer will go down to exactly zero bytes
            // (data could be corrupt or incomplete). Therefore code uses a threshold plus an additional fixed delay for safety.
            //-------------------------------------------------------------------------------------------------------------------
            YPrintf("Waiting up to %u s to for the science thread to empty the circular science buffer (go below %u bytes).\n",
                    sc_thread_cancel_threshold_timeout,
                    SC_THREAD_CANCEL_BUFF_SIZE_THRESHOLD);
            
            t_start_wait   = time(NULL);
            t_last_log_msg = t_start_wait; // Time of previous log message. (Initial value is not meaningful, but that is OK.)
            
            do {
                pthread_yield();
                nanosleep(&dose,NULL);   // Both pthread_yield() and nanosleep() unnecessary?!
                
                t_now = time(NULL);
                t_wait_elapsed = difftime(t_now, t_start_wait);          
                sc_buff_fill = GetBufferFill(&cbs);
                
                if (difftime(t_now, t_last_log_msg) > min_time_between_log_messages)
                {
                    YPrintf("   Waited %2.0f s; %u bytes left\n", t_wait_elapsed, sc_buff_fill);   // Disable log message?
                    t_last_log_msg = t_now;
                }
                
            } while ((sc_buff_fill > (unsigned int) SC_THREAD_CANCEL_BUFF_SIZE_THRESHOLD) && (t_wait_elapsed < sc_thread_cancel_threshold_timeout));
            //nanosleep(&dose,NULL);
            
            YPrintf("   Wait further %u s.\n", SC_THREAD_CANCEL_POSTTHRESHOLD_DELAY);   // Disable log message?
            nanosleep(&SC_THREAD_CANCEL_POSTTHRESHOLD_DELAY_struct, NULL); // Give the science thread some time to empty the remaining circular buffer contents.
            sc_buff_fill = GetBufferFill(&cbs);        
            YPrintf("Trying to cancel science thread again. %u bytes remaining in science buffer.\n", sc_buff_fill);   // Disable log message?
            
            pthread_cancel(scithread);
            pthread_join(scithread,NULL); // Wait for Science thread to exit
            
        } else {
            
            pthread_cancel(scithread);
            pthread_join(scithread,NULL); // Wait for Science thread to exit       
            
        }
        YPrintf("Science thread has been cancelled.\n");   // Disable log message?
    }
    
    if(hkthread>0)   
    {
        pthread_cancel(hkthread);
        pthread_join(hkthread,NULL); // Wait for HK thread to exit
    }
    
    if(sctmthread>0) 
    {
        pthread_cancel(sctmthread);
        pthread_join(sctmthread,NULL); // Wait for S/C TM thread to exit
    }
    
    // Last HK Label file is not closed properly
    if(pds.hlabel_fd!=NULL)
    {
        SetP(&hkl,"SPACECRAFT_CLOCK_STOP_COUNT",hk_info.obt_time_str,1);	// Set OBT stop time
        SetP(&hkl,"STOP_TIME",hk_info.utc_time_str,1);			// Update STOP_TIME in common PDS parameters
        sprintf(tstr1,"%d",hk_info.hk_cnt);
        SetP(&hkl,"FILE_RECORDS",tstr1,1);            	                // Set number of records
        SetP(&hkl,"ROWS",tstr1,1);				       	// Set number of rows
        FDumpPrp(&hkl,pds.hlabel_fd);                                     // Dump hkl to HK label file
        fprintf(pds.hlabel_fd,"END\r\n");
        fclose(pds.hlabel_fd);
    }
    
    if(pds.itable_fd!=NULL) 
    {
        WriteIndexLBL(&ind,&mp);   // Write index label file
        fflush(pds.itable_fd);     // Flush and close
        fclose(pds.itable_fd);     // Close table file
    }
    
    if(comm.no_prop!=0 && comm.properties!=NULL) // Something in comm structure?
        FreePrp(&comm);                           // Then free comm memory
        
        if(dict.no_prop!=0 && dict.properties!=NULL) // Something in dict structure?
        {
            //printf("%d %p\n",dict.no_prop,dict.properties);
            fflush(stdout);
            FreePrp(&dict);                           // Then free dict memory
        }
        
        if(anom.no_prop!=0 && anom.properties!=NULL) {  // Something in anom structure?
            FreePrp(&anom);                             // Then free anom memory
        }
            
            FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
            
            
            if(calib)
            {
                if(v_conv.C!=NULL) FreeDoubleMatrix(v_conv.C,v_conv.rows,v_conv.cols);
                if(i_conv.C!=NULL) FreeDoubleMatrix(i_conv.C,v_conv.rows,i_conv.cols);
                if(f_conv.C!=NULL) FreeDoubleMatrix(f_conv.C,f_conv.rows,f_conv.cols);
                
                if(m_conv.CF!=NULL) free(m_conv.CF);
                if(m_conv.CD!=NULL && m_conv.n!=0)
                {
                    for(i=0;i<m_conv.n;i++)
                    {
                        if(m_conv.CD[i].C!=NULL && m_conv.CD[i].rows!=0 && m_conv.CD[i].cols!=0)
                            free(m_conv.CD[i].C);
                    }
                    free(m_conv.CD);
                }
            }
            
            if(tcp.netries>0)
            {
                if(tcp.SCET!=NULL)
                    free(tcp.SCET);
                
                if(tcp.offset!=NULL)
                    free(tcp.offset);
                
                if(tcp.gradient!=NULL)
                    free(tcp.gradient);
            }
            
            
            if(pds.ylog_fd!=NULL)
            {
                fflush(pds.ylog_fd);    // Flush and close
                fclose(pds.ylog_fd);    // Close logging file
            }
            
            if(pds.dlog_fd!=NULL)
            {
                fflush(pds.dlog_fd);    // Flush and close
                fclose(pds.dlog_fd);    // Close logging file
            }
            
            if(pds.plog_fd!=NULL) 
            {
                fflush(pds.plog_fd);    // Flush and close
                fclose(pds.plog_fd);    // Close logging file
            }
            
            if(pds.clog_fd!=NULL) 
            {
                fflush(pds.clog_fd);    // Flush and close
                fclose(pds.clog_fd);    // Close logging file
            }
            
            if(pds.hlog_fd!=NULL) 
            {
                fflush(pds.hlog_fd);    // Flush and close
                fclose(pds.hlog_fd);    // Close logging file
            }
            
            if(pds.ilabel_fd!=NULL) 
            {
                fflush(pds.ilabel_fd);    // Flush and close
                fclose(pds.ilabel_fd);    // Close label file
            }
            
            if(pds.ddsp_fd!=NULL) 
            {
                fflush(pds.ddsp_fd);    // Flush and close
                fclose(pds.ddsp_fd);    // Close progress file
            }
            
            
            // Changing group and permissions
            printf("Changing archive permissions and group info\n");  
            sprintf(tstr1,"chown -R :rosetta %s;chmod -R g+rw %s",pds.apathpds,pds.apathpds); // Setup shell command line.
            printf("%s\n",tstr1);
            
            pipe_fp = popen(tstr1,"r"); // Runs shell commands
            
            while(!feof(pipe_fp)) // Read out stdout from executing shell commands
            {
                if((len=fread(tstr2,1,PATH_MAX,pipe_fp)>0))
                {
                    tstr2[len]='\0'; // Ensure null terminated string!
                    printf("%s",tstr2); // Dump stdout from commands
                }
                nanosleep(&dose,NULL); // Prevent hogging
            }
            pclose(pipe_fp); // Close pipeline
            
            // Start to compress log files. External shell commands cd, tar and gzip must exist!
            printf("Compressing log files, please wait\n");
            GetUTime(tstr3);                                                    // Get local UTC time.
            ReplCh(tstr3,':','#');                                              // Replace : in time string with #, since 
            // : is not allowed in a file name.
            sprintf(tstr1,"cd %s;tar -czvf logs_%s.tgz *.log",pds.lpath,tstr3); // Setup shell command line.
            printf("%s\n",tstr1);
            
            pipe_fp = popen(tstr1,"r"); // Runs shell commands
            
            while(!feof(pipe_fp)) // Read out stdout from executing shell commands
            {
                if((len=fread(tstr2,1,PATH_MAX,pipe_fp)>0))
                {
                    tstr2[len]='\0'; // Ensure null terminated string!
                    printf("%s",tstr2); // Dump stdout from commands
                }
                nanosleep(&dose,NULL); // Prevent hogging
            }
            pclose(pipe_fp); // Close pipeline
            
            printf("\nExiting, status: %d\n",status); // Print exit status
            fflush(stdout); 
            
            exit(status);                             // Exit with return code "status"
}   // ExitPDS



// Like printf but everything goes into PDS_LOG0.
// If it can't open, then messages are printed to stderr.
int YPrintf(const char *fmt, ...) 
{
    int status;
    va_list args;
    char strp[32];
    int oldstate;
    
    // We do not want to cancel on default cancel points in fprintf,vfprintf
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldstate);
    
    if((status=pthread_mutex_lock(&protect_log))==0)
    {
        if(OpenLogFile(&pds.ylog_fd,PDS_LOG0,stderr)>=0) // Open log file if not already opened
        {
            if(pds.ylog_fd!=NULL)
            {
                GetUTime(strp); // Get universal time
                fprintf(pds.ylog_fd,"%s (UTC): ",strp);
                va_start(args,fmt);
                status=vfprintf(pds.ylog_fd,fmt, args);
                fflush(pds.ylog_fd);
            }
        }
    }
    pthread_mutex_unlock(&protect_log);
    
    pthread_setcancelstate(oldstate,NULL);
    
    return(status);
}

// As printf but everything goes into dds packet filter log.
int DPrintf(const char *fmt, ...) 
{
    int status;
    va_list args;
    char  strp[32];
    
    int oldstate;
    
    // We do not want to cancel on default cancel points in fprintf,vfprintf
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldstate); 
    
    if((status=pthread_mutex_lock(&protect_log))==0)
    {
        if(OpenLogFile(&pds.dlog_fd,PDS_LOG1,pds.ylog_fd)>=0) // Open log file if not already opened
        {
            if(pds.dlog_fd!=NULL)
            {
                GetUTime(strp); // Get universal time
                fprintf(pds.dlog_fd,"%s (UTC): ",strp);
                va_start(args,fmt);
                status=vfprintf(pds.dlog_fd,fmt, args);
                fflush(pds.dlog_fd);
            }
        }
        
    }
    pthread_mutex_unlock(&protect_log);
    
    pthread_setcancelstate(oldstate,NULL);
    
    return(status);
}

// As printf but everything goes into rpc filter packet log.
int PPrintf(const char *fmt, ...) 
{
    int status;
    va_list args;
    char strp[32];
    
    int oldstate;
    
    // We do not want to cancel on default cancel points in fprintf,vfprintf
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldstate); 
    
    
    if((status=pthread_mutex_lock(&protect_log))==0)
    {
        if(OpenLogFile(&pds.plog_fd,PDS_LOG4,pds.ylog_fd)>=0) // Open log file if not already opened
        {
            if(pds.plog_fd!=NULL)
            {
                GetUTime(strp); // Get universal time
                fprintf(pds.plog_fd,"%s (UTC): ",strp);
                va_start(args,fmt);
                status=vfprintf(pds.plog_fd,fmt, args);
                fflush(pds.plog_fd);
            }
        }
    }
    pthread_mutex_unlock(&protect_log);  
    
    pthread_setcancelstate(oldstate,NULL);
    
    return(status);
}                  

// As printf but everything goes into Science decoding log.
int CPrintf(const char *fmt, ...)
{
    int status;
    va_list args;
    char strp[32];
    
    int oldstate;
    
    // We do not want to cancel on default cancel points in fprintf,vfprintf
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldstate); 
    
    if((status=pthread_mutex_lock(&protect_log))==0)
    {
        if(OpenLogFile(&pds.clog_fd,PDS_LOG5,pds.ylog_fd)>=0) // Open log file if not already opened
        {
            if(pds.clog_fd!=NULL)
            {
                GetUTime(strp); // Get universal time
                fprintf(pds.clog_fd,"%s (UTC): ",strp);
                va_start(args,fmt);
                status=vfprintf(pds.clog_fd,fmt, args);
                fflush(pds.clog_fd);
            }
        }
    }
    pthread_mutex_unlock(&protect_log);
    
    pthread_setcancelstate(oldstate,NULL);
    
    return(status);
}       

// As printf but everything goes into HK decoding log.
int HPrintf(const char *fmt, ...) 
{
    int status;
    va_list args;
    char strp[32];
    
    int oldstate;
    
    // We do not want to cancel on default cancel points in fprintf,vfprintf
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldstate); 
    
    if((status=pthread_mutex_lock(&protect_log))==0)
    {
        if(OpenLogFile(&pds.hlog_fd,PDS_LOG6,pds.ylog_fd)>=0) // Open log file if not already opened
        {
            if(pds.hlog_fd!=NULL)
            {
                GetUTime(strp); // Get universal time
                fprintf(pds.hlog_fd,"%s (UTC): ",strp);
                va_start(args,fmt);
                status=vfprintf(pds.hlog_fd,fmt, args);
                fflush(pds.hlog_fd);
            }
        }
    }
    pthread_mutex_unlock(&protect_log);  
    pthread_setcancelstate(oldstate,NULL);
    
    return(status);
}

int OpenLogFile(FILE **pfd,char *name,FILE *fderr)
{
    char tstr[PATH_MAX];
    
    if(*pfd==NULL)
    {
        if(pds.lpath[0]!='\0')
        {
            sprintf(tstr,"%s%s",pds.lpath,name);// Setup system log
            
            if((*pfd=fopen(tstr,"w"))==NULL) // Open log file
            {
                fprintf(fderr,"Couldn't open %s\n",name); 
                return -1;
            }
        }
    }
    return 0;
}

// Dump path information to system log
int AddPathsToSystemLog(pds_type *p)
{
    YPrintf("LAP PDS System paths\n"); 
    YPrintf("Mission calendar file     : %s\n",p->mcpath);
    YPrintf("LAP Macro desc            : %s\n",p->macrop);
    YPrintf("DataSetVersion            : %3.1f\n",p->DataSetVersion);
    YPrintf("DPL Number                : %d\n",p->DPLNumber);
    YPrintf("PDS Archive               : %s\n",p->apathpds);
    YPrintf("DDS Archive               : %s\n",p->apathdds);
    YPrintf("LOG Path                  : %s\n",p->lpath);
    YPrintf("UnAccepted Path           : %s\n",p->uapath);
    
    YPrintf("Path to calibration files : %s\n",p->cpathd);
    YPrintf(" Path to coarse voltage bias file : %s\n",p->cpathc);
    YPrintf(" Path to fine voltage bias file   : %s\n",p->cpathf);
    YPrintf(" Path to current bias file        : %s\n",p->cpathi);
    YPrintf(" Path to offset files             : %s\n",p->cpathm);
    return 0;
}


// Program option functions
//----------------------------------------------------------------------------------------------------------------------------------


/* Get command-line argument (option/flag) and optionally an argument to that option that follows the option.
 * 
 * If "arg" is not null then we expect there to be an argument after the option and which value is returned via "arg".
 * Return TRUE if-and-only-if the option was found and, if an associated argument was required, if that was found too.
 * NOTE: The function can not tell the difference between an option and an argument (associated with another option), other than by comparing
 * strings.
 * NOTE: The function is designed to only read the same command-line arguments once. Therefore, the function will MODIFY argv[] by setting
 * used-up arguments to NULL to make it possible to check if there are arguments left over that could not be interpreted. It will thus ignore
 * values argv[i] == null.
 * 
 * opt : Flag (string)
 * argv : (Remaining) command-line arguments.
 * argc : Number of command-line arguments (length of argv array)
 * arg : Iff non-null, then *arg will be assigned to the string value of the command-line argument that comes after the flag.
 */
int GetOption(char *opt, int argc, char *argv[], char *arg)
{
    int i;
    for(i=1;i<argc;i++)
    {
        if((argv[i] != NULL) && !strncmp(argv[i],opt,1024))   // Match option
        {
            if (arg==NULL) {   // We expect an argument to exist for this option if arg is non-null.
                argv[i] = NULL;
                return 1;     // CASE: Found option and expected no associated extra argument.
            }
            else
            {
                if ((i+1<argc) && (argv[i+1]!=NULL))   // If the argument list contains an (unused) argument after this option (flag)...
                {
                    strncpy(arg,argv[i+1],1024); // Copy next entry as argument
                    argv[i] = NULL;
                    argv[i+1] = NULL;
                    return 1;     // CASE: Found option and both required and found associated extra argument.
                }
                else
                {
                    return 0;     // CASE: Found option and required associated extra argument but did not find it.
                }
            }
        }
    }
    return 0;     // CASE: Did not find the option.
}


// Return true if-and-only-if argv[i] contains non-null components (for i >= 1).
int HasMoreArguments(int argc, char *argv[])
{
    int i;
    for(i=1;i<argc;i++)
    {
        if (argv[i] != NULL) {
            return 1;
        }
    }
    return 0;
}


// Functions to load and test external information 
//----------------------------------------------------------------------------------------------------------------------------------

// Load first part of configuration file
int  LoadConfig1(pds_type *p) // Loads configuration information
{ 
    FILE *fd;
    
    char line[PATH_MAX];  // Text line
    char l_str[PATH_MAX]; // left string
    char r_str[PATH_MAX]; //  right string
    
    printf("Loading first part of configuration file: %s\n\n",p->cpath);
    YPrintf("Loading first part of configuration file: %s\n\n",p->cpath);
    
    if((fd=fopen(p->cpath,"r"))==NULL)
    {
        perror(p->cpath);
        return -1;
    }
    
    // Remove initial comments..
    while(fgets(line,PATH_MAX,fd) != NULL)
        if (line[0] == '%') 
            continue; 
        else
            break;
        
        
        Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sscanf(l_str,"%d",&p->SCResetCounter);printf("SCResetCounter              : %d\n",p->SCResetCounter);
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);	
    strncpy(p->SCResetClock,l_str,20);    printf("ResetClock                  : %s\n",p->SCResetClock);
    
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->templp,l_str,PATH_MAX);
    if(SetupPath("Path to PDS archive template",p->templp)<0)  return -2;  // Test if path exists
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->macrop,l_str,PATH_MAX);
    if(SetupPath("LAP Macro descriptions path ",p->macrop)<0)  return -3;  // Test if path exists
    
    //fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    //sscanf(l_str,"%f",&p->DataSetVersion);printf("DataSetVersion              : %3.1f\n",p->DataSetVersion);
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->LabelRevNote,l_str,PATH_MAX);printf("Label Revision Note         : %s\n",p->LabelRevNote);
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    
    l_str[10]='\0';
    strncpy(p->ReleaseDate,l_str,11);printf("Release Date                : %s\n",p->ReleaseDate);
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->mcpath,l_str,PATH_MAX);    printf("Mission calendar file       : %s\n",p->mcpath);
    
    fclose(fd); // Close config file
    
    return 0;
}



// Load second part of configuration file
//
// NOTE: Does more than just read configuration file.
// Also creates dataset directory and copies files from template directory, and probably more.
int  LoadConfig2(pds_type *p,char *data_set_id) 
{ 
    FILE *fd;
    FILE *pipe_fp;
    int len;
    
    char line[PATH_MAX];  // Text line
    char l_str[PATH_MAX]; // left string
    char r_str[PATH_MAX]; //  right string
    char tstr[PATH_MAX];
    
    struct timespec dose={0,DOSE_TIME};  
    
    printf("\nLoading second part of configuration file: %s\n",p->cpath);
    YPrintf("\nLoading second part of configuration file: %s\n",p->cpath);
    
    if((fd=fopen(p->cpath,"r"))==NULL)
    {
        perror(p->cpath);
        return -1;
    }
    
    // Remove initial comments..
    while(fgets(line,PATH_MAX,fd) != NULL)
        if (line[0] == '%') 
            continue; 
        else
            break;
        
        // Skip first part already loaded in LoadConfig1
        fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->apathpds,l_str,PATH_MAX);
    
    if(SetupPath("PDS Archives base path      ",p->apathpds)<0) return -2; // Test if path exists
    
    sprintf(p->apathpds,"%s%s/",l_str,data_set_id);
    
    
    printf("\nCreate archive path: %s\n",p->apathpds);
    YPrintf("\nCreate archive path: %s\n",p->apathpds);
    
    if(mkdir(p->apathpds,0775)<0)
    {
        printf("Can not create archive path: %s\n",p->apathpds);
        YPrintf("Can not create archive path: %s\n",p->apathpds);
        return -3;
    }
    
    
    printf("Copying template archive to:  %s\n",p->apathpds);
    YPrintf("Copying template archive to:  %s\n",p->apathpds);
    
    sprintf(tstr,"cp -r %s* %s",p->templp,p->apathpds); // Setup shell command line.
    printf("%s\n",tstr);
    YPrintf("%s\n",tstr);
    
    pipe_fp = popen(tstr,"r");  // Runs shell commands
    
    while(!feof(pipe_fp))       // Read out stdout from executing shell commands
    {
        if((len=fread(tstr,1,PATH_MAX,pipe_fp)>0))
        {
            tstr[len]='\0';     // Ensure null terminated string!
            printf("%s",tstr);  // Dump stdout from commands
            YPrintf("%s",tstr); // Dump stdout from commands
            
        }
        nanosleep(&dose,NULL);  // Prevent hogging
    }
    pclose(pipe_fp);            // Close pipeline
    
    printf("\n");
    YPrintf("\n");
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->apathdds,l_str,PATH_MAX);
    
    sprintf(p->tpath,"%s%s",p->apathdds,"/archivex/aux/TCORR"); // Setup path to time correlation packets
    if(SetupPath("DDS Time corr. packets path ",p->tpath)<0)  return -4;  // Test if path exists
    
    strncat(p->apathdds,"/archive",8);                                     // Add subdirectory always "archive"
    if(SetupPath("DDS Archive path            ",p->apathdds)<0)  return -5;  // Test if path exists
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->lpath,l_str,PATH_MAX);
    if(SetupPath("Log path                    ",p->lpath)<0)  return -6;  // Test if path exists
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->uapath,l_str,PATH_MAX);
    if(SetupPath("Path to unaccepted data     ",p->uapath)<0)  return -7;  // Test if path exists
    
    
    if(SetupPath("LAP_PDS_ROOT                ",p->apathpds)<0) return -8;  // Test PDS root path
    strncpy(line,p->apathpds,PATH_MAX); 
    strncat(line,"DATA",PATH_MAX);
    if(SetupPath("DATA                        ",line)<0)  return -9;  // Test if DATA directory exists
    
    
    sprintf(p->dpathse,"%sEDITED/",line); 
    sprintf(p->dpathsc,"%sCALIBRATED/",line); 
    
    rmdir(p->dpathsc);  // Try to remove calibrated
    rmdir(p->dpathse);  // Try to remove edited
    
    if(calib)
    {
        if(MakeDir("CALIBRATED",line,p->dpathsc)<0) // Create CALIBRATED directory
            return -10;
    }
    else
    {
        if(MakeDir("EDITED",line,p->dpathse)<0) // Create EDITED directory
            return -11;
    }
    
    if(calib)
        strcpy(p->dpathse,p->dpathsc); // Overide edited!...
        
        strncpy(p->dpathh,p->dpathse,PATH_MAX);  // Same data path for HK
        
        strncpy(p->ipath,p->apathpds,PATH_MAX);
    strncat(p->ipath,"INDEX",PATH_MAX);
    if(SetupPath("INDEX                       ",p->ipath)<0)  return -12;  // Test if INDEX directory exists
    
    strncpy(p->cpathd,p->apathpds,PATH_MAX);
    strncat(p->cpathd,"CALIB",PATH_MAX);  // Root path to data used to perform calibrations 
    if(SetupPath("CALIB                       ",p->cpathd)<0)  return -13;  // Test if CALIB directory exists
    
    if(fgets(line,PATH_MAX,fd)==NULL) return -14;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathc,"%s%s",p->cpathd,l_str); // Set up path to coarse voltage bias calibration file
    if(fgets(line,PATH_MAX,fd)==NULL) return -15;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathf,"%s%s",p->cpathd,l_str); // Set up path to fine voltage bias calibration file
    if(fgets(line,PATH_MAX,fd)==NULL) return -16;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathi,"%s%s",p->cpathd,l_str); // Set up path to current voltage bias calibration file
    if(fgets(line,PATH_MAX,fd)==NULL) return -17;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathm,"%s%s",p->cpathd,l_str); // Set up path to offset calibration file
    if(fgets(line,PATH_MAX,fd)==NULL) return -18;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strcat(p->tpath,l_str);                   // Add filename to time correlation packet path 
    
    
    if(fgets(line,PATH_MAX,fd)==NULL) return -19;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathdfp1,"%s%s",p->cpathd,l_str); // Add filename to density frequency response probe 1
    
    if(fgets(line,PATH_MAX,fd)==NULL) return -20;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathdfp2,"%s%s",p->cpathd,l_str); // Add filename to density frequency response probe 2
    
    if(fgets(line,PATH_MAX,fd)==NULL) return -21;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathefp1,"%s%s",p->cpathd,l_str); // Add filename to e-field frequency response probe 1
    
    if(fgets(line,PATH_MAX,fd)==NULL) return -22;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    sprintf(p->cpathefp2,"%s%s",p->cpathd,l_str); // Add filename to e-field frequency response probe 2
    
    fclose(fd); // Close config file
    
    // Open DDS dds_progress.txt
    sprintf(tstr,"%s%s",p->lpath,PDS_LOG3);
    
    if((p->ddsp_fd=fopen(tstr,"w"))==NULL) // Open progress file for writing from beginning of file
    {
        YPrintf("Couldn't open: %s\n",PDS_LOG3); // Error
        ExitPDS(1);
    }
    else
        YPrintf("DDS decoding progress stored in: %s\n",PDS_LOG3);
    
    return 0;
}

// Load anomaly description file into a linked list of properties.
int  LoadAnomalies(prp_type *p,char *path)
{
    FILE *fd;
    char line[256]; // Line buffer
    char t_tok[80]; // Time token
    char m_tok[80]; // Macro/Select token
    
    
    printf("Loading anomaly file: %s\n",path);
    YPrintf("Loading anomaly file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    while (fgets(line,255,fd) != NULL)
    {
        if(line[0] == '\n') continue; // Empty line..
        if (line[0] == '#') continue; // Remove comments..
        Separate(line,t_tok,m_tok,'\t',1); // Separate at first occurrence of a tab character
        TrimWN(m_tok);
        InsertTopK(p,t_tok,m_tok); // Insert into linked list of property name value pairs.
    }
    fclose(fd);
    return 0;
}


// Load human mode descriptions into a linked list of properties.
int  LoadModeDesc(prp_type *p,char *path)
{
    FILE *fd;
    char line[1024];   // Line buffer
    char d_tok[1024];  // Description
    char m_tok[1024];  // Macro mode
    
    
    printf( "Loading human description of macro modes, file: %s\n",path);
    YPrintf("Loading human description of macro modes, file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    while (fgets(line, 1024, fd) != NULL)
    {
        if (line[0] == '\n') continue; // Empty line..
        if (line[0] == '#') continue; // Remove comments..
        
        // Separate at first occurrence of a ":" character   
        int result = SeparateOnce(line, m_tok, d_tok, ':');
        if (result < 0) {
            return -2;
        }
        
        TrimWN(d_tok);
        InsertTopK(p, m_tok, d_tok); // Insert into linked list of property name value pairs
    }
    fclose(fd);
    return 0;
}


int LoadBias(unsigned int ***bias_s,unsigned int ***mode_s,int *bias_cnt_s,int *mode_cnt_s,char *path) // Load bias settings
{
    FILE *fd;
    char line[256]; // Line buffer
    char l_tok[80]; // Left token
    char m_tok[80]; // Middle token
    char r_tok[80]; // Right token
    
    unsigned int **bias;
    unsigned int **mode;
    
    unsigned int mde;
    
    int bias_cnt=0;
    int mode_cnt=0;
    
    unsigned int de,ef;
    time_t t;
    
    printf("Loading bias file: %s\n",path);
    YPrintf("Loading bias file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    
    // Count bias table lines
    while(fgets(line,255,fd) != NULL)
    {
        if(line[0] == '\n') continue; // Empty line..
        if (line[0] == '#') continue; // Remove comments..
        if(line[0] == ' ')  continue;  // White line
        if(strstr(line,"*Mode*")==NULL)
            bias_cnt++;
        else
            mode_cnt++;
    }
    
    rewind(fd); // Rewind index label to start
    
    
    bias=CallocIntMatrix(bias_cnt,3); // Allocate a matrix of integers
    
    mode=CallocIntMatrix(mode_cnt,2);
    
    bias_cnt=0;
    mode_cnt=0;
    while (fgets(line,255,fd) != NULL)
    {
        if(line[0] == '\n') continue; // Empty line..
        if (line[0] == '#') continue; // Remove comments..
        if(line[0] == ' ')  continue;  // White line
        Separate(line,l_tok,m_tok,'\t',1); // Separate at first occurrence of a tab character
        Separate(line,m_tok,r_tok,'\t',2); // Separate at second occurrence of a tab character
        TimeOfDatePDS(l_tok,&t);          // Convert time to seconds
        
        
        if(strstr(m_tok,"*Mode*")==NULL)
        {
            sscanf(m_tok,"%x",&de);
            sscanf(r_tok,"%x",&ef);
            
            bias[bias_cnt][0]=(unsigned int)t; // Safe since time_t is a unsigned int!
            bias[bias_cnt][1]=de;
            bias[bias_cnt][2]=ef;
            bias_cnt++;
        }
        else
        {
            
            sscanf(r_tok,"%x",&mde);
            
            mode[mode_cnt][0]=(unsigned int)t; // Safe since time_t is a unsigned int!
            mode[mode_cnt][1]=mde;
            mode_cnt++;
        }
    }
    fclose(fd);
    
    
    *bias_s=bias;
    *mode_s=mode;
    
    *bias_cnt_s=bias_cnt;
    *mode_cnt_s=mode_cnt;
    return 0;
}

int LoadExclude(unsigned int **exclude,char *path) // Load exclude file
{
    FILE *fd;
    char line[256]; // Line buffer
    unsigned int macro;
    unsigned int *tmp_e;
    
    int excl_cnt=0;
    
    printf("Loading macro exclude file: %s\n",path);
    YPrintf("Loading macro exclude file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    // Count exclude table lines
    while(fgets(line,255,fd) != NULL)
    {
        if(line[0] == '\n') continue; // Empty line..
        if (line[0] == '#') continue; // Remove comments..
        if(line[0] == ' ')  continue;  // White line
        excl_cnt++;
    }
    
    rewind(fd); // Rewind index label to start
    
    tmp_e=(unsigned int *)CallocArray(excl_cnt,sizeof(unsigned int)); // Allocate 
    
    excl_cnt=0;
    while(fgets(line,255,fd) != NULL)
    {
        if(line[0] == '\n') continue; // Empty line..
        if (line[0] == '#') continue; // Remove comments..
        if(line[0] == ' ')  continue;  // White line
        sscanf(line,"%x",&macro);
        tmp_e[excl_cnt]=macro;
        excl_cnt++;
    }
    fclose(fd);
    
    
    *exclude=tmp_e;
    return excl_cnt;
}



//--------------------------------------------------------------------------------------------
// Erik P G Johansson 2015-03-25: Created function
// Load file with time intervals for which data should be excluded.
// Every line in the data exclude file specifies one time interval.
// 
// NOTE: Time is given in the form of spacecraft clock counts,
// like SPACECRAFT_CLOCK_START_COUNT and SPACECRAFT_CLOCK_STOP_COUNT.
// The reasons for using this format are:
// 1) Makes bugs more unlikely
// 2) One wants to use a time designation that is as absolute/permanent as possible
//    and therefore independent of time conversions, in particular time corrections,
//    and therefore as close to the packet data times as possible.
//    If one had not and the time conversions changed, then one would have to update
//    the data exclude list.
// 3) The code only partly contain functions for conversion from human-readable
//    strings to spacecraft clock counter (TimeOfDatePDS)?
// 
// NOTE: Current implementation does not return any information in the event of failure
// making soft error harder for caller.
// NOTE: One could remove time intervals that are outside the time limits of
// the mission phase to speed up comparisons, but that has not been implemented (2015-03-26).
// Doubtful how useful that would be.
//--------------------------------------------------------------------------------------------
int LoadDataExcludeTimes(data_exclude_times_type **dataExcludeTimes, char *depath) {
    FILE   *fd;
    char   line[256]; // Line buffer
    int    i = 0;
    int    SCResetCounter1 = 0;
    int    SCResetCounter2 = 0;
    double t_begin = 0.0;
    double t_end = 0.0;
    char   l_tok[256]; // Left token
    char   r_tok[256]; // Right token  
    data_exclude_times_type dataExcludeTimes_temp;
    
    YPrintf("Loading data exclude times file: %s\n", depath);  // Print to "pds system log".
    printf( "Loading data exclude times file: %s\n", depath);
    
    *dataExcludeTimes = (data_exclude_times_type*) NULL;   // Default value to be returned to the caller in case of error.
    
    if((fd=fopen(depath, "r"))==NULL) {
        perror(depath);
        return -1;
    }
    
    // Count number of data exclude time intervals by counting lines of actual data in file.
    dataExcludeTimes_temp.N_intervals = 0;
    while(fgets(line, 255, fd) != NULL)
    {
        if (line[0] == '\n') continue; // Ignore empty line.
        if (line[0] == '#')  continue; // Ignore comments.
        //if (line[0] == ' ')  continue; // Ignore white line
        dataExcludeTimes_temp.N_intervals++;
    }
    
    rewind(fd); // Set position of stream to beginning.
    dataExcludeTimes_temp.SCResetCounter_begin_list = (int *)    CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(int));
    dataExcludeTimes_temp.t_begin_list              = (double *) CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(double));
    dataExcludeTimes_temp.t_end_list                = (double *) CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(double));
    
    i = 0;
    YPrintf("LoadDataExcludeTimes: Ingested data exclude time intervals: file contents strings and interpretations (true decimals)\n");  // Print to "pds system log".
    while(fgets(line, 255, fd) != NULL)  // NOTE: "line" will end with a \n.
    {      
        if (line[0] == '\n') continue; // Ignore empty line..
        if (line[0] == '#')  continue; // Ignore comments..
        //if (line[0] == ' ')  continue; // Ignore white line
        if (sscanf(line, " %s %s ", l_tok, r_tok) != 2)   // Whitespace represent any sequence of whitespace and tab (incl. empty).
        {
            YPrintf("LoadDataExcludeTimes: ERROR: Can not interpret line in data exclude times file (sscanf): \"%s\"\n", line);
            printf( "LoadDataExcludeTimes: ERROR: Can not interpret line in data exclude times file (sscanf): \"%s\"\n", line);
            return -1;
        }
        
        if (OBT_Str2Raw(l_tok, &SCResetCounter1, &t_begin)) {
            YPrintf("LoadDataExcludeTimes: ERROR: Can not interpret interval _beginning_ in data exclude times file: \"%s\"\n", l_tok);
            printf( "LoadDataExcludeTimes: ERROR: Can not interpret interval _beginning_ in data exclude times file: \"%s\"\n", l_tok);
            return -1;
        }
        if (OBT_Str2Raw(r_tok, &SCResetCounter2, &t_end)) {    // NOTE: temp_int2 never used.
            YPrintf("LoadDataExcludeTimes: ERROR: Can not interpret interval _end_ in data exclude times file: \"%s\"\n", r_tok);
            printf( "LoadDataExcludeTimes: ERROR: Can not interpret interval _end_ in data exclude times file: \"%s\"\n", r_tok);
            return -1;
        }
        
        if (t_begin > t_end) {
            YPrintf("LoadDataExcludeTimes: ERROR: Found time interval runs backwards (t_begin > t_end) in data exclude times file.\n");
            printf( "LoadDataExcludeTimes: ERROR: Found time interval runs backwards (t_begin > t_end) in data exclude times file.\n");
        }
        dataExcludeTimes_temp.SCResetCounter_begin_list[i] = SCResetCounter1;
        dataExcludeTimes_temp.t_begin_list[i]              = t_begin;
        dataExcludeTimes_temp.t_end_list[i]                = t_end;
        
        // Print to "pds system log".
        // Useful for double-checking that the code interprets the file correctly. Could be deactivated
        // together with introductory "title line" that is also printed to the log before the loop.
        YPrintf("   %s %s  = %i/%f %i/%f\n", l_tok, r_tok, SCResetCounter1, t_begin, SCResetCounter1, t_end);
        
        i++;
    }
    
    // Check that the two different counts of time intervals agree to detect bugs, especially when changing the file format.
    // NOTE: Could possibly remove this code.
    if (i != dataExcludeTimes_temp.N_intervals) {
        YPrintf("LoadDataExcludeTimes: ERROR: Implementation bug. Can not obtain the number of time intervals.\n");  // Print to "pds system log".
        printf( "LoadDataExcludeTimes: ERROR: Implementation bug. Can not obtain the number of time intervals.\n");
    }
    fclose(fd);
    
    // Actually assign the value that is to returned to the caller.
    *dataExcludeTimes = (data_exclude_times_type *)   CallocArray(1, sizeof(data_exclude_times_type));
    **dataExcludeTimes = dataExcludeTimes_temp;
    
    return 0;  
}



/*--------------------------------------------------------------------------------------------
 * Erik P G Johansson 2015-03-25: Created function.
 * 
 * Checks whether a LBL/TAB file pair should be created at all depending on the data (time).
 * 
 * Value assigned to *excludeData:
 *      false: do not exclude data.
 *      true:  exclude data.
 *      
 * Return value: 0 (no error), -1 (error).
 *
 * NOTE: Requires SPACECRAFT_CLOCK_START_COUNT, SPACECRAFT_CLOCK_STOP_COUNT, START_TIME, STOP_TIME to already have been set.
 * 
 * IMPLEMENTATION NOTE: The code determines SPACECRAFT_CLOCK_START_COUNT and SPACECRAFT_CLOCK_STOP_COUNT
 * from property list used for writing LBL file(s). This means that the values are parsed from strings.
 * This may seem suboptimal but has the advantage of being largely independent of how the rest of
 * DecodeScience (a huge, complicated function) works.
 * NOTE: Uncertain whether to only look at checked or unchecked properties, or both, in property list.
 * Appears that SPACECRAFT_CLOCK_START/STOP_COUNT are set using UNCHECKED (default for SetP).
 * NOTE: Uncertain whether to assume exactly one occurrence of SPACECRAFT_CLOCK_START/STOP_COUNT in property list.
 *--------------------------------------------------------------------------------------------*/
int DecideWhetherToExcludeData(data_exclude_times_type *dataExcludeTimes, prp_type *file_properties, int *excludeData) {
    // PROPOSAL: Reverse exclude/include return result?
    // PROPOSAL: Boolean parameter-by-reference for include/exclude and separate error/no-error return value.
    // QUESTION: How handle not finding SPACECRAFT_CLOCK_START/STOP_COUNT? Include?
    // QUESTION: How handle multiple findings for one key/name? Checks?
    property_type *property1 = NULL;
    property_type *property2 = NULL;
    char *SPACECRAFT_CLOCK_START_COUNT = NULL;     // SCC = spacecraft count
    char *SPACECRAFT_CLOCK_STOP_COUNT  = NULL;
    char *START_TIME = NULL;
    char *STOP_TIME  = NULL;
    int junk_int, i = -1;
    int file_SCResetCounter_begin = -1;
    double file_t_begin = -1;
    double file_t_end   = -1;    
    
    // NOTE: int OBT_Str2Raw(char *stime, int *resetCounter, double *rawTime)
    
    
    if (
        (FindP(file_properties, &property1, "SPACECRAFT_CLOCK_START_COUNT", 1, DNTCARE) < 0) ||
        (FindP(file_properties, &property2, "SPACECRAFT_CLOCK_STOP_COUNT",  1, DNTCARE) < 0)
    ) {
        char *msg = "DecideWhetherToExcludeData: Can not find occurrence of SPACECRAFT_CLOCK_START/STOP_COUNT to use for determining whether to include/exclude data.";
        YPrintf("%s\n", msg);
        printf("%s\n", msg);  // NOTE: Using msg as format specifier instead makes gcc give warning (a warning which can be ignored, but still).
        return -1;
    }
    SPACECRAFT_CLOCK_START_COUNT = property1->value;
    SPACECRAFT_CLOCK_STOP_COUNT  = property2->value;
    OBT_Str2Raw(SPACECRAFT_CLOCK_START_COUNT, &file_SCResetCounter_begin, &file_t_begin);
    OBT_Str2Raw(SPACECRAFT_CLOCK_STOP_COUNT,  &junk_int,                  &file_t_end  );
    
    if (
        (FindP(file_properties, &property1, "START_TIME", 1, DNTCARE) < 0) ||
        (FindP(file_properties, &property2, "STOP_TIME",  1, DNTCARE) < 0)
    ) {
        YPrintf("DecideWhetherToExcludeData: Can not find occurrence of CLOCK_START/STOP_COUNT.\n");
        printf( "DecideWhetherToExcludeData: Can not find occurrence of CLOCK_START/STOP_COUNT.\n");
        return -1;
    }
    START_TIME = property1->value;
    STOP_TIME  = property2->value;
    
    
    
    for (i=0; i<dataExcludeTimes->N_intervals; i++) {
        // Check if the file data overlaps with a specific data exclusion time interval.
        // If it does, then it is not necessary to check with other data exclusion intervals.
        if (
            (file_SCResetCounter_begin == dataExcludeTimes->SCResetCounter_begin_list[i])
            && (file_t_end   >= dataExcludeTimes->t_begin_list[i])
            && (file_t_begin <= dataExcludeTimes->t_end_list[i])
        )
        {
            CPrintf("DecideWhetherToExcludeData: Data for file with data in the stated time interval should be excluded:\n");
            CPrintf("   SPACECRAFT_CLOCK_START_COUNT = %s\n", SPACECRAFT_CLOCK_START_COUNT);
            CPrintf("   SPACECRAFT_CLOCK_STOP_COUNT  = %s\n", SPACECRAFT_CLOCK_STOP_COUNT );
            CPrintf("   START_TIME = %s\n", START_TIME);
            CPrintf("   STOP_TIME  = %s\n", STOP_TIME);
            
            *excludeData = 1; // Assign "true".
            return 0;
        }
    }
    
    *excludeData = 0;  // Assign "false".
    return 0;
}
//*/



// Load time correlation packets
//
// For now we assume all time corr packets are in one file.
// We also assume that the file starting at 2004-03-01T00:00:00Z
// and that has the longest duration is the most current file to use.
//
// Time correlation strategy
//
// 1) UTC_TIME=OBT_TIME*gradient(n)+offset(n) 
// 2) Valid correction if SCET(n) <= UTC_TIME <= SCET(n+1)
// 3) If no SCET(n+1) exists yet, it is considered to be infinite.
//
// Time correlation structure
//
// typedef struct tc_type_def
// {
//  int netries;        // Number of entries
//  unsigned int *SCET; // Time correlated OBT from which correlation below is valid
//  double *offset;     // Offset
//  double *gradient;   // Gradient
// } tc_type;
//
//  pds->tpath          // Path time calib packets
//
int  LoadTimeCorr(pds_type *pds,tc_type *tcp)
{
    char name[PATH_MAX];
    char longest[80]; // Assume filename is not longer
    char *base_n;
    char *dir_n;
    unsigned char buff[TIME_CORR_PTOT_SIZE];
    int length;
    DIR           *dir;         // Directory
    struct dirent *dentry;      // Directory entry
    char l_tok[80];  // Left of token
    char r_tok[80]; //  Right of token
    time_t etime=0;
    time_t otime=0;
    int len;
    int tlen;
    
    //edit FJ 19/5 2014
    
    
    char newdate[40];
    char myear[4];
    char mmon[4];
    char mday[4];
    char mhour[4];
    char mmin[4];
    char msec[4];
    
    FILE *fd;
    int n;
    struct timespec nap={0,NAP_TIME}; 
    
    base_n=basename(pds->tpath); // Get basic name for time correlation files
    dir_n =dirname(pds->tpath);  // Get directory
    
    
    if((dir = opendir(dir_n))==NULL) // Open time correlation packet directory
    {
        perror("opendir");
        YPrintf("Couldn't open time correlation packet directory\n");
        return -1;
    }
    
    rewinddir(dir);   // Rewind
    dentry=readdir(dir);  // Get first entry
    
    // Find the file with longest duration starting at 2004-03-01
    while(dentry!=NULL) // Do a linear search through all files
    {
        if(!Match(base_n,dentry->d_name)) // Match filename to pattern
        {
            //edit FJ 19/5 2014 for new filename convention
            Separate(dentry->d_name,l_tok,r_tok,'_',5);
            
            strncpy(myear,r_tok,2);      // Copy myear from r_tok
            strncpy(mmon,&r_tok[2],2); // Copy mmon from r_tok
            strncpy(mday,&r_tok[4],2);  // Copy day of mmon to mday
            
            strncpy(mhour,&r_tok[7],2);      // Copy myear from r_tok
            strncpy(mmin,&r_tok[10],2);      // Copy myear from r_tok
            strncpy(msec,&r_tok[13],2);      // Copy myear from r_tok
            
            myear[2]='\0';  // Terminate
            mmon[2]='\0'; // Terminate
            mday[2]='\0';  // Terminate 
            mhour[2]='\0';  // Terminate 
            mmin[2]='\0';  // Terminate 
            msec[2]='\0';  // Terminate 
            
            
            sprintf(newdate,"20%s-%s-%sT%s:%s:%s",myear,mmon,mday,mhour,mmin,msec); // Setup time
            
            
            TimeOfDatePDS(newdate,&etime);
            if(etime>otime)
            {
                otime=etime;
                strcpy(longest,dentry->d_name);
            }
        }
        dentry=readdir(dir); // Get next filename 
    }
    closedir(dir);
    
    
    YPrintf("Using file %s as time correlation file\n",longest);
    
    sprintf(name,"%s/%s",dir_n,longest); // Set full path and file name
    
    if((fd=fopen(name,"r"))==NULL)
    {
        perror(name);
        YPrintf("Failed to open time correlation file %s\n",name);
        return -2;
    }
    
    tlen=FileLen(fd);
    // typedef struct tc_type_def
    // {
    //  int netries;        // Number of entries
    //  unsigned int *SCET; // Time correlated OBT from which correlation below is valid
    //  double *offset;     // Offset
    //  double *gradient;   // Gradient
    // } tc_type;
    //
    tcp->netries=(tlen/TIME_CORR_PTOT_SIZE); // Total number of time correlation packets
    
    // Allocate storage
    tcp->SCET=(double *)CallocArray(tcp->netries,sizeof(double));
    tcp->offset=(double *)CallocArray(tcp->netries,sizeof(double));
    tcp->gradient=(double *)CallocArray(tcp->netries,sizeof(double));
    
    n=0;
    while(!feof(fd) && n<tcp->netries)
    {
        len=0;
        // Read file loop, most of the time only one read will be done
        // but if an error occurs we do try again to read the rest..just good practice!
        while(len<TIME_CORR_PTOT_SIZE && !feof(fd))
        {
            len+=fread(&buff[len],1,TIME_CORR_PTOT_SIZE-len,fd);
            nanosleep(&nap,NULL); // Be nice                         
        }
        
        tcp->SCET[n]=DDSTime(buff); // Get time of DDS packet
        
        length=(buff[8]<<24 | buff[9]<<16 | buff[10]<<8 | buff[11]);
        
        if(length!=TIME_CORR_P_SIZE)
        {
            YPrintf("Corrupted time packet length: %d\n",length);
            return -3;
        }
        
        tcp->gradient[n] = GetDBigE(&buff[18]);
        tcp->offset[n]   = GetDBigE(&buff[26]);
        n++;
    }
    
    fclose(fd);
    return 0;
}

// Load all macro descriptions into memory
// NOTE: *.mds files: The left-most numbers appear to be ignored, but a tab is still required before the actual "variable assignment".
int LoadMacroDesc(prp_type macs[][MAX_MACROS_INBL],char *home) // Load all macro descriptions
{
    FILE *mac_fd;   // File desc
    int m_bl,m_n;   // macro block & macro number
    char path[256];  // Path + filename
    char line[256]; // Line buffer
    
    int state=0;    // State variable
    char l_tok[80];  // line nr token
    char t_tok[80]; //  temp    token
    char n_tok[80];  // name    token
    char v_tok[80];  // value   token
    
    int n_macs=0;
    
    printf("Loading detailed macro descriptions\n");
    for(m_bl=0;m_bl<MAX_MACRO_BLCKS;m_bl++) // All macro blocks
        for(m_n=0;m_n<MAX_MACROS_INBL;m_n++)   // All macro numbers
        {
            sprintf(path,"%sPRG_B%d_M%d.mds",home,m_bl,m_n);
            if((mac_fd=fopen(path,"r"))==NULL) continue;
            YPrintf("Loading detailed macro desc: %s Length: %d\n",path,FileLen(mac_fd));
            
            n_macs++;
            while (fgets(line,255,mac_fd) != NULL)
            {
                if(line[0] == '\n') continue; //Empty line..
                if (line[0] == '#') continue; //Remove comments..
                
                switch(state)
                {
                    case 0:
                        if (!strncmp(line,"<START>",7))  // Found START tag
                        {
                            InitP(&macs[m_bl][m_n]);
                            state=1; // Change to next state
                        }
                        break;
                    case 1: 
                        if (!strncmp(line,"<END>",5))  // Found END tag
                        {
                            state=2;		
                            break;
                        }
                        
                        Separate(line,l_tok,t_tok,'\t',1);    // NOTE: Requires (exactly one) tab between first and second column.
                        Separate(t_tok,n_tok,v_tok,'=',1);
                        TrimWN(l_tok);   // Meaningless? l_tok is never used(?)
                        TrimWN(n_tok);
                        TrimWN(v_tok);
                        sprintf(line,"ROSETTA:%s",n_tok); // New PDS standard! Add ROSETTA:
                        if((&macs[m_bl][m_n])!=NULL) {
                            Append(&macs[m_bl][m_n],line,v_tok);
                        }
                        
                        break;
                }
            }
            
            if(state==0) YPrintf("Warning: couldn't find start tag\n");
            if(state==1) YPrintf("Warning: couldn't find end tag\n");
            state=0; // Restore state
            fclose(mac_fd);
        }
        return n_macs;
}



// Get measured data calibration files 
// Return data in data structure "m".
int  GetMCFiles(char *rpath, char *fpath, m_type *m)
{
    // FKJN 4/9 2014
    // completely rewritten to accommodate for scandir (and alphanumerical sorting)
    prp_type mc_lbl; // Linked property/value list for measured data offset and conversion to volts/ampere.
    property_type *property;
    
    char name[PATH_MAX];
    char *base;                 // Basename
    
    // DIR           *dir;         // Directory
    struct dirent **namelist;   // 
    struct dirent *dentry;      // Directory entry
    int n=0;
    int i=0;
    int len;
    
    base=basename(fpath); // Get basic name for measured data calib files
    
    //if((dir = opendir(rpath))==NULL) // Open Calibration directory
    //FKJN sort it alphanumerically first. 2/9 2014
    len=scandir(rpath, &namelist, 0, alphasort);
    
    if (len <0)
    {
        perror("opendir");
        printf("FKJN message: len = %i  \t rpath = %s",len,rpath);
        YPrintf("Couldn't open calibration directory\n");
        return -1;
    }
    
    dentry=namelist[i];
    //rewinddir(dir);   // Rewind
    //dentry=readdir(dir);  // Get first entry
    
    // First time we just count matching files!
    while(dentry!=NULL) // Do a linear search through all filnames....to find the last one
    {
        if(!Match(base,dentry->d_name)) // Match filename to pattern
        {
            n++;
            YPrintf("Offset and TM conversion files: %s\n",dentry->d_name);  // Matching file name
        }
        i++;
        dentry=namelist[i]; // Get next filename 
        //dentry=readdir(dir); // Get next filename 
    }
    len = i-1;  //we now know length of namelist
    
    //  rewinddir(dir);   // Rewind
    // dentry=readdir(dir);  // Get first entry
    
    if(n==0)
    {
        YPrintf("No offset or factor calibration label files found\n");
        return -2;
    }
    
    // Allocate memory for factor and offset structure arrays..
    m->n=n;
    if((m->CF=(cf_type *)CallocArray(m->n,sizeof(cf_type)))==NULL)
    {
        YPrintf("Error allocating memory for array of factor structures\n");
        
        //closedir(dir);
        return -3;
    }
    
    if((m->CD=(c_type *)CallocArray(m->n,sizeof(c_type)))==NULL)
    {
        YPrintf("Error allocating memory for array of calibration structures\n");
        free(m->CF);
        
        //closedir(dir);
        return -4;
    }
    
    n=0;
    i=0;
    dentry=namelist[i];
    
    while(dentry!=NULL) // Do a linear search through all filnames....to find the last one
    {
        if(!Match(base,dentry->d_name)) // Match filename to pattern
        {
            
            // Modify/update calibration LBL files.
            sprintf(name,"%s%s",rpath,dentry->d_name); // Construct full path
            InitP(&mc_lbl);
            if(ReadLabelFile(&mc_lbl,name)<0) // Read offset and TM calibration file
            {
                // ExitPDS() will free m->CF and m->CD memory at exit.
                FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
                
                //closedir(dir);
                return -5;
            }
            WriteUpdatedLabelFile(&mc_lbl,name);                       // Write back label file with new info
            
            FindP(&mc_lbl,&property,"ROSETTA:LAP_VOLTAGE_CAL_16B",1,DNTCARE);
            sscanf(property->value,"\"%le\"",&m->CF[n].v_cal_16b);
            FindP(&mc_lbl,&property,"ROSETTA:LAP_VOLTAGE_CAL_20B",1,DNTCARE);
            sscanf(property->value,"\"%le\"",&m->CF[n].v_cal_20b);
            FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_16B_G1",1,DNTCARE);
            sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_16b_hg1);
            FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_20B_G1",1,DNTCARE);
            sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_20b_hg1);
            FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_16B_G0_05",1,DNTCARE);
            sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_16b_lg);
            FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_20B_G0_05",1,DNTCARE);
            sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_20b_lg);
            
            if(debug>2)
            {
                printf("ROSETTA:LAP_VOLTAGE_CAL_16B %e\n",m->CF[n].v_cal_16b);
                printf("ROSETTA:LAP_VOLTAGE_CAL_20B %e\n",m->CF[n].v_cal_20b);
                printf("ROSETTA:LAP_CURRENT_CAL_16B_G1 %e\n",m->CF[n].c_cal_16b_hg1);
                printf("ROSETTA:LAP_CURRENT_CAL_20B_G1 %e\n",m->CF[n].c_cal_20b_hg1);
                printf("ROSETTA:LAP_CURRENT_CAL_16B_G0_05 %e\n",m->CF[n].c_cal_16b_lg);
                printf("ROSETTA:LAP_CURRENT_CAL_20B_G0_05 %e\n",m->CF[n].c_cal_20b_lg);
            }
            
            if(ReadTableFile(&mc_lbl,&m->CD[n],rpath)<0) // Read data into array of data structures
            {
                // ExitPDS() will free m->CF and m->CD memory at exit.
                FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
                //closedir(dir);
                return -6;
            }
            n++;
            FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
        }
        i++;
        dentry=namelist[i];
        
        //dentry=readdir(dir); // Get next filename 
    }
    
    for (i = 0; i < len; i++) //need to free array of all “returns”.. some other time.
    {
        free(namelist[i]); //free pointer
    }
    free(namelist); //free pointer to array of pointers
    //closedir(dir);
    return 0;
}



/*
 * int  GetMCFiles_old(char *rpath,char *fpath,m_type *m)
 * {
 * prp_type mc_lbl; // Linked property/value list for measured data offset and conversion to volts/ampere.
 * property_type *property;
 * 
 * char name[PATH_MAX];
 * char *base;                 // Basename
 * 
 * DIR           *dir;         // Directory
 * struct dirent *dentry;      // Directory entry
 * int n=0;
 * 
 * base=basename(fpath); // Get basic name for measured data calib files
 * 
 * if((dir = opendir(rpath))==NULL) // Open Calibration directory
 *    {
 *    perror("opendir");
 *    YPrintf("Couldn't open calibration directory\n");
 *    return -1;
 *    }
 * 
 * rewinddir(dir);   // Rewind
 * dentry=readdir(dir);  // Get first entry
 * 
 * // First time we just count matching files!
 * while(dentry!=NULL) // Do a linear search through all filnames....to find the last one
 *    {  
 *    if(!Match(base,dentry->d_name)) // Match filename to pattern
 *    {
 *    n++; 
 *    YPrintf("Offset and TM conversion files: %s\n",dentry->d_name);  // Matching file name
 *    }
 *    dentry=readdir(dir); // Get next filename 
 *    }
 * 
 * rewinddir(dir);   // Rewind
 * dentry=readdir(dir);  // Get first entry
 * 
 * 
 * if(n==0)
 *    {
 *    YPrintf("No offset or factor calibration label files found\n");
 *    return -2;
 *    }
 * 
 * // Allocate memory for factor and offset structure arrays..
 * m->n=n;
 * if((m->CF=(cf_type *)CallocArray(m->n,sizeof(cf_type)))==NULL)
 *    {
 *    YPrintf("Error allocating memory for array of factor structures\n");
 *    closedir(dir);
 *    return -3;
 *    }
 * 
 * if((m->CD=(c_type *)CallocArray(m->n,sizeof(c_type)))==NULL)
 *    {
 *    YPrintf("Error allocating memory for array of calibration structures\n");
 *    free(m->CF);
 *    closedir(dir);
 *    return -4;
 *    }
 * 
 * n=0;
 * while(dentry!=NULL) // Do a linear search through all filnames....to find the last one
 *    {  
 *    if(!Match(base,dentry->d_name)) // Match filename to pattern
 *    {
 *    sprintf(name,"%s%s",rpath,dentry->d_name); // Construct full path
 *    InitP(&mc_lbl);
 *    if(ReadLabelFile(&mc_lbl,name)<0) // Read offset and TM calibration file
 *        {
 *        // ExitPDS() will free m->CF and m->CD memory at exit.
 *        FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
 *        closedir(dir);
 *        return -5;
 *        }
 * 
 *    SetP(&mc_lbl,"DATA_SET_ID",mp.data_set_id,1);       // Set DATA SET ID in calibration file
 *    SetP(&mc_lbl,"DATA_SET_NAME",mp.data_set_name,1);   // Set DATA SET NAME in calibration file     
 *    WriteLabelFile(&mc_lbl,name);                       // Write back label file with new info
 * 
 *    FindP(&mc_lbl,&property,"ROSETTA:LAP_VOLTAGE_CAL_16B",1,DNTCARE);
 *    sscanf(property->value,"\"%le\"",&m->CF[n].v_cal_16b);
 *    FindP(&mc_lbl,&property,"ROSETTA:LAP_VOLTAGE_CAL_20B",1,DNTCARE);
 *    sscanf(property->value,"\"%le\"",&m->CF[n].v_cal_20b);
 *    FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_16B_G1",1,DNTCARE);
 *    sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_16b_hg1);
 *    FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_20B_G1",1,DNTCARE);
 *    sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_20b_hg1);
 *    FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_16B_G0_05",1,DNTCARE);
 *    sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_16b_lg);
 *    FindP(&mc_lbl,&property,"ROSETTA:LAP_CURRENT_CAL_20B_G0_05",1,DNTCARE);
 *    sscanf(property->value,"\"%le\"",&m->CF[n].c_cal_20b_lg); 
 * 
 *    if(debug>2)
 *        {
 *        printf("ROSETTA:LAP_VOLTAGE_CAL_16B %e\n",m->CF[n].v_cal_16b);
 *        printf("ROSETTA:LAP_VOLTAGE_CAL_20B %e\n",m->CF[n].v_cal_20b);
 *        printf("ROSETTA:LAP_CURRENT_CAL_16B_G1 %e\n",m->CF[n].c_cal_16b_hg1); 
 *        printf("ROSETTA:LAP_CURRENT_CAL_20B_G1 %e\n",m->CF[n].c_cal_20b_hg1);
 *        printf("ROSETTA:LAP_CURRENT_CAL_16B_G0_05 %e\n",m->CF[n].c_cal_16b_lg); 
 *        printf("ROSETTA:LAP_CURRENT_CAL_20B_G0_05 %e\n",m->CF[n].c_cal_20b_lg); 
 *        }
 * 
 *    if(ReadTableFile(&mc_lbl,&m->CD[n],rpath)<0) // Read data into array of data structures
 *        {
 *        // ExitPDS() will free m->CF and m->CD memory at exit.
 *        FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
 *        closedir(dir);
 *        return -6;
 *        }
 *    n++;
 *    FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
 *    }
 *    dentry=readdir(dir); // Get next filename 
 *    }
 * 
 * closedir(dir);
 * return 0;
 * }
 */


// Given a path (p->mcpath) and mission phase abbreviation (m->abbrev), initialize an instance of mp_type with data from the mission calendar file.
// Function previously called "GetMissionP".
int InitMissionPhaseStructFromMissionCalendar(mp_type *m, pds_type *p)
{
    FILE *fd;
    
    char nline[256];  // Input new line assume shorter than 256 characters
    char duration[6]; // Duration of period
    char sdate[11];   // Date from mission calendar in string form
    char abbrev[5];
    int stat;         // Just a status variable
    int dur;
    
    if((fd=fopen(p->mcpath,"r"))==NULL) 
    {
        CPrintf("    Couldn't open mission calendar file\n");
        return -1;
    }
    nline[255]='\0';
    while(fgets(nline,255,fd)!= NULL)
    {
        if(nline[0] == '\n') continue;  // Skip empty lines..
        if(nline[0] == '#')  continue;  // Remove any comments..
        
        // Extract values from columns, two columns at a time.
        Separate(nline, m->phase_name,      abbrev,             ':', 1);  // Get mission phase name (incl. quotes) and abbreviation.
        Separate(nline, sdate,              duration,           ':', 3);  // Get Duration and Date string.
        Separate(nline, m->target_name_did, m->target_id,       ':', 5);  // Get "target name for DATA_SET_ID" and "target id".
        Separate(nline, m->target_type,     m->target_name_dsn, ':', 7);  // Get "target type" and "target name for DATA_SET_NAME".
        
        
        sdate[10]='\0';         // Null terminate
        abbrev[4]='\0';         // Null terminate
        m->target_id[5]='\0';   // Null terminate
        
        // Trim strings of whitespace (all but starting date).
        TrimWN(abbrev);
        TrimWN(duration);
        TrimWN(m->phase_name);
        TrimWN(m->target_name_did);
        TrimWN(m->target_id);
        TrimWN(m->target_type);
        TrimWN(m->target_name_dsn);
        
        if(!strcmp(m->abbrev,abbrev)) // Matching mission phase abbreviation
        {
            // Get new time from mission calendar, convert to seconds
            if((stat=TimeOfDatePDS(sdate,&(m->start)))<0)
                CPrintf("    Error mission phase time conversion: %02d\n",stat);
            
            sscanf(duration,"%d",&dur);
            
            m->stop = m->start + dur*24*3600; // Compute end time. NOTE: Does not take leap seconds (e.g. 2015-06-30, 23:59.60) into account.
            
            char* descr;
            if(calib) {
                descr = "CALIB";
            } else {
                descr = "EDITED";
            }
            DeriveDSIandDSN(
                m->data_set_id, m->data_set_name,
                m->target_id, p->DPLNumber, m->abbrev, descr, p->DataSetVersion, m->target_name_dsn);
            
            fclose(fd); // Close mission calendar
            return 0;   // Ok, Returns using previous ID and name, exactly what we want! 
        }
    }
    fclose(fd);
    
    printf("Could not find the mission phase in the mission calendar file.\n");
    CPrintf("    Could not find the mission phase in the mission calendar file.\n");
    return -3; // No phase found
}



// Derive DATA_SET_ID and DATA_SET_NAME keyword values, INCLUDING QUOTES!
void DeriveDSIandDSN(
    char* DATA_SET_ID, char* DATA_SET_NAME,
    char* targetID, int DPLNumber, char* mpAbbreviation, char* descr, float dataSetVersion, char* targetName_dsn)
{
    sprintf(DATA_SET_ID,  "\"RO-%s-RPCLAP-%d-%s-%s-V%3.1f\"",              targetID,       DPLNumber, mpAbbreviation, descr, dataSetVersion);
    sprintf(DATA_SET_NAME,"\"ROSETTA-ORBITER %s RPCLAP %d %s %s V%3.1f\"", targetName_dsn, DPLNumber, mpAbbreviation, descr, dataSetVersion);
}



// Dump macros..this is for debugging
void TestDumpMacs()
{
    int i,j;
    for(i=0;i<10;i++) {
        for(j=0;j<8;j++) {
            DumpPrp(&macros[i][j]);
        }
    }
}



// Write updated label file
// 
// "updated" refers to certain keyword values being set by the function.
// The Function is in practise used for updating LBL/CAT files copied from the template.
int WriteUpdatedLabelFile(prp_type *lb_data,char *name)
{
    // Added by Erik P G Johansson 2015-05-04
    // Sets/updates certain keywords BUT ONLY IF THEY CAN BE FOUND in the original list.
    // Not all LBL/CAT files should contain all of these keywords.
    // 
    // NOTE: Does not set VOLUME_ID, VOLUME_NAME which need extra code to be derived and only apply   to VOLDESC.CAT (?).
    // NOTE: Does not set DATA_SET_RELEASE_DATE which needs extra code to be derived and only applies to DATASET.CAT (?).
    SetP(lb_data, "DATA_SET_ID",        mp.data_set_id,           1);
    SetP(lb_data, "DATA_SET_NAME",      mp.data_set_name,         1);  
    SetP(lb_data, "PUBLICATION_DATE",   pds.ReleaseDate,          1); // Set publication date of dataset
    SetP(lb_data, "PRODUCER_ID",        "EJ",                     1);  
    SetP(lb_data, "PRODUCER_FULL_NAME", "\"ERIK P G JOHANSSON\"", 1);
    SetP(lb_data, "MISSION_PHASE_NAME", mp.phase_name,            1);  
    
    FILE *fd;
    if((fd=fopen(name,"w"))==NULL)
    {
        YPrintf("Error writing to: %s\n",pds.cpathc);
        return -1;
    }
    else
    {
        FDumpPrp(lb_data,fd); // Dump new label file
        fprintf(fd,"END\r\n");
        fclose(fd);
        return 0;
    }
}



// Read label file
// 
// NOTE: The function is implicitly used for modifying LBL/CAT files from the template directory
// by first reading a file into linked list, then modifying the linked property values list,
// then writing the linked list as LBL file.
// NOTE: I _think_ this code can handle LBL and CAT files
// with multiple line values. This is important for being able to modify *.CAT files
// with long texts in the form of "values".   /Erik P G Johansson 2015-04-27.
int ReadLabelFile(prp_type *lb_data,char *name)
{
    FILE *fd;
    char line[MAX_STR];   // Line buffer
    char l_tok[MAX_STR];  //
    char r_tok[MAX_STR];  //
    
    int pos1,pos2;
    int ch;
    int len1,len2;
    int cnt;
    int i;
    
    int msubone;
    char *bcross;    // Dynamic buffer for keywords that cross several lines
    // such as description fields, they can be very large
    
    InitP(lb_data);     // Initialize linked property/value list
    
    bcross  = NULL;
    msubone = (MAX_STR-1);
    

    // Open label file, for description of calibration table
    if((fd=fopen(name,"r"))==NULL)
    {
        YPrintf("Couldn't open label file:  %s\n",name);
        return -1;
    }
    
    while(fgets(line,msubone,fd)!= NULL)   // Reads line into variable "line". Appears to include ending line feed.
    {
        if(line[0] == '\n') continue;     // Empty line..
        if(line[0] == '\r') continue;
        if(line[0] == '#') continue;      // Remove comments..
        if(line[0] == '/') continue;      // Remove comments..
        
        if(!(strcmp(line,"END") && strcmp(line,"END\r\n"))) {
            continue; // Skip ending END not a keyword
        }
        if(Separate(line,l_tok,r_tok,'=',1)==0) {
            continue;
        }
        
        TrimWN(l_tok);
        TrimWN(r_tok);   // NOTE: Also trims away CR and/or LF at the end of a string.
        
        len1=strlen(r_tok);
        len2=strlen(line);
        
        if(len1>=MAX_STR) {
            len1=msubone;
        }
        
        // Count number of double-quotes ON THE LINE JUST READ.
        cnt=0;
        for(i=0;i<len1;i++) {
            if(r_tok[i]=='"') {
                cnt++;
            }
        }
        
        switch(cnt)
        {
            case 0:
            case 2:
                Append(lb_data,l_tok,r_tok);
                break;
            case 1:
                pos1=ftell(fd);    // Get current file position.
                
                // Continue reading file until next double-quote character AFTER THE LINE JUST READ.
                // NOTE: READS OVER MULTIPLE LINES IF NECESSARY.
                do
                {
                    ch=fgetc(fd);     // Read next character.
                } while(ch>=0 && ch!='"');
                
                if(ch=='"')    
                {
                    pos2=ftell(fd);
                    len2=pos2-pos1;    // Number of bytes read in the previous do-while loop.
                    
                    len1=strlen(r_tok); // New length r_tok is trimmed
                    
                    // Add CR+LF to the r_tok.
                    r_tok[len1]='\r';
                    r_tok[len1+1]='\n';
                    r_tok[len1+2]='\0';
                    len1 = len1 + 2;
                    
                    bcross=(char *)malloc(len2+len1+1);
                    if(bcross!=NULL)
                    {
                        fseek(fd,pos1,SEEK_SET);
                        strncpy(bcross,r_tok,len1);
                        fread(&bcross[len1],1,len2,fd);
                        bcross[len1+len2]='\0';
                        Append(lb_data,l_tok,bcross);
                        free(bcross);
                    }
                }
                break;
            default:
                break;
        }
    }
    
    if(debug>3)
    {
        DumpPrp(lb_data); // Debug
    }
    
    fclose(fd);
    return 0;
}



// Read table file described in "lbl_data" into
// the calibration structure "cal".
//
// I try to use the description in the label file as much as possible
// but it could probably be done better.
//
int ReadTableFile(prp_type *lbl_data, c_type *cal, char *path)
{
    int i,j;
    
    char name[PATH_MAX];
    
    // Following arrays are taken to be large enough for their purposes
    char format[256]; // A vector indicating column format..thus max 256 columns..we have 4 in reality.
    char line[8192];  // Temporary buffer  to read a row into.
    int start[256];   // A vector indicating start positions of columns
    
    // Pointers to properties
    property_type *property1;
    property_type *property2;
    property_type *property3;
    property_type *property4;
    property_type *property5;
    property_type *property6;
    
    int nrows;  // Number of rows
    int ncols;  // Number of columns
    
    FILE *fd;   // File descriptor
    
    int startpos; // Start position of a column
    int tmp1;     // Temporary data value
    double tmp2;   // Temporary data value
    
    // Interpreting label file data
    FindP(lbl_data,&property1,"^TABLE",1,DNTCARE);        // Get file name
    FindP(lbl_data,&property2,"START_TIME",1,DNTCARE);    // Get start time
    FindP(lbl_data,&property3,"ROWS",1,DNTCARE);          // Number of rows 
    FindP(lbl_data,&property4,"COLUMNS",1,DNTCARE);       // Number of columns
    strncpy(line,property1->value,8192);                   // Copy so we don't modify the real property list
    TrimQN(line);                                          // Trim quotes away..
    sprintf(name,"%s%s",path,line);                        // Construct path + filname
    
    YPrintf("Reading table file: %s\n",name);
    // Open calibration table file
    if((fd=fopen(name,"r"))==NULL)
    {
        YPrintf("Couldn't open table file:  %s\n",name);
        return -1;
    }
    
    if(debug>1) // If debugging level is larger than 1 
    {
        printf("Rows:    %s\n",property3->value);
        printf("Columns: %s\n",property4->value);
    }
    
    sscanf(property3->value,"%d",&nrows);
    sscanf(property4->value,"%d",&ncols);
    
    cal->C=CallocDoubleMatrix(nrows,ncols); // Allocate a matrix of doubles
    
    cal->rows=nrows; // Rows
    cal->cols=ncols; // Columns
    
    if(property2!=NULL) {
        strcpy(cal->validt,property2->value); // Copy valid from time
    } else {
        strcpy(cal->validt,"2003-01-01T00:00:00.000");
    }
    
    
    // Interpret label file
    for(j=0;j<ncols;j++) // Go through columns
    {
        FindP(lbl_data,&property5,"DATA_TYPE",j+1,DNTCARE); // What kind of column are we dealing with?
        
        if(!strncmp(property5->value,"ASCII_INTEGER",7)) { // Integer column ?
            format[j]=0;
        } else {
            if(!strncmp(property5->value,"ASCII_REAL",10)) { // Float column ?
                format[j]=1;
            } else {
                return -2; // Error..we only expect integer or double for now!
            }
        }
        
        FindP(lbl_data,&property6,"START_BYTE",j+1,DNTCARE); // Get start byte in line
        sscanf(property6->value,"%d",&start[j]);
    }
    
    // Read and parse table rows
    i=0;
    while(fgets(line,255,fd)!= NULL)
    {
        for(j=0;j<ncols;j++)
        {
            startpos=start[j]-1; // Compute start position of column
            if(format[j]==0) // Integer
            {
                sscanf(&line[startpos],"%d",&tmp1);
                tmp2=(float)tmp1;
            }
            
            if(format[j]==1) // Float
                sscanf(&line[startpos],"%le",&tmp2);
            
            cal->C[i][j]=tmp2; // Store data in matrix
        }
        i++; // Next row
    } 
    
    
    if(debug>2) // If debugging level is larger than 2 
    {
        printf("Valid from: %s\n",cal->validt);
        for(i=0;i<nrows;i++)
        {
            for(j=0;j<ncols;j++) {
                printf("%e ",cal->C[i][j]);
            }
            printf("\n");
        }
    }
    fclose(fd);
    return 0; // Ok
}



// Return the bias mode for a given "dop" value as it used for when writing to file.
// dop : See "WritePTAB_File".
// NOTE: The return type is chosen to agree with pds.h:curr_type_def#bias_mode1/2.
char getBiasMode(curr_type *curr, int dop) {
    if (curr->sensor==SENS_P1 || dop==1) {
        return curr->bias_mode1;
    } else if (curr->sensor==SENS_P2 || dop==2) {
        return curr->bias_mode2;
    } else if (curr->sensor==SENS_P1P2 && dop==0) {
        if (curr->bias_mode1 != curr->bias_mode2) {
            CPrintf("ASSERTION ERROR: getBiasMode: curr->bias_mode1 != curr->bias_mode2 for dop=0.\n");
            YPrintf("ASSERTION ERROR: getBiasMode: curr->bias_mode1 != curr->bias_mode2 for dop=0.\n");
        }
        return curr->bias_mode1;
    }
    
    CPrintf("Error: getBiasMode: Can not determine bias mode (curr->sensor=%i).\n", curr->sensor);
    return -1;
}



/*
 * Write TAB file, CALIB or EDITED.
 * Convert from TM units to physical units (i.e. calibrate) in CALIB.
 *
 *  sweep_type   sw_info;           // Sweep info structure steps, step height, duration, ....
 *  adc20_type   a20_info;          // ADC 20 info structure resampling, moving average, length, ...
 *
 * We use the mode information from command logs here since it also contain off information
 *
 * Uncertain what "dop" means, and compared to curr->sensor. Compare "dop" in WritePLBL_File.
 *    NOTE: Check how dop is set in calls to WritePTAB_File and WritePLBL_File. It is always a literal.
 *    NOTE: Check how dop is used in WritePLBL_File (very little).
 *    NOTE: "dop" seems different from curr_type_def#sensor. sensor==0 has different meaning at the very least.
 *    GUESS: curr-->sensor refers to the probe(s) for which there is data.
 *    GUESS: The combination of curr.sensor and "dop" determines which probe is being written to disk.
 *       curr.sensor=SENS_P1
 *          dop=0, 1 : P1
 *       curr.sensor=SENS_P2
 *          dop=0, 2 : P2
 *       curr.sensor=SENS_P1P2
 *          dop=0 : P3
 *          dop=1 : P1
 *          dop=2 : P2
 *       <=> (This is equivalent with...)
 *       dop=0
 *          curr.sensor=SENS_P1   : P1 
 *          curr.sensor=SENS_P2   : P2
 *          curr.sensor=SENS_P1P2 : P3
 *       dop=1
 *          curr.sensor=SENS_P1, SENS_P1P2 : P1
 *       dop=2
 *          curr.sensor=SENS_P2, SENS_P1P2 : P2
 * 
 */
int WritePTAB_File(
    unsigned char *buff,
    char *fname,
    int data_type,
    int samples,
    int id_code,
    int length,
    sweep_type *sw_info,
    curr_type *curr,
    int param_type,
    int dsa16_p1,
    int dsa16_p2,
    int dop,
    m_type *mc,
    unsigned int **bias,
    int nbias,
    unsigned int **mode,
    int nmode,
    int ini_samples,
    int samp_plateau)
{
    char tstr1[256];                // Temporary string
    char tstr2[256];
    char tstr3[256];
    
    
    int curr_step=3;                // Current sweep step, default value just there to get rid of compilation warning
    
    double td1;                     // Temporary double
    double td2;                     // Temporary double
    
    int i,j,k,l,m;
    int val;
    int ti2=0;

    int macro_id; 
    int current;                    // Current variable
    int voltage;                    // Voltage variable
    
    int ibias;                      // Temporary current variable
    int vbias;                      // Temporary voltage variable
    
    int ibias1;                      // Temporary current variable
    int vbias1;                      // Temporary voltage variable
    int ibias2;                      // Temporary current variable
    int vbias2;                      // Temporary voltage variable
    
    char bias_mode = getBiasMode(curr, dop);      // Name and type analogous to curr_type_def#bias_mode1/2.
    
    //  double ADC_offset =0.0;	// due to ADC errors around 0 (for 16bit data, possibly 20bit data also), we need to correct small offset
    
    // FKJN: Offset for 20 bit data needs to be taken into account.
    // Equals 16 for "true 20-bit data" (_NON-TRUNCATED_ ADC20 data), otherwise 1.
    // Can be seen as a conversion factor between "16 bit TM units" and the current (16 or 20 bit) "TM units".  /Erik P G Johansson
    double ocalf = 1.0;              
    
    double ccurrent;                // Calibrated current
    double cvoltage;                // Calibrated voltage
    double vcalf;                   // vcalf=Voltage Calibration Factor. Basic conversion TM-to-physical units.
    double ccalf;                   // ccalf=Current Calibration Factor. Basic conversion TM-to-physical units.
    
    // Like vcalf, ccalf, but always a conversion factor derived from ADC16 since this value needed for the calibration of both ADC16 and ADC20 data(!).
    double vcalf_ADC16 = 0.0/0.0;
    double ccalf_ADC16 = 0.0/0.0;
    
    
    
    time_t old_time;
    time_t stime;                   // Time of current data
    time_t vtime;                   // Valid time of calibration data
    
    double utime;                  // Current time in UTC for test of extra bias settings..
    int valid=0;                   // Valid time index
    
    property_type *property1; 	// declare temporary property1
    int D20_MA_on;		//D20 Moving average bug boolean
    D20_MA_on = 0;
    
    
    // Calibration: Number of TM units to subtract from data after subtracting "macro 104 calibration offsets".
    // --------------------------------------------------------------------------------------------------------
    // Functionality added by Erik P G Johansson 2015-06-02.
    // The actual purpose of this functionality is to remove constant offset from ADC20 data.
    // NOTE: Applies to both voltage and current (TM) data, both E-field and density mode.
    // NOTE: This subtraction is implemented in the code for all ADC20 cases, AND for non-sweep ADC16 cases
    // since sometimes the same code applies to both ADC20 and ADC16 data. For ADC16 data the value is zero.
    // Therefore, this variable can NOT YET be used to subtract values from all data if needed.
    // NOTE: calib_nonsweep_TM_delta does not take high-gain/low gain into consideration but can easily
    // be made to do so in the future. Note that it may not be obvious how to determine high/low gain for P1-P2.
    double calib_nonsweep_TM_delta;
    
    int extra_bias_setting;
    
    vcalf=1.0; // Assume 1 to begin with
    ccalf=1.0; // Assume 1 to begin with
    
    DecodeRawTime(curr->seq_time,tstr1,0); // First convert spacecraft time to UTC to get time calibration right
    
    TimeOfDatePDS(tstr1,&stime);            // Convert back to seconds
    
    if(calib)
    {
        //=============
        // CASE: CALIB
        //=============
        
        
        
        for(i=mc->n-1;i>=0;i--)
        {
            TimeOfDatePDS(mc->CD[i].validt,&vtime); // Convert valid time to seconds		  
            if(vtime<stime) 
            {
                valid=i;
                break;
            }
        }
        
        
        
        //######################################################################################
        //######################################################################################
        // Find the correct calibration factors depending on E-FIELD/DENSITY, GAIN, ADC16/ADC20
        //######################################################################################
        //######################################################################################
        calib_nonsweep_TM_delta = 0.0;   // Valid value until set to be non-zero for some cases.
        const int is_high_gain_P1 = !strncmp(curr->gain1, "\"GAIN 1\"", 8);  // BUGFIX: Now always reads 8 instead of 6 characters. /Erik P G Johansson 2015-06-03.
        const int is_high_gain_P2 = !strncmp(curr->gain2, "\"GAIN 1\"", 8);  // BUGFIX: Now always reads 8 instead of 6 characters. /Erik P G Johansson 2015-06-03.
        if(bias_mode==DENSITY) // Check if current data originates from a density mode measurement
        {
            //====================
            // CASE: DENSITY MODE
            //====================
            if(data_type==D16)
            {
                //=================
                //   CASE: ADC16
                //=================
                
                // NOTE: We have the same calibration factor for both P1 and P2 for now!
                if(curr->sensor==SENS_P1 || dop==1)
                {
                    //============
                    //  CASE: P1
                    //============
                    if (is_high_gain_P1) {
                        ccalf=mc->CF[valid].c_cal_16b_hg1;
                    } else {
                        ccalf=mc->CF[valid].c_cal_16b_lg;
                    }
                }
                
                if(curr->sensor==SENS_P2 || dop==2)
                {
                    //============
                    //  CASE: P2
                    //============
                    if (is_high_gain_P2) {
                        ccalf=mc->CF[valid].c_cal_16b_hg1;
                    } else {
                        ccalf=mc->CF[valid].c_cal_16b_lg;
                    }
                }
                
                if(curr->sensor==SENS_P1P2 && dop==0)
                {
                    //============
                    //  CASE: P3
                    //============
                    // NOTE: USES P1 to determine high/low gain for P3 for now!!! Undetermined what one should really use.
                    if (is_high_gain_P1) {
                        ccalf=mc->CF[valid].c_cal_16b_hg1;
                    } else {
                        ccalf=mc->CF[valid].c_cal_16b_lg;
                    }
                }
                
                ccalf_ADC16 = ccalf;
                // Other alternatives than above shouldn't be possible. If so keep default ccalf.
            }   // if(data_type==D16)
            else
            {
                //==============================
                //  CASE: ADC20 (Density mode)
                //==============================
                // NOTE: No initialization for the combination ADC20+Density mode+(P1-P2) (SENS_P1P2)
                // since this case is not physically interesting (although possible to use).
                
                if(curr->sensor==SENS_P1 || dop==1)
                {
                    //============
                    //  CASE: P1
                    //============
                    // Seĺect ADC20 calibration "delta" constant. This should ideally depend on high/low gain
                    // but we use the high-gain value for now. /Erik P G Johansson 2015-06-03.
                    calib_nonsweep_TM_delta = CALIB_ADC_G1_TM_DELTA_P1;
                    if (is_high_gain_P1) {
                        //printf("GAIN 1 20 Bit P1\n");
                        ccalf       = mc->CF[valid].c_cal_20b_hg1;
                        ccalf_ADC16 = mc->CF[valid].c_cal_16b_hg1 / 16;   // Should always be ADC16 value.
                    } else {
                        //printf("GAIN 0.05 20 Bit P1\n");
                        ccalf       = mc->CF[valid].c_cal_20b_lg;
                        ccalf_ADC16 = mc->CF[valid].c_cal_16b_lg / 16;   // Should always be ADC16 value.
                    } 
                    
                    if(data_type==D20T || data_type==D201T) { // If using ADC20 data that is truncated by 4 bits, increase (ADC20-)calibration factor by 16.
                        ccalf       *= 16;
                        ccalf_ADC16 *= 16;
                    }
                }
                
                if(curr->sensor==SENS_P2 || dop==2)
                {
                    //============
                    //  CASE: P2
                    //============
                    // Seĺect ADC20 calibration "delta" constant. This should ideally depend on high/low gain but
                    // we use the high-gain value for now. /Erik P G Johansson 2015-06-03.
                    calib_nonsweep_TM_delta = CALIB_ADC_G1_TM_DELTA_P2;
                    if (is_high_gain_P2) {
                        //printf("GAIN 1 20 Bit P2\n");
                        ccalf       = mc->CF[valid].c_cal_20b_hg1;
                        ccalf_ADC16 = mc->CF[valid].c_cal_16b_hg1 / 16;   // Should always be ADC16 value.
                    } else {
                        //printf("GAIN 0.05 20 Bit P2\n");
                        ccalf       = mc->CF[valid].c_cal_20b_lg;
                        ccalf_ADC16 = mc->CF[valid].c_cal_16b_lg / 16;   // Should always be ADC16 value.
                    }
                    
                    if(data_type==D20T || data_type==D202T) { // If using ADC20 data that is truncated by 4 bits, increase (ADC20-)calibration factor by 16.
                        ccalf       *= 16;
                        ccalf_ADC16 *= 16;
                    }
                }
                // Other alternative than above shouldn't be possible..if so keep default ccalf
            }
            
        }   // if(bias_mode==DENSITY)
        else
        {
            //====================
            // CASE: E-FIELD MODE
            //====================
            if(data_type==D16) {
                //=================
                //   CASE: ADC16
                //=================
                vcalf=mc->CF[valid].v_cal_16b;
                vcalf_ADC16 = vcalf;
            }
            else
            {
                //=================
                //   CASE: ADC20
                //=================
                vcalf       = mc->CF[valid].v_cal_20b;
                vcalf_ADC16 = mc->CF[valid].v_cal_16b / 16;   // Should always be ADC16 value.
                if (data_type==D20T || data_type==D201T || data_type==D202T) {
                    vcalf       *= 16;   // Increase cal factor by 16 for truncated ADC20 data.
                    vcalf_ADC16 *= 16;
                }
                
                if      (curr->sensor==SENS_P1P2 || dop==0) { calib_nonsweep_TM_delta = CALIB_ADC_G1_TM_DELTA_P1 - CALIB_ADC_G1_TM_DELTA_P2; }    // Should be "&&" in condition?!
                else if (curr->sensor==SENS_P1   || dop==1) { calib_nonsweep_TM_delta = CALIB_ADC_G1_TM_DELTA_P1;                            }
                else if (curr->sensor==SENS_P2   || dop==2) { calib_nonsweep_TM_delta = CALIB_ADC_G1_TM_DELTA_P2;                            }
            }
        }
    }   //  if(calib) ...
    

    
    //###############################
    //###############################
    // START WRITING DATA TABLE FILE
    //###############################
    //###############################
    
    strcpy(tstr2,pds.spaths);  // Copy data path
    strcat(tstr2,fname);       // For now put in root path, so add file name!
    
    // TABLE COLUMNS AND POSITIONS                                        
    // 1234567890123456789012345<26><27>  POSITIONS                  
    // SSSS,ffffff, 32767,-32768<CR><LF>  EXAMPLE ROW
    
    if((pds.stable_fd=fopen(tstr2,"w"))==NULL)                       // Open table file
    {
        CPrintf("    Couldn't open PDS TAB data file: %s!!\n",tstr2);
        return -1;
    }
    else
    {
        if(id_code==D_SWEEP_P2_LC_16BIT_BIP || id_code==D_SWEEP_P1_LC_16BIT_BIP) // Log compression used
            LogDeComp(buff,length,ilogtab); // Decompress log data in buff result returned in buff
            
            
            if(param_type==SWEEP_PARAMS)
            {
                if((sw_info->formatv ^(sw_info->formatv<<1)) & 0x2) // Decode the four sweep types
                    curr_step=-sw_info->height; // Store height of step and set sign to a down sweep 
                    else
                        curr_step=sw_info->height;  // Store height of step and set sign to a up sweep
                        ti2=sw_info->start_bias;      // Get start bias 
            }
            
            //============
            // Set biases
            //============
            if(curr->sensor==SENS_P1 || dop==1)
            {
                vbias=curr->vbias1;
                vbias1=curr->vbias1;
                ibias=curr->ibias1;
                ibias1=curr->ibias1;
            }
            
            if(curr->sensor==SENS_P2 || dop==2)
            {
                vbias=curr->vbias2;
                vbias2=curr->vbias2;
                ibias=curr->ibias2;
                ibias2=curr->ibias2;
            }
            
            // BUGFIX: if statement needed to interpret data for P3 (E field), macro 700, for 2006-12-19.
            // /Erik P G Johansson 2016-03-09
            if(curr->sensor==SENS_P1P2 && dop==0)
            {
                // P3=P1-P2 difference
                ibias1 = curr->ibias1;
                ibias2 = curr->ibias2;
                vbias1 = curr->vbias1;
                vbias2 = curr->vbias2;
            }
            
            old_time=0;
            
            
            // check if moving average is one. if so, and 
            FindP(&dict,&property1,"ROSETTA:LAP_P1P2_ADC20_MA_LENGTH",1,DNTCARE); // Do we have ADC20 data that is downsampled? output e.g. 0x0040
            if(property1!=NULL) {
                D20_MA_on = atoi(strndup(property1->value+2,4))>0;
            }
            
            
            //=================================================================
            // Iterate over all samples (i.e. over every row in the TAB file).
            //=================================================================
            for(k=0,i=0,j=0;i<samples;i++) 
            {
                // Convert data from signed 16 bit and signed 20 bit to native signed integer
                // In case of alternating 20 bit P1 and P2, this will be split into two label
                // and tab files.
                switch(data_type)
                {
                    case D20:
                        ocalf = 16.0;
                        // Put together 8+8+4 bits to signed 20 bit number
                        if(dop==2) // Doing probe 2, skip probe 1
                            j+=2;
                        // (Likely) EDIT FKJN 12/2 2015. ERROR IN LAP MOVING AVERAGE FLIGHT SOFTWARE FOR 20 BIT DATA, MAKING THE LAST 4 BITS GARBAGE.
                        val=buff[j]<<12 | buff[j+1]<<4 | (((buff[samples*2+(i>>1)])>>(4*((i+1)%2))) & 0x0F);
                        SignExt20(&val); // Convert 20 bit signed to native signed
                        if(D20_MA_on) {
                            val = val & 0xFFFF0;  // Clear the last 4 bits since a moving-average bug in the flight software renders them useless.
                        }
                        j+=2;
                        if(dop==1) { // Doing probe 1, skip probe 2
                            j+=2;
                        }
                        break;
                        
                    case D201:
                    case D202:
                        ocalf = 16.0;
                        // Put together 8+8+4 bits to signed 20 bit number
                        // EDIT FKJN 12/2 2015. ERROR IN LAP MOVING AVERAGE FLIGHT SOFTWARE FOR 20 BIT DATA, MAKING THE LAST 4 BITS GARBAGE.
                        val=buff[j]<<12 | buff[j+1]<<4 | (((buff[samples*2+(i>>1)])>>(4*((i+1)%2))) & 0x0F);
                        SignExt20(&val); // Convert 20 bit signed to native signed
                        if(D20_MA_on) {
                            val = val & 0xFFFF0;  // Clear the last 4 bits since a moving-average bug in the flight software renders them useless.
                        }
                        j+=2;
                        break;
                        
                    case D20T:
                        // Put together 8+8 bits to 16 bit number
                        if(dop==2) {// Doing probe 2, skip probe 1
                            j+=2;
                        }
                        val=((short int)(buff[j]<<8 | buff[j+1]));
                        j+=2;
                        if(dop==1) {// Doing probe 1, skip probe 2
                            j+=2;
                        }
                        break;
                        
                    case D16:
                    default:
                        val=((short int)(buff[j]<<8 | buff[j+1])); // Convert 16 bit into native
                        j+=2;
                }
                
                td1=i*curr->factor; // Time to add
                
                td2=curr->seq_time+td1;
                DecodeRawTime(td2,tstr3,1); // Decode raw time to UTC
                
                if(nbias>0 && bias!=NULL)
                {
                    // Figure out if any extra bias settings have been done outside of macros
                    utime=(unsigned int)stime+td1; // Current time in raw utc format
                    
                    extra_bias_setting=0;
                    for(l=(nbias-1);l>=0 && extra_bias_setting==0;l--) // Go through all extra bias settings
                    {
                        if(bias[l][0]<=utime && bias[l][0]>old_time) // Find any bias setting before current time
                        {
                            extra_bias_setting=1;
                            for(m=0;m<nmode;m++)
                            {
                                old_time=utime;
                                if(mode[m][0]>bias[l][0] && mode[m][0]<=utime) // Check a mode change happened after a extra bias setting
                                {
                                    extra_bias_setting=0;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    
                    /* FKJN 2014-09-25: Extra bias commands are only allowed for certain macros!!! That's three exclamation marks.
                     * We need to find the macro, compare to a list of macros and decide if we should let it pass or not.
                     * If a bias command is issued on forbidden macros, the bias will change for maximum one Macro Loop (see Meds),
                     * but pds will not know when it changes back.
                     */
                    // FKJN 2014-10-31 added macro 515 & 807
                    if(extra_bias_setting)
                    {
                        FindP(&comm,&property1,"INSTRUMENT_MODE_ID",1,DNTCARE);	 //tstr4 is now macro ID on form "MCID0X%04x" we need the 4 numerals.
                        
                        /* BUG FIX: Old code interpreted string as a decimal number when it should be a hexadecimal number.
                         * Old code should not have been a problem as long as
                         * (1) one does not need to check for macro numbers containing letters, and
                         * (2) if the code failed in a good way for non-decimal numbers which it seemed to do.
                         * /Erik P G Johansson 2015-12-07
                         */            
                        //macro_id = atoi(strndup(property1->value+6,4)); // Strip off "MCID0X" and converted to a DECIMAL number.  
                        char* tempstr = strndup(property1->value+6,4);   // Extract 4 (hex) digits. (Remove non-digit characters "MCID0X".)
                        sscanf(tempstr, "%x", &macro_id);    // Interpret string as a HEXADECIMAL representation of a number.
                        free(tempstr);
                        
                        if( macro_id == 0x505 || macro_id == 0x506 || macro_id == 0x604 || macro_id == 0x515 || macro_id == 0x807 )
                        {
                            extra_bias_setting = 0;
                            YPrintf("Forbidden bias setting found at %s. Macro %x\n", tstr1, macro_id);
                            // Add some way of detecting that this is the first macro loop with extra_bias_settings?
                        }
                    }
                    
                    if(extra_bias_setting)
                    {
                        vbias1 = ((bias[l][1] & 0xff00)>>8);  // Override macro present voltage bias p1
                        vbias2 =  (bias[l][1] & 0xff);        // Override macro present voltage bias p2
                        ibias2 = ((bias[l][2] & 0xff00)>>8);  // Override macro present current bias p1 
                        ibias1 =  (bias[l][2] & 0xff);        // Override macro present current bias p2 
                        /*** The above lines were corrected by aie@irfu.se 120822 as the current bias is permuted 
                         * in the bias command. Original code:
                         * curr->ibias1=((bias[l][2] & 0xff00)>>8);  // Override macro present current bias p1 
                         * curr->ibias2=(bias[l][2] & 0xff);         // Override macro present current bias p2 
                         ***/
                        
                        CPrintf("    Extra bias setting applied at: %s \n",tstr3);
                        CPrintf("      Density P1: 0x%02x P2: 0x%02x\n",vbias1,vbias2);
                        CPrintf("      E_Field P1: 0x%02x P2: 0x%02x\n",ibias1,ibias2);
                        
                        // Override biases
                        if(curr->sensor==SENS_P1 || dop==1)
                        {
                            vbias=vbias1;
                            ibias=ibias1;
                        }
                        
                        // Override biases
                        if(curr->sensor==SENS_P2 || dop==2)
                        {
                            vbias=vbias2;
                            ibias=ibias2;
                        }
                        
                        extra_bias_setting=0;
                    }
                }
                
                // EDIT FKJN 27/8 2014 ADC ZERO POINT OFFSET ERROR 
                if(data_type==D16) {
                    if(val>=0) {
                        val = val +2; // This value jumps when adc is set to 0, it would be nice to set it to 2.5 or something, but edited can't handle that.
                    }
                }
                
                //=====================================================================
                // Set "current" and "voltage"
                // ---------------------------
                // NOTE: This is the only place where "current" and "voltage" are set.
                // Always measured current in density mode. Current bias in E field mode for P1, P2 data (undefined for P3).
                // Always measured voltage in E field mode. Voltage bias in density mode for P1, P2 data (undefined for P3).
                //=====================================================================
                if(bias_mode==DENSITY)
                {
                    //====================
                    // CASE: DENSITY MODE
                    //====================
                    current=val; // Set sampled current value in TM units
                    if(param_type==SWEEP_PARAMS) // Do we have a sweep ?...
                    {
                        if(i<ini_samples) { // Sweep started ?
                            voltage=vbias; // Set initial voltage bias before sweep starts. Not defined for P3.
                        }
                        else
                        { 
                            voltage=ti2; // Set value used before changing the bias, prevents start bias value to be modified before used... 
                            k++;
                            if(!(k%samp_plateau)) {   // Every new step set a new bias voltage
                                ti2+=curr_step;     // Change bias
                            }
                            if(sw_info->formatv & 0x1) // If up-down or down-up sweep, check if direction shall change
                            {
                                if(k==(sw_info->steps*samp_plateau/2)) { // Time to change direction ? 
                                    curr_step=-curr_step; // Change direction
                                }
                            }
                        }
                    }
                    else
                    {
                        voltage=vbias; // Set FIX Density bias in TM unit. Not defined for P3.
                    }
                }
                else
                {
                    //====================
                    // CASE: E-FIELD MODE
                    //====================
                    current=ibias; // Set FIX Current bias in TM units. Not defined for P3.
                    voltage=val;   // Set sampled voltage value in TM units
                }
                
                pthread_testcancel();

                

                //###############
                //###############
                // WRITE TO DISK
                //###############
                //###############
                if(calib)
                {
                    //##################
                    // CASE: CALIB data
                    //##################
                    if(bias_mode==DENSITY)
                    {
                            
                        //====================
                        // CASE: DENSITY MODE
                        //====================
                        
                        if(param_type==SWEEP_PARAMS)
                        {
                            //=====================
                            // CASE: SWEEP (ADC16)
                            //=====================
                            // NOTE: No occurrence of SENS_P1P2.
                            if(strcmp(sw_info->resolution,"FINE")) // If it's NOT a fine sweep...
                            {
                                // FKJN 2014-09-02
                                // if 20bit data (non-truncated ADC20), ocalf = 16, otherwise 1.
                                // ccurrent=ccalf*((double)(current-16*mc->CD[valid].C[voltage][1]));
                                if(curr->sensor==SENS_P1)
                                {                                    
                                    ccurrent = ccalf * ((double)(current - ocalf * mc->CD[valid].C[voltage][1])); // Offset and factor calibration
                                    // Write time, current and calibrated voltage 
                                    fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,v_conv.C[voltage][1]); 
                                }
                                
                                if(curr->sensor==SENS_P2)
                                {
                                    ccurrent = ccalf * ((double)(current - ocalf * mc->CD[valid].C[voltage][2])); // Offset and factor calibration
                                    // Write time, current and calibrated voltage 
                                    fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,v_conv.C[voltage][2]); 
                                }
                            }
                            else   // Fine sweep
                            {
                                if(curr->sensor==SENS_P1)
                                {
                                    // Offset and factor calibration, voltage is not entirely correct here!!
                                    // We should perhaps have a calibration mode for fine sweeps in space
                                    // but it would be rather many 4096..
                                    
                                    // Edit FKJN 02/09 2014. Here we need to convert a number from 0-4096 (p1_fine_offs*256 + voltage)
                                    // to a number from 0-255 if we want to use the same offset calibration file
                                    
                                    ccurrent=ccalf*((double)(current - ocalf*mc->CD[valid].C[voltage][1])); 
                                    // Write time, current and calibrated voltage
                                    // EDIT 2014-08-06 FKJN
                                    fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,f_conv.C[(sw_info->p1_fine_offs*256+voltage)][2]); 
                                }
                                
                                if(curr->sensor==SENS_P2)
                                {
                                    // Offset and factor calibration, voltage is not entierly correct here!!
                                    // We should pherhaps have a calibration mode for fine sweeps in space
                                    // but it would be rather many 4096..
                                    
                                    ccurrent=ccalf*((double)(current - ocalf*mc->CD[valid].C[voltage][2]));
                                    // Write time, current and calibrated voltage 
                                    fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,f_conv.C[(sw_info->p1_fine_offs*256+voltage)][3]); 
                                }
                            }
                        }   // if(param_type==SWEEP_PARAMS)
                        else
                        {
                            //===============================
                            // CASE: NOT SWEEP (ADC16/ADC20)
                            //===============================
                            
                            if(curr->sensor==SENS_P1P2 && dop==0)
                            {
                                //ccurrent = ccalf*((double)(current - ocalf * (mc->CD[valid].C[vbias1][1] - mc->CD[valid].C[vbias2][2] - calib_nonsweep_TM_delta))); // Offset and factor calibration
                                ccurrent  = ccalf * ((double) current);
                                ccurrent -= ccalf_ADC16 * ocalf * (mc->CD[valid].C[vbias1][1] - mc->CD[valid].C[vbias2][2]);
                                ccurrent -= ccalf_ADC16 * ocalf * calib_nonsweep_TM_delta;
                                
                                // Write time, current (one difference) and calibrated voltages (two)
                                fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,v_conv.C[vbias1][1],v_conv.C[vbias2][2]); 
                            }
                            
                            if(curr->sensor==SENS_P1 || dop==1)
                            {
                                //ccurrent = ccalf * ((double)(current - ocalf*(mc->CD[valid].C[vbias1][1] - calib_nonsweep_TM_delta))); // Offset and factor calibration                          
                                ccurrent  = ccalf * ((double) current);
                                ccurrent -= ccalf_ADC16 * ocalf * mc->CD[valid].C[vbias1][1];
                                ccurrent -= ccalf_ADC16 * ocalf * calib_nonsweep_TM_delta;
                                
                                // Write time, current and calibrated voltage 
                                fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,v_conv.C[vbias1][1]);
                            }
                            
                            if(curr->sensor==SENS_P2 || dop==2)
                            {
                                //ccurrent = ccalf * ((double)(current - ocalf*(mc->CD[valid].C[vbias2][2] - calib_nonsweep_TM_delta))); // Offset and factor calibration                          
                                ccurrent  = ccalf * ((double) current);
                                ccurrent -= ccalf_ADC16 * ocalf * mc->CD[valid].C[vbias2][2];
                                ccurrent -= ccalf_ADC16 * ocalf * calib_nonsweep_TM_delta;
                                
                                // Write time, current and calibrated voltage 
                                fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,ccurrent,v_conv.C[vbias2][2]);
                            }
                        }   // if(param_type==SWEEP_PARAMS) ... else ...
                        
                    }   // if(bias_mode==DENSITY)
                    else // Assume bias mode is E_FIELD no other possible
                    {
                        //====================
                        // CASE: E-FIELD MODE
                        //====================
                        
                        // NOTE: calib_nonsweep_TM_delta == 0 for ADC16 data.
                        //cvoltage = vcalf * ((double)voltage - calib_nonsweep_TM_delta);   // Voltage offset and factor calibration
                        cvoltage  = vcalf * ((double) voltage);
                        cvoltage -= vcalf_ADC16 * ocalf * calib_nonsweep_TM_delta;
                        
                        
                        if(curr->sensor==SENS_P1P2 && dop==0) {
                            // Write time, calibrated currents (two) and voltage (one).
                            fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e,%14.7e\r\n",tstr3,td2,i_conv.C[ibias1][1],i_conv.C[ibias2][2],cvoltage); // Write time, calibrated currents 1 & 2, and voltage
                        }
                        
                        if(curr->sensor==SENS_P1 || dop==1) {
                            fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,i_conv.C[ibias1][1],cvoltage); // Write time, calibrated current and voltage
                        }
                        
                        if(curr->sensor==SENS_P2 || dop==2) {
                            fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",tstr3,td2,i_conv.C[ibias2][2],cvoltage); // Write time, calibrated current and voltage
                        }
                    }   // if(bias_mode==DENSITY) ... else ...
                }   // if(calib)
                else
                {
                    //###################
                    // CASE: EDITED data
                    //###################
                    if(curr->sensor==SENS_P1P2 && dop==0)
                    {                  
                        // For difference data P1-P2 we need to add two bias vectors. They can be different!
                        if(bias_mode==DENSITY) {
                            fprintf(pds.stable_fd,"%s,%016.6f,%6d,%6d,%6d\r\n",tstr3,td2,current,vbias1,vbias2); // Add two voltage bias vectors
                        } else {
                            fprintf(pds.stable_fd,"%s,%016.6f,%6d,%6d,%6d\r\n",tstr3,td2,ibias1,ibias2,voltage); // Add two current bias vectors
                        }
                    }
                    else {
                        fprintf(pds.stable_fd,"%s,%016.6f,%6d,%6d\r\n",tstr3,td2,current,voltage); // Write time, current and voltage 
                    }
                }   // if(calib) ... else ...
            }   // for(k=0,i=0,j=0;i<samples;i++)    // Iterate over all samples
            
            
            
            fclose(pds.stable_fd);
            pthread_testcancel();
    }  // if((pds.stable_fd=fopen(tstr2,"w"))==NULL) ... else ...

    if(debug>1) // If debugging level is larger than 1, dump common and LAP-dictionary PDS parameters.
    {
        DumpPrp(&comm); // Debugging  
        DumpPrp(&dict); // Debugging  
    }
    
    return 0;
}   // WritePTAB_File



/* WRITE TO DATA LABEL FILE .LBL
 * 
 * Uncertain what "dop" refers to and what the difference compared to "curr-->sensor" is. See "WritePTAB_File" (assuming it has the same meaning).
 * NOTE: This function only has very little dependence on "dop". Compare "WritePTAB_File".
 */
int WritePLBL_File(
    char *path,
    char *fname,
    curr_type *curr,
    int samples,
    int id_code,
    int dop,
    int ini_samples,
    int param_type)
{
    char fullname[PATH_MAX];
    char tstr1[256];
    char tstr2[256];
    
    char bias_mode = getBiasMode(curr, dop);      // Name and type analogous to curr_type_def#bias_mode1/2.
    
    int diff;   // Indicate if it is diff data.
    
    int row_bytes;
    int columns=4;
    int start_byte=1;
    
    sprintf(fullname,"%s%s",path,fname); // Make filename
    CPrintf("    Writing PDS Data label file: %s\n",fname);
    
    if((pds.slabel_fd=fopen(fullname,"w"))==NULL)
    {
        CPrintf("    Couldn't open PDS LBL data file: %s!!\n",fullname);
        return -1;
    }
    else
    {
        // Prepare temporary sensor string
        // At this time sensor is set so we don't bother to check if it's not!
        diff = 0;    // Assume it is not diff data.
        
        switch(curr->sensor)
        {
            case SENS_P1:
                strcpy(tstr1,"P1");
                break;
            case SENS_P2:
                strcpy(tstr1,"P2");
                break;
            case SENS_P1P2:
                if(dop==0)
                {
                    // NOTE/BUG?: The tstr1 value does end up in PDS keyword/attributes and those must not contain
                    // dash. This does however not happen in practice for diff=1.
                    // The tstr1 value also ends up in unquoted PDS __values__ which might not permit dash for "symbolic" values.
                    // Possibly the combinations are such that the value is only used legally for diff=1, dop=0, curr->sensor=SENS_P1P2 (?!!).
                    // Source: "Object Description Language Specification and Usage", Version 3.8, Section 12 (Sections 12.4.2, 12.5, 12.5.4, 12.3.4?)
                    // (Footnote: Rosetta officially uses PDS V3.6 but it is most likely identical to V3.8 here.)
                    strcpy(tstr1,"P1-P2"); // Difference
                    diff=1;   // Must be 16 bit diff data.
                    // The LAP instrument permits using 20 bit data for P3 but that is for some reason not an interesting case
                    // and Anders Eriksson states (2016-03-10) that that will never be used in practice.
                }
                if(dop==1)
                    strcpy(tstr1,"P1");
                if(dop==2)
                    strcpy(tstr1,"P2");
                break;
        }
        
        row_bytes=59;
        
        if(calib) // If we do calibration
        {
            row_bytes+=16; // Calibrated data has wider columns.
            
            if(diff) // Any 16 bit difference data ?
            {
                row_bytes+=15; // Extra calibrated bias column current or voltage
                columns++;     // One extra column
            }
        }
        else
        {
            if(diff)
            {
                row_bytes+=7; // Extra bias column current or voltage
                columns++;    // One extra column
            }
        }
        sprintf(tstr2,"%02d",row_bytes);

        
        // if-else looks unnecessary...
        if(calib) {
            SetP(&comm,"RECORD_BYTES",tstr2,1); // Set number of bytes in a column of a record
        } else {
            SetP(&comm,"RECORD_BYTES",tstr2,1); // Set number of bytes in a column of a record
        }
        
                //#######################
                // NOTE: BAD INDENTATION
                //#######################
                
                if(param_type==SWEEP_PARAMS)
                {
                    // Edit, Erik P G Johansson 2015-02-17: Change of keyword name to be probe-specific.
                    sprintf(tstr2, "ROSETTA:LAP_%s_INITIAL_SWEEP_SMPLS", tstr1);
                    InsertTopQV(&dict, tstr2, ini_samples); 
                    // InsertTopQV(&dict,"ROSETTA:LAP_INITIAL_SWEEP_SMPLS",ini_samples);   // Original line.
                }
                
                
                FDumpPrp(&comm,pds.slabel_fd); // Dump common properties to PDS label file 
                FDumpPrp(&dict,pds.slabel_fd); // Dump LAP dictionary to PDS label file
                
                
                
                // CREATE TABLE OBJECT
                
                // TABLE COLUMNS AND POSITIONS                                     
                // 00000000011111111112222222222333333333344444444445555555555555555 POSITION
                // 12345678901234567890123456789012345678901234567890123456788889999                   
                // YYYY-MM-DDThh:mm:ss.ffffff,FFFFFFFFFFFFFFFF,CCCCCC,VVVVVV<CR><LF>  EXAMPLE ROW
                // 2004-03-18T00:00:09.394193,038188809.394193, 32767,-32768<CR><LF>  EXAMPLE ROW
                fprintf(pds.slabel_fd,"OBJECT     = TABLE\r\n");
                fprintf(pds.slabel_fd,"INTERCHANGE_FORMAT = ASCII\r\n");
                fprintf(pds.slabel_fd,"ROWS               = %d\r\n",samples);
                fprintf(pds.slabel_fd,"COLUMNS            = %d\r\n",columns);
                
                
                fprintf(pds.slabel_fd,"ROW_BYTES          = %d\r\n",row_bytes);
                
                strcpy(tstr2,IDList[id_code]);      // Get ID code name as description
                TrimWN(tstr2);                      // Remove trailing whitespace
                fprintf(pds.slabel_fd,"DESCRIPTION        = \"%s\"\r\n",tstr2); // Add it
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");
                fprintf(pds.slabel_fd,"NAME        = UTC_TIME\r\n");
                fprintf(pds.slabel_fd,"DATA_TYPE   = TIME\r\n");
                fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte); 
                fprintf(pds.slabel_fd,"BYTES       = 26\r\n"); start_byte+=(26+1);
                fprintf(pds.slabel_fd,"DESCRIPTION = \"UTC TIME\"\r\n");
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");
                fprintf(pds.slabel_fd,"NAME        = OBT_TIME\r\n");
                fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                fprintf(pds.slabel_fd,"BYTES       = 16\r\n"); start_byte+=(16+1);
                fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                fprintf(pds.slabel_fd,"UNIT        = SECONDS\r\n");
                fprintf(pds.slabel_fd,"FORMAT      = \"F16.6\"\r\n");
                fprintf(pds.slabel_fd,"DESCRIPTION = \"SPACECRAFT ONBOARD TIME SSSSSSSSS.FFFFFF (TRUE DECIMALPOINT)\"\r\n");
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");

                //===============================================
                // CREATE 1 OR 2 CURRENT COLUMNS (2 ONLY FOR P3)
                //===============================================
                if(calib)
                {
                    // We have difference data P1-P2
                    if(diff && bias_mode==E_FIELD) // We need an extra current bias column
                    {
                        fprintf(pds.slabel_fd,"NAME        = P1_CURRENT\r\n");  
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        fprintf(pds.slabel_fd,"BYTES       = 14\r\n");start_byte+=(14+1);
                        fprintf(pds.slabel_fd,"UNIT        = AMPERE\r\n");
                        fprintf(pds.slabel_fd,"FORMAT      = \"E14.7\"\r\n");
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CALIBRATED CURRENT BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");
                        fprintf(pds.slabel_fd,"NAME        = P2_CURRENT\r\n");  
                    }
                    else
                        fprintf(pds.slabel_fd,"NAME        = %s_CURRENT\r\n",tstr1);  
                    
                    fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                    fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                    fprintf(pds.slabel_fd,"BYTES       = 14\r\n");start_byte+=(14+1);
                    fprintf(pds.slabel_fd,"UNIT        = AMPERE\r\n");
                    fprintf(pds.slabel_fd,"FORMAT      = \"E14.7\"\r\n");
                    
                    if(bias_mode==E_FIELD)
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CALIBRATED CURRENT BIAS\"\r\n");
                    else
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"MEASURED CALIBRATED CURRENT\"\r\n");
                    
                }
                else
                {
                    if(diff && bias_mode==E_FIELD) // We need an extra current bias column
                    {
                        fprintf(pds.slabel_fd,"NAME        = P1_CURRENT\r\n");  
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CURRENT BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");		  
                        fprintf(pds.slabel_fd,"NAME        = P2_CURRENT\r\n");  
                    }
                    else
                        fprintf(pds.slabel_fd,"NAME        = %s_CURRENT\r\n",tstr1);  
                    
                    fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                    fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                    fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                    
                    if(bias_mode==E_FIELD)
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CURRENT BIAS\"\r\n");
                    else
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"MEASURED CURRENT\"\r\n");
                }
                
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");
                
                //===============================================
                // CREATE 1 OR 2 VOLTAGE COLUMNS (2 ONLY FOR P3)
                //===============================================
                if(calib)
                {
                    // We have difference data P1-P2
                    if(diff && bias_mode==DENSITY) // We need an extra voltage bias column
                    {
                        fprintf(pds.slabel_fd,"NAME        = P1_VOLTAGE\r\n");
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        fprintf(pds.slabel_fd,"BYTES       = 14\r\n");start_byte+=(14+1);
                        fprintf(pds.slabel_fd,"UNIT        = VOLT\r\n");
                        fprintf(pds.slabel_fd,"FORMAT      = \"E14.7\"\r\n");
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CALIBRATED VOLTAGE BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");		  
                        fprintf(pds.slabel_fd,"NAME        = P2_VOLTAGE\r\n"); 
                    }
                    else
                        fprintf(pds.slabel_fd,"NAME        = %s_VOLTAGE\r\n",tstr1);  
                    
                    fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                    fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                    fprintf(pds.slabel_fd,"BYTES       = 14\r\n");start_byte+=(14+1);
                    fprintf(pds.slabel_fd,"UNIT        = VOLT\r\n");
                    fprintf(pds.slabel_fd,"FORMAT      = \"E14.7\"\r\n");
                    
                    if(bias_mode==DENSITY)
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CALIBRATED VOLTAGE BIAS\"\r\n");
                    else
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"MEASURED CALIBRATED VOLTAGE\"\r\n");
                    
                }
                else
                {
                    // We have difference data P1-P2
                    if(diff && bias_mode==DENSITY) // We need an extra voltage bias column
                    {
                        fprintf(pds.slabel_fd,"NAME        = P1_VOLTAGE\r\n");
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"VOLTAGE BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");		  
                        fprintf(pds.slabel_fd,"NAME        = P2_VOLTAGE\r\n"); 
                    }
                    else
                        fprintf(pds.slabel_fd,"NAME        = %s_VOLTAGE\r\n",tstr1);  
                    
                    fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                    fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                    fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                    
                    if(bias_mode==DENSITY)
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"VOLTAGE BIAS\"\r\n");
                    else
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"MEASURED VOLTAGE\"\r\n");
                }
                
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                
                fprintf(pds.slabel_fd,"END_OBJECT = TABLE\r\n");
                fprintf(pds.slabel_fd,"END\r\n");
                fclose(pds.slabel_fd);
    }
    return 0;
}   // WritePLBL_File



// Buffer and TM functions
//----------------------------------------------------------------------------------------------------------------------------------

// Free circular buffers 
void FreeBuffs(buffer_struct_type *b0,buffer_struct_type *b1,buffer_struct_type *b2,buffer_struct_type *b3)
{
    YPrintf("Freeing up circular buffers\n");
    if(b0->data!=NULL) FreeBuffer(b0); // Free circular buffer
    if(b1->data!=NULL) FreeBuffer(b1); // Free circular buffer
    if(b2->data!=NULL) FreeBuffer(b2); // Free circular buffer
    if(b3->data!=NULL) FreeBuffer(b3); // Free circular buffer
}

// Get data from circular buffer
// 
// NOTE: Does NOT RETURN until it has all the requested data.
int GetBuffer(buffer_struct_type *bs,unsigned char *buff,int len)
{
    if(len>0)
    {
        while(GetB(bs,buff,len)<0)
        {
            pthread_testcancel(); // Test if we are to die
            sched_yield();        // yield
        }
    }
    return 0;
}

// Look ahead in circular buffer
int LookBuffer(buffer_struct_type *bs,unsigned char *buff,int len)
{
    if(len>0)
    {
        while(LookB(bs,buff,len)<0)
        {
            pthread_testcancel(); // Test if we are to die
            sched_yield();        // nanosleep is not good here, cant delay less than us resolution
        }
    }
    return 0;
}

// Return data for one HK packet.
// 
// NOTE: Does NOT RETURN until it has all the requested data.
int GetHKPacket(buffer_struct_type *ch,unsigned char *buff,double *rawt)
{
    unsigned int length; // Length variable
    
    // Get data
    GetBuffer(ch,buff,16);
    
    length=((buff[4]<<8) | buff[5])-9-2;     // Get LAP HK data length
    HPrintf("HK packet, length: %d\n",length);
    
    if(length>LAP_HK_LEN) {
        length=LAP_HK_LEN; // Packet to long, force standard length...add warning in the future...
    }
    
    *rawt=DecodeSCTime(&buff[6]);            // Decode S/C time into raw time
    
    GetBuffer(ch,buff,2);                    // Skip 2 bytes
    
    GetBuffer(ch,buff,length);  // Get data from circular HK buffer
    
    return 0;
}     

// Skips current TM packet
void DumpTMPacket(buffer_struct_type *cs,unsigned char packet_id)
{
    unsigned int length;
    char tstr[32];
    double rawt;
    unsigned char buff[14];
    
    GetBuffer(cs,buff,14); // Get 14 bytes from circular buffer
    
    length=((buff[2]<<8) | buff[3])-9;           // Get "data" length
    PPrintf("    Packet ID: 0x0d%02x , Data length: %d Discarding\n",packet_id,length);
    rawt=DecodeSCTime(&buff[4]);    // Decode S/C time into raw time
    DecodeRawTime(rawt,tstr,0);     // Decode raw time to PDS compliant date format
    PPrintf("    SCET Time: %s OBT Raw Time: %014.3f\n",tstr,rawt);
    Forward(cs,length);// Move forward, thus skip packet
}


// Test syncronisation ahead..
int SyncAhead(buffer_struct_type *cb,int len)
{
    unsigned char ch;
    unsigned int byte_sum=0;
    struct timespec nap ={0,NAP_TIME}; 
    
    while(byte_sum==0)
    {
        while(Look(cb,&ch,len)<0)
        {
            pthread_testcancel();
            nanosleep(&nap,NULL);    // Sleep, no CPU hogg!
        }
        len++;
        byte_sum+=ch;
        if(len>=RIDICULUS) {
            return -1;
        }
    }
    
    if(byte_sum==M_HEAD || byte_sum==S_HEAD)
        return 1;
    else
        return 0;
}

// Functions handling/working with linked lists of property/value pairs
//----------------------------------------------------------------------------------------------------------------------------------

// Removes all properties and adds them again with default values.
int ClearCommonPDS(prp_type *p)
{ 
    if(FreePrp(p)>=0) // Free old stuff
    {
        Append(p,"PDS_VERSION_ID","PDS3");
        Append(p,"RECORD_TYPE","FIXED_LENGTH");
        Append(p,"RECORD_BYTES","43");
        Append(p,"FILE_RECORDS","XX");
        Append(p,"FILE_NAME","XX");
        Append(p,"^TABLE","XX");
        Append(p,"DATA_SET_ID","\"XX\"");
        Append(p,"DATA_SET_NAME","\"XX\"");
        Append(p,"DATA_QUALITY_ID","\"1\"");
        Append(p,"MISSION_ID","ROSETTA");
        Append(p,"MISSION_NAME","\"INTERNATIONAL ROSETTA MISSION\"");
        Append(p,"MISSION_PHASE_NAME","XX");
        Append(p,"PRODUCER_INSTITUTION_NAME","\"SWEDISH INSTITUTE OF SPACE PHYSICS, UPPSALA\"");
        Append(p,"PRODUCER_ID","EJ");
        Append(p,"PRODUCER_FULL_NAME","\"ERIK P G JOHANSSON\"");
        Append(p,"LABEL_REVISION_NOTE","N/A");
        Append(p,"PRODUCT_ID","XX");
        Append(p,"PRODUCT_TYPE","\"EDR\"");
        Append(p,"PRODUCT_CREATION_TIME","XX");
        Append(p,"INSTRUMENT_HOST_ID","RO");
        Append(p,"INSTRUMENT_HOST_NAME","\"ROSETTA-ORBITER\"");
        Append(p,"INSTRUMENT_NAME","\"ROSETTA PLASMA CONSORTIUM - LANGMUIR PROBE\"");
        Append(p,"INSTRUMENT_ID","RPCLAP");
        Append(p,"INSTRUMENT_TYPE","\"PLASMA INSTRUMENT\"");
        Append(p,"INSTRUMENT_MODE_ID","MCID0X9999");
        Append(p,"INSTRUMENT_MODE_DESC","\"N/A\"");
        Append(p,"TARGET_NAME","XX");
        Append(p,"TARGET_TYPE","XX");
        Append(p,"PROCESSING_LEVEL_ID","\"N\"");
        Append(p,"START_TIME","XX");
        Append(p,"STOP_TIME","XX");
        Append(p,"SPACECRAFT_CLOCK_START_COUNT","\"N/A\"");
        Append(p,"SPACECRAFT_CLOCK_STOP_COUNT","\"N/A\"");
        Append(p,"DESCRIPTION","XX");
        return 0; // Ok!
    }
    return -1; // Err!
} 


int ClearDictPDS(prp_type *p)
{
    if(FreePrp(p)>=0) {// Free old stuff
        return 0; // OK!
    }
    return -1; // Err
}

// Setup HK label
int SetupHK(prp_type *p)
{ 
    if(p!=NULL) 
    {
        Append(p,"OBJECT","TABLE");
        Append(p,"INTERCHANGE_FORMAT","ASCII");
        Append(p,"ROWS","16");
        Append(p,"COLUMNS","29");
        Append(p,"ROW_BYTES",HK_LINE_SIZE_STR);
        Append(p,"DESCRIPTION","\"LAP HK Data table.\"");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","UTC_TIME");
        Append(p,"DATA_TYPE","TIME");
        Append(p,"START_BYTE","1");
        Append(p,"BYTES","26");
        Append(p,"DESCRIPTION","\"UTC TIME\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","OBT_TIME");
        Append(p,"START_BYTE","28");
        Append(p,"BYTES","16");
        Append(p,"DATA_TYPE","ASCII_REAL");
        Append(p,"UNIT","SECONDS");
        Append(p,"FORMAT","\"F16.6\"");
        Append(p,"DESCRIPTION","\"SPACECRAFT ONBOARD TIME SSSSSSSSS.FFFFFF (TRUE DECIMALPOINT)\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");		  
        Append(p,"NAME","PMAC");  
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","45");
        Append(p,"BYTES","1");
        Append(p,"DESCRIPTION","\"CURRENTLY PROGRAMMING MACRO\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");	  
        Append(p,"NAME","EMAC");
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","47");
        Append(p,"BYTES","1");
        Append(p,"DESCRIPTION","\"CURRENTLY EXECUTING MACRO\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","WATCHD");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","49");
        Append(p,"BYTES","8");
        Append(p,"DESCRIPTION","\"WATCHDOG STATUS\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","PROMEN");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","58");
        Append(p,"BYTES","8");
        Append(p,"DESCRIPTION","\"PROM AND FLASH MEMORY STATUS\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","OSC");
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","67");
        Append(p,"BYTES","1");
        Append(p,"DESCRIPTION","\"USING OSCILLATOR 0 or 1\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","LDLMODE");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","69");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"LDL MODE AND PHASE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","TEMP");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","77");
        Append(p,"BYTES","8");
        Append(p,"DESCRIPTION","\"TEMPERATURE SENS STATUS, VALID TEMPERATURE IF IN E-FIELD MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","CDRIV2");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","86");
        Append(p,"BYTES","4");
        Append(p,"DESCRIPTION","\"RANGE PROBE 2 BIAS \"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","CDRIV1");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","91");
        Append(p,"BYTES","4");
        Append(p,"DESCRIPTION","\"RANGE PROBE 1 BIAS \"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","E2D216");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","96");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"ADC 16 PROBE 2 MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","E1D116");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","104");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"ADC 16 PROBE 1 MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","E2D120");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","112");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"ADC 20 PROBE 2 MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","E1D120");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","120");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"ADC 20 PROBE 1 MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","CNTRE2");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","128");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"P2 FEEDBACK\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","CNTRE1");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","136");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"P1 FEEDBACK\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","MIPLAP");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","144");
        Append(p,"BYTES","3");
        Append(p,"DESCRIPTION","\"INSTRUMENT USING PROBE 2\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","BTSTRP");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","148");
        Append(p,"BYTES","8");
        Append(p,"DESCRIPTION","\"INTERNAL BOOTSTRAP STATUS\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","F2122");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","157");
        Append(p,"BYTES","2");
        Append(p,"DESCRIPTION","\"P2 CONNECTED TO, RX=ANALOG INPUT TX=TRANSMITTER\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","F22ED");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","160");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"P2 BIAS MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","F22EDDEDC");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","168");
        Append(p,"BYTES","5");
        Append(p,"DESCRIPTION","\"P2 DENSITY RANGE OR E-FIELD STRATEGY\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","F1121");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","174");
        Append(p,"BYTES","2");
        Append(p,"DESCRIPTION","\"P1 CONNECTED TO, RX=ANALOG INPUT TX=TRANSMITTER\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","F11ED");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","177");
        Append(p,"BYTES","7");
        Append(p,"DESCRIPTION","\"P1 BIAS MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","F11EDDEDC");
        Append(p,"DATA_TYPE","CHARACTER");
        Append(p,"START_BYTE","185");
        Append(p,"BYTES","5");
        Append(p,"DESCRIPTION","\"P1 DENSITY RANGE OR E-FIELD STRATEGY\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","CALIBRATIONA");
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","191");
        Append(p,"BYTES","3");
        Append(p,"DESCRIPTION","\"FLASH CHECKSUM AT START, THEN FREE FOR OTHER USES\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","CALIBRATIONB");
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","195");
        Append(p,"BYTES","3");
        Append(p,"DESCRIPTION","\"FLASH CHECKSUM AT START, THEN FREE FOR OTHER USES\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","TMP12");
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","199");
        Append(p,"BYTES","4");
        Append(p,"DESCRIPTION","\"UNCALIBRATED TEMP, VALID IF TEMP IS ENABLED AND E-FIELD MODE\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"OBJECT","COLUMN");
        Append(p,"NAME","SWVERSION");
        Append(p,"DATA_TYPE","ASCII_INTEGER");
        Append(p,"START_BYTE","205");
        Append(p,"BYTES","2");
        Append(p,"DESCRIPTION","\"SOFTWARE VERSION\"");
        Append(p,"END_OBJECT","COLUMN");
        Append(p,"END_OBJECT","TABLE");
        return 0; // Ok
    } 
    return -1; // Err!
}

// Create property list corresponding to INDEX.LBL file.
// 
// Modified by Erik P G Johansson 2015-05-12 to adjust correctly to changing length of DATA_SET_ID which in turn varies
// one column width in INDEX.TAB/INDEX.LBL.
int SetupIndex(prp_type *p)
{ 
    if(FreePrp(p)>=0) // Free old stuff
    {
        char tempstr[1024];
        
        int DATA_SET_ID_length = strlen(mp.data_set_id);    // Current implementation includes quotes in mp.data_set_id but this might change.
        if (mp.data_set_id[0] == '"') {
            DATA_SET_ID_length = DATA_SET_ID_length - 2;   // Remove quotes.
        }
        
        // NOTE: START_BYTE refers to the "byte number", i.e. (START_BYTE-1) bytes into the line. Thus, the first byte is "1".
        int START_BYTE_col5 = 111 + DATA_SET_ID_length + 3;
        int START_BYTE_col6 = START_BYTE_col5 + 4 + 3;          // +4 (BYTES=column width) +3 (quote+comma+quote)
        int bytes_per_line  = START_BYTE_col6 + 4 + 1 + 2 - 1;  // +4 (BYTES=column width) +1 (quote) +2 (CR+LF) -1 (START_BYTE-1=bytes to the left)
        
        Append(p,"PDS_VERSION_ID",        "PDS3");
        sprintf(tempstr, "%i", bytes_per_line);
        Append(p,"RECORD_BYTES",          tempstr);
        
        Append(p,"RECORD_TYPE",           "FIXED_LENGTH");
        Append(p,"FILE_RECORDS",          "XX");
        Append(p,"RELEASE_ID",            "0001");
        Append(p,"REVISION_ID",           "0000");
        Append(p,"DATA_SET_ID",           "XX");
        Append(p,"PRODUCT_ID",            "INDEX");
        Append(p,"PRODUCT_CREATION_TIME", "YYYY-MM-DDTHH:MM:SS.FFF");
        
        Append(p,"^INDEX_TABLE",          "\"INDEX.TAB\"");   
        
        Append(p,"OBJECT",                "INDEX_TABLE");
        Append(p,"INTERCHANGE_FORMAT",    "ASCII");
        Append(p,"ROWS",                  "XX");      
        Append(p,"ROW_BYTES",             tempstr);
        Append(p,"COLUMNS","6");
        Append(p,"INDEX_TYPE",            "SINGLE");
        Append(p,"INDEXED_FILE_NAME",     "{\"DATA/*.LBL\"}");
        Append(p,"DESCRIPTION",           "\"INDEX.TAB lists all label files in this volume.\"");
        
        Append(p,"OBJECT",        "COLUMN");
        Append(p,"NAME",          "FILE_SPECIFICATION_NAME");
        Append(p,"DATA_TYPE",     "CHARACTER");
        Append(p,"START_BYTE",    "2");
        Append(p,"BYTES",         "58");
        Append(p,"COLUMN_NUMBER", "1");
        Append(p,"DESCRIPTION",   "\"Pathname to the detached label which identifies this data file, or path name to text file.\"");
        Append(p,"END_OBJECT",    "COLUMN");
        
        Append(p,"OBJECT",        "COLUMN");
        Append(p,"NAME",          "PRODUCT_ID"); // This I think is weird..however PVV complains with FILE_NAME
        Append(p,"DATA_TYPE",     "CHARACTER");
        Append(p,"START_BYTE",    "63");
        Append(p,"BYTES",         "25");
        Append(p,"COLUMN_NUMBER", "2");
        Append(p,"DESCRIPTION",   "\"The name of the data file, which is unique within this data set.\"");
        Append(p,"END_OBJECT",    "COLUMN");
        
        Append(p,"OBJECT",        "COLUMN");
        Append(p,"NAME",          "PRODUCT_CREATION_TIME");
        Append(p,"DATA_TYPE",     "TIME");
        Append(p,"START_BYTE",    "90");
        Append(p,"BYTES",         "19");
        Append(p,"COLUMN_NUMBER", "3");
        Append(p,"DESCRIPTION",   "\"Time at which the data file was created.\"");
        Append(p,"END_OBJECT",    "COLUMN");
        
        Append(p,"OBJECT",        "COLUMN");
        Append(p,"NAME",          "DATA_SET_ID");
        Append(p,"DATA_TYPE",     "CHARACTER");
        Append(p,"START_BYTE",    "111");
        Append(p,"COLUMN_NUMBER", "4");
        sprintf(tempstr, "%i", DATA_SET_ID_length);
        Append(p,"BYTES",       tempstr);
        Append(p,"DESCRIPTION",   "\"An identifier unique for this dataset\""); 
        Append(p,"END_OBJECT",    "COLUMN");
        
        Append(p,"OBJECT",        "COLUMN");
        Append(p,"NAME",          "RELEASE_ID");
        Append(p,"DATA_TYPE",     "CHARACTER");
        sprintf(tempstr, "%i", START_BYTE_col5);
        Append(p,"START_BYTE",    tempstr);
        Append(p,"COLUMN_NUMBER", "5");
        Append(p,"BYTES","4");
        Append(p,"DESCRIPTION",   "\"Release ID\"");
        Append(p,"END_OBJECT",    "COLUMN");
        
        Append(p,"OBJECT",        "COLUMN");
        Append(p,"NAME",          "REVISION_ID");
        Append(p,"DATA_TYPE",     "CHARACTER");
        sprintf(tempstr, "%i", START_BYTE_col6);
        Append(p,"START_BYTE",    tempstr);
        Append(p,"COLUMN_NUMBER", "6");
        Append(p,"BYTES","4");
        Append(p,"DESCRIPTION",   "\"Revision ID\"");
        Append(p,"END_OBJECT",    "COLUMN");
        
        Append(p,"END_OBJECT",    "INDEX_TABLE");
        return 0; // Ok!
    }
    return -1; // Err!
} 

void WriteIndexLBL(prp_type *p,mp_type *m)
{
    
    char tstr[32];
    int index_cnt=0;
    char dline[PATH_MAX]; // Dummy line!
    if(p!=NULL)
    {
        // I do not need to make this thread safe since it's called either from ExitPDS 
        // which has already canceled all threads or the main thread.
        fflush(pds.itable_fd); // Flush index table file
        rewind(pds.itable_fd); // Rewind index table file
        
        // Count index table lines! Assume all lines are shorter than dummy line...
        // It is needed in a keyword below
        while(fgets(dline,PATH_MAX,pds.itable_fd) != NULL) {
            index_cnt++;
        }
        
        rewind(pds.ilabel_fd); // Rewind index label to start
        
        sprintf(tstr,"%d",index_cnt);
        SetP(p,"FILE_RECORDS",tstr,1); // Set number of file records in index label file
        SetP(p,"ROWS",tstr,1);         // Set number of rows
        SetP(p,"DATA_SET_ID",m->data_set_id,1); // Set DATA SET ID 
        
        GetUTime(tstr);  // Get current UTC time     
        SetP(p,"PRODUCT_CREATION_TIME",tstr,1); // Set product creation time
        
        FDumpPrp(p,pds.ilabel_fd);     // Dump LAP dictionary to PDS index label file
        fprintf(pds.ilabel_fd,"END\r\n");
        fflush(pds.ilabel_fd);      // Flush label file 
        
        printf( "Index file written.\n");
    }
}



// Created by Erik P G Johansson 2015-05-08.
// 
// Write one line to "INDEX.TAB".
// 
// The implementation (of pds_3.08.c) as of 2015-05-08 makes four almost identical calls to fprintf in
// four different locations. Each calls writes one line to INDEX.TAB. 
// This function is intended to replace those four calls and define the columns in one single place in the code. 
// and would make it easy to change the columns, e.g. omit the columns for RELEASE_ID and REVISION_ID.
// 
// NOTE: If one modifies what is written to INDEX.TAB, then one also has to modify
// the code that generates the INDEX.LBL file.
// NOTE: This function uses some already existing global variables to reduce the number of parameters. Change to parameters anyway?
//
// Original function calls that this function replaces.
// 1198  fprintf(pds.itable_fd,"\"%s\",\"%s\",%s,%s,\"%04d\",\"%04d\"\r\n",tstr3,tstr2,prod_creat_time, mp.data_set_id,(unsigned int)pds.DataSetVersion,0);
// 2892  fprintf(pds.itable_fd,"\"%s\",\"%s\",%s,%s,\"%04d\",\"%04d\"\r\n",tstr4,tstr2,property2->value,mp.data_set_id,(unsigned int)pds.DataSetVersion,0);
// 2946  fprintf(pds.itable_fd,"\"%s\",\"%s\",%s,%s,\"%04d\",\"%04d\"\r\n",tstr4,tstr2,property2->value,mp.data_set_id,(unsigned int)pds.DataSetVersion,0);
// 2920  fprintf(pds.itable_fd,"\"%s\",\"%s\",%s,%s,\"%04d\",\"%04d\"\r\n",tstr4,tstr2,property2->value,mp.data_set_id,(unsigned int)pds.DataSetVersion,0);
//
void WriteToIndexTAB(char* relative_LBL_file_path, char* product_ID, char* prod_creation_time) {
    
    // Old version with REVISION_ID and RELEASE_ID columns.
    fprintf(pds.itable_fd, "\"%s\",\"%s\",%s,%s,\"%04d\",\"%04d\"\r\n", relative_LBL_file_path, product_ID, prod_creation_time, mp.data_set_id, (unsigned int)pds.DataSetVersion, 0);
    
    // New version without REVISION_ID and RELEASE_ID columns.
    //fprintf(pds.itable_fd, "\"%s\",\"%s\",%s,%s\r\n", relative_LBL_file_path, product_ID_QM, prod_creation_time, mp.data_set_id);
}



// Returns the total number of AQPs leading up to 
// the sequence numbered n (in a macro) 
// starting at the beginning of the sequence.
//
// Returns a negative number on error.
//
int TotAQPs(prp_type *p,int n)
{
    property_type *property1;
    property_type *property2;
    
    int tot_aqps=0;
    int haqp;
    int sub=1;
    
    if(p!=NULL && p->properties!=NULL && p->head!=NULL)
    { 
        if(FindP(p,&property1,"ROSETTA:LAP_TRANSFER_DATA_TO_OUT_FROM",n,DNTCARE)>0)
        {
            while(property1!=NULL)
            {
                if(FindB(p,&property1,&property2,"ROSETTA:LAP_HOLD_N_AQPS",DNTCARE)>0)
                {
                    if(!sscanf(property2->value,"\"%x\"",&haqp))
                        return -1; // Err 
                }
                else
                    break;
                
                if(FindB(p,&property1,&property2,"ROSETTA:LAP_HOLD_AQP_TYPE",DNTCARE)>0)
                {
                    if(!strcmp(property2->value,"\"LDL AQPS\""))
                    {
                        haqp*=2; // In Mix LDL double the number of aqps!
                        sub=2;   // In LDL we compensate by two AQP:s at end
                    }
                    else
                        sub=1; // Normal AQP:s we compensate by one AQP at end
                }
                else
                    break;
                
                property1=property2;
                tot_aqps+=haqp;
            }
            // Return total number of aqps leading up to this sequence from start of sequence
            if(tot_aqps>0) {
                return (tot_aqps-sub); //OK! Subtract sub aqps since we start on an aqp pulse
            } else {
                return -3;// Err!
            }
        }
        else
            return -4; //Err not found
    }
    return -5; // Err empty
}



// Find ID code in macro overriding ID code in data if anomaly correction is
// set for the current SCET, or error -1 is returned.
// If not the id_code for the measurement sequence n in the current data set
// is returned.
// 
// Return value:
// -1 : Could not find any value, or it could not be parsed.
// >=0 : Value of the first ROSETTA:LAP_SET_SUBHEADER to be found when searching backwards from the n'th ROSETTA:LAP_TRANSFER_DATA_TO_OUT_FROM.
int FindIDCode(prp_type *p, int n)
{
    property_type *property1;
    property_type *property2;
    
    int subh;
    
    if(p!=NULL && p->properties!=NULL && p->head!=NULL)
    { 
        if(FindP(p,&property1,"ROSETTA:LAP_TRANSFER_DATA_TO_OUT_FROM",n,DNTCARE)>0)
        {
            if(property1!=NULL)
            {
                if(FindB(p,&property1,&property2,"ROSETTA:LAP_SET_SUBHEADER",DNTCARE)>0)
                {
                    if(sscanf(property2->value,"\"%x\"",&subh)) {
                        return subh;
                    }
                }
            }
        }
    }
    return -1; // Err empty
}



// Program state handler functions
//----------------------------------------------------------------------------------------------------------------------------------

// Display current state of state machine
// This is also debugging.
void DispState(int s,char *str)
{
    static int old_s=0;
    
    if(old_s>0)
    {
        if(old_s!=s)
        {
            if(debug>0) {
                printf("%s",str);
            }
            old_s=s;
        }
    }
    else
    {
        if(debug>0) {
            printf("%s",str);
        }
        old_s=s;
    }
}



// String and alpha numeric handling functions
//----------------------------------------------------------------------------------------------------------------------------------

/*
 * Routine to separate a string into left and right substrings separated
 * by the k_sep'th occurrence of "separator".
 * The right substring is bounded by both occurrences of "separator" and of end-of-string.
 * 
 * Return value = min(occurs, <nbr of separators in str>)
 */
int Separate(char *str, char *left, char *right, char separator, int k_sep)
{
    int len;
    int i;
    int i_sep = 0;
    char *lpos;
    char *rpos;
    
    lpos=str; 
    rpos=str;
    
    len=strlen(str);
    if(str!=NULL)
    {
        for(i=0;i<(len-1);i++)
        {
            if(str[i]==separator)
            {
                lpos=rpos;
                rpos=&str[i+1];
                i_sep++;         // Increment occurrence of separator.
            }
            
            if(i_sep==k_sep)
            {
                for(;lpos<(rpos-1);) {
                    *(left++)=*(lpos++);
                }
                *left='\0';
                
                for(;(*rpos!=separator && *rpos!='\0');) {
                    *(right++)=*(rpos++);
                }
                *right='\0';
                
                break;
            }
        }
        return i_sep;
    }
    else
        return -1;
}



/*
 * Split up string by first occurrence of character.
 * NOTE: Function "Separate" does not ignore righthand instances of the separator.
 * 
 * str = String to split up.
 * separator = Character that is not \0.
 * 
 * If "separator" is found in "str" (at least once):
 *    strLeft  = Will be set to the part of "str" to the left  of the first occurrence of "separator".
 *    strRight = Will be set to the part of "str" to the right of the first occurrence of "separator".
 *    Return value = The position of "separator" in "str" (non-negative value).
 * 
 * If "separator" is NOT found in "str":
 *    strLeft  : String not altered.
 *    strRight : String not altered.
 *    Return value = -1
 * 
 * Created by Erik P G Johansson 2015-12-17
 */
int SeparateOnce(char* str, char* strLeft, char* strRight, char separator)
{
    char* sepPos = strchr(str, separator);
    
    if (sepPos == NULL) {
        return -1;
    }
    
    const int strLeftLen = &sepPos[0] - &str[0];
    
    strcpy(strRight, &sepPos[1]);
    strncpy(strLeft, str, strLeftLen);
    strLeft[strLeftLen] = '\0';
    
    return strLeftLen;
}



// Convert CR (carriage return) and LF (line feed) to whitespace.
// Then trim initial and trailing whitespace.
// ==> Any sequence of CR & LF at the END OF THE STRING are removed too.
int TrimWN(char *str)
{
    int len,nlen,i;
    char *pos;
    int state=0;
    if(str==NULL) return -1;
    if((len=strlen(str))<=0) return -1;
    
    
    for(i=0;i<len;i++) {    // First make all newlines or carriage returns to whitespace.
        if(str[i]=='\n' || str[i]=='\r') {
            str[i]=' ';
        }
    }
    
    pos=str;
    
    nlen=len;
    for(i=0;i<len;i++)    // Remove all initial whitespace.
    {
        if(state==0)
        {
            if(str[i]==' ') 
            {
                nlen--;
                continue;
            }
            else
                state=1;
        }
        *(pos++)=str[i];
    }
    
    for(i=nlen-1;i>=0;i--)    // Remove all trailing whitespace.
    {
        if(str[i]==' ') 
            continue;
        else
        {
            str[i+1]='\0'; //Set new end of string
            break;
        }
    }
    
    return 0;
}



// Trim initial and trailing quotes and all newlines away.
int TrimQN(char *str)
{
    int len,nlen,i;
    char *pos;
    int state=0;
    if(str==NULL) return -1;
    if((len=strlen(str))<=0) return -1;
    
    for(i=0;i<len;i++) {   // First replace all newlines and carriage returns with whitespace.
        if(str[i]=='\n' || str[i]=='\r') {
            str[i]=' ';
        }
    }
    
    pos=str;
    
    nlen=len;
    for(i=0;i<len;i++) // Remove all initial quotes
    {
        if(state==0)
        {
            if(str[i]=='\"') 
            {
                nlen--;
                continue;
            }
            else
                state=1;
        }
        *(pos++)=str[i];
    }
    
    for(i=nlen-1;i>=0;i--) // Remove all trailing quotes
    {
        if(str[i]=='\"') 
        {
            str[i]='\0';
            continue;
        }
        else
        {
            if(str[i]==' ') continue;
            str[i+1]='\0'; //Set new end of string
            break;
        }
    }
    
    return 0;
}


// Make a new string "dest" using "src" and padded with character "ch" to length "elen". 
// We assume that "dest" has enough space.
int ExtendStr(char *dest,char *src,int elen,char ch)
{
    int i;
    int len;
    if(dest==NULL || src==NULL) return -1;
    
    len=strlen(src);        // Get length of src string
    
    strncpy(dest,src,elen); // Copy Data Set ID to temporary string
    
    for(i=len;i<elen;i++)  // Fill end of destination string with character ch
        dest[i]=ch; 
    
    dest[elen]='\0'; // Insert new null character at end of extended string
    return 0;
}


//Replace all ch1 in str with character ch2
void ReplCh(char *str,char ch1,char ch2)
{
    int i;
    if(str!=NULL) {
        for(i=0;i<strlen(str);i++) {
            if(str[i]==ch1) {
                str[i]=ch2;
            }
        }
    }
}


// Increments the 3 digit alphanumeric number stored in string n returns <0 on error
int IncAlphaNum(char *n) 
{
    if(n==NULL || strlen(n)!=3) return -1; // No good string
    
    if(!isalnum(n[0])) return -2; // Third  digit not alpha numeric!
    if(!isalnum(n[1])) return -3; // Second digit not alpha numeric!
    if(!isalnum(n[2])) return -4; // First  digit not alpha numeric!
    
    n[2]++;
    if(n[2]==':') n[2]='A';
    if(n[2]=='[') 
    {
        n[2]='0';
        n[1]++;
        if(n[1]==':') n[1]='A';
        if(n[1]=='[') 
        {
            n[1]='0';  
            n[0]++;
            if(n[0]==':') n[0]='A';
            if(n[0]=='[') n[0]='0';
        }
    }
    return 0; // OK!
}

// Get alphanumeric value from last filename in a directory
int GetAlphaNum(char *n,char *path,char *pattern)
{
    DIR           *de;              // Directory
    struct dirent *dentry;          // Directory entry
    
    char oldn[4]="000";
    char newn[4];
    
    int oval=0;
    int nval;
    
    if((de = opendir(path))==NULL) // Open directory
        return -1;
    
    rewinddir(de);       // Rewind directory
    dentry=readdir(de);  // Get first entry
    
    // Do a linear search through all filnames....
    // To find the last one with highest alphanumeric value
    
    while(dentry!=NULL && pattern!=NULL) 
    {              
        if(!fnmatch(pattern,dentry->d_name,0))  // Match filename to pattern
        {
            strncpy(newn,&dentry->d_name[13],3);        // Remember matching file name
            newn[3]='\0';
            // alphanumeric value in filename larger than previously largest ?
            nval=Alpha2Num(newn);
            if(nval>oval)
            {
                strcpy(oldn,newn); // Remember new number
                oval=nval;
            }
        }
        dentry=readdir(de); // Get next filename 
    }
    strcpy(n,oldn); // Return largest alphanumeric value
    n[3]='\0'; // Just to be safe
    closedir(de);
    return 0;
}

// Converts a positive alpha numeric value to a number
// returns negative value on error
int Alpha2Num(char *n)
{
    int val=0;
    
    if(n==NULL || strlen(n)!=3) return -1; // No good string
    if(!isalnum(n[0])) return -2; // Third  digit not alpha numeric!
    if(!isalnum(n[1])) return -2; // Second digit not alpha numeric!
    if(!isalnum(n[2])) return -2; // First  digit not alpha numeric!
    
    if(isdigit(n[0])) 
        val+=((n[0]-'0')*1296);
    else
        val+=((n[0]-'A'+10)*1296);
    
    if(isdigit(n[1])) 
        val+=((n[1]-'0')*36);
    else
        val+=((n[1]-'A'+10)*36);
    
    if(isdigit(n[2])) 
        val+=((n[2]-'0'));
    else
        val+=((n[2]-'A'+10));
    
    return val;
}

// Returns true if string only contain numbers
int IsNumber(char *str)
{
    int len;
    int i;
    len=strlen(str);
    
    for(i=0;i<len;i++) {
        if(!isdigit(str[i])) {
            return 0;
        }
    }
        
        return 1; // String only contain numbers return true
}


// File handling functions
//----------------------------------------------------------------------------------------------------------------------------------


// Returns length of file for filedescriptor fd
//
int  FileLen(FILE *fd)
{
    struct stat sp; // File status pointer
    int len;
    if(FileStatus(fd,&sp)<0) // Get file status
        return -1;
    else
    {
        len=(unsigned int)sp.st_size; 
        return len;
    }
}

// Returns status struct of file for file stream fs
// if sp points to NULL, memory will be
// declared dynamically and must be freed.
int  FileStatus(FILE *fs,struct stat *sp)
{
    int fd;
    if(sp==NULL) {
        if((sp=malloc(sizeof(struct stat)))==NULL) {
            return -1;
        }
    }
        if(fs==NULL) return -2;
        if((fd=fileno(fs))<0) return -3;
        if(fstat(fd,sp)<0) return -4;
        return 0;
}

// Accepts a string and checks if it is a real path, and returns a "resolved" path (no symbolic links, no /../ etc).
// Makes sure there is "/" at the end.
// NOTE: Should only be used for DIRECTORY PATHS.
// NOTE: "info_txt" is only used for log messages.
int  SetupPath(char *info_txt,char *path)
{
    char tp[PATH_MAX];
    int l;
    
    // Resolves full path and returns a pointer to it in tp
    // returning tp is a static array of PATH_MAX chars.
    // This means we have to copy it to another array before
    // calling realpath again!!
    
    if(realpath(path,tp)==NULL) 
    {
        fprintf(stderr,"%s\n",info_txt);
        perror(path);
        return -1;
    }
    strcpy(path,tp);
    
    // Ensure that the last character of the path string is "/".
    if((l=strlen(path))>0)
    {
        if(path[l-1]!='/') strcat(path,"/");
    }
    printf("%s: %s\n",info_txt,path);  // Dump to console
    return 0;
}

// Tests if directory exists
int TestDir(char *name)
{
    
    DIR *directory;
    if((directory=opendir(name))==NULL)
        return -1;
    
    closedir(directory);
    return 0;
}
// Test if file exists
int TestFile(char *name)
{
    FILE *fd;
    if((fd=fopen(name,"r"))==NULL)
        return -1;
    fclose(fd);
    return 0;
}


// Make a directory with name "name" in "path".
// 
// Return path to new directory 
// in "npath" assuming it has enough space.
int MakeDir(char *name,char *path,char *npath)
{
    int err;
    strcpy(npath,path);
    strcat(npath,name);
    strcat(npath,"/");
    
    // If mkdir is not thread safe this might cause problems..if so add mutex lock.
    err=mkdir(npath,0775); 
    
    if(!err || err==EEXIST) return 0; // If no error or it already exist, return ok!
    
    return err; // Else we return the error code..
}

// Make DATA directory structure
// 
// date : String beginning with "XXYY-MM-DD".
// 
// BUG FIX: No longer alters "date". /Erik P G Johansson 2016-03-21.
int StrucDir(char *date,char *ipath,char *opath)
{
    char months[12][4] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
    char yeardir[PATH_MAX];
    char monthdir[PATH_MAX];
    char daydir[PATH_MAX];
    char tmp[PATH_MAX];
    char split_date[PATH_MAX];
    int month;
    
    strncpy(split_date, date, PATH_MAX);
    
    // Date is CCYY-MM-DD.
    // NOTE: Alters the ARGUMENT "date".
    split_date[4]='\0';
    split_date[7]='\0';
    split_date[10]='\0';

    // Make sure there is a YEAR directory.
    sprintf(tmp,"%s",split_date);
    MakeDir(tmp,ipath,yeardir);                     // Make a directory for current Year, if it's not already there!
    
    // Make sure there is a MONTH directory.
    if(!sscanf(&split_date[5],"%d",&month)) {       // Month number
        return -1;                                  // Could not convert to month invalid date!
    }    
    month--;                                        // Start at 0
    if(month>=0 && month<=11)                       // In range
    {
        MakeDir(months[month],yeardir,monthdir);    // Make a month directory for current month, if it's not already there!
    }
    else {
        return -2;                                  // Month out of range!
    }
    
    // Make sure there is a DAY-OF-MONTH directory.
    sprintf(tmp,"D%s",&split_date[8]);              // Day of month
    MakeDir(tmp,monthdir,daydir);                   // Make day dir!
    
    strcpy(opath,daydir); // Return data directory!
    return 0;
}

// Dump archive directory, for debugging purposes! 
// compare() assumes archive hierarchy
//
void DumpDir(char *path)
{
    FTS *af;    // Archive file structure
    FTSENT *fe; // Entry
    
    char *path_arrays[2];
    
    path_arrays[0]=path;
    path_arrays[1]=NULL;
    
    // Traverse logical file hierarchy
    af=fts_open(path_arrays,FTS_LOGICAL,Compare);
    do    
    {
        fe=fts_read(af); // Get next file
        if(fe!=NULL)
            printf("%s\n",fe->fts_path);
    }
    while(fe!=NULL);
}

// Returns a unique file name similar to
// UNA_00000_2003-01-03T03:13:01.lap
//
// In this file we store data that is not understod/accepted by this PDS program
// it needs manual attention to be converted into PDS. It can be streamed
// directly into a LAP GSE for analysis.
//
int GetUnacceptedFName(char *name)
{ 
    char lastm[256];                // Last matching file
    DIR           *una_dir;         // UnAccepted directory
    struct dirent *dentry;          // Directory entry
    char strp[32];
    int lastnum=0;
    int tmp;
    
    GetUTime(strp); // Get universal time
    sprintf(name,"Default_%s.lap",strp);
    
    if((una_dir = opendir(pds.uapath))==NULL) // Open UnAccepted_Data directory
        return -1;
    
    rewinddir(una_dir);       // Rewind
    dentry=readdir(una_dir);  // Get first entry
    
    while(dentry!=NULL) // Do a linear search through all filnames....to find the last one
    {  
        if(!fnmatch("UNA_*_*.lap",dentry->d_name,0)) // Match filename to pattern
        {
            strcpy(lastm,dentry->d_name);            // Get matching file name
            lastm[9]='\0';                           // We only care about number part
            if(sscanf(&lastm[4],"%d",&tmp))           // Get number of file
            {
                if(tmp>lastnum)   // Number of file larger than previously largest
                    lastnum=tmp;    // Remember new number
            }
        }
        dentry=readdir(una_dir); // Get next filename 
    }
    sprintf(name,"UNA_%05d_%s.lap",lastnum+1,strp); // Return new name, Containing largest num + 1
    closedir(una_dir);
    return 0;
}

// Debugging
void FTSDump(FTSENT *fe)
{
    printf("Access path %s\n",fe->fts_accpath);
    printf("Root path   %s\n",fe->fts_path);
    printf("Path len    %d\n",fe->fts_pathlen);
    printf("Name        %s\n",fe->fts_name);
    printf("Name len    %d\n",fe->fts_namelen);
    printf("Depth       %d\n",fe->fts_level);
    printf("File errno  %d\n",fe->fts_errno);
    printf("local numeric val %ld\n",fe->fts_number);
    printf("local address    %p\n",fe->fts_pointer);
    printf("parent dir       %p\n",fe->fts_parent);
    printf("next file struc  %p\n",fe->fts_link);
    printf("cycle struc      %p\n",fe->fts_cycle);
    printf("stat(2) info     %p\n",fe->fts_statp);
}

// Match filename to pattern
// If strb match stra zero is returned.s
// stra may contain #, representing a digit 0 to 9. 
int Match(char *stra,char *strb)
{
    int len;
    int i;
    
    if((len=strlen(stra))!=strlen(strb)) return -1; // No match lengths not equal
    
    for(i=0;i<len;i++) {
        if(stra[i]!=strb[i])
        {
            if(stra[i]=='#' && strb[i]>='0' && strb[i]<='9')
                continue;
            else
                return -2; // Chars don't match
        }
    }
        return 0; // All matched
}



// HK Functions
//----------------------------------------------------------------------------------------------------------------------------------

// ASSEMBLE A HK LINE
//
// NOTE: line must be a pointer to a buffer of at least 165 characters
//
// HOUSE KEEPING EXAMPLE ROW
// -------------------------
//
// NOTE1:  Longest strings used, thus DISABLED is 8 characters
//        and ENABLED is 7 so if enabled we put in " ENABLED"
//        with an initial white space.
//
// NOTE2:  We use delimiters to make it easier for other software to
//        quickly read the file for testing purposes.
//
//  
// NOTE3:  Line is terminated with both carriage return and line feed
//        as in a DOS system (This is not strictly needed anymore).
//
// Examle of a HK line with positions displayed vertically, time is in format description.
//
// Beginning of line:
// ---------------------------------------------------------------------------------------------------
// 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
// 000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999
// 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// YYYY-MM-DDThh:mm:ss.ffffff,FFFFFFFFFFFFFFFF,P,E,DISABLED,DISABLED,1,MIXED 0,DISABLED,+-05,+-32,DENS
//
// Continuation of line:
// -------------------------------------------------------------------------------------------------------------------
// 1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111222222222222222
// 0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000000000
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345677778888
// ITY,DENSITY,DENSITY,DENSITY,DENSITY,DENSITY,LAP,DISABLED,RX,DENSITY,FLOAT,RX,DENSITY,FLOAT,255,255,00000,00<CR><LF>
//
void AssembleHKLine(unsigned char *b,char *line,double time,char *macro_id_str)
{
    char ldlmode_str[4][8]={"NONE   ","MIXED 0"," NORMAL","MIXED 1"};
    char tstr[80]; // Temporary string
    
    int temp;
    int f22dedc;
    int f11dedc;
    double t;
    
    DecodeRawTime(time,tstr,1); // Decode raw time to UTC
    
    // Start assembly of table line described above
    sprintf(line,"%s,%016.6f,%1d,%1d,",tstr,time,GetBitF(b[0],3,5),GetBitF(b[0],3,2));   // Set time and pmac and emac   
    
    if(GetBitF(b[0],1,1)) // Make watchd string
        strcpy(tstr,"ENABLED ,");
    else
        strcpy(tstr,"DISABLED,");
    
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[0],1,0)) // Make promen string
        strcpy(tstr,"DISABLED,");
    else
        strcpy(tstr,"ENABLED ,");
    strcat(line,tstr); // Add string to table line
    
    sprintf(tstr,"%1d,",GetBitF(b[1],1,0)); // Make oscillator string
    strcat(line,tstr); // Add string to table line
    
    strcat(line,ldlmode_str[GetBitF(b[2],2,6)]); // Add ldl mode, ldlmde and ldlsnc bits are put together
    
    temp=GetBitF(b[2],1,0);
    if(temp)  // Make temp disabled enabled string
        strcpy(tstr,",DISABLED,");
    else 
        strcpy(tstr,",ENABLED ,");  
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,7))  // Make string
        strcpy(tstr,"+-32,");
    else
        strcpy(tstr,"+-05,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,6))  // Make string
        strcpy(tstr,"+-32,");
    else
        strcpy(tstr,"+-05,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,5))  // Make string
        strcpy(tstr,"DENSITY,");
    else
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,4))  // Make string
        strcpy(tstr,"DENSITY,");
    else
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,3))  // Make string
        strcpy(tstr,"DENSITY,");
    else
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,2))  // Make string
        strcpy(tstr,"DENSITY,");
    else
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,1))  // Make string
        strcpy(tstr,"DENSITY,");
    else
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[3],1,0))  // Make string
        strcpy(tstr,"DENSITY,");
    else
        strcpy(tstr,"E-FIELD,");    
    strcat(line,tstr); // Add string to table line
    
    if (GetBitF(b[4],1,7)) 
        strcpy(tstr,"LAP,");
    else
        strcpy(tstr,"MIP,");      
    strcat(line,tstr); // Add string to table line
    
    if (GetBitF(b[4],1,6)) 
        strcpy(tstr,"DISABLED,");
    else 
        strcpy(tstr,"ENABLED ,");
    strcat(line,tstr); // Add string to table line
    
    
    if (GetBitF(b[4],1,5))  
        strcpy(tstr,"TX,");
    else 
        strcpy(tstr,"RX,");
    strcat(line,tstr); // Add string to table line
    
    
    if (GetBitF(b[4],1,4))
        strcpy(tstr,"DENSITY,");
    else 
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    f22dedc  = GetBitF(b[4],1,3);
    if(GetBitF(b[4],1,4))
    {
        if(f22dedc)
            strcpy(tstr,"G1.0 ,");   
        else
            strcpy(tstr,"G0.05,");   
    }
    else
    {
        if(f22dedc)
            strcpy(tstr,"FLOAT,");   
        else
            strcpy(tstr,"BIAS ,");   
    }
    strcat(line,tstr); // Add string to table line
    
    
    if(GetBitF(b[4],1,2))  
        strcpy(tstr,"TX,");
    else 
        strcpy(tstr,"RX,");
    strcat(line,tstr); // Add string to table line
    
    if(GetBitF(b[4],1,1))  
        strcpy(tstr,"DENSITY,");
    else 
        strcpy(tstr,"E-FIELD,");
    strcat(line,tstr); // Add string to table line
    
    f11dedc  = GetBitF(b[4],1,0);
    if(GetBitF(b[4],1,1))
    {
        if(f11dedc)
            strcpy(tstr,"G1.0 ,");   
        else
            strcpy(tstr,"G0.05,");   
    }
    else
    {
        if(f11dedc)
            strcpy(tstr,"FLOAT,");   
        else
            strcpy(tstr,"BIAS ,");   
    }
    strcat(line,tstr); // Add string to table line
    
    sprintf(tstr,"%03d,",b[6]); // Calibration A
    strcat(line,tstr); // Add string to table line
    
    sprintf(tstr,"%03d,",b[7]); // Calibration B
    strcat(line,tstr); // Add string to table line
    
    sprintf(macro_id_str,"MCID0X%02x%02x",b[6],b[7]);
    
    if(temp) {
        t  = ((b[8]<<8 | b[9]) ^ 0x8000); // If temp is off this is just a sample from ADC20 probe 2.
    } else {
            t  = ((b[8]<<8 | b[9]) ^ 0x8000)*T_SCALE+T_OFFSET; //CALIB values educated guesses!!
    }
    
    sprintf(tstr,"%05.f,",t);      
    strcat(line,tstr); // Add string to table line
    
    sprintf(tstr,"%02d\r\n",b[11]); // Make software version string
    strcat(line,tstr); // Add string to table line
    
}



// Low level data functions
//----------------------------------------------------------------------------------------------------------------------------------

// Sign extend 20 bit value to native int form.
void SignExt20(int *val)
{
    // Sign extend 20 bit signed value to signed integer
    if(*val & 0x080000) // Is it negative
    {
        *val=~*val;      // Invert it
        *val&=0x07ffff;  // Cut off anything extra
        *val+=1;         // Add 1 
        *val=-*val;      // Make it negative X number of bits...
    }
    // If not negative do nothing
}

// Returns double..from 64 bit big endian data
// Assumes this is a little endian machine!!
double GetDBigE(unsigned char *buff)
{
    double ld;
    unsigned char *tmp;
    
    tmp=(unsigned char *)&ld;
    
    tmp[0]=buff[7];
    tmp[1]=buff[6];
    tmp[2]=buff[5];
    tmp[3]=buff[4];
    tmp[4]=buff[3];
    tmp[5]=buff[2];
    tmp[6]=buff[1];
    tmp[7]=buff[0];
    
    return ld;
}

// Returns a bit field.
// nb number of bits starting at bit sb
unsigned int GetBitF(unsigned int word,int nb,int sb)
{
    unsigned int rval;
    
    rval=word>>sb;
    rval&=((1<<nb)-1);
    return rval;
}

// Time related functions
//----------------------------------------------------------------------------------------------------------------------------------

// Compute number of seconds since
// Epoch 1970 to time in PDS time string
// without fractional seconds

unsigned int E2Epoch(char *rtime)
{ 
    struct tm at; // Broken down time structure
    time_t t;
    
    // Put year,month,day,hour,minutes and seconds in structure
    sscanf(rtime,"%4d-%2d-%2dT%2d:%2d:%2d",&at.tm_year,&at.tm_mon,&at.tm_mday,&at.tm_hour,&at.tm_min,&at.tm_sec); 
    
    at.tm_mon--;      // Month ranges from 0 to 11 and not as usual 1 to 12
    at.tm_year-=1900; // Get number of years since 1900, that's what mktime wants 
    
    at.tm_wday=0;     // Day of week doesn't matter here
    at.tm_yday=0;     // Day in year doesn't matter here
    at.tm_isdst=0;    // Daylight saving unknown
    
    t=mktime(&at);   // Calculates UTC time in seconds since 1970 1 Jan 00:00:00
    
    t+=at.tm_gmtoff; // Add number of second east of UTC..to get UTC
    return t;
}


// Decodes S/C time 
//
// This functions returns the raw S/C time as a double
//	      
double DecodeSCTime(unsigned char *buff)
{
    unsigned int full_s; // Full seconds
    double frac_s;       // Fractional seconds
    
    //Full seconds since 2000 jan 1 00:00:00
    full_s=((buff[0]<<24) | buff[1]<<16 | buff[2]<<8 | buff[3]); // Full seconds
    frac_s=((double)(buff[4]<<2 | buff[5]>>6))/1024.0; // Fractional seconds
    
    return(full_s+frac_s);
}



// Decodes lap S/C time 

// two highest bits are truncated away fitting the time
// into five bytes
//
// This functions returns the raw S/C time as a double
//
double DecodeLAPTime(unsigned char *buff)
{
    unsigned int full_s; // Full seconds
    double frac_s;       // Fractional seconds
    
    //Seconds since spacecraft clock reset
    full_s=((buff[0]<<22) | buff[1]<<14 | buff[2]<<6 | buff[3]>>2); // Full seconds
    frac_s=((double)((buff[3] & 0x3)<<8 | buff[4]))/1024.0;         // Fractional seconds
    
    return(full_s+frac_s); // Return raw time in seconds
}



// Given raw S/C time this function returns 
// a PDS compliant string for this time.
// Time is estimated UTC thus no calibration
// is done.
// A negative return code means an error occured
// 
// NOTE: It appears that this function is not used as of 2015-06-22. /Erik P G Johansson 2015-06-22
// 
///*
int DecodeRawTimeEst(double raw,char *stime)
{
    double s;            // Seconds and fractional seconds
    struct tm bt;        // Broken down time
    time_t t;            // Time since 1970 in secs to current S/C time
    unsigned int full_s; // Full seconds
    double frac_s;       // Fractional seconds
    
    unsigned int d,h,m;  // Days, hours and minutes
    
    //line edited 10/7 2014. FJ. We need to make sure seconds are rounded properly.
    raw= floor(1000*raw+0.5)/1000;//need to round value properly!
    
    // Compile time since 1970 jan 1 00:00:00 to current S/C time 
    t = raw+sec_epoch; // Full secs + seconds from 1970 to reset of S/C clock. 
    
    // Use gmtime to stay POSIX compliant and to be consistent.
    full_s=raw; // Truncate whole number of seconds and put them in a unsigned integer
    frac_s=raw-full_s; // Get remaining fractional seconds
    
    // Compile plausible date, time and year, use thread-safe version of gmtime
    if((gmtime_r(&t,&bt))==NULL)  
    {
        d=full_s/86400;            // Full number of days.
        h=(full_s-=d*86400)/3600;  // Remaining hours.
        m=(full_s-=h*3600)/60;     // Remaining minutes. 
        s=(full_s- m*60)+frac_s;   // Remaining seconds.
        // Couldn't resolve corresponding UTC date of S/C time
        sprintf(stime,"CCYY-MM-DDT%02d:%02d:%06.3f",h,m,s);
        return -1; 
    }
    
    // Compile PDS compliant time string
    // Note: if gmtime does anything funny(It doesn't look like it) relative time should be okay.
    sprintf(stime,"%4d-%02d-%02dT%02d:%02d:%06.3f",bt.tm_year+1900,bt.tm_mon+1,bt.tm_mday,bt.tm_hour,bt.tm_min,((double)bt.tm_sec)+frac_s);
    return 0; // Ok!
}
//*/



// Decodes and correlates raw time into UTC time
//
// Input: raw, 
// Input: lfrac - choose 3 (false) or 6 (true) digits of fractional seconds.
// Output: stime
//
// NOTE: The function uses the global tcp structure! 
// 1) Can do this since it's the only user
// 2) It only reads! No thread conflicts..
//
// NOTE: The function and its input value is used by WritePTAB_File to produce the first two columns in (science data) TAB files: 
//    stime: UTC_TIME (DESCRIPTION = "UTC TIME")
//    raw:   OBT_TIME (DESCRIPTION = "SPACECRAFT ONBOARD TIME SSSSSSSSS.FFFFFF (TRUE DECIMALPOINT)")

int DecodeRawTime(double raw, char *stime, int lfrac)
{
    double craw;
    double gradient;
    double offset;
    int i;
    double correlated;
    double tmp=0.0;
    
    //CALIBRATE RAW
    
    // Default coefficients
    gradient=1.0; 
    offset=sec_epoch;
    
    for(i=0;i<tcp.netries;i++) // Go through all entries
    {
        correlated=raw*tcp.gradient[i]+tcp.offset[i]; // Compute UTC time using all gradient offset candidates
        
        if(i<(tcp.netries-1)) {   // If we are not at the end
            tmp=tcp.SCET[i+1];   // Use next SCET to test the upper validity limit
        } else {
                tmp=1e100; // No next SCET ..last correlation packet valid until ground station produces new ones..
        }
                
                if(correlated>=tcp.SCET[i] && correlated<tmp) // Test if in valid range
                {
                    gradient=tcp.gradient[i]; 
                    offset=tcp.offset[i];
                    break;
                }
    }
    
    craw=raw*gradient+offset; // Compute correlation...if no time calibration data found default coefficients are used.

    Scet2Date_2(craw,stime,lfrac); // Compute a PDS date string
    return 0;
}



// Convert RAW time (decimal number of seconds) to a corresponding OBT string.
// Due to the fact that the point . in:
//
// SPACECRAFT_CLOCK_START/STOP_COUNT="1/21339876.237"
//
// is not a decimal point.. (NOT specified in PDS) but now specified
// in PSA to be a fraction of 2^16 thus decimal .00123 seconds is stored as
// 0.123*2^16 ~ .81
//
// We run the raw time into this function to convert back to fractions
// and create an apropriate string. Reason for not converting
// the DecodeSCTime is that other parts of the code rely on this function.
// In our .TAB files we have a column with UTC and a column with OBT
// but with higher precision we can not use the 2^16 fractions there
// it need to be a real_ascii with a true decimal point.
// 
// raw : Decimal number of seconds (true decimals)
// rcount : Reset counter
// stime : "OBT string", with digits after"decimals" representing the number of fractions 2^-16 [s].
//
int Raw2OBT_Str(double raw, int rcount, char *stime)
{
    unsigned int sec;
    unsigned int frac;
    
    sec=(unsigned int)(raw); // Truncate seconds
    frac=(unsigned int)(((raw-sec)*65536.0)+0.5); // Get fractions back in terms of 2^16
    
    sprintf(stime,"\"%d/%010d.%d\"",rcount,sec,frac); // Compile raw S/C time string
    
    return sec;
}



//--------------------------------------------------------------------------------------------------
// Erik P G Johansson 2015-03-25: Created function.
//
// Converts
// from string representation of spacecraft clock counter ("OBT_Str", "stime"; uses false decimals)
// to   double representation of spacecraft clock counter ("raw"; true decimals).
// 
// NOTE: Opposite conversion of Raw2OBT_Str (hence the name) with the difference that this function
// does accepts strings both with and without surrounding quotes.
// The former is used when reading the data exclusion file, the latter is used for reading
// SPACECRAFT_CLOCK_START/STOP_COUNT values in property lists (property_type).
//--------------------------------------------------------------------------------------------------
int OBT_Str2Raw(char *stime, int *resetCounter, double *rawTime)
{
    int resetCounter_temp;
    double sec, false_frac;
    char   s[MAX_STR], s_temp[MAX_STR];
    
    // Trick to remove surrounding quotes from string, but only if there are any.
    if (stime[0] == '"') {
        Separate(stime, s_temp, s, '"', 1);   // s_temp value is never used.
    } else {
        strncpy(s, stime, MAX_STR);
    }
    
    // Parse numbers in string.
    // 1) Use doubles instead of integers (except for reset counter) to ensure that
    // the variables can represent large enough integers. (C's "int" is not guaranteed to have many bits.)
    // 2) Use two separate doubles for "seconds" and "fractions" since double can represent all (not too
    //    large) integers exactly, but not all decimal numbers.
    if (   (3 != sscanf(s,      "%i/%[^.].%lf", &resetCounter_temp, s_temp, &false_frac))
        || (1 != sscanf(s_temp, "%lf",          &sec)))
    {
        YPrintf("ERROR: OBT_Str2Raw: Can not interpret spacecraft clock counter string: \"%s\"\n", stime);
        printf( "ERROR: OBT_Str2Raw: Can not interpret spacecraft clock counter string: \"%s\"\n", stime);
        return -1;
    }
    
    // Assign values to "parameters".
    *rawTime      = sec + false_frac/65536.0;
    *resetCounter = resetCounter_temp;
    
    return 0;
}//*/




// Compute a PDS date from raw time
// lfrac is used to choose 3 or 6 digits 
// fractional seconds
// ESA recommends the following approach with OASWlib
// and dj2000 however it gives dubious results!
/*
 * int Scet2Date(double raw,char *stime,int lfrac)
 * {
 * 
 * double day,s;
 * int yy,mm,dd,h,m;
 * 
 * // The strategy in the following two lines is the ESA recomendation to compute the date
 * 
 * day=raw/(60.0*60.0*24.0)-(23.0*365.0+7.0*366.0); // Compute number of days a double
 * dj2000_(&day,&yy,&mm,&dd,&h,&m,&s); // Call FORTRAN Routine from OASW package to compute the date.
 * 
 * if(lfrac)
 *    sprintf(stime,"%4d-%02d-%02dT%02d:%02d:%09.6f",yy,mm,dd,h,m,s); // Use 6 digit fractional seconds
 * else
 *    sprintf(stime,"%4d-%02d-%02dT%02d:%02d:%06.3f",yy,mm,dd,h,m,s); // Use 3 digit fractional seconds PDS standard
 * return 0;
 * }
 */


// Compute a PDS date from raw time
//
// lfrac is used to choose 3 (false) or 6 (true) digits of fractional seconds.
int Scet2Date_2(double raw,char *stime,int lfrac)
{ 
    double s;            // Seconds and fractional seconds
    struct tm bt;        // Broken down time
    time_t t;            // Time since 1970 in secs to current S/C time
    
    unsigned int full_s; // Full seconds
    double frac_s;       // Fractional seconds
    unsigned int d,h,m;  // Days, hours and minutes
    
    
    // Compile time since 1970 jan 1 00:00:00 to current S/C time
    //t = raw+sec_epoch; // Full secs + seconds from 1970 to reset of S/C clock. 
    //line edited 10/7 2014. FJ. We need to make sure seconds are rounded properly.
    if(lfrac)
    {
        raw= floor(1000*1000*raw+0.5)/(1000*1000);//need to round value properly, otherwise sprintf will do it for us ( randomly)  
    }
    else
    {
        raw= floor(1000*raw+0.5)/1000;//need to round value properly!  
    }
    
    
    t=raw;  
    
    // Use gmtime to stay POSIX compliant and to be consistent.
    full_s=raw; // Truncate whole number of seconds and put them in a unsigned integer
    frac_s=raw-full_s; // Get remaining fractional seconds
    
    // Compile plausible date, time and year, use thread-safe version of gmtime
    if((gmtime_r(&t,&bt))==NULL)  
    {
        d=full_s/86400;            // Full number of days.
        h=(full_s-=d*86400)/3600;  // Remaining hours.
        m=(full_s-=h*3600)/60;     // Remaining minutes. 
        s=(full_s- m*60)+frac_s;   // Remaining seconds.
        // Couldn't resolve corresponding UTC date of S/C time
        sprintf(stime,"CCYY-MM-DDT%02d:%02d:%06.3f",h,m,s);
        return -1; 
    }
    
    // Compile PDS compliant time string
    if(lfrac)
    {
        // Use 6 digit fractional seconds
        sprintf(stime,"%4d-%02d-%02dT%02d:%02d:%09.6f",bt.tm_year+1900,bt.tm_mon+1,bt.tm_mday,bt.tm_hour,bt.tm_min,((double)bt.tm_sec)+frac_s);
    }
    else
    {
        // Use 3 digit fractional seconds PDS standard
        sprintf(stime,"%4d-%02d-%02dT%02d:%02d:%06.3f",bt.tm_year+1900,bt.tm_mon+1,bt.tm_mday,bt.tm_hour,bt.tm_min,((double)bt.tm_sec)+frac_s);
    }
    
    return 0;
}

/*
 * Interprets a string, of format "YYYY-MM-DD" or "YYYY MM DD hh mm ss" and returns a time_t value.
 * NOTE: The function ignores the characters between the number fields and can therefore be set arbitrarily (only their absolute positions are important).
 * NOTE: The function will ignore characters after the end "ss" (two-digit seconds) and will therefore ignore decimals. Fractions can not be returned in time_t anyway.
 * NOTE: Can handle PDS compliant time.
 * NOTE: If no hour-minute-seconds are given, then they will take zero (i.e. midnight at beginning of day) as default value.
 * Values are fed into mktime. mktime calculates UTC time since 1970 1 Jan 00:00:00
 * Negative return value is an error.
 * 
 * 01234567890123456789012
 * CCYY-MM-DDThh:mm:ss.fff
 */
int TimeOfDatePDS(char *sdate,time_t *t)
{
    char mday[3];
    char month[3];
    char year[6];
    char sec[3];
    char min[3];
    char hour[3];
    
    int len;
    
    struct tm atime; // Broken down time structure
    
    if(sdate==NULL) return -1; // Error no input date
    
    strncpy(year,sdate,4);      // Copy year from sdate
    strncpy(month,&sdate[5],2); // Copy month from sdate
    strncpy(mday,&sdate[8],2);  // Copy day of month to mday
    
    year[4]='\0';  // Terminate
    month[2]='\0'; // Terminate
    mday[2]='\0';  // Terminate  
    
    // Set default values in case they are not supplied by the caller (sdate).
    atime.tm_sec=0;  // Choose
    atime.tm_min=0;  // time
    atime.tm_hour=0; // at midnight.
    
    if((len=strlen(sdate))>10) // If we have a full PDS time string with more than just YYYY-MM-DD (year-month-day). Assume hour-minute-second.
    {
        strncpy(min,&sdate[14],2);      // Copy minutes from sdate
        min[2]='\0';                   // Terminate
        if(!sscanf(min,"%d",&(atime.tm_min))) {
            return -1;// Error couldn't resolve minutes
        }
        
        strncpy(hour,&sdate[11],2);    // Copy hours from sdate
        hour[2]='\0';                  // Terminate
        if(!sscanf(hour,"%d",&(atime.tm_hour))) {
            return -2;// Error couldn't resolve minutes
        }
        
        strncpy(sec,&sdate[17],2); // Copy seconds to sec
        sec[2]='\0';               // Terminate
        if(!sscanf(sec,"%d",&(atime.tm_sec))) {
            return -3;// Error couldn't resolve seconds
        }
    }
    
    
    if(!sscanf(mday,"%d",&(atime.tm_mday))) {  // Put day of month in structure
        return -4; // Error couldn't resolve day of month
    }
    
    if(!sscanf(month,"%d",&(atime.tm_mon))) {  // Put month in structure
        return -5; // Error couldn't resolve month
    }
    
    if(!sscanf(year,"%d",&(atime.tm_year))) {   // Put year in structure
        return -6; // Error couldn't resolve year
    }
    
    if(atime.tm_year<1970) {
        return -7;       // Err only do time after 1970, could be 2000..
    }
    
    atime.tm_year-=1900; // Get number of years since 1900, that's what mktime wants 
    atime.tm_mon-=1;     // Month ranges from 0 to 11 and not as usual 1 to 12
    
    atime.tm_wday=0;     // Day of week doesn't matter here
    atime.tm_yday=0;     // Day in year doesn't matter here
    atime.tm_isdst=0;    // Daylight saving unknown
    
    if((*t=mktime(&atime))<0) { // Calculates UTC time in seconds since 1970 1 Jan 00:00:00
        return -8;              // Error couldn't calculate time
    }
    
    
    *t=*t-timezone; // Compensates for assuming that the input time was in the local computer timezone
    // timezone is defined as negative number for Sweden so thus it needs to be a minus sign.
    
    return 0; // Ok!
}


// Returns current UTC time in CCYY-MM-DDThh:mm:ss  format
//
// Assumes that ltime has enough space 
// for the result, at least 20 chars.
int GetUTime(char *ltime) 
{
    
    time_t tsec;
    struct tm bt;
    
    tsec = time(NULL);   // Get time since 1970 1 Jan 00:00:00 in seconds
    gmtime_r(&tsec,&bt); // Convert time to broken down time, use thread safe version of ctime
    
    sprintf(ltime, "%04d-%02d-%02dT%02d:%02d:%02d",
            bt.tm_year+1900,
            bt.tm_mon+1,
            bt.tm_mday,
            bt.tm_hour,
            bt.tm_min,
            bt.tm_sec);
    
    return 0;
}

// LAP Logarithmic decompression functions
//----------------------------------------------------------------------------------------------------------------------------------

//
// This log is used for sweep compression.
// First X=|x|+1 is done..to get all values in the range 1 to 32768.
//
// The log is then:	 L(X)=17*b-17+(17*X)*2^-b
//
// b is the bit number of the highest bit set in X.
// L(X) is now in the range [0..255] thus fits in one byte.
// Input value and return value are stored in ACC
//
// The log is the following fit, scaled by 17 to get range [0..255].
//
// Y=(X-x1)*k+y1
// 
// k=(y2-y1)/(x2-x1)
//
// There:	x1=2^b
//		x2=2^(b+1)
//		y1=b
//		y2=b+1
// 
// A slice of L(X)
// .
// .
// L(00451)=148
// L(00452)=149   -
// L(00453)=149   |  Note that several input values will correspond to the same output value of the logarithm.
// L(00454)=149   |  Now if we choose the "middle value" then computing the inverse we will be least wrong in an average sense.
// L(00455)=149   |
// L(00456)=149   |
// L(00457)=149   |
// L(00458)=149   |
// L(00459)=149   > Choosing this value for inverse will be least wrong in an average sense..thus L_Inverse(149)=459
// L(00460)=149   |
// L(00461)=149   |
// L(00462)=149   |
// L(00463)=149   |
// L(00464)=149   |
// L(00465)=149   |
// L(00466)=149   _
// L(00467)=150
// L(00468)=150
// L(00469)=150
// L(00470)=150
// L(00471)=150
// L(00472)=150
// L(00473)=150
// L(00474)=150
// L(00475)=150
// L(00476)=150
// L(00477)=150
// L(00478)=150
// L(00479)=150
// L(00480)=150
// L(00481)=150
// L(00482)=151
// L(00483)=151
// .
// .
void DoILogTable(unsigned int *ilog)
{
    int i;
    unsigned int X;
    unsigned int b;
    unsigned int L;
    unsigned int oldL=0;
    unsigned int cnt=0;
    
    for(i=0;i<256;i++) ilog[i]=0;
    
    for(X=1;X<=32768;X++)
    {
        b=HighestBit(X); // Get highest bit
        L=17*b-17+((17*X)>>b); // Calculate log
        ilog[L]+=X;
        if(L!=oldL) 
        {
            ilog[oldL]/=cnt;
            //  printf("ilog[%d]=%d\n",oldL,ilog[oldL]);
            oldL=L;      
            cnt=0;
        }
        cnt++;
    }
    
    for(i=0;i<255;i++)
    {
        if(ilog[i]==0)
            ilog[i]=oldL;
        else
            oldL=ilog[i];
    }
    
    ilog[255]=32767;
}

//
// This log is used for sweep compression.
// First X=|x|+1 is done..to get all values in the range 1 to 32768.
//
// The log is then:	 L(X)=17*b-17+(17*X)*2^-b
//
// b is the bit number of the highest bit set in X.
// L(X) is now in the range [0..255] thus fits in one byte.
// Input value and return value are stored in ACC
//
// The log is the following fit, scaled by 17 to get range [0..255].
//
// Y=(X-x1)*k+y1
// 
// k=(y2-y1)/(x2-x1)
//
// There:	x1=2^b
//		x2=2^(b+1)
//		y1=b
//		y2=b+1
// 


int LogDeComp(unsigned char *buff,int ilen,unsigned int *ilog)
{
    int i;
    int len;
    unsigned int hi;
    unsigned int lo;
    unsigned char *tbuff,*tmp;
    unsigned int il;
    if(ilen>0)
    {
        len=ilen*2;
        if((tbuff=(unsigned char *)malloc(len))!=NULL)
        {
            tmp=tbuff;
            for(i=0;i<ilen;i++) // Go through in buffer
            {
                il=ilog[buff[i]]; // Get inverse log of value
                il--; // Reduce by one according to algorithm
                hi=(0xff & (il>>8)); // Get highest byte 
                lo=(0xff &  il);     // Get lowest byte
                *(tmp++)=hi;         // Put hb in data stream
                *(tmp++)=lo;         // Put lb in data stream 
            }
            memcpy(buff,tbuff,len); // Copy over original data
            free(tbuff); // Free temporary buffer
            return len; // Return new length
        }
        else
        {
            printf("Error allocating memory\n");
            exit(2);
        }
    }
    else
        return 0; // Return 0 length
}

// Return highest bit set in value start checking at bit 15
int HighestBit(unsigned int value)
{
    int i;
    unsigned int mask=0x8000;
    
    for(i=15;i>=0;i--)
    {
        if(mask & value)
            return i;
        else
            mask>>=1;
    }
    return -1;
}

// DDS Archive functions (input archive)
//----------------------------------------------------------------------------------------------------------------------------------

// Traverse through DDS archive
// If end is reached it will wake up every 10th second
// and look for new data.

void TraverseDDSArchive(pds_type *p)
{
    FTS *af;               // Archive file structure
    FTSENT *fe;            // Entry
    struct stat sp;        // File status pointer
    
    int len;               //  Length
    int alen;              //  Acctual read length
    int slen;              //  Summ of lengths
    
    int skip_len;          // Skip base
    
    char *path_arrays[2];
    unsigned char *ibuff;
    struct timespec coma={COMA_TIME,0};  
    
    path_arrays[0]=p->apathdds; // DDS archive path
    path_arrays[1]=NULL;
    
    
    skip_len = strlen(p->apathdds); // Number of chars in DDS archive path
    
    // Make it work with messy input DDS Archive 050113
    while(1)
    {
        af=fts_open(path_arrays,FTS_LOGICAL,Compare); // Get file structure
        if(af!=NULL)
        {
            do
            {
                fe=fts_read(af); // Get next file
                if(fe!=NULL)     // Exists ?
                {
                    // Ignore directories not starting with 20
                    if(fe->fts_accpath[skip_len]=='2' && fe->fts_accpath[skip_len+1]=='0')
                    {
                        len=strlen(fe->fts_name);
                        if(len==DDS_TM_FILE_LEN)
                        {            
                            if(!Match("rpc######Sci00_##h##m##s_##h##m##s.tm",fe->fts_name))  // Match filename to pattern, # is a digit 0 to 9
                            {
                                if((p->ddsr_fd=fopen(fe->fts_accpath,"r"))==NULL) // Open matching file
                                {
                                    DPrintf("Error opening: %s\n",fe->fts_accpath); // Error
                                }
                                else
                                {
                                    if(FileStatus(p->ddsr_fd,&sp)<0) // Get file status
                                    {
                                        DPrintf("Error getting status: %s\n",fe->fts_name); // Error
                                    }
                                    else
                                    {
                                        len=(unsigned int)sp.st_size; // Get size in bytes
                                        
                                        if(len>0)
                                        {
                                            if((ibuff=malloc(len))==NULL) // Allocate buffer for whole file!!
                                                DPrintf("Error allocating in buffer for DDS file");
                                            else
                                            {
                                                slen=0; // Summ of lengths
                                                // Read file loop 
                                                // Most of the time only one read will be done
                                                // but if an error occurs we do try again to read the rest!
                                                // Maybe add error counter?
                                                while(len>slen) // While not end of file keep going
                                                {
                                                    alen=fread(ibuff,1,len-slen,p->ddsr_fd);  // Try to read all in one go! 
                                                    slen+=alen;                               // Remember already read data
                                                }
                                                ProcessDDSFile(ibuff,len,&sp,fe); // Process DDS file buffer
                                                free(ibuff);
                                            }
                                        }
                                    }
                                    fclose(p->ddsr_fd);
                                }
                            }
                        }
                    }
                }
                // Here we can do a gracefull exit! if Ctrl-C is pressed
                if(exit_gracefully)
                {
                    fts_close(af);
                    ExitPDS(0);
                }
            } while(fe!=NULL);
        } // if
        
        
        // At this point the whole archive is traversed
        fts_close(af); // Close file structure
        
        // Wait until circular TM data buffer has been processed
        nanosleep(&coma,NULL);
        
        // Here we can do a gracefull exit!
        // Index is written in ExitPDS()
        ExitPDS(0);
    }
}   // TraverseDDSArchive



// User defined function to fts_open which introduces order to hierarchy traversal.
// return a negative value, zero, or a positive value to indicate if the file referenced  by
// its first argument comes before, in any order with respect to, or after, the file 
// referenced by its second argument.
int Compare(const FTSENT **af, const FTSENT **bf)
{
    time_t at,bt;
    
    FTSENT *a;
    FTSENT *b;
    
    a=*((FTSENT **)af);
    b=*((FTSENT **)bf);
    
    if(a!=NULL && b!=NULL)
    {
        
        at=DDSFileStartTime(a); // Get start time
        bt=DDSFileStartTime(b); // Get start time
        
        if(at<bt)
            return -1;
        
        if(at>bt)
            return 1;
    }
    return 0;
}

// Decode a DDS file
void ProcessDDSFile(unsigned char *ibuff,int len,struct stat *sp,FTSENT *fe)
{
    char tmp_str[32];
    unsigned int length;
    unsigned short int gsid; // Ground station ID
    unsigned short int vch;  // Virtual channel
    unsigned char SLE;
    unsigned char TQ;
    unsigned char *endp;
    
    static double old=0.0;   // Previous DDS packet time
    static double scet=1.0;  // DDS Packet time
    
    double eps=1e-5; // Epsilon..cant have to small
    
    int toggle=1; // Just print one message to the log, until time is current
    
    int i;
    
    endp=ibuff+len;
    do
    {
        length=(ibuff[8]<<24 | ibuff[9]<<16 | ibuff[10]<<8 | ibuff[11]);
        
        if(length>len)
        {
            DPrintf("Corrupted length\n");
            break;
        }
        
        if(scet>=old) // As long as we at least have the same DDS time or move forward in time..
        {
            old=scet; // Remember old time
            toggle=1; // Indicate monotonic time
        }
        
        scet=DDSTime(ibuff); // Get time of DDS packet
        
        // DEBUG
        //Scet2Date_2(scet,tmp_str,0); 
        //printf("DDS scet %f %s mpstart %d mpstop %d\n",scet,tmp_str,mp.start,mp.stop);
        
        // Select data within the mission phase only
        if(mp.start>0)
            if(scet<mp.start || scet>mp.stop)
            {
                ibuff+=(18+length); // Skip DDS packet
                continue;
            }
            
            // We are most likely to have already processed this DDS packet if a jump back in time occurs 
            // because we are going through the data in time order.
            if(scet<(old-eps)) // - eps makes sure we really detect a true jump back in time
            {
                // If previous DDS packets had monotonic time do one message that it is not so anymore
                // toggle prevents multiple messages!
                if(toggle) 
                {
                    DPrintf("Found likely DDS packet duplicate, skipping\n");
                    toggle=0;
                }
                ibuff+=(18+length); // Skip DDS packet, most probably done this one before!
                continue;
            }      
            
            Scet2Date_2(scet,tmp_str,0);
            DPrintf("SCET: %s\n",tmp_str);
            
            DPrintf("Length: %d\n",length);
            
            gsid=(ibuff[12]<<8 | ibuff[13]);        // Ground station ID
            DDSGroundSN(gsid,tmp_str);              // Get Ground station name from global variable gstations
            
            DPrintf("Ground station:%s\n",tmp_str);
            vch=DDSVirtualCh(ibuff);                // Get virtual channel
            DPrintf("Virtual channel %d\n",vch);
            
            SLE=ibuff[16];                          // SLE type
            TQ=ibuff[17];                           // Time quality
            
            DPrintf("SLE Type %d Time quality %d\n",SLE,TQ);
            DPrintf("SC TM FIFO buffer usage %4.1f %\n",100.0*cbtm.fill/cbtm.max);
            
            ibuff+=18;                              // Skip DDS packet header
            
            // CRC=crc16(ibuff,length);
            // DPrintf("Data CRC: 0x%x\n",CRC);
            
            InB(&cbtm,ibuff,length);                // Store data in cicular S/C TM buffer
            
            ibuff+=length;                          // Go forward
            
            // If the circular S/C TM buffer is filled to more than 85% then relinquish cpu until its not
            while(FullBuffer(&cbtm,0.85)) 
            {
                pthread_testcancel();
                sched_yield();
            }
            
            // If the circular S/C TM buffer is filled to more than 55% then relinquish cpu, test finite times
            for(i=0;i<FINITE_YIELDS;i++)
            {
                if(FullBuffer(&cbtm,0.55)) 
                {
                    pthread_testcancel();             
                    sched_yield();
                }
                else
                    break;
            }
            
    }while(ibuff<endp);
    
    
    // Log information that tells us this file has been processed
    
    // OLD log format @Process time:Path+Name
    //time_t tmp_t;
    //tmp_t=time(NULL);                       // Get current time
    //strncpy(tmp_str,ctime(&tmp_t),24);      // Copy time to temp string, skip \n and \0.
    //tmp_str[24]='\0';                       // Add \0
    //fprintf(pds.ddsp_fd,"@%s:%s\n",tmp_str,fe->fts_path);
    
    GetUTime(tmp_str);   // Get universal time string.
    fprintf(pds.ddsp_fd, "%s (UTC): %s\n", tmp_str, fe->fts_path);
    
    fflush(pds.ddsp_fd);
}


// Returns the duration of the DDS file name ex:
//
// rpc040319Sci00_03h40m13s_03h41m50s.tm
// 0123456789012345678901234567890123
//
// (Duration is returned in seconds)
int DDSFileDuration(char *str)
{
    char tstr[64]; // Temporary string
    int bh,bm,bs,eh,em,es;
    
    int bgn;
    int end;
    int dff;
    
    strncpy(tstr,str,64); // Copy to temporary string
    
    tstr[17]='\0';
    tstr[20]='\0';
    tstr[23]='\0';
    tstr[27]='\0';
    tstr[30]='\0';
    tstr[33]='\0';
    
    sscanf(&tstr[15],"%02d",&bh);
    sscanf(&tstr[18],"%02d",&bm);
    sscanf(&tstr[21],"%02d",&bs);
    sscanf(&tstr[25],"%02d",&eh);
    sscanf(&tstr[28],"%02d",&em);
    sscanf(&tstr[31],"%02d",&es);
    
    
    bgn=bh*3600+bm*60+bs;
    end=eh*3600+em*60+es;
    
    // In case time starts in day x and ends in day x+1
    if((dff=end-bgn)<0) 
        dff+=86400;
    
    if(debug>2)
        printf("DDS FILE:%s DURATION:%d\n",str,dff);
    
    return dff;
}

// Returns the start time of any file/directory
// in a DDS archive hierarchy..
// Function assumes that it is a true DDS archive
// hierarchy. Start time is in seconds)

time_t DDSFileStartTime(FTSENT *f) 
{
    char tstr[64]; // Temporary string
    struct tm at; // Broken down time structure
    time_t t;
    
    strncpy(tstr,f->fts_name,64); // Copy to temporary string
    
    // Default 2002 Jan 1 00:00:00
    at.tm_mday=1;
    at.tm_mon=1;  
    at.tm_year=2; 
    
    at.tm_hour=0;
    at.tm_min=0;
    at.tm_sec=0;
    
    // If its a DDS filename, extract info
    if(f->fts_namelen==DDS_TM_FILE_LEN && !strncmp("rpc",f->fts_name,3))
    {
        tstr[9]='\0';
        sscanf(&tstr[7],"%02d",&at.tm_mday);
        tstr[7]='\0';
        sscanf(&tstr[5],"%02d",&at.tm_mon);
        tstr[5]='\0';
        sscanf(&tstr[3],"%02d",&at.tm_year);
        
        tstr[17]='\0';
        tstr[20]='\0';
        tstr[23]='\0';
        
        sscanf(&tstr[15],"%02d",&at.tm_hour);
        sscanf(&tstr[18],"%02d",&at.tm_min);
        sscanf(&tstr[21],"%02d",&at.tm_sec);
        //      printf("Year  %d Month %d Day %d %02d:%02d:%02d\n",at.tm_year+2000,at.tm_mon,at.tm_mday,at.tm_hour,at.tm_min,at.tm_sec);
    }
    else
    {
        //Get, if only  year
        if(f->fts_namelen==4 && IsNumber(f->fts_name))
        {
            sscanf(&f->fts_name[2],"%02d",&at.tm_year);
            //	  printf("Year %d\n",at.tm_year+2000);
        }
        // Get, if month and year
        if(f->fts_namelen==2 && f->fts_level==2 && f->fts_pathlen>5 && IsNumber(f->fts_name))
        {
            sscanf(f->fts_name,"%02d",&at.tm_mon);
            sscanf(&f->fts_path[f->fts_pathlen-5],"%02d",&at.tm_year);
            //	  printf("Year %d Month %d\n",at.tm_year+2000,at.tm_mon);
        }
        
        // Get, if year, month, day
        if(f->fts_namelen==2 && f->fts_level==3 && f->fts_pathlen>8 && IsNumber(f->fts_name))
        {
            sscanf(f->fts_name,"%02d",&at.tm_mday);
            sscanf(&f->fts_path[f->fts_pathlen-5],"%02d",&at.tm_mon);
            sscanf(&f->fts_path[f->fts_pathlen-8],"%02d",&at.tm_year);
            //	  printf("Year %d Month %d Day %d\n",at.tm_year+2000,at.tm_mon,at.tm_mday);
        }
        
        // Get, if year, month, day, sorted
        if(f->fts_namelen==6 && f->fts_level==4 && f->fts_pathlen>15)
        {
            sscanf(&f->fts_path[f->fts_pathlen-9],"%02d",&at.tm_mday);
            sscanf(&f->fts_path[f->fts_pathlen-12],"%02d",&at.tm_mon);
            sscanf(&f->fts_path[f->fts_pathlen-15],"%02d",&at.tm_year);
            //	  printf("Year %d Month %d Day %d\n",at.tm_year+2000,at.tm_mon,at.tm_mday);
        }
        
        // Get, if year, month, day, empty
        if(f->fts_namelen==5 && f->fts_level==4 && f->fts_pathlen>15)
        {
            sscanf(&f->fts_path[f->fts_pathlen-8],"%02d",&at.tm_mday);
            sscanf(&f->fts_path[f->fts_pathlen-11],"%02d",&at.tm_mon);
            sscanf(&f->fts_path[f->fts_pathlen-12],"%02d",&at.tm_year);
            //	  printf("Year %d Month %d Day %d\n",at.tm_year+2000,at.tm_mon,at.tm_mday);
        }
    }
    
    at.tm_mon--;      // Month ranges from 0 to 11 and not as usual 1 to 12
    at.tm_year+=100;  // Get number of years since 1900, that's what mktime wants 
    
    at.tm_wday=0;     // Day of week doesn't matter here
    at.tm_yday=0;     // Day in year doesn't matter here
    at.tm_isdst=0;    // Daylight saving unknown
    
    t=mktime(&at);   // Calculates UTC time in seconds since 1970 1 Jan 00:00:00
    
    t+=at.tm_gmtoff; // Add number of second east of UTC..to get UTC
    return t;
}

// Returns DDS Time from a DDS packet
double DDSTime(unsigned char *ibuff)
{
    unsigned int secs;
    unsigned int usecs;
    
    // We assume that we run on a little endian system! Linux i386 platform
    secs=(ibuff[0]<<24 | ibuff[1]<<16 | ibuff[2]<<8 | ibuff[3]);
    usecs=(ibuff[4]<<24 | ibuff[5]<<16 | ibuff[6]<<8 | ibuff[7]);
    
    return((secs+usecs/1.0e6));
}

// Returns DDS virtual channel
int DDSVirtualCh(unsigned char *ibuff)
{
    unsigned int vch;  // Virtual channel
    vch=(ibuff[14]<<8 | ibuff[15]);
    return vch;  
}

// Get Ground station name from global variable gstations
// As long as global vars do not change thus like constants
// It's not bad to use them..

void DDSGroundSN(unsigned short int gsid,char *str)
{
    int i;
    strncpy(str,"NO MATCHING GROUND STATION",27);
    
    for(i=0;i<NGSTATIONS;i++)
    {
        if(gsid==gstations[i].ID)
        {
            strncpy(str,gstations[i].gname,32);
            return;
        }
    }
}


// Dynamic allocation functions
//----------------------------------------------------------------------------------------------------------------------------------

void *CallocArray(int n,int s)
{
    void *P;
    
    if ((P=(void *) calloc (n,s)) == NULL)
    {
        perror ("calloc");
        return NULL;
    }
    
    return P;
}

double **CallocDoubleMatrix(int rows, int cols)
{
    int i;
    double **C;
    
    C = (double **) calloc (rows,sizeof (double *));
    
    if (C == NULL)
    {
        perror ("calloc rows");
        exit (1);
    }
    
    for (i = 0; i < rows; i++)
    {
        if ((C[i] = (double *) calloc (cols,sizeof (double))) == NULL)
        {
            perror ("calloc cols");
            exit (1);
        }
    }
    return C;
}

unsigned int **CallocIntMatrix(int rows, int cols)
{
    int i;
    unsigned int **C;
    
    C = (unsigned int **) calloc (rows,sizeof (unsigned int *));
    
    if (C == NULL)
    {
        perror ("calloc rows");
        exit (1);
    }
    
    for (i = 0; i < rows; i++)
    {
        if ((C[i] = (unsigned int *) calloc (cols,sizeof (unsigned int))) == NULL)
        {
            perror ("calloc cols");
            exit (1);
        }
    }
    return C;
}

void FreeIntMatrix(unsigned int **C, int rows, int cols)
{
    int i;
    for (i = 0; i < rows; i++)
    {
        if (C[i] != NULL)
            free (C[i]);
    }
    if (C != NULL)
        free (C);
}

void FreeDoubleMatrix(double ** C, int rows, int cols)
{
    int i;
    for (i = 0; i < rows; i++)
    {
        if (C[i] != NULL)
            free (C[i]);
    }
    if (C != NULL)
        free (C);
}

// RT Version functions
//----------------------------------------------------------------------------------------------------------------------------------
// Set thread priority and scheduling policy.
// For Real time version of PDS, this turned out not to be needed.
//
int SetPRandSched(pthread_t thread,int priority,int policy)
{
    int error;
    struct sched_param param;
    int dummy;
    
    if(!(error=pthread_getschedparam(thread,&dummy,&param)))
    {
        param.sched_priority=priority;
        YPrintf("Setting scheduling and priority\n");
        if((error=pthread_setschedparam(thread,policy,&param)))
            YPrintf("Can set priority or policy\n");
    }
    
    return error;
}


//##################################################################################################################
// Alternative main function that can be temporarily used instead of the real one for testing purposes.
// The real main function can conveniently be renamed (not commented out, not deleted) when using this function.
// This is useful for having test code that has access to other pds-internal functions.
int main_DISABLED(int argc, char* argv[]) {
    printf("###################################################################################\n");
    printf("The normal main() function has been DISABLED in this executable. This is test code.\n");
    printf("###################################################################################\n");
    ProtectPlnkInit();

    
    
    prp_type p;
    int errorCode;
    printf("Read file\n");
//  int ReadLabelFile(prp_type *lb_data,char *name)
    errorCode = ReadLabelFile(&p, "/home/erjo/temp/RPCLAP030101_CALIB_FRQ_E_P2.LBL");
    printf("errorCode = %i\n", errorCode);

    printf("Write file\n");
    //  int WriteUpdatedLabelFile(prp_type *lb_data,char *name)
    errorCode = WriteUpdatedLabelFile(&p, "/home/erjo/temp/RPCLAP030101_CALIB_FRQ_E_P2.LBL_modif");
    printf("errorCode = %i\n", errorCode);
    return -1;
}

