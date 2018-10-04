/*====================================================================================================================
 * LAP PDS SOFTWARE
 * ----------------
 *
 * See 00_README.TXT for more information.
 *
 *
 * VERSION: 3.09
 *
 * AUTHOR: Original author is Reine Gill; many modifications by Erik P G Johansson
 * Lines 4603-4604 modified from 2.98 by aie@irfu.se,
 * Lines 6661 & 6806-6815 edited by fj@irfu.se
 * DATE: 140710
 * ...
 *
 *
 * CHANGES FROM VERSION 3.07 TO 3.08
 * =================================
 *  * Fixed bug that assigns command-line argument specified configuration file paths to the wrong string variables.
 *       /Erik P G Johansson 2015-02-xx
 *  * Changed the name of three PDS keywords to being probe-specific (in addition to four previously renamed PDS keywords).
 *       /Erik P G Johansson 2015-03-xx
 *  * Implemented functionality for ignoring data within specific time intervals (pds.dataexcludetimes).
 *       /Erik P G Johansson 2015-03-31
 *  * Corrected typos in PDS DESCRIPTION fields in LBL files.
 *       /Erik P G Johansson 2015-03-30
 *  * Fixed bug that sometimes terminates the DecodeScience thread prematurely thus omitting the last hour(s) of data.
 *       /Erik P G Johansson 2015-03-31
 *  * Specifically permit the faulty macro 515 to set new vbias2 in every macro loop (not just the first loop) thus
 *    permitting vbias2 to change value in every macro loop cycle, as described in the .mds file.
 *       /Erik P G Johansson 2015-04-10
 *  * Modified and simplified code that updates certain PDS keyword values in LBL/CAT files copied from
 *    the template directory so that it now updates more PDS keyword values.
 *    Thus modified WriteLabelFile and renamed it WriteUpdatedLabelFile.
 *       /Erik P G Johansson 2015-05-04
 *  * Fixed bug that made INDEX.LBL not adjust to the DATA_SET_ID column changing width.
 *       /Erik P G Johansson 2015-05-12
 *
 *
 * CHANGES FROM VERSION 3.08 TO 3.09
 * =================================
 *  * Fixed bug that made ADC20 always choose high-gain.
 *       /Erik P G Johansson 2015-06-03
 *  * Added code for calibrating ADC20 data relative to ADC16
 *    See Chapter 4, "LAP Offset Determination and Calibration", Anders Eriksson 2015-06-02.
 *    Also see added constants in "pds.h".
 *       /Erik P G Johansson 2015-06-10
 *  * Fixed bug that multiplied ccalf_ADC16 (later renamed to ccalf_ADC16_old and abolished) by 16 for
 *  * non-truncated ADC20 data, density mode, P1/P2. This led to calibration offsets being multiplied by 16.
 *    (Braces surrounding the statements after two if-then were missing.)
 *       /Erik P G Johansson 2015-08-31
 *  * Fixed potential bug that interpreted macro ID strings as decimal numbers rather than hexadecimal numbers.
 *    Code still worked since the result was only used to compare with decimal macro ID numbers
 *    and the code (seemed to) fail well for hexadecimal string representations.
 *       /Erik P G Johansson 2015-12-07
 *  * Added extra flags for overriding the MISSION_PHASE_NAME, period start date & duration, and description
 *    string used in DATA_SET_ID and DATA_SET_NAME.
 *       /Erik P G Johansson 2015-12-09
 *  * Start time & duration can now be specified with higher accuracy than days using CLI parameters.
 *  * All logs now display wall time the same way ("3pds_dds_progress.log" was previously different).
 *  * Bug fix: pds.modes can now handle colons in the description string.
 *  * Raised permitted string length for code interpreting "pds.modes". Can now (probably) handle all rows up
 *    to 1024 characters, including CR & LF.
 *       /Erik P G Johansson 2015-12-17
 *  * Bug fix: Reading LBL files failed to add CR at end-of-line for first line of multiline keyword values.
 *    Bug was in ReadLabelFile(..).
 *    This caused newlines without CR when pds modified
 *       CALIB/RPCLAP030101_CALIB_FRQ_D_P1.LBL
 *       CALIB/RPCLAP030101_CALIB_FRQ_E_P1.LBL
 *       CALIB/RPCLAP030101_CALIB_FRQ_D_P2.LBL
 *       CALIB/RPCLAP030101_CALIB_FRQ_E_P2.LBL
 *       /Erik P G Johansson 2015-12-10, 2016-03-21
 *  * Bug fix: StrucDir no longer alters parameter "date".
 *       /Erik P G Johansson 2016-03-21.
 *  * Bug fix: DecodeHK/ExitPDS:
 *    Last HK LBL file in data set: SPACECRAFT_CLOCK_STOP_COUNT was erroneously effectively the same as
 *    SPACECRAFT_CLOCK_START_COUNT. Every HK LBL file with only one row (in TAB file): STOP_TIME was only a year.
 *       /Erik P G Johansson 2016-03-21.
 *  * Bug fix: BUG: HK LBL files always had INSTRUMENT_MODE_DESC = "N/A". Now they use the macro descriptions.
 *       /Erik P G Johansson 2016-03-22
 *  * Updated to update LBL files under DOCUMENT/ (recursively).
 *       /Erik P G Johansson 2016-04-04
 *  * Bug fix: Corrected incorrect catching of errors when reading (some) calibration files: =+ --> +=
 *       /Erik P G Johansson 2016-04-11
 *  * Updated the way offset calibrations and TM conversion factors are selected, i.e. RPCLAPyymmdd_CALIB_MEAS.LBL/.TAB files.
 *    Uses the nearest calibration by default. Added a manual "calibration selection" list (override; later renamed).
 *    Removes unused calibration files (replaces the previous functionality for removing unused calibration files).
 *       /Erik P G Johansson 2016-04-13
 *  * Added tests for erroneous .mds files/macro descriptions to warn the user that he/she is probably using (incompatible) files generated
 *    with the older version of MEDS.
 *       /Erik P G Johansson 2016-04-25
 *  * ~Bug fix: Modified misconfigured SAMP_FREQ_ADC20 60.0 --> 57.8 (pds.h).
 *       /Erik P G Johansson 2016-04-26
 *  * Renamed the functionality "calibration selection" to "offset calibration exceptions" (OCE).
 *    Modified code to read the data from LBL+TAB file pair under CALIB/. Picks TAB file from LBL file.
 *       /Erik P G Johansson 2016-06-22
 *  * Modified the compressed output logs file name to contain the data set ID.
 *       /Erik P G Johansson 2016-06-22
 *  * Bug fix: WriteUpdatedLabelFile now updates TARGET_NAME. Needed for updating DATASET.DAT.
 *       /Erik P G Johansson 2016-07-21
 *  * Bug fix: Wrote illegal unquoted PDS keyword value containing dash/hyphen in P1-P2_CURRENT/VOLTAGE.
 *       /Erik P G Johansson 2016-07-22
 *  * Bug fix: Shortened RPCLAP_CALIB_MEAS_EXCEPTIONS.* to RPCLAP_CALIB_MEAS_EXCEPT.* too keep lengths of filenames
 *       within 27+1+3 (PDS requirement).
 *       /Erik P G Johansson 2016-10-06
 *  * Bug fix: HK label column "TMP12" BYTES=4 --> BYTES=5.
 *       /Erik P G Johansson 2016-10-25
 *  * Bug fix: Observed illegally changing P2_CURRENT TAB column widths. All EDITED current+voltage column widths (incl. for
 *    P3) changed from 6 to 7 bytes to accomodate for full range of ADC20 values.
 *       /Erik P G Johansson 2016-11-23
 *  * ~Bug fix: Removes CALIB_MEAS_EXCEPT files (LBL+TAB) for EDITED datasets. Simultaneously changed to prescribing TAB filename
 *       instead of reading it from the LBL file (since one now needs the TAB path twice in the code, rather than once).
 *       /Erik P G Johansson 2016-12-14
 * * Bug fix: ADC16-to-ADC20 offset calibration bug. E-field at times when there were both P1 and P2 data (which was not P3 data) used
 *      the wrong offset (the P3 offset) which was far too small.
 *      /Erik P G Johansson 2017-03-01
 * * Added offsets for 8 kHz filter (ADC16), compensation for moving average factor bug (in flight s/w).
 *      /Erik P G Johansson 2017-05-16/30.
 * * Functionality for arbitrary ADC16 jump magnitude for EDITED (integer) and CALIB (decimal number) separately.
 *      /Erik P G Johansson 2017-06-01
 * * Bugfix: Used offset for P1 when writing calibrated P2 fine sweeps. Changed p1_fine_offs --> p2_fine_offs.
 *      /Erik P G Johansson 2017-06-07
 * * Experimentally using C_ADC20 := C_ADC16 / 16.0, where C=calibration factor, due to the ground calibration being faulty.
 *      /Erik P G Johansson 2017-06-09
 * * No longer reads C_ADC20 from CALIB_MEAS files PDS keywords. Derives them from C_ADC16 values and hardcoded probe-specific
 *   C_ADC20/C_ADC16 ratios instead.
 *      /Erik P G Johansson 2017-0x-xx
 * * Configures SPICE but does not use it.
 *   Loads SPICE metakernel (extra row in pds.conf specifies the path). Configures SPICE error behaviour. Does not use SPICE
 *   except for in test code.
 *      /Erik P G Johansson 2017-07-06
 * * Uses SPICE for ConvertSccd2Utc and implicitly for LBL (PDS keywords) and TAB files (columns). (Other conversions UTC<-->time_ still do not use SPICE.)
 *      /Erik P G Johansson 2017-07-10
 * * Will not write files for macro 710 P2 HF, and macro 910 P1 HF (hardcoded data exclusion).
 *      /Erik P G Johansson 2017-07-14
 * * No longer read ADC16 calibration factors from PDS keywords in CALIB_MEAS files. Uses hardcoded values instead.
 *      /Erik P G Johansson 2017-08-21
 * * Can now use CALIB_COEFF (or CALIB_MEAS) for bias-dependent current offsets.
 *      /Erik P G Johansson 2017-08-22
 * * CALIB_COEFF LBL files are updated.
 *      /Erik P G Johansson 2017-08-31
 * * Bugfix: Use SPICE mutex for ConvertUtc2Sccd_SPICE. Seems to prevent random crashes for CALIB+CALIB_COEFF_PRELOAD=FALSE.
 *      /Erik P G Johansson 2017-09-01
 * * Bugfix: DecideWhetherToExcludeData now uses only TM timestamps (i.e. never corrected for group delay).
 *      /Erik P G Johansson 2017-09-19
 * * Bugfix: DATASET.CAT:START_TIME first set according to mission calender (or arguments), but is then adjusted for those data products
 *   (only SCI data; not HK yet) which often begin before the official mission phase due to(?) not splitting data products.
 *   Note that data products in some mission phases, e.g. AST1, begin long __AFTER__ DATASET.CAT:START_TIME.
 *      /Erik P G Johansson 2017-11-17
 * * Implemented saturation detection.
 *      /Erik P G Johansson 2018-02-23
 * * Modified saturation detection: Use two special TM values per bit resolution (16/20) (instead of just one).
 *      /Erik P G Johansson 2018-03-12
 * * Added flags for:
 *      - Setting arbitrary output dataset parent directory.
 *      - Setting end of time interval (an alternative to setting duration).
 *      - Overriding the remaining values in the mission calendar.
 *      /Erik P G Johansson 2018-07-06
 * * Bugfix: Corrected the sweep timestamp increment.
 *      /Erik P G Johansson 2018-07-30
 * * Bugfix: HK files used to NOT break at midnight, and the following HK files began where
 *   the last HK file ended, i.e. not at the beginning of the day if previous HK file
 *   stretched over midnight.
 *      /Erik P G Johansson 2018-08-01
 * 
 *
 * BUGS
 * ====
 * "BUG": INDEX.LBL contains keywords RELEASE_ID and REVISION_ID, and INDEX.TAB and INDEX.LBL contain columns
 *        RELEASE_ID and REVISION_ID which should all be omitted for Rosetta.
 *     NOTE: Changing this will/might cause backward-compatibility issues with lapdog and write_calib_meas.
 *     NOTE: Not important to fix. The INDEX is in practice generated separately with "pvv index".
 *     
 * "BUG": Code requires output directory path in pds.conf to end with slash.
 * BUG: HK label files do not use "fingerprinting" for identifying macros (only DecodeScience does) and can therefor
 *      not recognize all macros.
 * BUG?: HK LBL files and INDEX.LBL have PRODUCT_ID without quotes. Uncertain if it is required but (1) the examples
 * in "Planetary Data System Standards Referense, Version 3.6" imply that one probably should, and (2) it is
 * inconsistent with the SCI LBL files and the CALIB/RPCLAP*.LBL files which do have quotes.
 *      Update 2017-10-03: Changed INDEX.LBL file to have quoted PRODUCT_ID.
 * BUG?: Probably does not handle leap seconds correctly all the time. Has found a ~one second error at leap second in
 *    DATA/EDITED/2015/JUL/D01/RPCLAP150701_001_H.LBL : START_TIME-SPACECRAFT_CLOCK_START_COUNT.
 *    /Erik P G Johansson, 2016-10-17
 *    Guessing (but not verifying) that this is FIXED with SPICE.
 *    /Erik P G Johansson, 2017-08-16
 * 
 * BUG: Rare instances of bad INSTRUMENT_MODE_ID values.
 *      RO-C-RPCLAP-2-ESC3-MTP020-V1.0/DATA/EDITED/2015/SEP/D03/RPCLAP150903_01A_H.LBL
 *          INSTRUMENT_MODE_ID = MCID0Xe2fc
 *          INSTRUMENT_MODE_DESC = "Open Sweep Test Calibration"   // Corresponds to macro 104.
 *          NOTE: grep INSTRUMENT_MODE_ID *_01*_H.LBL  ==> Overlaps with 104 data.
 *      RO-C-RPCLAP-2-ESC3-MTP020-V1.0/DATA/EDITED/2015/SEP/D21/RPCLAP150921_001_H.LBL
 *          INSTRUMENT_MODE_ID = MCID0Xe2fc
 *          INSTRUMENT_MODE_DESC = "Density P1P2 Burst Mode, Fix Bias -30/-30V, Cont. truncated no down, Sweeps P1 & P2 +-30V"
 *              // Corresponds to macro 914 (before correcting BM/NM typo in pds.modes).
 *          NOTE: grep INSTRUMENT_MODE_ID D21/RPCLAP*_00*_H.LBL  ==> 914 data afterwards
 *          NOTE: First HK after several days without data.
 *      RO-C-RPCLAP-2-ESC3-MTP021-V1.0/DATA/EDITED/2015/OCT/D17/RPCLAP151017_03X_H.LBL
 *          INSTRUMENT_MODE_ID = MCID0Xe2fc
 *          INSTRUMENT_MODE_DESC = "EE float Cont. 20 bit, Every AQP 16 bit P1 & P2"   // Fits macro 802
 *          NOTE: grep INSTRUMENT_MODE RPCLAP151017_0[34]*_H.LBL ==> Just when switching macro 802-->914
 *      NOTE: Odd INSTRUMENT_MODE_ID value. Usually MCID0Xe2fc (= 0xe2fc = 58108), but not always.
 *      NOTE: _Valid_ INSTRUMENT_MODE_DESC that is inherited from previous iteration.
 *      NOTE: Only HK-LBL files.
 *      NOTE: Appears that the macro_id value returned by AssembleHKLine is wrong. It in turn comes from the bitstream.
 *      NOTE: The time interval covered by the dataset influences when the bug shows up, probably because only a subset
 *      of the macro_id values in the HK bitstream are actually used for the HK-LBL, and which subset depends on the dataset start time.
 *      HK_NUM_LINES determines the number of HK packets(?) per HK TAB file, and (presumably) only one of these packets
 *      is used for the INSTRUMENT_MODE_ID in the corresponding HK-LBL file.
 *
 * BUG?: Some science data files in density mode do not contain ROSETTA:LAP_P1/P2_STRATEGY_OR_RANGE which specifies gain (among others).
 *      See data for 2015-05-20.
 *      /Erik P G Johansson =<2017
 * 
 * 
 * NOTES
 * =====
 * NOTE: Source code indentation is largely OK, but with some exceptions. Some switch cases use different indentations.
 *       This is due to dysfunctional automatic indentation in the Kate editor.
 * NOTE: Contains multiple occurrances of "0xCC" which one can suspect should really be replaced with S_HEAD.
 * NOTE: time_t:
 * (1) The code relies on that time_t can be interpreted as seconds since epoch 1970 which is true for POSIX and UNIX but not C in general.
 * (Source: http://stackoverflow.com/questions/471248/what-is-ultimately-a-time-t-typedef-to)
 * Code that uses this includes (incomplete list): function ConvertUtc2Timet_2 (used only once), WritePTAB_File,
 * possibly the use of bias e.g. in LoadBias. Empirically (playing with TimeOfDatePDS/ConvertUtc2Timet, pds' default compiler),
 * time_t appears to correspond to number of seconds after 1970-01-01 00:00.00, not counting leap seconds.
 * (2) Code partly uses standard POSIX C functions that convert time_t <--> ~UTC which appear to NOT take leap seconds into account!
 *      Data as such seems to be correct wrt. leap seconds though.
 * 
 * 
 * NAMING CONVENTIONS, DEFINITIONS OF TERMS
 * ========================================
 * Functions/variables for (1) interpreting time from byte streams, and (2) converting between different time formats, have been renamed to
 * make their functions easier to understand. The names use the following naming conventions:
 *      UTC   = A string on the form 2016-09-01T00:16:51.946, although the number of second decimals may vary (or be ignored on reading).
 *      Timet = Variable which value can be interpreted as time_t, although many times it is actually a double, int, or unsigned int.
 *      SCCS  = Spacecraft clock count string, e.g. 1/0431309243.15680 (false decimals).
 *      SCCD  = Spacecraft clock count double, i.e. approximately counting seconds (true decimals). Note that the reset counter has to be handled separately.
 * 
 * TM                         : Telementry. Can refer to: (1) Quantity in TM units, (2) Time derived from TM without correcting for signal delay (ADC20).
 * corrected                  : In variable name: Time adjusted for signal delay (ADC20).
 * MC                         : Measurement/measured calibration(?)
 * MA                         : Moving average
 * tsweep = true sweep        : Part of a raw sweep that contains the actual, intended science data. Samples taken on a well-defined sequence of bias plateaus. Subset of a raw sweep.
 *                              This excludes the last sample on the last plateau, which according to pds source code comments (by Reine Gill) does not exist.
 * rsweep = raw sweep         : True sweep plus some samples taken before the true sweep.
 * insmp = internal sample(s) : Samples taken internally by RPCLAP and which may or may not have been later downsampled, averaged over etc.
 * tmsmp = TM sample(s)       : Samples which are actually output from the RPCLAP instrument.
 * 
 *
 *====================================================================================================================
 * PROPOSAL: Add check for mistakenly using quotes in MISSION_PHASE_NAME (CLI argument).
 * PROPOSAL: Flag --test for triggering test code, instead of preprocessing variable.
 * PROPOSAL: Add CALIB_COEFF file pattern to pds.conf, analogous to CALIB_COEFF file pattern.
 *    NOTE: lap_agility.sh will need to be updated?
 * PROPOSAL: Be able to not use mission calendar data at all, including mission phase abbreviation which is still REQUIRED to be in the mission calendar.
 *      CON/PROBLEM: Only want this when specifying exactly ALL mission calendar values. If not, then one still wants the remaining
 *                   value(s) read from the mission calendar.
 * 
 * PROPOSAL: Let InitMissionPhaseStructFromMissionCalendar set DPL_number.
 *====================================================================================================================
 */

// *************************************************************
// -= "Emancipate yourself from mental slavery..", Bob Marley =-
// *************************************************************

#include <string.h>       // String handling
#include <ctype.h>        // Character types
#include <sys/stat.h>     // POSIX Standard file charateristics, fstat()
#include <errno.h>        // Used for error handling, perror()
#include <sys/types.h>    // POSIX Primitive System Data Types 
#include <dirent.h>       // POSIX Directory operations
#include <sys/wait.h>     // POSIX Wait for process termination
#include <sys/time.h>     // Time definitions, nanosleep()
#include <stdio.h>        // Standard Input/output 
#include <stdlib.h>       // Standard General utilities
#include <limits.h>       // Standard limits of integer types, PATH_MAX
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
#include <math.h>         // Floor function
#include <SpiceUsr.h>     // Required for calling CSPICE (SPICE for C) functions.

#include "id.h"           // LAP Data ID codes
#include "esatm.h"        // S/C TM Definitions
#include "pds.h"          // PDS & LAP definitions and structures
#include "plnk.h"         // Code for linked lists of property/value pairs
#include "plnkdec.h"      // plnk declarations
#include "cirb.h"         // Code for simple power of two circular buffers 
#include "cirbdec.h"      // cirb declarations
#include "nice.h"         // Sleep definitions etc
#include "calib_coeff.h"  // Functionality for loading, finding, and interpolating CALIB_COEFF coefficients.


// We skipped using OASWlib and ESA provided time cal. approach it produced weird results.
// We still use time calibration packets though.
//
//     FORTRAN ROUTINES PROVIDED BY ESOC FROM DDS SYSTEM
//extern void dj2000_(double *DAY,int *I,int *J,int *K,int *JHR,int *MI,double *SEC);


void PrintUserHelpInfo(FILE *stream, char *executable_name);    // Print user help info.
void InitSpice(char *metakernel_path);
int CheckSpiceError(char *caller_error_msg, int exit_pds, int print_to_stdout);

// Thread functions
//----------------------------------------------------------------------------------------------------------------------------------
void *SCDecodeTM(void *);					// Decode S/C TM thread
void *DecodeHK(void *);						// Decode HK data thread
void *DecodeScience(void *);					// Decode Science data thread

// Signal handler
//----------------------------------------------------------------------------------------------------------------------------------
static void ExitWithGrace(int signo);				// Graceful exit

// Logging and exit functions
//----------------------------------------------------------------------------------------------------------------------------------
void ExitPDS(int status);					// Closes logging and exits with status
int YSPrintf(const char *fmt, ...);
int  YPrintf(const char *fmt, ...);				// Prints to pds system log
int  YPrintf2(const char *fmt, va_list args);
int  DPrintf(const char *fmt, ...);				// Prints to DDS packet filter log 
int  PPrintf(const char *fmt, ...);				// Prints to S/C packet filter log
int  CPrintf(const char *fmt, ...);				// Prints to Science decoding log
int  HPrintf(const char *fmt, ...);				// Prints to HK decoding log 
int  OpenLogFile(FILE **pfd,char *name,FILE *fderr);		// Opens log file with name "name"
int  AddPathsToSystemLog(pds_type *pds);			// Adds paths to system log

// Program option functions
//----------------------------------------------------------------------------------------------------------------------------------
int  GetOption(char *opt,int argc, char *argv[],char *arg);        // Get an input option
int  HasMoreArguments(int argc, char *argv[]);    // Return true if-and-only-if argv[i] contains non-null components for i >= 1.

// Pointer to file desc pointer pfd is needed and an error stream
// Functions to load and test external information
//----------------------------------------------------------------------------------------------------------------------------------
int  OpenFileCountDataRows(char *file_path, FILE **file_descr, int *N_rows);
int  LoadConfig1(pds_type *p);                          // Loads configuration information first part
int  LoadConfig2(pds_type *p, char *data_set_id);       // Loads configuration information second part

int  LoadAnomalies(prp_type *p,char *path);             // Load anomaly file
int  LoadModeDesc(prp_type *p,char *path);              // Load human description of macro modes into a linked list of properties.
int  LoadBias(unsigned int ***bias_s,unsigned int ***mode_s,int *bias_cnt_s,int *mode_cnt,char *path);		// Load bias settings file
int  LoadExclude(unsigned int **exclude,char *path);                                   // Load exclude file
int  LoadDataExcludeTimes(data_exclude_times_type **dataExcludeTimes, char *depath);   // Load data exclude times file.
int DecideWhetherToExcludeData(
    data_exclude_times_type *dataExcludeTimes,
    curr_type curr,
    int param_type,
    sweep_type *sw_info,
    unsigned int macro_id,
    int dop,
    int *shouldExcludeFilePair);

int  LoadTimeCorr(pds_type *pds,tc_type *tcp);                                  // Load time correlation packets
int  LoadMacroDesc(prp_type macs[][MAX_MACROS_INBL],char *);                    // Loads all macro descriptions
int  InitCalibMeas(char *rpath, char *fpath, char *pathocel, char *pathocet, m_type *m);             // Get measured data calibration files
void FreeDirEntryList(struct dirent **dir_entry_list, int N_dir_entries);
// int  InitMissionPhaseStructFromMissionCalendar(mp_type *mp, pds_type *pds);      // Given a path, data set version and mission abbreviation (in mp)
int  InitMissionPhaseStructFromMissionCalendar(mp_type *m, char *mission_calendar_path, int DPL_number, float data_set_version);

// Derive DATA_SET_ID and DATA_SET_NAME keyword values, INCLUDING QUOTES!
void DeriveDSIandDSN(
    char* DATA_SET_ID, char* DATA_SET_NAME,
    char* target_name_dsi, int DPLNumber, char* mpAbbreviation, char* descr, float data_set_version, char* target_name_dsn);

// void TestDumpMacs();   // Test dump of macro descriptions

// Label and Table functions
//----------------------------------------------------------------------------------------------------------------------------------
int UpdateDirectoryODLFiles(const char *dir_path, const char *filename_pattern, int update_PUBLICATION_DATE);
int UpdateODLFile(char *file_path, prp_type *odl_prp, int update_PUBLICATION_DATE);
int WriteUpdatedLabelFile(prp_type *pds, char *name, int update_PUBLICATION_DATE);      // Write label file
int ReadLabelFile(prp_type *pds,char *name);                                            // Read a label file 
int ReadTableFile(prp_type *lbl_data,c_type *cal,char *path, char *msg);                // Read "generic" table file
int UpdateDATASET(mp_type *mp_arg, pds_type *pds_arg);                                  // Update DATASET.CAT

// Miscellaneous functions
//----------------------------------------------------------------------------------------------------------------------------------
char GetBiasMode(curr_type *curr, int dop);
int  SelectCalibrationData(time_t, char*, m_type*);
int  DestroyCalibMeas(char*, char *pathocel, char *pathocet, m_type*);

// Write data to data product table file.
int WritePTAB_File(
    unsigned char *buff, char *fname, int data_type, int N_tmsmp, int id_code, int N_bytes, sweep_type *sw_info, adc20_type *a20_info,
    curr_type *curr, int param_type, int ADC16_P1_insmp_per_tmsmp, int ADC16_P2_insmp_per_tmsmp, int dop,
    m_type *m_conv, unsigned int **bias, int nbias, unsigned int **mode, int nmode, int N_non_tsweep_tmsmp, int N_plateau_tmsmp);

void set_saturation_limits(
    double* x_phys_min,
    double* x_phys_max,
    int* x_TM_saturation_1,
    int* x_TM_saturation_2,
    int is_ADC20_nontrunc,
    int is_high_gain,
    int is_Efield);
double handle_saturation(
    double x_phys,
    int    x_TM,
    double x_phys_min,
    double x_phys_max,
    int x_TM_saturation_1,
    int x_TM_saturation_2);


// Write to data product label file .lbl
int WritePLBL_File(char *path,char *fname,curr_type *curr,int N_tmsmp,int id_code,int dop,int N_non_tsweep_tmsmp,int param_type);

// Buffer and TM functions
//----------------------------------------------------------------------------------------------------------------------------------
void FreeBuffs(buffer_struct_type *b0,buffer_struct_type *b1,buffer_struct_type *b2,buffer_struct_type *b3); // Free buffer memory
int  GetBuffer(buffer_struct_type *cs,unsigned char *buff,int len);			// Get data from circular buffer
int  LookBuffer(buffer_struct_type *bs,unsigned char *buff,int len);			// Look ahead in circular buffer
int  GetHKPacket(buffer_struct_type *,unsigned char *,double *);			// Get one packet of HK data
void DumpTMPacket(buffer_struct_type *cs,unsigned char packet_id);			// Dump the non interesting SC TM packets
int  SyncAhead(buffer_struct_type *cb,int len);						// Test data synchronisation ahead.

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
int  ExtendStr(char *dest,char *src,int elen,char ch);	// Make a new string dest using src extended with ch to elen length. 
void ReplCh(char *str,char ch1,char ch2);				// Replace all characters ch1 in str with characters ch2
int  IncAlphaNum(char *n);								// Increments the alphanumeric number stored in string n 
// returns <0 on error
int  GetAlphaNum(char *n,char *path,char *pattern);		// Get largest alphanumeric value stored in file names matching 
// a specific pattern
// In directory path
int  Alpha2Num(char *n);								// Convert a positive alpha numeric value to a number 
// returns negative value on error
int  IsNumber(char *str);								// Check if string is a number


// File handling functions
//----------------------------------------------------------------------------------------------------------------------------------
int  FileLen(FILE *);									// Get length of file
int  FileStatus(FILE *fd,struct stat *sp);				// Return FileStatus
int  SetupPath(char *error_txt,char *path);				// Resolves,tests and returns a usable path 
int  TestDir(char *name);								// Test if directory exists
int  TestFile(char *name);								// Test if file exists and if we can read it.
int  MakeDir(char *,char *,char *);						// Make a directory for current YYMMDDD, if it's not already there!
int  StrucDir(char *,char *,char *);					// Test and create directory structure for data
void DumpDir(char *path);								// Dump a directory. Mostly for debugging
int  GetUnacceptedFName(char *name);					// Get new filename for manual unaccepted file
void FTSDump(FTSENT *fe);								// Dump FTSENT structure for debbuging
int  Match(char *,char *);								// Match filename to pattern

// HK Functions
//----------------------------------------------------------------------------------------------------------------------------------
void AssembleHKLine(unsigned char *, char *, double, char*, unsigned int *);       // Assemble a HK line entry

// Low level data functions
//----------------------------------------------------------------------------------------------------------------------------------
void         SignExt20(int *);								// Sign extend 20 bit to 32 bit
double       GetDBigE(unsigned char *buff);						// Get bigendian double assuming we are on a little endian machine
unsigned int GetBitF(unsigned int word,int nb,int sb);					// Returns nb number of bits starting at bit sb

// Time-related functions
//----------------------------------------------------------------------------------------------------------------------------------
double DecodeSCTime2Sccd(unsigned char *buff);                      // Decoding S/C time, returns raw S/C time in seconds as a double
double DecodeLAPTime2Sccd(unsigned char *buff);                     // Decoding lap time, returns raw S/C time in seconds as a double
//int DecodeRawTimeEst(double raw,char *stime);                       // Decodes raw S/C time (estimates UTC no calibration) and returns 
// a PDS compliant time string. UNUSED

int ConvertSccd2Utc         (double sccd, char *utc_3decimals, char *utc_6decimals);         // Decodes raw S/C time (calibration included) 
int ConvertSccd2Utc_nonSPICE(double sccd, char *utc_3decimals, char *utc_6decimals);
int ConvertSccd2Utc_SPICE   (double sccd, char *utc_3decimals, char *utc_6decimals);

void ConvertUtc2Sccd_SPICE(char *utc, int *reset_counter, double *sccd);

int ConvertSccd2Sccs(double sccd, int n_resets, char *sccs, int quote_sccs);    // Convert raw time to an OBT string (On Board Time)
int ConvertSccs2Sccd(char *sccs, int *reset_counter, double *sccd);             // Convert OBT string to raw time.

//int Scet2Date(double raw,char *stime,int lfrac);     // Decodes SCET (Spacecraft event time, Calibrated OBT) into a date
// lfrac is long or short fractions of seconds.

// Replacement for Scet2Date ESA approach with OASWlib give dubious results. (dj2000 returns 60s instead of 59s etc.)
int ConvertTimet2Utc(double raw, char *utc, int use_6_decimals);            // Decodes SCET (Spacecraft event time, Calibrated OBT) into a date
// use_6_digits is long or short fractions of seconds.

void ConvertTimet2Sccd_SPICE(time_t t, int *reset_counter, double *sccd);

int ConvertUtc2Timet(char *sdate,time_t *t);                                // Returns UTC time in seconds (since 1970) for a PDS date string
// NOTE: This is not the inverse of Scet2Date!
unsigned int ConvertUtc2Timet_2(char *utc);                                 // Get seconds from 1970 epoch to epoch "epoch"

int ConvertUtc2Timet_midday(char *sdate, time_t *t);
int GetCurrentUtc0(char *);                                                 // Returns current UTC date and time as string CCYY-MM-DDThh:mm:ss

// LAP Logarithmic decompression functions
//----------------------------------------------------------------------------------------------------------------------------------
void DoILogTable(unsigned int *);
int  LogDeComp(unsigned char *buff,int ilen,unsigned int *ilog);
int  HighestBit(unsigned int value);


// DDS Archive functions (input archive)
//----------------------------------------------------------------------------------------------------------------------------------
void   TraverseDDSArchive(pds_type *p);                         // Traverse DDS archive path
int    Compare(const FTSENT **af, const FTSENT **bf);           // Used for traversal of DDS archives using fts functions.
// Also used in DumpDir() ( Can be seen as adding a metric to a mathematical file space :) )

void   ProcessDDSFile(unsigned char * ibuff,int len,struct stat *sp,FTSENT *fe); // Process DDS file
int    DDSFileDuration(char *str);                          // Returns DDS file duration in seconds, computed on filename!
time_t DDSFileStartTime(FTSENT *f);                         // Returns the start time of entries in a DDS archive for sorting purposes 
double DecodeDDSTime2Timet(unsigned char *ibuff);           // Return DDS packet time
int    DDSVirtualCh(unsigned char *ibuff);                  // Returns DDS packet virtual channel
void   DDSGroundSN(unsigned short int gsid,char *str);      // Get ground station name

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

// Test code
//----------------------------------------------------------------------------------------------------------------------------------
int main_TEST(int argc, char* argv[]);



//-=SOME GLOBAL VARIABLES AND STRUCTURES=- 
//----------------------------------------------------------------------------------------------------------------------------------

static volatile sig_atomic_t exit_gracefully=0; // Used to cleanly exit then Ctrl-C is pressed


unsigned int sec_epoch;    // Seconds from 1970 epoch to epoch "epoch"
int debug=0;               // Debug message level. 0=Off; higher number=increasing verbosity

int macro_priority=0;      // Priority of macros (Trust more or less than info in data). 0=Trust data, 1=Trust macro info

int calib=0;               // Indicate whether we are creating a calibrated (1) or edited (0) archive.

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
// prp_type cc_lbl;          // Linked property/value list for Coarse Bias Voltage Calibration Data Label
// prp_type ic_lbl;          // Linked property/value list for Bias Current Calibration Data Label
// prp_type fc_lbl;          // Linked property/value list for Fine Bias Voltage Calibration Data Label

c_type v_conv;                            // Coarse-voltage-bias-conversion-to-TM data structure (table).
c_type f_conv;                            // Fine  -voltage-bias-conversion-to-TM data structure (table. (fine=fine sweep)
c_type i_conv;                            // Current       -bias-conversion-to-TM data structure (table).
m_type m_conv;                            // CALIB_MEAS data structure.
calib_coeff_data_type calib_coeff_data;   // CALIB_COEFF data structure

tc_type tcp={0,NULL,NULL,NULL}; // Time correlation packets structure

unsigned int ilogtab[256];      // Inverse logarithmic table for decoding of logarithmic sweeps


hk_info_type hk_info;     // Some HK info needed to dump last HK label at exit

// Matrix of linked property/value lists containing macros
prp_type macros[MAX_MACRO_BLCKS][MAX_MACROS_INBL]; 

mp_type mp;     // Mission phase data

extern FILE *stdout;            // Keep track of standard output 



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
    "",         // Path to data exclude file
    "",         // Path to macro description file
    "",         // Mission calendar path and file name
    "",         // Dataset parent directory
    "",         // Archive path PDS (Out dataset)
    "",         // Archive path DDS (In data/TM)
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
    "",         // Offset calibration exceptions data (LBL)
    "",         // Offset calibration exceptions data (TAB)
    "",         // Data subdirectory path for PDS science
    "",         // Data path PDS HK
    "",         // Data subdirectory path for PDS HK
    "",         // Path to data that has not been accepted
    "",         // Index table file path.
    "",         // Path to SPICE metakernel.
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

// Can not place in pds.h, since pds.h is used in multiple files and this is a variable declaration.
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




// Circular Buffers
//--------------------------------------------------------------------------
// Originally intended to support realtime stream processing.
// Now they are usefull to spread processing load on multicore machines.
buffer_struct_type cbtm;        // Circular S/C TM buffer
buffer_struct_type cbs;         // Circular in Science buffer
buffer_struct_type cbh;         // Circular in HK buffer
buffer_struct_type cmb;         // Circular mirror buffer

int sc_thread_cancel_threshold_timeout;
    

// Mutexes for protecting code which is not thread-safe.
static pthread_mutex_t protect_log   = PTHREAD_MUTEX_INITIALIZER;   // Added mutex protector for logging functions
// Mutex for SPICE functions. SPICE functions are not thread-safe.
// NOTE: Can not handle recursive locking(?) (mutex-protected blocks within mutex-protected blocks).
static pthread_mutex_t protect_spice = PTHREAD_MUTEX_INITIALIZER;   



// -=MAIN FUNCTION=-
//----------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
//     if (RUN_TEST_CODE) {
//         return main_TEST(argc, argv);   // INFORMAL TEST CODE. WILL (PROBABLY) EXIT PDS ON ITS OWN.
//     }
    
    

    struct sigaction act;
    int status;             // Just a temporary status/error code variable
    char tstr1[MAX_STR];    // Temporary string
    char tstr2[MAX_STR];    // Temporary string
    char tstr3[MAX_STR];    // Temporary string
    
    prp_type tmp_lbl;     // Linked property/value list for temporary use
    
    arg_type scarg;       // Argument structure S/C TM thread
    arg_type sarg;        // Argument structure Science thread
    arg_type harg;        // Argument structure HK thread
    
    mp.t_start = 0;            // Mission start time, small dummy
    mp.t_stop  = INT32_MAX;    // Mission stop time, big dummy ~68 years
    mp.sccd_start_data = NAN;
    mp.sccd_stop_data  = NAN;
    
    unsigned int volume_id_nbr;   // The four-digit number in VOLUME_ID, ROLAP_xxxx
    

    
    if(GetOption("-h",argc,argv,NULL))
    {
        PrintUserHelpInfo(stdout, argv[0]);
        exit(0);
    }
    
    // Basic check on number of input arguments.
    if (argc>1+(2*21+1) || argc<1+6)
    {
        fprintf(stderr, "Called with too few or too many arguments.\n\n");
        PrintUserHelpInfo(stdout, argv[0]);    // NOTE: Prints to stderr.
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
        strncpy(mp.mission_phase_abbrev, tstr1, 4);
        mp.mission_phase_abbrev[4]='\0';
        printf("Processing mission phase with ID %s\n", mp.mission_phase_abbrev);
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
        sscanf(tstr1,"%d\n",&volume_id_nbr);
        printf("Volume ID: %d\n",volume_id_nbr);
        
        if(volume_id_nbr==0 || volume_id_nbr>9999)
        {
            fprintf(stderr,"Can not interpret -vid <volume_id_nbr> argument.\n");
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
            fprintf(stderr,"Can not interpret -dsv <data_set_version> argument.\n");
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
    
    sec_epoch=ConvertUtc2Timet_2(pds.SCResetClock);	// Compute seconds from epoch 1970 to S/C Clock reset. 
    


    // Get mission phase data
//     if(InitMissionPhaseStructFromMissionCalendar(&mp, &pds) < 0) {
    if(InitMissionPhaseStructFromMissionCalendar(&mp, pds.mcpath, pds.DPLNumber, pds.DataSetVersion) < 0) {
        exit(1);
    }



    /*================================================================================================
     * Get options for overriding mission phase parameters (and description string)
     *================================================================================================
     * Overwrite mission phase values if values can be found among (optional) command-line arguments.
     * NOTE: Current implementation requires 4 options and optionally another 3.
     * NOTE: These options have to be read AFTER reading and interpreting the mission calendar
     *       so that those values are available if they are not overwritten here.
     *================================================================================================*/
    char descr_str[MAX_STR];
    if (GetOption("-ds", argc, argv, descr_str))
    {
        if (GetOption("-mpn", argc, argv, tstr1)) {
            sprintf(mp.mission_phase_name, "\"%s\"", tstr1);    // NOTE: Surround with quotes since the archiving standard requires it.
        } else {
            fprintf(stderr, "Can not find option -mpn.\n");
            exit(1);
        }
        
        
        if (GetOption("-pb", argc, argv, tstr1)) {
            if((status=ConvertUtc2Timet(tstr1,&(mp.t_start))) < 0) {
                fprintf(stderr, "Can not convert argument \"%s\" to a time: error code %i\n", tstr1, status);
                exit(1);
            }
        } else {
            fprintf(stderr, "Can not find option -pb.\n");
            exit(1);
        }
        
        // Require exactly one of two options.
        if (GetOption("-pd", argc, argv, tstr1))
        {
            float dataset_duration_days;
            if (!sscanf(tstr1, "%e", &dataset_duration_days)) {
                fprintf(stderr, "Can not interpret argument \"%s\".\n", tstr1);    // NOTE: Periods shorter than one day are useful for debugging. Therefore permit decimal numbers.
                exit(1);
            }
            // Compute end time. NOTE: Does not take leap seconds (e.g. 2015-06-30, 23:59.60) that occurred in time interval
            // into account. However, these are time_t variables which might not use leap seconds anyway.
            mp.t_stop = mp.t_start + dataset_duration_days*24*3600;
        }
        else if (GetOption("-pe", argc, argv, tstr1))
        {
            if((status=ConvertUtc2Timet(tstr1,&(mp.t_stop))) < 0) {
                fprintf(stderr, "Can not convert argument \"%s\" to a time: error code %i\n", tstr1, status);
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Can not find option -pd or -pe (exactly one of them is required).\n");
            exit(1);
        }

        // ASSERTION
        if (mp.t_stop < mp.t_start ) {
            fprintf(stderr, "Dataset start time comes after dataset end time.\n");
            exit(1);
        }



        if (GetOption("-dsitn", argc, argv, tstr1)) {
            strncpy(mp.target_name_dsi, tstr1, 8);
            
            if (GetOption("-dsntn", argc, argv, tstr1)) {
                strncpy(mp.target_name_dsn, tstr1, 32);
            } else {
                fprintf(stderr, "Can not find option -dsntn.\n");
                exit(1);
            }
            
            if (GetOption("-tn", argc, argv, tstr1)) {
                sprintf(mp.target_name, "\"%s\"", tstr1);    // NOTE: Surround with quotes since the archiving standard requires it.
            } else {
                fprintf(stderr, "Can not find option -tn.\n");
                exit(1);
            }
            
            if (GetOption("-tt", argc, argv, tstr1)) {
                sprintf(mp.target_type, "\"%s\"", tstr1);    // NOTE: Surround with quotes since the archiving standard requires it.
            } else {
                fprintf(stderr, "Can not find option -tt.\n");
                exit(1);
            }
        }



        // Update DATA_SET_ID and DATA_SET_NAME since the following have or might have changed (above):
        //      DATA_SET_ID/DATA_SET_NAME description string.
        //      target in DATA_SET_ID,
        //      target in DATA_SET_NAME,
        //      TARGET_TYPE
        DeriveDSIandDSN(
            mp.data_set_id, mp.data_set_name,
            mp.target_name_dsi, pds.DPLNumber, mp.mission_phase_abbrev, descr_str, pds.DataSetVersion, mp.target_name_dsn);
        
//         // Would like to execute these commands here but SPICE is not initialized at this point.
//         ConvertTimet2Sccd_SPICE(mp.t_start, NULL, &(mp.sccd_start_data));
//         ConvertTimet2Sccd_SPICE(mp.t_stop,  NULL, &(mp.sccd_stop_data));
    }
    


    if (GetOption("-dpd", argc, argv, tstr1))
    {
        strncpy(pds.apathpds_parent, tstr1, PATH_MAX);
        fprintf(stdout, "Reading from CLI option: apathpds_parent\n");   // DEBUG
    }
        
        
        
    YPrintf("DATA_SET_ID                 : %s\n",mp.data_set_id);
    printf( "DATA_SET_ID                 : %s\n",mp.data_set_id);
    

    
    //========================================================================================================
    // Loads second part of configuration information into the PDS structure and opens some log/status files.
    // 
    // NOTE: LoadConfig2 creates data set directory!! Therefore needs sufficiently complete "pds" struct.
    //========================================================================================================
    strcpy(tstr1, mp.data_set_id);    // Make temporary copy.
    TrimQN(tstr1);                    // Remove quotes in temporary copy.
    if((status=LoadConfig2(&pds, tstr1))<0)
    {
        // NOTE: Misleading error message for error=-3 or -2.
        fprintf(stderr,"Mangled configuration file (part 2), or dataset directory already exists: %d\n",status); // Check arguments
        exit(1);
    }



    //===================================================
    // Configure SPICE, including loading the METAKERNEL
    //===================================================
    InitSpice(pds.pathmk);
    
    /* IMPLEMENTATION NOTE: Would like to execute these commands when initializing the "mp" struct (the t_start and t_stop fields)
     * (a) in InitMissionPhaseStructFromMissionCalendar and (b) when parsing the optional CLI arguments.
     * However:
     * (1) To initialize SPICE, pds.pathmk has to be initialized.
     * (2) pds.pathmk is initialized in LoadConfig2 which needs the DATA_SET_ID (for creating data set directory),
     * (3) DATA_SET_ID is partly initialized through either (a) InitMissionPhaseStructFromMissionCalendar or (b) CLI arguments.
     * Thus, ideally, to solve this neatly, LoadConfig2 should be broken up somehow.
     */
    ConvertTimet2Sccd_SPICE(mp.t_start, NULL, &(mp.sccd_start_data));
    ConvertTimet2Sccd_SPICE(mp.t_stop,  NULL, &(mp.sccd_stop_data));
    
    
    
    if(LoadTimeCorr(&pds,&tcp)<0)         // Load the time correlation packets, once and for all!
    {
        fprintf(stderr,"Warning: No time correlation packets found.\n"); 
        fprintf(stderr,"All UTC times from this point are estimates.\n");
    }


    
    if (RUN_TEST_CODE) {
        return main_TEST(argc, argv);   // INFORMAL TEST CODE. WILL (PROBABLY) EXIT PDS ON ITS OWN.
    }
    
    

    // Choose file for offset calibration exceptions.
    // NOTE: pds.templp   is loaded in LoadConfig1.
    // NOTE: pds.apathpds is loaded in LoadConfig2,.
    sprintf(pds.cpathocel, "%s/CALIB/%s", pds.apathpds, "RPCLAP_CALIB_MEAS_EXCEPT.LBL");
    sprintf(pds.cpathocet, "%s/CALIB/%s", pds.apathpds, "RPCLAP_CALIB_MEAS_EXCEPT.TAB");



    //===========================================
    // Open VOLDESC.CAT and change some keywords
    //===========================================
    sprintf(tstr1, "%sVOLDESC.CAT", pds.apathpds);             // Get full path
    status = ReadLabelFile(&tmp_lbl, tstr1);                   // Read catalog keywords into property value pair list
    
    sprintf(tstr2, "ROLAP_%04d", volume_id_nbr);
    SetP(&tmp_lbl, "VOLUME_ID", tstr2, 1);                     // Set VOLUME_ID

    // Create unquoted mission phase name
    strcpy(tstr3, mp.mission_phase_name);   // Make temporary copy
    TrimQN(tstr3);                          // Remove quotes in temporary copy
    
    // Construct and set VOLUME_NAME.
    if(calib) {
        sprintf(tstr2, "\"RPCLAP CALIBRATED DATA FOR %s\"", tstr3);
    } else {
        sprintf(tstr2, "\"RPCLAP EDITED RAW DATA FOR %s\"", tstr3);
    }    
    SetP(&tmp_lbl, "VOLUME_NAME", tstr2, 1); // Set VOLUME_NAME
    
    WriteUpdatedLabelFile(&tmp_lbl, tstr1, 1);                  // Write back label file with new info
    FreePrp(&tmp_lbl);                                          // Free property value list



    //===================
    // Get option -stctt
    //===================
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
        fprintf(stderr, "\nCould not interpret all command-line options, or some command-line options occurred multiple times,\nor the combination of command-line options is illegal.\n\n");
        exit(1);
    }



    //=======================================
    // Update DATASET.CAT with some keywords
    //=======================================
    //UpdateDATASET(&mp, &pds));    // Moved to ExitPDS.

    
    
    //================================================================================
    // Update LBL files copied from the template directory.
    //
    // NOTE: There are no *.CAT files under DOCUMENT/
    // NOTE: It is unnecessary to update CATALOG/ since the only file to modify is
    // DATASET.CAT, which is modified elsewhere (including some additional fields).
    // NOTE: Under the current way of composing deliverably data sets, the DOCUMENT/
    // LBL files are generated by a completely separate MATLAB code. This should
    // therefore be unnecessary, although also harmless.
    //================================================================================
    sprintf(tstr1,"%sDOCUMENT",pds.apathpds);     // Get full path to subdirectory.
    UpdateDirectoryODLFiles(tstr1,      "*.LBL", 0);    // DOCUMENT/ subdirectory
    UpdateDirectoryODLFiles(pds.cpathd, "*.LBL", 0);    // CALIB/    subdirectory



    AddPathsToSystemLog(&pds);   // Add paths to system log file
    
    
    
    // Write initial message to system log
    YPrintf("LAP PDS SYSTEM STARTED     \n");
    YPrintf("========================================================================\n");
    
    if(calib) {
        YPrintf("Generating calibrated PDS data archive\n");
    }
    
    //==================================================================================================================================
    // Read some calibration files.
    //---------------------------------------------------------------------------------------------------------------------------------
    // Calibration files are supposed to reside in the CALIB directory, both for EDITED and CALIBRATED archives
    // however for EDITED archives the files are not used.
    //
    // pds->cpathd   Path to calib data
    // pds->cpathf   Path to fine    bias calibration data
    // pds->cpathc   Path to coarse  bias calibration data
    // pds->cpathi   Path to current bias calibration data
    //==================================================================================================================================
    status = ReadLabelFile(&tmp_lbl, pds.cpathc);               // Read coarse bias voltage calibration label into property value pair list
    if (status>=0) {
        status += ReadTableFile(&tmp_lbl, &v_conv, pds.cpathd, "Reading coarse bias voltage calibration data");    // Read coarse bias voltage calibration data into v_conv structure
    }
    FreePrp(&tmp_lbl);                                          // Deallocate dynamic memory.
    //==================================================================================================================================
    status += ReadLabelFile(&tmp_lbl, pds.cpathf);              // Read fine bias voltage calibration label into property value pair list
    if (status>=0) {
        status += ReadTableFile(&tmp_lbl, &f_conv, pds.cpathd, "Reading fine bias voltage calibration data");    // Read fine bias voltage calibration data into f_conv structure
    }
    FreePrp(&tmp_lbl);                                          // Deallocate dynamic memory.
    //==================================================================================================================================
    status += ReadLabelFile(&tmp_lbl, pds.cpathi);              // Read current bias voltage calibration label into property value pair list
    if (status>=0) {
        status += ReadTableFile(&tmp_lbl, &i_conv, pds.cpathd, "Reading current bias calibration data");    // Read current bias calibration data into i_conv structure
    }
    FreePrp(&tmp_lbl);                                          // Deallocate dynamic memory.

    // Check if any of the (several) preceeding functions generated error.
    if(status<0)
    {
        YPrintf("Can not get or interpret the calibration files. Exiting.\n");
        ExitPDS(1);
    }

    

    //=======================================
    // Initialize CALIB_MEAS data structures
    //=======================================
    status = InitCalibMeas(pds.cpathd, pds.cpathm, pds.cpathocel, pds.cpathocet, &m_conv);        // Get measurement calibration files
    if(status<0)
    {
        YPrintf("Can not load offset calibration TM conversion files or the offset calibration exceptions files. Function error code %i. Exiting.\n", status);
        ExitPDS(1);
    }
    
    //========================================
    // Initialize CALIB_COEFF data structures
    //========================================
    status = InitCalibCoeff(pds.cpathd, mp.t_start, mp.t_stop, &calib_coeff_data);        // Get measurement calibration files
    if(status<0)
    {
        YPrintf("Can not load offset calibration TM conversion files or the offset calibration exceptions files. Function error code %i. Exiting.\n", status);
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
    else {
        YPrintf("Loaded %d macro descriptions\n",status);
    }
    
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
    
    
    // Set up a graceful exit if Ctrl-C is pressed! Exits at a convenient starting point! May take a long time!
    act.sa_handler=ExitWithGrace;
    act.sa_flags=0;
    if((sigemptyset(&act.sa_mask)==-1) || (sigaction(SIGINT,&act,NULL)==-1))
    {
        perror("Failed to set SIGINT handler");
        exit(1);
    }
    
    // For real time version of PDS, (not needed)
    /*
     * minp=sched_get_priority_min(SCHEDULING); // [max - min]>=32 guaranteed.
     * SetPRandSched(pthread_self(),minp+3,SCHEDULING); // Set priority and scheduling of main thread
     */

    // Load macro descriptions
    InitP(&mdesc); // Initialize linked property/value list for macro descriptions.    
    if (LoadModeDesc(&mdesc,pds.mpath) < 0)
    {
        YPrintf("Warning: Can not load macro descriptions. INSTRUMENT_MODE_DESC can not be set.\n");
        printf("Warning: Can not load macro descriptions. INSTRUMENT_MODE_DESC can not be set.\n");
    }
    
    
    
    //=======================
    // Starting data threads
    //=======================
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
    
    sarg.arg1 = &cbs; // Pass circular science buffer pointer as an argument.
    sarg.arg2 = &cmb; // Pass circular mirror  buffer pointer as an argument.
    
    //======================
    // DecodeScience thread
    //======================
    if(pthread_create(&scithread,NULL,DecodeScience,(void *)&sarg)!=0)
    { 
        YPrintf("Error starting science thread");   
        FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
        ExitPDS(1); 
    }
    
    // For real time version of PDS, (not needed)
    //SetPRandSched(scithread,minp+1,SCHEDULING); // Set priority and scheduling
    
    //=================
    // DecodeHK thread
    //=================
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
    // NOTE: Reuses the current thread.
    TraverseDDSArchive(&pds);
    

    // End this thread, while the other threads (most likely) continue.
    return 0;
}   // main



// executable_name : <String to be displayed as command name>.
void PrintUserHelpInfo(FILE *stream, char *executable_name) {
    fprintf(stream, "Usage: %s\n", executable_name);
    fprintf(stream, "           [-debug <Level>]                  Set debugging level.\n");
    fprintf(stream, "           [-h]                              Display help.\n");
    fprintf(stream, "           [-c <pds.conf path>]\n");
    fprintf(stream, "           [-a <pds.anomalies path>]\n");
    fprintf(stream, "           [-b <pds.bias path>]\n");
    fprintf(stream, "           [-e <pds.exclude path>]\n");
    fprintf(stream, "           [-m <pds.modes path>]\n");
    fprintf(stream, "           [-d <pds.dataexcludetimes path>]\n");
    fprintf(stream, "           [-calib]                          Produce CALIB dataset (EDITED is default).\n");
    fprintf(stream, "\n");
    fprintf(stream, "            -mp  <Mission phase abbrev.>     Mission phase abbreviation in DATA_SET_ID, e.g. ESC1, PRL.\n");
    fprintf(stream, "            -vid <Volume ID number>          The four-digit number xxxx in VOLUME_ID = ROLAP_xxxx (VOLDESC.CAT).\n");
    fprintf(stream, "            -dsv <Data set version>          Version number in PDS keywords DATA_SET_ID and DATA_SET_NAME.\n");
    fprintf(stream, "\n");
    fprintf(stream, "           [-stctt <seconds>]                Science thread cancel threeshold timeout (STCTT), i.e. the time the program\n");
    fprintf(stream, "                                             waits for the science thread to empty the science buffer to below a certain\n");
    fprintf(stream, "                                             threshold before exiting.\n");
    fprintf(stream, "                                             Default value: %i s.\n", SC_THREAD_CANCEL_THRESHOLD_TIMEOUT_DEFAULT);
    fprintf(stream, "\n");
    fprintf(stream, "           [-dpd <Dataset parent dir>]       Directory under which to put the new dataset\n");
    fprintf(stream, "                                             NOTE: This setting overrides the corresponding setting in pds.conf.\n");
    fprintf(stream, "\n");
    fprintf(stream, "   Override (1) values read from the mission calendar, and (2) default description string\n");
    fprintf(stream, "        [   -ds <Description string>         The free-form component of DATA_SET_ID and DATA_SET_NAME. E.g. EDITED, CALIB, MTP014.\n");

    // Values normally obtained from the mission calendar.
    // NOTE: Start and duration and MISSION_PHASE_NAME(!) are not necessarily those of an entire mission phase,
    // since deliveries may split up mission phases.
    fprintf(stream, "            -mpn <MISSION_PHASE_NAME>        PDS keyword MISSION_PHASE_NAME, e.g. \"COMET ESCORT 2\", \"COMET ESCORT 2 MTP014\".\n");
    fprintf(stream, "            -pb <Period begin time>          Specific day or day+time, e.g. \"2015-12-13\", or \"2015-12-17 12:34:56\".\n");
    fprintf(stream, "                                             (Characters between field values are not important, only their absolute positions in the string.)\n");
    fprintf(stream, "         (  -pe <Period end time>            Specific day or day+time, e.g. \"2015-12-13\", or \"2015-12-17 12:34:56\".\n");
    fprintf(stream, "                                             (Characters between field values are not important, only their absolute positions in the string.)\n");
    fprintf(stream, "          | -pd <Period duration>  )          Positive decimal number. Unit: days. E.g. \"28\", \"0.0416666\"\n");    
    fprintf(stream, "                                             Footnote: Should be given as if there are no leap seconds, i.e. 1 day = 86400 s always.\n");
    fprintf(stream, "          [ -dsitn <Target in DSI>           Target in PDS keyword DATA_SET_ID (DSI).\n");
    fprintf(stream, "            -dsntn <Target in DSN>           Target in PDS keyword DATA_SET_NAME (DSN).\n");
    fprintf(stream, "            -tn <TARGET_NAME>                PDS keyword TARGET_NAME (long target name).\n");
    fprintf(stream, "            -tt <TARGET_TYPE>      ]  ]\n");
    fprintf(stream, "\n");
    fprintf(stream, "       NOTE: Even if one specifies options for all eight values represented in the mission calendar, the mission calendar still\n");
    fprintf(stream, "             needs to contain the mission phase abbreviation used.\n");
    fprintf(stream, "       NOTE: Options must not contain quotes, including PDS keywords which are quoted in LBL/CAT files.\n");
    fprintf(stream, "\n");
    //fprintf(stream, "NOTE: The caller should NOT submit parameter values surrounded by quotes (more than what is required by the command shell.\n");
}



/* Function for initializing SPICE.
 * 
 * It is useful to have a separate function that does this,
 * (1) to structure the source code (split it into separate, isolated modules), and
 * (2) to make it easy for test code initialize SPICE the same way.
 * 
 * NOTE: Handles errors itself.
 * NOTE: This function is THREAD-SAFE and must NOT be called using the SPICE mutex.
 */
void InitSpice(char *metakernel_path) {
    char tstr1[MAX_STR];    // Temporary string
    char tstr2[MAX_STR];    // Temporary string
    char initial_working_dir[MAX_STR];
    
    pthread_mutex_lock(&protect_spice);
    
    // Configure SPICE's error behaviour
    // ---------------------------------
    // Tell SPICE that on error, it should
    // (1) not abort the executable (exit pds; from the SPICE function itself), and
    // (2) print the error message (depends on other SPICE settings)
    // Therefore, for every call to a SPICE function after this command, the caller should
    // (1) manually check if an error has occurred,
    // (2) manually retrieve and print/log the error message, and
    // (3) manually exit pds.
    erract_c("SET", MAX_STR, "REPORT");
    
    // Configure SPICE to not print any error messages by itself.
    // (pds uses failed_c, getmsg_c via CheckSpiceError instead.)
    errprt_c("SET", -1, "NONE");
    
    // Disable "tracing" inside SPICE functions.
    // This should speed up execution, but potentially make debugging harder.
    // 
    // Example 2017-07-10: birra, small ddsData/ directory, time pds -mp AST1 -vid 9999 -dsv 0.1  (Note: No -stctt flag.)
    //  Without trcoff_c():
    //     real    7m4.034s
    //     user    13m17.508s
    //     sys     13m31.694s
    //  With    trcoff_c():
    //     real    5m58.744s
    //     user    11m16.831s
    //     sys     11m20.230s
    trcoff_c();
    
    // Change working directory
    // ------------------------
    // The metakernel file may contain relative paths. Must therefore, at least
    // temporarily, change current working directory to the directory of the metakernel file.
    strcpy(tstr1, metakernel_path);
    strcpy(tstr2, dirname(tstr1));   // NOTE: dirname might modify the second argument(!). ==> Submit copy.
    getcwd(initial_working_dir, MAX_STR);
    if (chdir(tstr2)) {
        printf( "Failed to change current directory to \"%s\".\n", tstr2);
        YPrintf("Failed to change current directory to \"%s\".\n", tstr2);
        
        pthread_mutex_unlock(&protect_spice);
        ExitPDS(1);
        pthread_mutex_lock(&protect_spice);     // This statement should never be reached. It is just there for safety.
    }

    // LOAD SPICE METAKERNEL
    printf( "Loading SPICE metakernel: %s\n", metakernel_path);
    YPrintf("Loading SPICE metakernel: %s\n", metakernel_path);
    strcpy(tstr1, metakernel_path);
    furnsh_c(basename(tstr1));        // NOTE: Function "basename" might modify the argument, therefore submit a copy of the string.
    CheckSpiceError("Could not load SPICE metakernel.", TRUE, TRUE);
    
    
    // Change back to original working directory.
    if (chdir(initial_working_dir)) {
        printf( "Failed to change current directory to \"%s\".\n", initial_working_dir);
        YPrintf("Failed to change current directory to \"%s\".\n", initial_working_dir);
        
        // Exiting pds is not really necessary. pds does seem to work fine even if the working directory is not changed back.
        // /Erik P G Johansson 2017-07-10
        pthread_mutex_unlock(&protect_spice);
        ExitPDS(1);
        pthread_mutex_lock(&protect_spice);     // This statement should never be reached. It is just there for safety.
    }
    
    pthread_mutex_unlock(&protect_spice);
}



/* Function that can/should be called after every call to a SPICE function to handle SPICE errors, if any has occurred.
 * 
 * caller_error_msg : Manually composed error message. Intended as a one-line message.
 *                    This should not to be confused with error messages produced by SPICE itself and which
 *                    this function (might) in addition obtains and prints & logs.
 * exit_pds         : TRUE/FALSE for whether the function should exit pds itself when detecting SPICE error.
 * print_to_stdout  : TRUE/FALSE for whether additionally to print to stdout.
 * Return value     :  0=No SPICE error.
 *                    -1=SPICE error (and implicitly, the function does not exit PDS by itself).
 * 
 * IMPLEMENTATION NOTE: This function by itself is NOT TREAD-SAFE (uses no mutex despite calling not-threadsafe SPICE functions).
 * The caller is responsible for using the SPICE mutex, since it is meant to be called by functions which use SPICE functions
 * and therefore the SPICE mutex anyway. (And the SPICE mutex is not recursive.)
 */
int CheckSpiceError(char *caller_error_msg, int exit_pds, int print_to_stdout)
{
    // PROPOSAL: "Inline" the function.
    // PROPOSAL: Add flag for printing error messages to stdout.
    if (failed_c()) {
        char SPICE_error_msg[MAX_STR];

//         getmsg_c("SHORT", MAX_STR, SPICE_error_msg);
        getmsg_c("LONG", MAX_STR, SPICE_error_msg);
//         getmsg_c("EXPLAIN", MAX_STR, SPICE_error_msg);

        if (print_to_stdout) {
            printf("%s\n", caller_error_msg);
            printf("   SPICE-generated error message: %s\n", SPICE_error_msg);            
        }
        YPrintf("%s\n", caller_error_msg);
        YPrintf("   SPICE-generated error message: %s\n", SPICE_error_msg);

        if (exit_pds) {
            ExitPDS(1);                                          // EXIT PDS
            return -1;    // Logically unnecessary. Command only here to avoid compiler warning message.
        } else {
            reset_c();    // Reset SPICE error.
            return -1;
        }
    } else {
        return 0;
    }
}



// Thread functions
//----------------------------------------------------------------------------------------------------------------------------------



// Decodes S/C TM (RPC TM) thread
//
// cs : arg_type*
//    .arg1 : buffer_struct_type* = Circular buffer
//
// Reads all TM, both SCI and HK packets, from all instruments.
//
void *SCDecodeTM(void *arg)
{
    char utc[64];
    unsigned int N_bytes;           // Length of data
    unsigned char packet_id;       // S/C packet ID, low byte
    double sccd;                   // Raw time
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
                    
                    N_bytes=((buff[2]<<8) | buff[3])-9;           // Get data length
                    PPrintf("LAP HK packet found\n");
                    PPrintf("    Packet ID: 0x0d%02x , Data length: %d\n",packet_id,N_bytes);
                    if(N_bytes!=HK_LENGTH)
                    {
                        PPrintf("    Weird HK length discarding!\n");
                        continue;
                    }
                    sccd = DecodeSCTime2Sccd(&buff[4]);    // Decode S/C time into raw time.
                    ConvertSccd2Utc(sccd, utc, NULL);      // Decode raw time to PDS compliant date format.
                    PPrintf("    SCET Time: %s OBT Raw Time: %014.3f\n",utc,sccd);
                    
                    In(&cbh,HBYTE);         // Keep packet ID high byte
                    In(&cbh,HK);            // Keep packet ID low  byte
                    InB(&cbh,buff,14);      // Store full HK S/C header, because we want to keep the time code
                    
                    GetBuffer(cs,buff,N_bytes); // Get length bytes from circular buffer 
                    InB(&cbh,buff,N_bytes);     // Store data in cicular HK input buffer
                    
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
                    
                    N_bytes=((buff[2]<<8) | buff[3])-9;           // Get data length
                    PPrintf("LAP Science packet found\n");
                    
                    if(N_bytes<0) {
                        continue;
                    }
                    
                    sccd = DecodeSCTime2Sccd(&buff[4]);    // Decode S/C time into raw time.
                    ConvertSccd2Utc(sccd, utc, NULL);      // Decode raw time to PDS compliant date format.
                    PPrintf("    Packet ID: 0x0d%02x , Data length: %d\n",packet_id,N_bytes);
                    PPrintf("    SCET Time: %s OBT Raw Time: %014.3f\n",utc,sccd);
                    GetBuffer(cs,buff,N_bytes); 
                    
                    
                    InB(&cbs,buff,N_bytes); // Store data in circular science input buffer
                    InB(&cmb,buff,N_bytes); // Store data in circular mirror buffer
                    
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
// arg : arg_type*; .arg1 = HK circular buffer.
//
// 
// IMPORTANT NOTE: It appears that HK TAB/LBL file pairs group together up to HK_NUM_LINES subsequent HK packets.
// Exactly which HK packets that are grouped together with which depend depends on when the previous group
// of HK packets ended, and hence it depends on the exact start time of the whole data set. Data
// for the same day from two different data sets with different start times will thus (likely) have different
// HK files with different groups of HK packets.
// /Erik P G Johansson 2016-03-21
void *DecodeHK(void *arg)
{   
    // PROPOSAL: Rewrite function to iterate over HK packets/rows (instead of over files, and then rows-in-file).

    char alphanum_h[4]="000";           // Default starting alpha numeric value for unique HK file name
    unsigned char buff[16];             // Temporary in buffer
    
    char lbl_fname[MAX_STR];            // Data HK label file name and path (complies to 27.3 file name standard)
    char stub_fname[MAX_STR];           // Stub file name and path
    char tab_fname[MAX_STR];            // Data HK table file name and path (complies to 27.3 file name standard)
    
    unsigned int macro_id;
    char prod_creat_time[MAX_STR];      // Product Creation time
    char line[MAX_STR];                 // HK line in PDS file    
    
    char tstr1[MAX_STR];                // Temporary string
    char tstr2[MAX_STR];                // Temporary string
    char tstr3[MAX_STR];                // Temporary string
    char utc_time_str_next[MAX_STR];    // UTC time string for next HK packet/row in HK TAB file(s).
    
    property_type *property1;           // Temporary property pointer
    
    double sccd;                        // Raw time
    
    unsigned ti;                        // Temporary integer
    buffer_struct_type *ch;             // Pointer to circular buffer structure type, for HK decoding
    
    int oldstate;
    int oldtype;
    int status;
    
    // Set thread to only be able to be canceled at cancellation points points.
    // NOTE: pthread_testcancel() is a cancellation point, but certain other system functions may also be, e.g. fwrite, fclose, fflush, fopen, mkdir.
    // One can thus not be certain where/when the function is ended. hk_info is used by ExitPDS to finish the HK LBL file and therefore needs to be accurate
    // when the function is exited. It is therefore updated carefully to be most likely to be consistent with what has been written to the HK TAB file
    // at that time.
    // 
    // http://man7.org/linux/man-pages/man7/pthreads.7.html
    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate);
    status+= pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&oldtype);
    
    if(status>0) {
        HPrintf("DecodeHK thread might not work properly.");
    }
    
    
    
    InitP(&hkl);                            // Initialize linked property/value list for PDS LAP HK
    ClearCommonPDS(&hkl);                   // Set the common PDS header keywords 
    SetupHK(&hkl);                          // Setup HK keywords
    SetP(&hkl,"RECORD_BYTES",       HK_LINE_SIZE_STR,1);        // Set number of bytes in a column of a record
    SetP(&hkl,"DESCRIPTION",        "\"LAP HK Data, Each line is a separate HK packet sent every 32s\"",1);
    SetP(&hkl,"MISSION_PHASE_NAME", mp.mission_phase_name,1);   // Set mission phase name in HK parameters
    SetP(&hkl,"TARGET_TYPE",        mp.target_type,1);          // Set target type in  HK parameters
    SetP(&hkl,"TARGET_NAME",        mp.target_name,1);          // Set target name in  HK parameters
    SetP(&hkl,"DATA_SET_ID",        mp.data_set_id,1);          // Set DATA SET ID in HK parameters
    SetP(&hkl,"DATA_SET_NAME",      mp.data_set_name,1);        // Set DATA SET NAME in HK parameters
    
    if(calib) {// Set product type
        SetP(&hkl,"PRODUCT_TYPE","\"RDR\"",1);
    } else {
        SetP(&hkl,"PRODUCT_TYPE","\"EDR\"",1);
    }
    
    ch=(buffer_struct_type *)((arg_type *)arg)->arg1;   // Get circular house keeping buffer pointer
    

    
    pthread_testcancel();                             // Test if we are to cancel

    //==============================================
    // Derive first row of first HK TAB file + time
    //==============================================
    GetHKPacket(ch,buff,&sccd);    // Get first HK packet data and raw time.
    AssembleHKLine(buff, line, sccd, utc_time_str_next, &macro_id);    // Assemble the very first HK line in a TAB file. Sets hk_info.utc_time_str.



    //===========================================================================================================
    // Loop over HK LBL & TAB file pairs
    // ---------------------------------
    // PRECONDITION at beginning of every loop:
    //  - One HK packet/row + time has been read but written to any HK TAB file.
    //  - Variables line, sccd, utc_time_str_next have been correctly set.
    //===========================================================================================================
    while(1)
    {
        // Set hk_info to match the most recently read HK packet/row (which is meant to eventually be written to HK TAB file).
        // IMPLEMENTATION NOTE: It is (marginally) useful to set "hk_info.hk_cnt" here in case the function is cancelled and ExitPDS needs
        // hk_info to finish writing the HK LBL file (0 ==> ROWS=0).
        strcpy(hk_info.utc_time_str, utc_time_str_next);
        ConvertSccd2Sccs(sccd, pds.SCResetCounter, hk_info.obt_time_str, TRUE);    // Compile OBT string and add reset number of S/C clock
        hk_info.hk_cnt = 0;

        HPrintf("S/C time PDS Format: %s, Raw Time: %014.3f\n", hk_info.utc_time_str, sccd);
        HPrintf("Mission ID: %s, Phase: %s\n", mp.mission_phase_abbrev, mp.mission_phase_name);

        //===============================================================
        // Prepare for writing LBL & TAB file
        // ----------------------------------
        // Make sure TAB & LBL parent directory exist, create filenames.
        //===============================================================
        // Create data path for current day
        StrucDir(hk_info.utc_time_str, pds.dpathh, pds.spathh);   // Test if RPCLAPCCYY/MONTH exists for current time, if not it creates them.        
        strncpy(tstr2, hk_info.utc_time_str, 20);   // Truncate raw time and store fractions of a seconds
        // Replace - in CCYY-MM-DD by null terminations so we can convert date from CCYY-MM-DD into YYMMDD
        tstr2[4]='\0'; 
        tstr2[7]='\0';
        tstr2[10]='\0';
        
        // Get highest alphanumeric value in filenames in dpathh matching pattern "RPCLAPYYMM*_*_H.LBL",
        // This causes the alphanum value to restart at zero every new day.
        // (Any matching days from previous runs are not overwritten until alphanum wraps.)
        // NOTE: Does not make use of the day-of-month string. Not sure why.
        sprintf(tstr3,"RPCLAP%s%s*_*_*H.LBL",&tstr2[2],&tstr2[5]);
        GetAlphaNum(alphanum_h,pds.spathh,tstr3);
        IncAlphaNum(alphanum_h); // Increment alphanumeric value (HK)
        
        sprintf(stub_fname,"RPCLAP%s%s%s_%s_H",&tstr2[2],&tstr2[5],&tstr2[8],alphanum_h);       // Set filename without suffix.
        
        sprintf(tab_fname,"%s.TAB",stub_fname);                     // Compile HK data TAB path+filename
        sprintf(lbl_fname,"%s.LBL",stub_fname);                     // Compile HK data LBL path+filename

        
        
        //========================
        // Add to LBL data struct
        //========================
        sprintf(tstr2, "\"%s\"", pds.LabelRevNote);             // Assemble label revison note    // Modified 2015-04-10 /Erik P G Johansson
        SetP(&hkl,"LABEL_REVISION_NOTE",tstr2,1);               // Set LABEL Revision note        // Removed 2015-02-27 /Erik P G Johansson

        SetP(&hkl, "SPACECRAFT_CLOCK_START_COUNT", hk_info.obt_time_str, 1);    // Set OBT start time
        SetP(&hkl, "START_TIME",                   hk_info.utc_time_str, 1);    // Update START_TIME in common PDS parameters
        
        sprintf(tstr1, "MCID0X%04x", macro_id);
        SetP(&hkl, "INSTRUMENT_MODE_ID", tstr1, 1);
        
        // Find human description of macro in macro mode descriptions
        sprintf(tstr1, "0x%04x", macro_id);                     // Create search variable to search for in linked list of property name value pairs.
        if(FindP(&mdesc, &property1, tstr1, 1, DNTCARE)>0)
        {
            SetP(&hkl, "INSTRUMENT_MODE_DESC", property1->value, 1);   // Set human description of mode
        }
        
        SetP(&hkl,"PRODUCT_ID",stub_fname,1);                       // Add PRODUCT_ID HK LBL
        
        GetCurrentUtc0(prod_creat_time);                                  // Get current UTC time     
        HPrintf("    UTC Creation Time: %s\n",prod_creat_time,1);
        SetP(&hkl,"PRODUCT_CREATION_TIME",    prod_creat_time,1);       // Set creation time in common PDS parameters, no quotes!
        
        sprintf(tstr3,"\"%s\"",lbl_fname);          // Add PDS quotes ".."
        SetP(&hkl,"FILE_NAME",tstr3,1);             // Add file name to HK LBL
        sprintf(tstr3,"\"%s\"",tab_fname);          // Add PDS quotes ".." 
        SetP(&hkl,"^TABLE",tstr3,1);                // Add pointer to HK TAB file
        
        
        
        //===============================================================================
        // Write to the index file
        // (We have at least one HK line otherwise GetHKPacket would not have returned.)
        //===============================================================================
        ti=strlen(pds.apathpds);                            // Find position where the root of the PDS archive starts
        sprintf(tstr2,"%s%s",&pds.spathh[ti],lbl_fname);    // Set path and file name together
        ExtendStr(tstr3,tstr2,58,' ');                      // Make a new string extended with whitespace to 58 characters
        ExtendStr(tstr2,stub_fname,25,' ');                 // Make a new string extended with whitespace to 25 characters
        WriteToIndexTAB(tstr3, tstr2, prod_creat_time);
        
        // Construct LBL file path.
        strcpy(tstr2,pds.spathh);                       // Copy data path
        strcat(tstr2,lbl_fname);                        // Add lbl filename to data path 
        
        
        
        //===============================================================================================
        // (1) Write all rows of TAB file (one row has already been generated).
        // (2) Gather remaining information for LBL file.
        // (3) Write LBL file.
        //===============================================================================================
        if ((pds.hlabel_fd=fopen(tstr2,"w"))==NULL)     // Open HK LBL file for writing (overwrite any pre-existing one).
        {
            HPrintf("Couldn't open PDS HK LBL data file: %s!!\n",tstr2);
        }
        else
        {
            // Construct TAB file path.
            strcpy(tstr2,pds.spathh);               // Copy data path
            strcat(tstr2,tab_fname);                // Add tab filename to data path 
            
            if((pds.htable_fd=fopen(tstr2,"w"))==NULL)          // Open HK TAB file (overwrite any pre-existing one).
            {
                HPrintf("Couldn't open PDS HK TAB data file: %s!!\n",tab_fname);
            }
            else
            {
                // CASE: Successfully opened both LBL and TAB file. First TAB line (variable "line") has already been assembled.
                
                //===========================================================================================================
                // Derive HK TAB rows + time, and write them to TAB file
                // -----------------------------------------------------
                // PRECONDITION & POSTCONDITION for while loop:
                //  - One HK TAB file row string has already been derived but not been written to any HK TAB file.
                //  - Variables line, sccd, utc_time_str_next have been correctly set.
                //===========================================================================================================
                while(1)
                {
                    HPrintf("%s", line);                                    // Print HK TAB file content to log(!)
                    fwrite(line,HK_LINE_SIZE,1,pds.htable_fd);              // Write line to HK TAB file.
                    fflush(pds.htable_fd);

                    // Set hk_info to match the most recently written HK packet/row.
                    // IMPLEMENTATION NOTE: It is useful to set hk_info here so that it corresponds to the last HK row actually written so that,
                    // in case the function is abrutly interrupted by ExitPDS, then ExitPDS can use it for finishing the LBL file.
                    // Consider that the function could be interrupted/cancelled in the call to GetHKPacket by ExitPDS.
                    strcpy(hk_info.utc_time_str, utc_time_str_next);
                    ConvertSccd2Sccs(sccd, pds.SCResetCounter, hk_info.obt_time_str, TRUE);    // Compile OBT string and add reset number of S/C clock.
                    hk_info.hk_cnt++;   // NOTE: hk_info.hk_cnt represents the number of HK packets/rows already written to TAB file; NOT the number of HK packets read.
                    HPrintf("S/C time PDS Format: %s Raw Time: %014.3f\n", hk_info.utc_time_str, sccd);

                    //==============================================
                    // Derive next row to put in a TAB file + time
                    // (not necessarily in the current HK TAB file)
                    //==============================================
                    GetHKPacket(ch,buff,&sccd);
                    AssembleHKLine(buff, line, sccd, utc_time_str_next, &macro_id);    // Sets hk_info.utc_time_str. Return value "macro_id" not used.



                    // IMPLEMENTATION NOTE: Check if new HK data UTC string represents a different day compared to the previous one ==> strncmp(..., 10).
                    if ((hk_info.hk_cnt > HK_NUM_LINES) || (strncmp(utc_time_str_next, hk_info.utc_time_str, 10))) {                        
                        // CASE: The HK row that was constructed, should NOT be written to the currently open HK TAB file. Save it for next HK TAB file.
                        break;
                    }
                    // CASE: The HK row that was constructed, should/will also be written to the TAB file that is currently open.
                }
                
                fclose(pds.htable_fd);    // Close HK TAB file.
            }

            // IMPLEMENTATION NOTE: The function "ExitPDS" will execute the following commands to complete the HK LBL file too,
            // if it thinks the file has not been closed. Note that this requires "hk_info" to have the correct values.
            
            //===============================
            // Add to the HK LBL data struct
            //===============================
            SetP(&hkl, "SPACECRAFT_CLOCK_STOP_COUNT", hk_info.obt_time_str, 1);     // Set OBT stop time
            SetP(&hkl, "STOP_TIME",                   hk_info.utc_time_str, 1);     // Update STOP_TIME in common PDS parameters
            sprintf(tstr1,"%d",hk_info.hk_cnt);
            SetP(&hkl, "FILE_RECORDS", tstr1, 1);               // Set number of records
            SetP(&hkl, "ROWS",         tstr1, 1);               // Set number of rows
            
            //====================================
            // Write to and close the HK LBL file
            //====================================
            FDumpPrp(&hkl, pds.hlabel_fd);   // Dump hkl to HK label file
            fprintf(pds.hlabel_fd,"END\r\n");
            fclose(pds.hlabel_fd);
            pds.hlabel_fd=NULL;
            
            HPrintf("(DecodeHK:) Closed HK LBL file.");   // Useful for determining whether the above code closed the LBL file, or if ExitPDS did.
        }   // if ((pds.hlabel_fd=fopen(tstr2,"w"))==NULL) ... else ...   (If can open HK LBL+TAB file...)
        
        pthread_testcancel();                             // Test if we are to cancel
    }    // while(1)
}   // DecodeHK







// -=THREAD TO DECODE SCIENCE STREAM=-
//
// Only handles SCI packets.
// Programmed in state machine style.
//
// arg : arg_type*
//     .arg1 : buffer_struct_type *cb;         // Pointer to circular buffer structure type, for science decoding
//     .arg2 : buffer_struct_type *ct;         // Pointer to circular buffer structure type, for temporary science storage
//
void *DecodeScience(void *arg)
{ 
    char alphanum_s[4]="000";       // Default starting alpha numeric value for unique sience data file name and product ID
    
    unsigned int state;             // Current state of state machine
    unsigned char buff[RIDICULUS];  // Temporary in buffer using a ridiculously large size. Thus whole buffer shall never be needed.
    unsigned char tbuff[8];         // Temporary buffer
    unsigned int main_h_sum=0;      // Main header sum
    unsigned int byte_sum=0;        // Byte sum scanning for header
    
    int in_sync;                    // Indicates if we are in sync or not
    int id_code=0;                  // Temporary ID code variable
    int params=0;                   // Parameter flag, indicates if parameters exists in science stream
    int param_type=0;               // Indicate type of parameters. Only set under S04_GET_ID_CODE.
    unsigned int N_bytes=0;         // Length of science data,
    unsigned int hb=0;              // high
    unsigned int lb=0;              // and low byte.
    unsigned int N_tmsmp = 0;       // Number of samples in science data (Not same as length!)
    int macro_descr_NOT_found=0;    // Indicates that we have NOT found a matching macro description for the macro ID. 0==Found, 1==Not found (!).
    
    unsigned int macro_id = 0;      // Macro ID tag.
    
    // Measurement sequence.
    // Specifies one of the LAP_SET_SUBHEADER/LAP_TRANSFER_DATA_TO_OUT_FROM in macro description (.mds file).
    // Value is only changed in case S01_GET_MAINH (set to zero) and case S04_GET_ID_CODE (incremented by one)
    unsigned int meas_seq = 0;
    
    int aqps_seq = 0;               // Number of aqps to the start of a sequence
    unsigned int ma=0;              // Macro number in block
    unsigned int mb=0;              // Macro block number
    int ADC16_P1_insmp_per_tmsmp;   // Downsampling rate ADC16 P1. Number of "internal samples" per "TM sample".
    int ADC16_P2_insmp_per_tmsmp;   // Downsampling rate ADC16 P2. Number of "internal samples" per "TM sample".
    int N_plateau_tmsmp=0;          // Samples on a plateau
    int N_non_tsweep_tmsmp=0;       // Number of initial plateau samples in a sweep, not the true number due to a well known bug.
    // that we compensate for in this code.
    
    // Structure with current settings for various parameters
//     curr_type curr={0,0,0,0,0,0,0,0,0,0x7f,0x7f,0};
    curr_type curr={0,0,0,0,0, 0,0,0,0,0, 0, 0x7f, 0x7f, 0};    // Correct? Does not set all curr_type fields and 0x7f assignments do not make sense. Important?
    
    int          finger_printing=0; // Are we doing fingerprinting ?
    sweep_type   sw_info;           // Sweep info structure steps, step height, duration, ...
    adc20_type   a20_info;          // ADC 20 info structure resampling, moving average, length, ...
    
    unsigned int W0;                // Temporary word 0
    unsigned int W1;                // Temporary word 1
    unsigned int W2;                // Temporary word 2
    
    char tstr1[256];                // Temporary string
    char tstr2[256];                // Temporary string
    char tstr3[256];                // Temporary string 
    char tstr5[256];                // Temporary string 
    
    int i;                          // Temporary counters
    int val,ti1;                    // Temporary integers
    
    char *tp;                       // Temporary string pointer
    char utc[30];                   // S/C time string in PDS format
    char tm_rate='Z';               // Telemetry rate (Z)ero,(M)inimum,(N)ormal and (B)urst
    double sccd=0.0;                // Raw S/C time in seconds
    
    // Status string for 20 Bit ADC:s
    char a20status[16][10]={"EMPTY","P2T","P1T","P1T & P2T","","P2F","","P1T P2F","","","P1F","P1F P2T","","","","P1F & P2F"};
    
    unsigned int **bias=NULL;       // Extra bias settings
    unsigned int **mode=NULL;       // Mode changes from command files
    
    unsigned int *exclude=NULL;     // List of macros that should be present in EDITED but not CALIB.
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



    //#############################################################################################################################
    /* Function to remove repetition and shorten the code that writes TAB/LBL file pairs.
     * It represents the writing of one LBL/TAB file pair for one probe (P1,P2,P3).
     * 
     * IMPORTANT NOTE: The function uses MANY variables defined in the enclosing outer function
     * (DecodeScience), to avoid a very long and awkward argument list, but none of these are temporary variables.
     * 
     * NOTE: Function will modify lbl_fname, tab_fname, and prod_id.
     * lbl_fname, tab_fname: Sets byte 19 (E=E-field/D=Density) and 21 (probe number) 
     * prod_id:              Same as for lbl_fname, tab_fname but for byte 20 and 22, presumably because the first byte/character is a quote.
     *
     * 
     * ARGUMENTS
     * =========
     * dop       : Defined in analogy with in "WritePTAB_File".
     * probe_nbr : 1, 2, or 3
     */
    void WriteTABLBL_FilePair(int dop, unsigned int probe_nbr) {
        // PROPOSAL: Re-write as true function, not using variables defined outside of function, and with all input data as parameters.

        char tempChar;
        char tstr10[MAX_STR];
        char indexStr[MAX_STR];
        
        // Modify filenames and product ID.
        if(GetBiasMode(&curr, dop)==E_FIELD) {
            tempChar = 'E';
        } else {
            tempChar = 'D';
        }
        lbl_fname[19]=tempChar;
        tab_fname[19]=tempChar;
        prod_id[19+1]=tempChar;
        
        sprintf(tstr10, "%1d", probe_nbr);
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
        
        int shouldExcludeFilePair = 0;   // Boolean flag.
        if (DecideWhetherToExcludeData(dataExcludeTimes, curr, param_type, &sw_info, macro_id, dop, &shouldExcludeFilePair)) {
            YSPrintf("ERROR: WriteTABLBL_FilePair: When trying to determine whether to exclude data.\n");
            ExitPDS(1);
        }
        if (shouldExcludeFilePair) {
            // Really superfluous printout since DecideWhetherToExcludeData also prints.
            CPrintf("WriteTABLBL_FilePair: Excluding data - Not writing TAB & LBL file pair.\n");
            return;
        }
        
        // CASE: The TAB+LBL file pair will be written to disk.

        //==============================================
        // Update mp.sccd_start_data, mp.sccd_stop_data
        //==============================================
        property_type *property_start_tmp, *property_stop_tmp;
        double        sccd_start_file,          sccd_stop_file;
        FindP(&comm, &property_start_tmp,"SPACECRAFT_CLOCK_START_COUNT", 1, DNTCARE);
        FindP(&comm, &property_stop_tmp, "SPACECRAFT_CLOCK_STOP_COUNT",  1, DNTCARE);
        ConvertSccs2Sccd(property_start_tmp->value, NULL, &sccd_start_file);
        ConvertSccs2Sccd(property_stop_tmp->value,  NULL, &sccd_stop_file );
        mp.sccd_start_data = fmin(mp.sccd_start_data, sccd_start_file);
        mp.sccd_stop_data  = fmax(mp.sccd_stop_data,  sccd_stop_file);
        
        //====================
        // Write LBL+TAB file
        //====================
        if(WritePLBL_File(pds.spaths,lbl_fname,&curr,N_tmsmp,id_code, dop, N_non_tsweep_tmsmp,param_type)>=0)
        {
            WritePTAB_File(
                buff, tab_fname, data_type, N_tmsmp, id_code, N_bytes, &sw_info, &a20_info, &curr, param_type, ADC16_P1_insmp_per_tmsmp, ADC16_P2_insmp_per_tmsmp, dop,
                &m_conv, bias, nbias, mode, nmode, N_non_tsweep_tmsmp, N_plateau_tmsmp);
            
            strncpy(tstr10,lbl_fname,29);
            tstr10[25]='\0';   // Remove the file type ".LBL".
            
            WriteToIndexTAB(indexStr, tstr10, property2->value);
        } else {
            YSPrintf("Failed to write LBL file.\n");
            ExitPDS(1);
        }
    }   // WriteTABLBL_FilePair
    //#############################################################################################################################



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
    
    
    // Load anomalies from anomaly file
    if(LoadAnomalies(&anom,pds.apath)<0)
    {
        YPrintf("Warning: anomaly correction can not be done\n");
        printf( "Warning: anomaly correction can not be done\n");
    }
    
    // Load bias settings from bias file
    if((LoadBias(&bias,&mode,&nbias,&nmode,pds.bpath))<0) 
    {
        YPrintf("Warning: Extra bias settings can not be done\n");
        printf( "Warning: Extra bias settings can not be done\n");
    }
    
    if(nbias>0 && bias!=NULL && debug>1) {
        for(i=0;i<nbias;i++) {
            printf("Time %d DBIAS %x EBIAS %x\n",bias[i][0],bias[i][1],bias[i][2]);
        }
    }
    
    if(nmode>0 && mode!=NULL && debug>1) {
        for(i=0;i<nmode;i++) {
            printf("Time %d Mode 0x%02x\n",mode[i][0],mode[i][1]);
        }
    }
    
    // Load Exclude file
    if(calib) {
        if((nexcl=LoadExclude(&exclude, pds.epath))<0) {
            YSPrintf("ERROR: Calibration macros will not be excluded\n");
            ExitPDS(1);
        }
    }
    
    // Erik P G Johansson 2015-03-25: Added reading data exclude list file.
    if((LoadDataExcludeTimes(&dataExcludeTimes, pds.depath)))
    {
        YSPrintf("ERROR: Can not load data exclude times.\n");
        ExitPDS(1);
    }



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
                    if(in_sync) {            // Are we nicely in sync?   NOTE: This is the only place where in_sync is read.
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
                // NOTE: State can only be arrived at from: Launch of state machine, (and) S01_GET_MAINH.
                DispState(state,"STATE = S02_TEST_SYNC\n");
                ClearCommonPDS(&comm);         // Clear common PDS parameters
                ClearDictPDS(&dict);           // Clear dictionary PDS LAP parameters
                UnCheckAll(&macros[mb][ma]);   // Uncheck all in current macro description 
                
                if(main_h_sum==M_HEAD) {       // Test if we are in sync
                    state=S03_GET_TIME_CODE;   // In sync pds has been written go get time code
                } else {
                    in_sync=0;                 // Indicate not nicely in sync
                    state=S01_GET_MAINH;       // Not in sync! try to get in sync!
                }
                break;
                
            case S03_GET_TIME_CODE:
                // NOTE: State can only be arrived at from S02_TEST_SYNC.
                DispState(state,"STATE = S03_GET_TIME_CODE\n");
                
                GetBuffer(cb,buff,5);        // Get 5 bytes from circular science buffer
                
                CPrintf("============================================================================\n");
                sccd = DecodeLAPTime2Sccd(buff);    // Get raw time/SCCD, decode LAP S/C time in Science data.
                if (ConvertSccd2Utc(sccd, utc, NULL) == -1) {      // Decode raw time/SCCD into PDS compliant UTC time.
                    // CASE: ConvertSccd2Utc/SPICE could not interpret SCCD, (most likely) due to value not existing
                    // in the stated partition count/reset count. This indicates an illegal SCCD value
                    // (unreasonably large/small), which likely indicates the state machine being out of sync or
                    // bit error. Since this is expected to happen and pds can recover from it, pds should NOT exit.
                    // 
                    // EXAMPLE: 2016-09-20. After 2016-09-20T16:48:20.504, some invalid
                    // dates/times appear: 2036-12-24T06:17:57.845, 2009-12-31T05:55:29.838, ...
                    
                    
                    // NOTE: Log messages manually line-broken to fit the width of the science log.
                    //       ============================================================================
                    CPrintf("SPICE failed to convert SCCD (spacecraft clock count, double) value from\n");
                    CPrintf("bytestream, likely (but not necessarily) because of illegal value\n");
                    CPrintf("SCCD=%g. Trying to resync.\n", sccd);
                    in_sync = 0;                // Indicate not nicely in sync
                    state   = S01_GET_MAINH;    // Not in sync! try to get in sync!
                    break;
                }
                CPrintf("    SCET time: %s OBT time: %016.6f\n",utc,sccd);
                CPrintf("Mission ID: %s Phase: %s\n",mp.mission_phase_abbrev,mp.mission_phase_name);
                SetP(&comm,"MISSION_PHASE_NAME", mp.mission_phase_name,1);  // Set mission phase name in common PDS parameters
                SetP(&comm,"TARGET_TYPE",        mp.target_type,1);         // Set target type in common PDS parameters
                SetP(&comm,"TARGET_NAME",        mp.target_name,1);         // Set target name in common PDS parameters
                
                if(calib) {
                    SetP(&comm,"PRODUCT_TYPE","\"RDR\"",1);
                } else {
                    SetP(&comm,"PRODUCT_TYPE","\"EDR\"",1);
                }
                
                GetCurrentUtc0(tstr1);                                   // Get current UTC time     
                CPrintf("    UTC Creation Time: %s\n",tstr1,1);
                SetP(&comm,"PRODUCT_CREATION_TIME",tstr1,1);       // Set creation time in common PDS parameters, no quotes!
                
                TrimWN(tstr1);                                     // Trim whitespace
                //sprintf(tstr2,"\"%s, %s\"",tstr1,pds.LabelRevNote);
                sprintf(tstr2, "\"%s\"", pds.LabelRevNote);        // Modified to not include current time. /Erik P G Johansson 2015-04-10
                SetP(&comm,"LABEL_REVISION_NOTE",tstr2,1);         // Set LABEL Revision note
                
                curr.old_macro=macro_id; // Remember old macro ID at this point

                //================================================================================================================
                // Search for UTC times in anomalies list that matches the current UTC.
                // NOTE/BUG: This appears unsafe, since UTC values can change with updated conversion SCCD-->UTC (kernels/TCORR).
                // NOTE: This is the UTC value BEFORE any timestamp modification due to ADC20 delay.
                //================================================================================================================
                if(FindP(&anom,&property1,utc,1,DNTCARE)>0)
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
                
                //###################
                //###################
                case S04_GET_ID_CODE:
                //###################
                //###################
                    DispState(state,"STATE = S04_GET_ID_CODE\n");
                    GetBuffer(cb,buff,1); // Get a byte from circular science buffer
                    
                    if(buff[0]==0xCC) // Found Sub Header
                    {
                        GetBuffer(cb,buff,1); // Get a byte from circular science buffer
                        
                        id_code=buff[0];        // Remember ID code
                        
                        params     = 0;                // Assume no parameters in science stream
                        param_type = NO_PARAMS;        // Default type of parameters, none
                        state      = S11_GET_LENGTH;   // Assume next state is this one
                        
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
                        // in the anomaly file. This we can easily do!
                        
                        // Macro information has high priority and a macro description has been found!
                        // Thus we want to override ID code in data!
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
                                    curr.sensor=SENS_P1;   // Set current sensor to sensor 1/probe 1.
                                    state=S11_GET_LENGTH;  // Ok! No params in this. Go get length.
                                    meas_seq++;            // Increase number of measurement sequences
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
                                    curr.sensor=SENS_P2;   // Set current sensor to sensor 2/probe 2.
                                    state=S11_GET_LENGTH;  // Ok! No params in this. Go get length.
                                    meas_seq++;            // Increase number of measurement sequences
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
                                    curr.sensor=SENS_P1;   // Set current sensor to sensor 1/probe 1.
                                    state=S11_GET_LENGTH;  // Ok! No params in this. Go get length.
                                    meas_seq++;            // Increase number of measurement sequences.
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
                                    curr.sensor=SENS_P2;   // Set current sensor to sensor 2/probe 2.
                                    state=S11_GET_LENGTH;  // Ok! No params in this. Go get length.
                                    meas_seq++;            // Increase number of measurement sequences
                                    CPrintf("    Found science data, from generic LDL macro, ID CODE: 0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                }
                                break;
                                
                            case D_SWEEP_P2_RAW_16BIT_BIP:
                            case D_SWEEP_P2_LC_16BIT_BIP:   // LOG COMPRESSION I HAVE TO DO SEPARATE PROCESSING FOR THIS!!!
                                curr.sensor++;              // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
                            case D_SWEEP_P1_RAW_16BIT_BIP:
                            case D_SWEEP_P1_LC_16BIT_BIP:   // LOG COMPRESSION I HAVE TO DO SEPARATE PROCESSING FOR THIS!!!
                                curr.sensor++;              // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
                                params=6;
                                meas_seq++;                 // Increase number of measurement sequences
                                CPrintf("    Found sweep science data, ID CODE: 0x%.2x Sequence: %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                param_type = SWEEP_PARAMS;
                                state      = S07_GET_PARAMS;
                                break;

                            case E_P1_D_P2_INTRL_20_BIT_RAW_BIP:
                            case D_P1_E_P2_TRNC_20_BIT_RAW_BIP:
                            case E_P1_D_P2_TRNC_20_BIT_RAW_BIP:
                            case D_P1P2INTRL_TRNC_20BIT_RAW_BIP:
                            case D_P1P2INTRL_20BIT_RAW_BIP:
                            case E_P1P2INTRL_TRNC_20BIT_RAW_BIP:
                            case E_P1P2INTRL_20BIT_RAW_BIP:
                                // Effectively curr.sensor = 3 (SENS_P1P2)
                                curr.sensor++;   // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
                                // NOTE: No break!
                                
                            case D_P2_TRNC_20_BIT_RAW_BIP:
                            case E_P2_TRNC_20_BIT_RAW_BIP:
                            case D_P2_20_BIT_RAW_BIP:
                            case E_P2_20_BIT_RAW_BIP:    
                                // Effectively curr.sensor = 2 (SENS_P2)
                                curr.sensor++;   // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
                                // NOTE: No break!
                                
                            case D_P1_TRNC_20_BIT_RAW_BIP:
                            case D_P1_20_BIT_RAW_BIP:
                            case E_P1_TRNC_20_BIT_RAW_BIP:
                            case E_P1_20_BIT_RAW_BIP:
                                // Effectively curr.sensor = 1 (SENS_P1)
                                curr.sensor++;   // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
                                
                                params=2;
                                meas_seq++;   // Increase number of measurement sequences
                                CPrintf("    Found science data, ID CODE: 0x%.2x Sequence %d Sensor: %d\n",id_code,meas_seq,curr.sensor);
                                param_type = ADC20_PARAMS;
                                state      = S07_GET_PARAMS;
                                break;
                                
                            case D_DIFF_P1P2:
                            case E_DIFF_P1P2:
                                curr.sensor++;   // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
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
                                curr.sensor++;   // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
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
                                curr.sensor++;   // Increment current sensor. (Is always initially set to 0=SENS_NONE before switch().)
                                meas_seq++;      // Increase number of measurement sequences
                                CPrintf("    Found science data, no parameters, ID CODE: 0x%.2x Sequence: %d Sensor: %d \n",id_code,meas_seq,curr.sensor);
                                state=S11_GET_LENGTH;
                                break; 
                                
                            default:
                                CPrintf("    Found undefined ID CODE:   0x%.2x\n",id_code);
                                break;
                        }
                    }   // if(buff[0]==0xCC) // Found Sub Header
                    else
                    {
                        in_sync=0; // Indicate out of sync.
                        state=S01_GET_MAINH; // No sub ID, End or out of sync
                    }
                    break;   // switch-case
                    
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
                                sprintf(tstr1, "0x%04x", macro_id);   // Create search variable to search for in linked list of property name value pairs.
                                if (FindP(&mdesc, &property1, tstr1, 1, DNTCARE)>0)
                                {
                                    SetP(&comm, "INSTRUMENT_MODE_DESC", property1->value, 1);   // Set human description of mode
                                }
                                
                                sprintf(tstr1,"\"%d\"",pds.DPLNumber);
                                
                                SetP(&comm,"PROCESSING_LEVEL_ID",tstr1,1);  // Set processing level ID.
                                
                                // Search for macro with right macro ID in macs matrix
                                macro_descr_NOT_found=1;   // Indicate that we haven't found the macro
                                for(mb=0;macro_descr_NOT_found && mb<MAX_MACRO_BLCKS;mb++)
                                {
                                    for(ma=0;macro_descr_NOT_found && ma<MAX_MACROS_INBL;ma++)
                                    {
                                        if(FindP(&macros[mb][ma], &property1, "ROSETTA:LAP_MACRO_ID_TAG", 1, DNTCARE)>0)
                                        {
                                            //TrimQN(property1->value);
                                            if(!sscanf(property1->value,"\"%x\"",&val)) 
                                            {
                                                // CASE: Can NOT parse string as macro number.
//                                                 CPrintf("    MACRO ID corrupt in macro description\n");
                                                CPrintf("    MACRO ID corrupt in macro description. Stated macro ID: \"%s\" (string)\n", property1->value);

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
                                                // CASE: Can parse string as macro number.
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
                                        } else {
                                            /** Can be useful to print error message when the property ROSETTA:LAP_MACRO_ID_TAG can not be found, which indicates
                                             * something wrong with the macro description (read from the .mds file).
                                             * The downside is that if one macro description is wrong, then this error message comes up everytime
                                             * a macro description needs to be looked up.
                                             */
                                            //CPrintf("Could not find ROSETTA:LAP_MACRO_ID_TAG in the macro description. (Hint: Is the .mds file correcty formatted?\n");
                                        }
                                    }   // for
                                }   // for

                                if(macro_descr_NOT_found)
                                {
                                    CPrintf("    ERROR: Could not find a matching macro description for MACRO ID: 0x%.4x\n", macro_id);
                                }
                                else {
                                    mb--; // mb is one step to much here since break above only breaks out of inner loop!
                                }

                                if(finger_printing) // Are we fingerprinting ?
                                {
                                    finger_printing=0;
                                    state=S09_COMPARE_PARAMS;
                                }
                                else {
                                    state=S04_GET_ID_CODE;
                                }
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
                                            
                                            sw_info.p1_fine_offs        = GetBitF(W2,4,12);       // LAP_P1_FINE_SWEEP_OFFSET
                                            sw_info.p2_fine_offs        = GetBitF(W2,4,8);        // LAP_P2_FINE_SWEEP_OFFSET
                                            sw_info.N_plateau_insmp     = 1<<(GetBitF(W1,4,0)+1); // LAP_SWEEP_PLATEAU_DURATION
                                            sw_info.N_tsweep_bias_steps = (GetBitF(W1,4,4)<<4);   // LAP_SWEEP_STEPS 
                                            sw_info.height              = GetBitF(W1,4,8)+1;      // LAP_SWEEP_STEP_HEIGHT Range is from 1 to 16
                                            sw_info.start_bias          = GetBitF(W0,8,0);        // LAP_SWEEP_START_BIAS

                                            // Total duration of sweep in samples (Not same as it's length!)                                            
                                            // "+1" since number of plateaus is number of steps plus 1
                                            // "+2" since true sweep is ideally also preceeded by two "plateau lengths" of samples.
                                            // 
                                            // ~BUG: Does not take into account that the number of pre-true sweep samples (INITIAL_SWEEP_SMPLS) varies in practice.
                                            // ~BUG: Does not consider that the last sample (in last plateau) is always/sometimes(?) missing according to implementation and
                                            // other pds source code comment by Reine Gill.
                                            // However, due to how sw_info.N_rsweep_insmp is used, this does not matter more than for log printouts.
                                            sw_info.N_rsweep_insmp  = (sw_info.N_tsweep_bias_steps+1+2) * sw_info.N_plateau_insmp; 
                                            
                                            
                                            // POPULATE PDS LAP Dictionary with sweep info.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_START_BIAS",sw_info.start_bias);         // Removed/moved 2015-02-17, Erik P G Johansson.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_STEP_HEIGHT",sw_info.height);            // Removed/moved 2015-02-23, Erik P G Johansson.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_STEPS",sw_info.N_tsweep_bias_steps);                   // Removed/moved 2015-02-17, Erik P G Johansson.
                                            //InsertTopQV(&dict,"ROSETTA:LAP_SWEEP_PLATEAU_DURATION",sw_info.N_plateau_insmp);  // Removed/moved 2015-02-17, Erik P G Johansson.
                                            //InsertTopQ(&dict,"ROSETTA:LAP_SWEEP_RESOLUTION",sw_info.resolution);          // Removed/moved 2015-02-23, Erik P G Johansson.
                                            //InsertTopQ(&dict,"ROSETTA:LAP_SWEEP_FORMAT",sw_info.format);                  // Removed/moved 2015-02-23, Erik P G Johansson.
                                            
                                            if(!strcmp("YES",sw_info.p1)) // If sweeping P1
                                            {
                                                // Edit 2015-02-17, 2015-02-23, Erik P G Johansson:
                                                // Changing keyword names to be probe-specific. This replaces lines of code a few lines up.
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_START_BIAS",       sw_info.start_bias);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_STEP_HEIGHT",      sw_info.height);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_STEPS",            sw_info.N_tsweep_bias_steps);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P1_SWEEP_PLATEAU_DURATION", sw_info.N_plateau_insmp);
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
                                                InsertTopQV(&dict, "ROSETTA:LAP_P2_SWEEP_STEPS",            sw_info.N_tsweep_bias_steps);
                                                InsertTopQV(&dict, "ROSETTA:LAP_P2_SWEEP_PLATEAU_DURATION", sw_info.N_plateau_insmp);
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
                                            
                                            a20_info.N_MA_length_insmp = (1<<GetBitF(buff[0],4,4)); // Length of moving average filter
                                            a20_info.adc20_control     = GetBitF(buff[0],4,0);      // Indicate full, truncated, and so on
                                            a20_info.insmp_per_tmsmp   = (1<<GetBitF(buff[1],4,0)); // Downsampling factor (Thus keep every n:th samp)
                                            
                                            // POPULATE PDS LAP Dictionary with 20 bit ADC info.
                                            InsertTopQV(&dict,"ROSETTA:LAP_P1P2_ADC20_DOWNSAMPLE",a20_info.insmp_per_tmsmp);
                                            InsertTopQV(&dict,"ROSETTA:LAP_P1P2_ADC20_MA_LENGTH", a20_info.N_MA_length_insmp);
                                            InsertTopQ( &dict,"ROSETTA:LAP_P1P2_ADC20_STATUS",    a20status[a20_info.adc20_control & 0xf]);
                                            
                                            if((a20_info.adc20_control & 0x3)==0x02) { curr.sensor=SENS_P1; }// Modify current sensor from both P1 and P2 to P1 only
                                            if((a20_info.adc20_control & 0x3)==0x01) { curr.sensor=SENS_P2; }// Modify current sensor from both P1 and P2 to P2 only
                                            // Above: I don't treat the combination where only the lowest bits are used for the 20 bit ADC:s
                                            //        because we will never use it. It's not scientific!
                                        }
                                        //CPrintf("  sw_info.N_plateau_insmp=%i\n", sw_info.N_plateau_insmp);
                                        state=S11_GET_LENGTH; // Error unknown parameter type, continue to get length
                                        break;
                                        
                                        
                                        //######################
                                        //######################
                                        case S09_COMPARE_PARAMS:
                                        //######################
                                        //######################
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
                                                            CPrintf("    Warning: Mismatch between parameters and macro description\n");
                                                            CPrintf("    MACRO: %s=%s\n",property2->name,property2->value);
                                                            CPrintf("    PARAM: %s=%s\n",property1->name,property1->value);
                                                            // Remove warning keyword. Enough with warning in logs.
                                                            //InsertTopQ(&dict,"ROSETTA:LAP_SC_VS_MACRO_MISMATCH","WARNING");
                                                            // NOTE: Seems that the "PARAM" value (from "dict") is used, not the macro value.
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
                                            
                                            //##################
                                            //##################
                                            case S11_GET_LENGTH:
                                            //##################
                                            //##################
                                                DispState(state,"STATE = S11_GET_LENGTH\n");
                                                
                                                GetBuffer(cb,buff,2); // Get 2 bytes from circular science buffer
                                                
                                                hb=buff[0]<<8;
                                                lb=buff[1];
                                                N_bytes=(hb | lb)*2;
                                                CPrintf("    Data length: %d Bytes\n",N_bytes);
                                                
                                                if(N_bytes>RIDICULUS) // If length is ridiculously long
                                                {
                                                    CPrintf("    Ridiculously long length in science data, trying to resync\n");
                                                    in_sync=0; // Indicate not in sync
                                                    state=S01_GET_MAINH; //Resync
                                                }
                                                else
                                                {
                                                    if(SyncAhead(cb,N_bytes)) // Extra sync test added 2004-04-13.
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
                                                GetBuffer(cb,buff,N_bytes); 
                                                
                                                if(param_type==NO_PARAMS)
                                                {
                                                    state=S14_RESOLVE_MACRO_PARAMETERS; // Resolve macro parameters
                                                }
                                                else
                                                {
                                                    state=S09_COMPARE_PARAMS;           // Check data parameters macro consistency
                                                }
                                                break;

                                            //#######################
                                            //#######################
                                            case S13_RECONNECT:
                                            //#######################
                                            //#######################
                                                DispState(state,"STATE = S13_RECONNECT\n");
                                                break;

                                            //################################
                                            //################################
                                            case S14_RESOLVE_MACRO_PARAMETERS:
                                            //################################
                                            //################################
                                                DispState(state,"STATE = S14_RESOLVE_MACRO_PARAMETERS\n");
                                                
                                                
                                                // FINGERPRINTING for the right macro description.
                                                // This is a bit dirty, but we need to do it on the prom macros (8 of them).
                                                // All macros in flash contain the macro ID in the science data stream.
                                                if(macro_descr_NOT_found) // If no macro description
                                                {
                                                    CPrintf("    WARNING: Fingerprinting macro ID.\n");
                                                    finger_printing=1;
                                                    if(N_bytes==40 && id_code==D_P1P2INTRL_TRNC_20BIT_RAW_BIP)
                                                    {
                                                        // Brute force macro settings!
                                                        macro_id=0x201; // Set macro ID
                                                        state=S06_GET_MACRO_DESC; // Break out of switch, and get macro description
                                                        break;
                                                    }
                                                    
                                                    if(400<=N_bytes && N_bytes<=512 && id_code==D_SWEEP_P1_RAW_16BIT_BIP)
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
                                                    
                                                    if(1930<=N_bytes && N_bytes<=1950 && id_code==D_SWEEP_P1_RAW_16BIT_BIP)
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
                                                    
                                                    if(N_bytes==8950 && id_code==D_P1P2INTRL_TRNC_20BIT_RAW_BIP)
                                                    {
                                                        // Brute force macro settings!
                                                        macro_id=0x205; // Set macro ID
                                                        state=S06_GET_MACRO_DESC;  // Break out of switch, and get macro description
                                                        break;
                                                    }
                                                    
                                                    if(N_bytes==1024 && id_code==E_P1_RAW_16BIT_D4)
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
                                                
                                                ADC16_P1_insmp_per_tmsmp = -1;   // Indicate that "down sampl 16 bit sensor 1" value is not resolved
                                                ADC16_P2_insmp_per_tmsmp = -1;   // Indicate that "down sampl 16 bit sensor 2" value is not resolved

                                                // strstr : Return pointer to the first occurrence of a given string within another string.
                                                if((tp=strstr(IDList[id_code],"16BIT_D"))!=NULL)
                                                {
                                                    if(curr.sensor==SENS_P1) {
                                                        if(!sscanf(&tp[7],"%d",&ADC16_P1_insmp_per_tmsmp)) {
                                                            ADC16_P1_insmp_per_tmsmp=-1; // Error in conversion
                                                            CPrintf("WARNING: Sets default value ADC16_P1_insmp_per_tmsmp=%i since can not parse substring tp=\"\%s\" of IDList[id_code]=\"%s\".\n", ADC16_P1_insmp_per_tmsmp, tp, IDList[id_code]);
                                                        }
                                                    }
                                                    
                                                    if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2)
                                                    {
                                                        if(!sscanf(&tp[7],"%d",&ADC16_P2_insmp_per_tmsmp)) {
                                                            ADC16_P2_insmp_per_tmsmp=-1; // Error in conversion
                                                            CPrintf("WARNING: Sets default value ADC16_P2_insmp_per_tmsmp=%i since can not parse substring tp=\"\%s\" of IDList[id_code]=\"%s\".\n", ADC16_P2_insmp_per_tmsmp, tp, IDList[id_code]);
                                                        }
                                                    }
                                                }
                                                //CPrintf("5 ADC16_P1_insmp_per_tmsmp=%i\n", ADC16_P1_insmp_per_tmsmp);
                                                //CPrintf("  ADC16_P2_insmp_per_tmsmp=%i\n", ADC16_P2_insmp_per_tmsmp);
                                                //CPrintf("  sw_info.N_plateau_insmp=%i\n", sw_info.N_plateau_insmp);

                                                curr.bias_mode1=DENSITY;   // Assume density mode unless there is a reason not to.
                                                curr.bias_mode2=DENSITY;

                                                // Check if IDList[id_code] (human-readable string!!) indicates E-FIELD mode.
                                                if((tp=strstr(IDList[id_code],"E_"))!=NULL)
                                                {
                                                    curr.bias_mode1=E_FIELD;
                                                    curr.bias_mode2=E_FIELD;                                                    
                                                }
                                                
                                                // NOTE: Special case for id_code==E_P1_D_P2_INTRL_20_BIT_RAW_BIP since it does not fit in
                                                // with any other rule, and can not be easily fitted into the switch(id_code) segment.
                                                //if (id_code==E_P1_D_P2_INTRL_20_BIT_RAW_BIP) {
                                                // NOTE: Modified special case to be broader /2016-07-11.
                                                if ((tp=strstr(IDList[id_code],"E_P1_D_P2"))!=NULL) {
                                                    curr.bias_mode1=E_FIELD;
                                                    curr.bias_mode2=DENSITY;
                                                }
                                                if ((tp=strstr(IDList[id_code],"D_P1_E_P2"))!=NULL) {
                                                    curr.bias_mode1=DENSITY;
                                                    curr.bias_mode2=E_FIELD;
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
                                                                N_tmsmp=N_bytes*2/5; // Sensor P1 only full 20 Bit ADC
                                                                data_type=D201;
                                                            }
                                                            else
                                                            {
                                                                // Sensor P1 only truncated 20 Bit ADC
                                                                data_type=D201T;
                                                                N_tmsmp=N_bytes/2;   
                                                            }
                                                            break;
                                                        case SENS_P2:
                                                            if(a20_info.adc20_control & 0x04) 
                                                            {
                                                                N_tmsmp=N_bytes*2/5; // Sensor P2 only full 20 Bit ADC
                                                                data_type=D202;
                                                            }
                                                            else
                                                            {
                                                                // Sensor P2 only truncated 20 Bit ADC
                                                                data_type=D202T;
                                                                N_tmsmp=N_bytes/2;   
                                                            }
                                                            break;
                                                        case SENS_P1P2:
                                                            if((a20_info.adc20_control & 0x0C)==0x0C) 
                                                            {
                                                                N_tmsmp=N_bytes/5; // Full 20 Bit ADC:s P1 and P2
                                                                data_type=D20;
                                                            }
                                                            else
                                                            {
                                                                // Truncated 20 Bit ADC:s P1 and P2
                                                                data_type=D20T; 
                                                                N_tmsmp=N_bytes/4; 
                                                            }
                                                            break;
                                                        default:
                                                            break;
                                                    }
                                                }
                                                else
                                                {
                                                    N_tmsmp=N_bytes/2; // Number of samples then 20 Bit ADC:s are not used and no compression. 
                                                    if(id_code==D_SWEEP_P2_LC_16BIT_BIP || id_code==D_SWEEP_P1_LC_16BIT_BIP) // If log compression is used
                                                    {
                                                        N_tmsmp=N_bytes; // Log compression used
                                                    }
                                                }
                                                
                                                sprintf(tstr1,"%d",N_tmsmp);
                                                SetP(&comm,"FILE_RECORDS",tstr1,1); // Set number of records (in data table file) in common PDS parameters
                                                
                                                CPrintf("    Number of samples current record: %d\n",N_tmsmp);
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
                                                        //CPrintf("6 ADC16_P1_insmp_per_tmsmp=%i\n", ADC16_P1_insmp_per_tmsmp);
                                                        //CPrintf("  ADC16_P2_insmp_per_tmsmp=%i\n", ADC16_P2_insmp_per_tmsmp);
                                                        if (debug>0) {
                                                            printf("    Uses ROSETTA:LAP_SET_SUBHEADER = %s, meas_seq=%i in macro (.mds file).\n", property1->value, meas_seq);
                                                        }

                                                        //CPrintf("4 ADC16_P1_insmp_per_tmsmp=%i\n", ADC16_P1_insmp_per_tmsmp);
                                                        //CPrintf("  ADC16_P2_insmp_per_tmsmp=%i\n", ADC16_P2_insmp_per_tmsmp);
                                                        //DumpPrp(&macros[mb][ma]);   // DEBUG
                                                        // Find downsampling value probe 1 in macro and if no ID value exists use macro desc. value
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P1_ADC16_DOWNSAMPLE",DNTCARE)>0)
                                                        {
                                                            sscanf(property2->value,"\"%x\"",&val);
                                                            if(ADC16_P1_insmp_per_tmsmp==-1 || macro_priority) { // Value not resolved from ID or macro has high priority use macro value instead
                                                                ADC16_P1_insmp_per_tmsmp=val;
                                                                //CPrintf("    Using .mds value: ROSETTA:LAP_P1_ADC16_DOWNSAMPLE=ADC16_P1_insmp_per_tmsmp=%i\n", ADC16_P1_insmp_per_tmsmp);
                                                            }
                                                            
                                                            if(ADC16_P1_insmp_per_tmsmp!=val) {
                                                                CPrintf("    Warning mismatch between data ID code info. and macro description info.\n");
                                                            }
                                                            //CPrintf("%s = 0x%04x\n",property2->name,ADC16_P1_insmp_per_tmsmp);
                                                        }

                                                        // Find downsampling value probe 2 in macro and if no ID value exists use macro desc. value
                                                        if(FindB(&macros[mb][ma],&property1,&property2,"ROSETTA:LAP_P2_ADC16_DOWNSAMPLE",DNTCARE)>0)
                                                        {
                                                            sscanf(property2->value,"\"%x\"",&val);
                                                            if(ADC16_P2_insmp_per_tmsmp==-1 || macro_priority)  { // Value not resolved from ID  or macro has high priority use macro value instead
                                                                ADC16_P2_insmp_per_tmsmp=val;
                                                                //CPrintf("    Using .mds value: ROSETTA:LAP_P2_ADC16_DOWNSAMPLE=ADC16_P2_insmp_per_tmsmp=%i\n", ADC16_P2_insmp_per_tmsmp);
                                                            }
                                                            
                                                            if(ADC16_P2_insmp_per_tmsmp!=val && !macro_priority)  { // Print mismatch warning. Only if macro desc. has low priority
                                                                CPrintf("    Warning mismatch between data ID code info. and macro description info.\n");
                                                            }
                                                            //CPrintf("%s = 0x%04x\n",property2->name,ADC16_P2_insmp_per_tmsmp);
                                                        }
                                                        //CPrintf("3 ADC16_P1_insmp_per_tmsmp=%i\n", ADC16_P1_insmp_per_tmsmp);
                                                        //CPrintf("  ADC16_P2_insmp_per_tmsmp=%i\n", ADC16_P2_insmp_per_tmsmp);

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
                                                                            InsertTopQ(&dict,property2->name,"E-FIELD"); // No! Trust the ID code more.
                                                                        }
                                                                        
                                                                        // Need to keep old bias in case of no macro change to accomodate for extra bias
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
                                                                        
                                                                        // Need to keep old bias in case of no macro change to accomodate for extra bias
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
                                                                                    printf("    Macro: Set fix voltage bias VBIAS1 = %04x (curr.vbias1; hex TM units)\n", curr.vbias1);
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
                                                                            InsertTopQ(&dict,property2->name,"E-FIELD"); // No! Trust the ID code more.
                                                                        }
                                                                        
                                                                        // Need to keep old bias in case of no macro change to accomodate for extra bias
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
                                                                        // Is it so according to macro description, or macro description has high priority
                                                                        if(!strcmp(property2->value,"\"DENSITY\"") || macro_priority) {                                                                                        
                                                                            InsertTopK(&dict,property2->name,property2->value);   // Yes, let's set it
                                                                        }
                                                                        else {
                                                                            InsertTopQ(&dict,property2->name,"DENSITY"); //  No! Trust the ID code more.
                                                                        }
                                                                        
                                                                        //----------------------------------------------------------------------------------------------------
                                                                        // Need to keep old bias in case of no macro change to accomodate for extra bias
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
                                                                                    printf("    Macro: Set fix voltage bias VBIAS2 = %04x (curr.vbias2; hex TM units)\n", curr.vbias2);
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
                                                        
                                                        /**
                                                         * NOTE: DECEIVING IF STATEMENT.
                                                         * The "if" condition is an assignment and only returns false in case of error.
                                                         * Therefore the if statement will basically always be executed.
                                                         * NOTE: This is the only location where "aqps_seq" is assigned.
                                                         * NOTE: This entire section could basically be moved to S15_WRITE_PDS_FILES, since it fits with creating files there.
                                                         * However, this if statement is also contained within three other if statements:
                                                         *    if((aqps_seq=TotAQPs(&macros[mb][ma],meas_seq))>=0) { ... }
                                                         *    if(FindP(&macros[mb][ma],&property1,"ROSETTA:LAP_SET_SUBHEADER",meas_seq,DNTCARE)>0) { ... }
                                                         *    if(!macro_descr_NOT_found) { ... }
                                                         * and one might not want to move the middle one to S15_WRITE_PDS_FILES(?).
                                                         * /Erik P G Johansson 2016-03-10
                                                         */
                                                        if((aqps_seq=TotAQPs(&macros[mb][ma],meas_seq))>=0)
                                                        {
                                                            CPrintf("    %d sequence starts %d AQPs from start of sequence\n", meas_seq, aqps_seq);
                                                            curr.offset_time = aqps_seq*32.0;
                                                            curr.seq_time_TM = sccd + curr.offset_time;   // Calculate time of current sequence.
                                                            
                                                            /*==================================================
                                                             * Adjust the timing of calibrated-level ADC20 data.
                                                             *==================================================
                                                             * NOTE: This change should affect
                                                             * (1) TAB files (timestamp columns),
                                                             * (2) LBL files: SPACECRAFT_CLOCK_START/STOP_COUNT,
                                                             * (3) LBL files: START/STOP_TIME
                                                             *
                                                             * NOTE: On the one hand, the time is also used for other purposes and care has to be taken to make sure that these
                                                             * use the desired time. On the other hand, the actual effect on these is very, very small, given the size of the
                                                             * time delay, but still, in principle, the delay MIGHT affect all of these.
                                                             * (1) When commanded bias (pds.bias) is interpreted as having been set.
                                                             * (2) How anomaly timestamps (pds.anomalies) are interpreted.
                                                             *     NOTE: This requires exact matching of timestamps. Approximate does not give approximate result (?).
                                                             * (3) Which bias-dependent current offset to use (CALIB_MEAS).
                                                             * (4) In which day directory a data file ends up.
                                                             * Therefore, both the original TM value and the adjusted ("corrected") value are kept.
                                                             */
                                                            if (calib && (param_type==ADC20_PARAMS)) {
                                                                curr.seq_time_corrected = curr.seq_time_TM - ADC20_DELAY_S;
                                                            } else {
                                                                curr.seq_time_corrected = curr.seq_time_TM;
                                                            }
                                                            
                                                            ConvertSccd2Sccs(curr.seq_time_corrected, pds.SCResetCounter, tstr1, TRUE);  // Compile OBT string and add reset number of S/C clock.                                                            
                                                            SetP(&comm,"SPACECRAFT_CLOCK_START_COUNT", tstr1, 1);
                                                            
                                                            ConvertSccd2Utc(curr.seq_time_corrected, tstr1, NULL);   // Decode raw time into PDS compliant UTC time
                                                            CPrintf("    Current sequence start time (possibly corrected for delay) is: %s\n", tstr1);
                                                            SetP(&comm, "START_TIME", tstr1, 1);
                                                            
                                                            
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
                                                            IncAlphaNum(alphanum_s);         // Increment alphanumeric value (SCI)


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
                                                            // [18] : [C]alibrated or Calibrated [R]aw (=Edited)
                                                            // [19] : [E]-Field or [D]ensity
                                                            // [20] : [S]weep or [B]ias.
                                                            // [21] : Probe. P[1], P[2], P[3].
                                                            // [22] : [4]/[8] kHz filter
                                                            // Note: [i] : i=byte index in string (i=0: first character)
                                                            // NOTE: Sets probe number and density/E field to "x" since these will be overwritten later (case S15_WRITE_PDS_FILES).
                                                            sprintf(tstr2, "RPCLAP%s%s%s_%sS_RxBx%1d%cS",
                                                                    &tstr1[2], &tstr1[5], &tstr1[8],
                                                                    alphanum_s, curr.afilter, tm_rate);     // Compile product ID=base filename (filename without extension).
                                                                    
                                                            if(param_type==ADC20_PARAMS)    { tstr2[16]='T'; }  // Set to [T]wenty bit ADC:s or keep [S]ixteen bit.
                                                            if(calib)                       { tstr2[18]='C'; }  // Set to [C]alibrated       or keep Calibrated [R]aw.
                                                            if(param_type==SWEEP_PARAMS)    { tstr2[20]='S'; }  // Set to [S]weep            or keep [B]ias.

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


                                                            curr.sec_per_tmsmp=0.0; // Init
                                                            // Compute stop time of current sequence
                                                            if(param_type==SWEEP_PARAMS)
                                                            {
                                                                // Old implementation containing bug:
                                                                // BUG: Only aproximately correct. Varies with INITIAL_SWEEP_SMPLS and more.
                                                                //curr.sec_per_tmsmp = sw_info.N_rsweep_insmp / (SAMP_FREQ_ADC16 * N_tmsmp);

                                                                if(curr.sensor==SENS_P1) {
                                                                    curr.sec_per_tmsmp = ADC16_P1_insmp_per_tmsmp / SAMP_FREQ_ADC16;
                                                                } else {
                                                                    curr.sec_per_tmsmp = ADC16_P2_insmp_per_tmsmp / SAMP_FREQ_ADC16;
                                                                }
                                                            }
                                                            else    // if(param_type==SWEEP_PARAMS)
                                                            {
                                                                
                                                                if(param_type!=ADC20_PARAMS)
                                                                {
                                                                    if(ADC16_P1_insmp_per_tmsmp==-1) // -1 here => Probably generic macro
                                                                    {
                                                                        ADC16_P1_insmp_per_tmsmp=1;
                                                                        CPrintf("    Warning, parameter not resolved probably generic macro, using default no resampling on ADC16 P1\n");
                                                                    }
                                                                    
                                                                    if(ADC16_P2_insmp_per_tmsmp==-1) // -1 here => Probably generic macro
                                                                    {
                                                                        ADC16_P2_insmp_per_tmsmp=1;
                                                                        CPrintf("    Warning, parameter not resolved probably generic macro, using default no resampling on ADC16 P2\n");
                                                                    }
                                                                }
                                                                
                                                                //========================
                                                                // Set curr.sec_per_tmsmp
                                                                //========================
                                                                switch(curr.sensor)
                                                                {
                                                                    case SENS_P1:
                                                                        if(param_type==ADC20_PARAMS)
                                                                            curr.sec_per_tmsmp = a20_info.insmp_per_tmsmp/SAMP_FREQ_ADC20;
                                                                        else
                                                                            curr.sec_per_tmsmp = ADC16_P1_insmp_per_tmsmp/SAMP_FREQ_ADC16;
                                                                        break;
                                                                    case SENS_P2:
                                                                    case SENS_P1P2: // In this case ADC16_P1_insmp_per_tmsmp==ADC16_P2_insmp_per_tmsmp
                                                                        if(param_type==ADC20_PARAMS)
                                                                            curr.sec_per_tmsmp = a20_info.insmp_per_tmsmp/SAMP_FREQ_ADC20; 
                                                                        else
                                                                            curr.sec_per_tmsmp = ADC16_P2_insmp_per_tmsmp/SAMP_FREQ_ADC16; 
                                                                        break;
                                                                    default:
                                                                        curr.sec_per_tmsmp = 1/SAMP_FREQ_ADC16;
                                                                        break;
                                                                }
                                                            }    // (param_type==SWEEP_PARAMS) ... else
                                                            
                                                            // Calculate current STOP time.
                                                            curr.stop_time_corrected = curr.seq_time_corrected + (N_tmsmp-1)*curr.sec_per_tmsmp;   
                                                            curr.stop_time_TM        = curr.seq_time_TM        + (N_tmsmp-1)*curr.sec_per_tmsmp;
                                                            
                                                            ConvertSccd2Sccs(curr.stop_time_corrected, pds.SCResetCounter, tstr5, TRUE);
                                                            SetP(&comm,"SPACECRAFT_CLOCK_STOP_COUNT", tstr5, 1);
                                                            
                                                            ConvertSccd2Utc(curr.stop_time_corrected, tstr5, NULL);  // Decode raw time into PDS compliant UTC time.
                                                            CPrintf("    Current sequence stop  time is: %s\n", tstr5);
                                                            SetP(&comm, "STOP_TIME",  tstr5, 1);         // Update STOP_TIME in common PDS parameters.
                                                        }   // if((aqps_seq=TotAQPs(&macros[mb][ma],meas_seq))>=0)
                                                    }   // if(FindP(&macros[mb][ma],&property1,"ROSETTA:LAP_SET_SUBHEADER",meas_seq,DNTCARE)>0)
                                                }   // if(!macro_descr_NOT_found)
                                                else
                                                {
                                                    //================================================================
                                                    // CASE: No macro description fits and no anomaly override exists
                                                    //================================================================
                                                    // Derive all that we can without anomaly override and send it to log file.
                                                    // Problematic data are stored in the UnAccepted_Data directory.
                                                    // case S15_WRITE_PDS_FILES uses "macro_descr_NOT_found" to know that the data needs a different treatment.
                                                    
                                                    CPrintf("    No macro description fits, data will be stored in the UnAccepted_Data directory\n");
                                                    if(param_type==NO_PARAMS)
                                                    {
                                                        if(ADC16_P1_insmp_per_tmsmp==-1) 
                                                        {
                                                            ADC16_P1_insmp_per_tmsmp=1; // Conversion errors use default 1
                                                            CPrintf("    Warning, parameter not resolved, using default no resampling on ADC16 P1\n");
                                                        }
                                                        
                                                        if(ADC16_P2_insmp_per_tmsmp==-1) 
                                                        {
                                                            ADC16_P2_insmp_per_tmsmp=1; // Conversion errors use default 1 
                                                            CPrintf("    Warning, parameter not resolved, using default no resampling on ADC16 P2\n");
                                                        }
                                                    }
                                                    
                                                    ConvertSccd2Sccs(sccd, pds.SCResetCounter, tstr1, TRUE); // Compile OBT string and add reset number of S/C clock
                                                    
                                                    SetP(&comm,"SPACECRAFT_CLOCK_START_COUNT",tstr5,1);                 // BUG? Meant to read tstr1?
                                                    CPrintf("    OBT time start of measurement cycle: %s \n",tstr5);    // BUG? Meant to read tstr1?
                                                    
                                                    if(param_type==NO_PARAMS) 
                                                    {
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P1 %d/[samples]\n",ADC16_P1_insmp_per_tmsmp*N_tmsmp);
                                                        }
                                                        if(curr.sensor==SENS_P2) {
                                                            CPrintf("    Duration P2 %d/[samples]\n",ADC16_P2_insmp_per_tmsmp*N_tmsmp);
                                                        }
                                                    }
                                                    
                                                    if(param_type==SWEEP_PARAMS)
                                                    {                                                            
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P1 %d (sweep)\n",sw_info.N_rsweep_insmp);
                                                        }                                                            
                                                        if(curr.sensor==SENS_P2) {
                                                            CPrintf("    Duration P2 %d (sweep)\n",sw_info.N_rsweep_insmp);
                                                        }
                                                    }
                                                    
                                                    if(param_type==ADC20_PARAMS)
                                                    { 
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P1\n",N_tmsmp*a20_info.insmp_per_tmsmp);
                                                        }                                                            
                                                        if(curr.sensor==SENS_P1) {
                                                            CPrintf("    Duration P2\n",N_tmsmp*a20_info.insmp_per_tmsmp);
                                                        }                                                            
                                                        if(curr.sensor==SENS_P1P2) {
                                                            CPrintf("    Duration P1 & P2\n",N_tmsmp*a20_info.insmp_per_tmsmp);
                                                        }
                                                    }
                                                }   // if(!macro_descr_NOT_found) ... else ...

                                                // Downsampling values should be resolved at this point so let's put them in!
                                                // This part is always executed if we have a macro description or not.
                                                if(param_type==NO_PARAMS || param_type==SWEEP_PARAMS)
                                                {
                                                    if(curr.sensor==SENS_P1 || curr.sensor==SENS_P1P2) {
                                                        InsertTopQV(&dict,"ROSETTA:LAP_P1_ADC16_DOWNSAMPLE",ADC16_P1_insmp_per_tmsmp);
                                                    }                                                        
                                                    if(curr.sensor==SENS_P2 || curr.sensor==SENS_P1P2) {
                                                        InsertTopQV(&dict,"ROSETTA:LAP_P2_ADC16_DOWNSAMPLE",ADC16_P2_insmp_per_tmsmp);
                                                    }
                                                }
                                                state = S15_WRITE_PDS_FILES;   // Change state
                                                break;

                                                //#######################
                                                //#######################
                                                case S15_WRITE_PDS_FILES:
                                                //#######################
                                                //#######################
                                                    /* NOTE: case S14_RESOLVE_MACRO_PARAMETERS is ALWAYS followed by case S15_WRITE_PDS_FILES
                                                     * and case S15_WRITE_PDS_FILES is always preceeded by case S14_RESOLVE_MACRO_PARAMETERS.
                                                     * They could thus really be merged into one if one really wanted to, or be thought of as the same case.
                                                     **/
                                                    
                                                    DispState(state,"STATE = S15_WRITE_PDS_FILES\n");
                                                    //CPrintf("1 ADC16_P1_insmp_per_tmsmp=%i\n", ADC16_P1_insmp_per_tmsmp);
                                                    //CPrintf("  ADC16_P2_insmp_per_tmsmp=%i\n", ADC16_P2_insmp_per_tmsmp);
                                                    
                                                    // -------------------------------------------------------------------------------------
                                                    // Look for the existence of a macro ID before checking for macros to exclude in CALIB.
                                                    // Early macros (before the first flight software update in space) did not have macro
                                                    // descriptions/IDs since that functionality had not been implemented yet.
                                                    // /Erik P G Johansson summarizing Reine Gill 2015-03-26.
                                                    // -------------------------------------------------------------------------------------
                                                    if(!macro_descr_NOT_found)
                                                    {
                                                        //===============================================================================
                                                        // CASE: We do HAVE a macro description (macro_descr_NOT_found==0 means we have).
                                                        //===============================================================================
                                                        if(calib)
                                                        {
                                                            for(i=0;i<nexcl;i++)
                                                            {
                                                                if(exclude[i]==macro_id)
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
                                                        
                                                        FindP(&comm,&property1,"INSTRUMENT_MODE_ID",   1,DNTCARE); // Get macro ID
                                                        FindP(&comm,&property2,"PRODUCT_CREATION_TIME",1,DNTCARE); // Get product creation time
                                                        //FindP(&comm,&property3,"START_TIME",1,DNTCARE);          // Get start time
                                                        //FindP(&comm,&property4,"STOP_TIME",1,DNTCARE);           // Get stop time
                                                        
                                                        
                                                        // Find position where the root of the PDS archive starts
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
                                                                N_plateau_tmsmp = sw_info.N_plateau_insmp / ADC16_P1_insmp_per_tmsmp;   // TM samples per plateau.
                                                            } else {
                                                                N_plateau_tmsmp = sw_info.N_plateau_insmp / ADC16_P2_insmp_per_tmsmp;   // TM samples per plateau.
                                                            }
                                                            // Initial TM samples before true sweep starts.
                                                            // IMPLEMENTATION NOTE: Assumes that the last TM sample of the last plateau does not exist.
                                                            // Compare with how the bias values are derived/set.
                                                            // This values is used for
                                                            // (1) Setting ROSETTA:LAP_Px_INITIAL_SWEEP_SMPLS
                                                            // (2) Determining how to pair measured samples with sweep bias voltages, including whether or not the last 
                                                            // sweep plateau lacks its last sample.
                                                            N_non_tsweep_tmsmp = N_tmsmp - ((sw_info.N_tsweep_bias_steps+1)*N_plateau_tmsmp - 1);   
                                                            
                                                            if ((N_non_tsweep_tmsmp<0) || (16<N_non_tsweep_tmsmp)) {   // NOTE: Positive warning threshold is documented max INITIAL_SWEEP_SMPLS value.
                                                                // Have seen many cases of suspicious values. Therefore extra error message.
                                                                // Likely due to misconfigured .mds file(s).
                                                                CPrintf("WARNING: Suspicious number of initial sweep samples, N_non_tsweep_tmsmp=%i=0x%x\n", N_non_tsweep_tmsmp, N_non_tsweep_tmsmp);
                                                                //CPrintf("         N_tmsmp=%i; sw_info.N_tsweep_bias_steps=%i; N_plateau_tmsmp=%i; sw_info.N_plateau_insmp=%i\n", N_tmsmp, sw_info.N_tsweep_bias_steps, N_plateau_tmsmp, sw_info.N_plateau_insmp);
                                                                //CPrintf("         ROSETTA:LAP_P1_ADC16_DOWNSAMPLE=ADC16_P1_insmp_per_tmsmp=%i=0x%x\n", ADC16_P1_insmp_per_tmsmp, ADC16_P1_insmp_per_tmsmp);
                                                                //CPrintf("         ROSETTA:LAP_P2_ADC16_DOWNSAMPLE=ADC16_P2_insmp_per_tmsmp=%i=0x%x\n", ADC16_P2_insmp_per_tmsmp, ADC16_P2_insmp_per_tmsmp);

                                                            }

                                                        }    // if(param_type==SWEEP_PARAMS)
                                                        
                                                        //##########################################################################
                                                        // WRITE TO DATA LABEL FILE (.LBL), TABLE FILE (.TAB), and add to INDEX.TAB
                                                        //##########################################################################
                                                        /* NOTE: data_type!=D20 && data_type!=D20T
                                                         * <=>   (curr.sensor != SENS_P1P2 || data_type==D16)
                                                         * <=>   Not (ADC20 and both probes).
                                                         * NOTE: The if condition _APPEARS_TO_BE_ a very crude way of determining
                                                         * if-and-only-if there is data from exactly one probe (P3 counts as one probe).
                                                         * Not sure why it should work, but it does seem to be consistent with id.h:
                                                         * no subheaders read ADC16 from both probes.
                                                         * Why not use (curr.sensor==SENS_P1 || curr.sensor==SENS_P2), or curr.sensor!=SENS_P1P2?
                                                         * /Erik P G Johansson 2016-03-10
                                                         */
                                                        if(data_type!=D20 && data_type!=D20T)
                                                        {
                                                            //=====================================================
                                                            // CASE: There is data for exactly one probe (dop==0).
                                                            // ---------------------------------------------------
                                                            // NOTE: P3 counts as one probe here.
                                                            //=====================================================
                                                            if (debug >= 1) {
                                                                CPrintf("    Creating LBL/TAB file pair (dop=0) - There is data for exactly one probe (?).\n");
                                                            }

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
                                                                CPrintf("    Creating LBL/TAB file pair for P1 data (dop=1) - There is data for exactly two probes.\n");
                                                            }
                                                            WriteTABLBL_FilePair(1, 1);
                                                            

                                                            
                                                            //====================
                                                            // Handle dop==2 (P2)
                                                            //====================
                                                            if (debug >= 1) {
                                                                CPrintf("    Creating LBL/TAB file pair for P2 data (dop=2) - There is data for exactly two probes.\n");
                                                            }
                                                            WriteTABLBL_FilePair(2, 2);
                                                        }
                                                        
                                                        // Clear dictionary PDS LAP parameters, common parameters are not cleared until a new measurement cycle begins.
                                                        ClearDictPDS(&dict);
                                                    }   // if(!macro_descr_NOT_found)
                                                    else
                                                    {
                                                        //==============================================
                                                        // CASE: A macro description could not be found.
                                                        //==============================================
                                                        // If fingerprinting was enabled it must have failed.
                                                        // Anomaly correction must also have failed at this point.
                                                        // 
                                                        // Data will be stored in the UnAccepted_Data directory instead
                                                        // and requires manual attention.
                                                        // NOTE: TAB & LBL file pair will NOT be created
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
                                                    
                                                //#######################
                                                //#######################
                                                default:
                                                //#######################
                                                //#######################
                                                    DispState(state,"STATE = UNKNOWN\n");
                                                    break;
        }   // switch
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



/*
 * Closes logging and exits with status (exit/error code).
 * When a thread wants pds to quit (and all threads to quit), it calls this function.
 * Also used for exiting without error.
 * 
 * NOTE: Uncertain what the convention for exit codes is.
 *
 * status : Exit error code
 */
void ExitPDS(int status)
{
    char tstr1[8192]; // Temporary string
    char tstr2[PATH_MAX+1]; // Temporary string
    char tstr3[64];         // Temporary string for UTC time
    
    FILE *pipe_fp;
    int len;
//     int i;
    
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
            // It has been previously observed (pds v3.07/v3.08) that the DecodeScience thread may otherwise sometimes be terminated
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
        YPrintf("Science thread has been cancelled.\n");
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
        HPrintf("Finish last HK LBL file after HK thread has been canceled.\n");
        
        //===============================
        // Add to the HK LBL data struct
        //===============================
        SetP(&hkl, "SPACECRAFT_CLOCK_STOP_COUNT", hk_info.obt_time_str,1);  // Set OBT stop time. NOTE: hk_info is a global variable.
        SetP(&hkl, "STOP_TIME",                   hk_info.utc_time_str,1);  // Update STOP_TIME in common PDS parameters
        sprintf(tstr1, "%d", hk_info.hk_cnt);
        SetP(&hkl, "FILE_RECORDS", tstr1, 1);                               // Set number of records
        SetP(&hkl, "ROWS",         tstr1, 1);                               // Set number of rows

        //====================================
        // Write to and close the HK LBL file
        //====================================
        FDumpPrp(&hkl,pds.hlabel_fd);                                       // Dump hkl to HK label file
        fprintf(pds.hlabel_fd,"END\r\n");
        fclose(pds.hlabel_fd);
        
        HPrintf("(ExitPDS:) Closed HK LBL file.");   // Useful for determining whether the above code closed the LBL file, or if DecodeHK did.
    }
    
    if(pds.itable_fd!=NULL) 
    {
        WriteIndexLBL(&ind,&mp);   // Write index label file
        fflush(pds.itable_fd);     // Flush and close
        fclose(pds.itable_fd);     // Close table file
    }


    int func_status = DestroyCalibMeas(pds.cpathd, pds.cpathocel, pds.cpathocet, &m_conv);
    if (func_status < 0) {
        YPrintf("Error when deleting unused CALIB_MEAS files or destroying CALIB_MEAS data structure. Function error code %i\n", func_status);
    }
    func_status = DestroyCalibCoeff(pds.cpathd, &calib_coeff_data);
    if (func_status != 0) {
        YPrintf("Error when deleting unused CALIB_COEFF files or destroying CALIB_COEFF data structure. Function error code %i\n", func_status);
    }

    
    if(comm.no_prop!=0 && comm.properties!=NULL) { // Something in comm structure?
        FreePrp(&comm);                           // Then free comm memory
    }
    
    //===========================================
    // Open DATASET.CAT and change some keywords
    //===========================================
    UpdateDATASET(&mp, &pds);

        
        
        /*=============================
         * NOTE: Bad indentation below
         ============================*/
            
        if(dict.no_prop!=0 && dict.properties!=NULL) // Something in dict structure?
        {
            //printf("%d %p\n",dict.no_prop,dict.properties);
            fflush(stdout);
            FreePrp(&dict);                           // Then free dict memory
        }
        
        if(anom.no_prop!=0 && anom.properties!=NULL) {  // Something in anom structure?
            FreePrp(&anom);                             // Then free anom memory
        }
        
            /*=============================
             * NOTE: Bad indentation below
             ============================*/
            
            FreeBuffs(&cbtm,&cbs,&cmb,&cbh); // Free circular buffers
            
            if(calib)
            {
                if(v_conv.C!=NULL) FreeDoubleMatrix(v_conv.C,v_conv.rows,v_conv.cols);
                if(i_conv.C!=NULL) FreeDoubleMatrix(i_conv.C,v_conv.rows,i_conv.cols);
                if(f_conv.C!=NULL) FreeDoubleMatrix(f_conv.C,f_conv.rows,f_conv.cols);
            }
            
            if(tcp.n_entries > 0)
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
            


            //================================
            // Changing group and permissions
            //================================
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
            
            //==============================================================================================
            // Start to compress log files
            // ---------------------------
            // NOTE: External shell commands cd, tar and gzip must exist!
            // NOTE: Using DATA_SET_ID in compressed logs file name makes the application a little bit more
            // fragile in case of error, since mp.data_set_id must have been successfully initialized.
            //==============================================================================================
            printf("Compressing log files, please wait\n");
            GetCurrentUtc0(tstr3);                                              // Get local UTC time.
            ReplCh(tstr3,':','#');                                              // Replace : in time string with #, since 
            // : is not allowed in a file name.

            strncpy(tstr2, mp.data_set_id, PATH_MAX);   // Copy to temporary variable.
            TrimQN(tstr2);
            sprintf(tstr1, "cd %s ; tar -czvf logs_%s___%s.tgz *.log", pds.lpath, tstr3, tstr2);   // Construct shell command line.
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




// Like YPrintf (Y) + printf (S=stdout).
// Intended for replacing many identical subsequent calls to YPrintf and printf.
int YSPrintf(const char *fmt, ...)
// PROPOSAL: Replace with function for error messages, EPrintf.
//      PRO: Can add standardized prefix(es).
//          Ex: "ERROR: "
//          Ex: Argument for the calling function.
{
    va_list args;    
    va_start(args,fmt);
    
    vprintf(fmt, args);                  // Equivalent of printf that uses va_list.
    int status = YPrintf2(fmt, args);    // Equivalent of YPrintf that uses va_list.
    
    va_end(args);    
    return status;
}


// Like printf but everything goes into PDS_LOG0.
// If it can't open, then messages are printed to stderr.
int YPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    
    int status = YPrintf2(fmt, args);
    
    va_end(args);
    return status;
}

int YPrintf2(const char *fmt, va_list args)
{
    int status;
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
                GetCurrentUtc0(strp); // Get universal time
                fprintf(pds.ylog_fd,"%s (UTC): ",strp);
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
    char strp[32];
    
    int oldstate;
    
    // We do not want to cancel on default cancel points in fprintf,vfprintf
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldstate); 
    
    if((status=pthread_mutex_lock(&protect_log))==0)
    {
        if(OpenLogFile(&pds.dlog_fd,PDS_LOG1,pds.ylog_fd)>=0)   // Open log file if not already opened
        {
            if(pds.dlog_fd!=NULL)
            {
                GetCurrentUtc0(strp); // Get universal time
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
                GetCurrentUtc0(strp); // Get universal time
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

// Like printf but everything goes into Science decoding log.
// Mostly used in the state machine and some related functions.
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
                GetCurrentUtc0(strp); // Get universal time
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
                GetCurrentUtc0(strp); // Get universal time
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



// Dump path information to system log.
int AddPathsToSystemLog(pds_type *p)
{
    YPrintf("LAP PDS System paths\n"); 
    YPrintf("Mission calendar file     : %s\n",    p->mcpath);
    YPrintf("LAP Macro desc            : %s\n",    p->macrop);
    YPrintf("DataSetVersion            : %3.1f\n", p->DataSetVersion);
    YPrintf("DPL Number                : %d\n",    p->DPLNumber);
    YPrintf("PDS Archive               : %s\n",    p->apathpds);
    YPrintf("DDS Archive               : %s\n",    p->apathdds);
    YPrintf("LOG Path                  : %s\n",    p->lpath);
    YPrintf("UnAccepted Path           : %s\n",    p->uapath);
    YPrintf("Path to SPICE metakernel  : %s\n",    p->pathmk);
    
    YPrintf("Path to calibration files : %s\n", p->cpathd);
    YPrintf(" Path to coarse voltage bias file : %s\n", p->cpathc);
    YPrintf(" Path to fine voltage bias file   : %s\n", p->cpathf);
    YPrintf(" Path to current bias file        : %s\n", p->cpathi);
    YPrintf(" CALIB_MEAS offset files          : %s\n", p->cpathm);
    
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
 * opt  : Flag (string)
 * argv : (Remaining) command-line arguments.
 * argc : Number of command-line arguments (length of argv array)
 * arg  : If-and-only-if non-null, then *arg will be assigned to the string value of the command-line argument that comes after the flag.
 *
 * Return value : 0=failed, 1=success.
 */
int GetOption(char *opt, int argc, char *argv[], char *arg)
{
    int i;
    for(i=1;i<argc;i++)
    {
        if ((argv[i] != NULL) && !strncmp(argv[i], opt, MAX_STR))
        {
            // CASE: Found the option.
            if (arg==NULL)
            {
                // CASE: We do NOT expect an associated extra argument.
                argv[i] = NULL;
                return 1;
            }
            else
            {
                // CASE: We DO expect an extra associated argument.
                if ((i+1<argc) && (argv[i+1]!=NULL))   // If the argument list contains an (unused) argument after this option (flag)...
                {
                    strncpy(arg, argv[i+1], MAX_STR); // Copy next entry as argument
                    argv[i] = NULL;
                    argv[i+1] = NULL;
                    return 1;     // CASE: We found the extra argument.
                }
                else
                {
                    return 0;     // CASE: We did NOT find the extra argument.
                }
            }
        }
    }
    return 0;     // CASE: Did NOT find the option.
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

/* Prepare the reading of a datafile by
 * (1) opening generic text file (for reading),
 * (2) counting the number of rows with data, 
 * (3) rewinding file descriptor, 
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
        if (row_str[0] == ' ')  continue;   // Ignore line beginning with whitespace.
        N_rows_p++;
    }
    *N_rows = N_rows_p;

    rewind(*file_descr);   // Rewind index label to beginning of file.
    return 0;
}



// Load first part of configuration file
int  LoadConfig1(pds_type *p) // Loads configuration information
{ 
    FILE *fd;
    
    char line[PATH_MAX];    // Text line
    char l_str[PATH_MAX];   // Left string
    char r_str[PATH_MAX];   // Right string
    
    printf( "Loading first part of configuration file: %s\n\n",p->cpath);
    YPrintf("Loading first part of configuration file: %s\n\n",p->cpath);
    
    if((fd=fopen(p->cpath,"r"))==NULL)
    {
        perror(p->cpath);
        return -1;
    }
    
    // Remove initial comments..
    while(fgets(line,PATH_MAX,fd) != NULL) {
        if (line[0] == '%') {
            continue; 
        } else {
            break;
        }
    }

    Separate(line,l_str,r_str,'%',1);   TrimWN(l_str);
    sscanf(l_str,"%d",&p->SCResetCounter);   printf("SCResetCounter              : %d\n",p->SCResetCounter);
    
    fgets(line,PATH_MAX,fd);   Separate(line,l_str,r_str,'%',1);   TrimWN(l_str);
    strncpy(p->SCResetClock,l_str,20);       printf("ResetClock                  : %s\n",p->SCResetClock);
    
    
    fgets(line,PATH_MAX,fd);   Separate(line,l_str,r_str,'%',1);   TrimWN(l_str);
    strncpy(p->templp,l_str,PATH_MAX);
    if(SetupPath("Path to PDS archive template",p->templp)<0)  return -2;  // Test if path exists
    
    fgets(line,PATH_MAX,fd);   Separate(line,l_str,r_str,'%',1);  TrimWN(l_str);
    strncpy(p->macrop,l_str,PATH_MAX);
    if(SetupPath("LAP Macro descriptions path ",p->macrop)<0)  return -3;  // Test if path exists
    
    fgets(line,PATH_MAX,fd);   Separate(line,l_str,r_str,'%',1);   TrimWN(l_str);
    strncpy(p->LabelRevNote,l_str,PATH_MAX);   printf("Label Revision Note         : %s\n",p->LabelRevNote);
    
    fgets(line,PATH_MAX,fd);   Separate(line,l_str,r_str,'%',1);   TrimWN(l_str);
    
    l_str[10]='\0';
    strncpy(p->ReleaseDate,l_str,11);   printf("Release Date                : %s\n",p->ReleaseDate);
    
    fgets(line,PATH_MAX,fd);   Separate(line,l_str,r_str,'%',1);   TrimWN(l_str);
    strncpy(p->mcpath,l_str,PATH_MAX);    printf("Mission calendar file       : %s\n",p->mcpath);
    
    fclose(fd); // Close config file
    
    return 0;
}



/**
 * Load second part of configuration file
 *
 * Sets among others: p->apathpds_parent = Parent dir of dataset. NOTE: Only set if pre-existing string is empty.
 *                    p->apathpds        = PDS dataset path (out data)
 *                    p->pathmk          = Path to SPICE metakernel
 *
 * NOTE: The function name is deceiving. The function does more than just read the configuration file.
 * NOTE: COPIES TEMPLATE DIRECTORY TO CREATE THE DATA SET DIRECTORY, and probably more.
 */
int LoadConfig2(pds_type *p, char *data_set_id)
{ 
    FILE *fd;
    FILE *pipe_fp;
    int len;
    
    char line[PATH_MAX];    // Text line
    char l_str[PATH_MAX];   // Left string
    char r_str[PATH_MAX];   // Right string
    char tstr[PATH_MAX];
    
    struct timespec dose={0,DOSE_TIME};  
    
    printf( "\nLoading second part of configuration file: %s\n",p->cpath);
    YPrintf("\nLoading second part of configuration file: %s\n",p->cpath);
    
    if((fd=fopen(p->cpath,"r"))==NULL)
    {
        perror(p->cpath);
        return -1;
    }
    
    // Remove initial comments..
    while(fgets(line,PATH_MAX,fd) != NULL) {
        if (line[0] == '%') 
            continue; 
        else
            break;
    }
        
    // Skip first part already loaded in LoadConfig1
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    fgets(line,PATH_MAX,fd);
    
    fgets(line, PATH_MAX, fd);
    Separate(line, l_str, r_str, '%', 1);

    // Assign dataset parent path unless there already is a value.
    if (strlen(p->apathpds_parent) == 0) {
        TrimWN(l_str);
        strncpy(p->apathpds_parent, l_str, PATH_MAX);
    }    
    if(SetupPath("PDS data set parent path    ", p->apathpds_parent)<0) return -2;   // NOTE: SetupPath adds trailing slash if necessary.
    
    sprintf(p->apathpds,"%s%s/", p->apathpds_parent, data_set_id);    // Assigns final value to p->apathpds.
    
    
    printf(    "\nCreate archive path:         %s\n",p->apathpds);
    YPrintf(   "\nCreate archive path:         %s\n",p->apathpds);
    
    if(mkdir(p->apathpds,0775)<0)
    {
        printf( "Can not create archive path: %s\n",p->apathpds);
        YPrintf("Can not create archive path: %s\n",p->apathpds);
        return -3;
    }
    if(SetupPath("PDS data set path           ", p->apathpds)<0) return -2;     // NOTE: Checks if created directory exists.
    
    printf( "Copying template archive to:  %s\n",p->apathpds);
    YPrintf("Copying template archive to:  %s\n",p->apathpds);
    
    sprintf(tstr,"cp -r %s* %s", p->templp, p->apathpds); // Setup shell command line.
    printf( "%s\n",tstr);
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
    
    fgets(line,PATH_MAX,fd);Separate(line,l_str,r_str,'%',1);
    TrimWN(l_str);
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

    // Trying to remove preexisting DATA/EDITED, DATA/CALIBRATED? Unnecessary?
    sprintf(p->dpathse,"%sEDITED/",line); 
    sprintf(p->dpathsc,"%sCALIBRATED/",line);
    rmdir(p->dpathsc);
    rmdir(p->dpathse);

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

    if(calib) {
        strcpy(p->dpathse,p->dpathsc); // Override edited!...
    }

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

    if(fgets(line,PATH_MAX,fd)==NULL) return -23; // Read line to be ignored since it is used by lap_agility.

    if(fgets(line,PATH_MAX,fd)==NULL) return -24;
    Separate(line,l_str,r_str,'%',1);TrimWN(l_str);
    strncpy(p->pathmk, l_str, PATH_MAX);          // Set SPICE metakernel path.
    
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
} // LoadConfig2



// Load anomaly description file into a linked list of properties.
int  LoadAnomalies(prp_type *p,char *path)
{
    FILE *fd;
    char line[256]; // Line buffer
    char t_tok[80]; // Time token
    char m_tok[80]; // Macro/Select token
    
    
    printf( "Loading anomaly file: %s\n", path);
    YPrintf("Loading anomaly file: %s\n", path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    while (fgets(line,255,fd) != NULL)
    {
        if (line[0] == '\n') continue;   // Empty line..
        if (line[0] == '#')  continue;   // Remove comments..
        Separate(line, t_tok, m_tok, '\t', 1);   // Separate at first occurrence of a tab character
        TrimWN(m_tok);
        InsertTopK(p, t_tok, m_tok);   // Insert into linked list of property name value pairs.
    }
    fclose(fd);
    return 0;
}



// Load human mode descriptions into a linked list of properties.
int  LoadModeDesc(prp_type *p,char *path)
{
    FILE *fd;
    char line[MAX_STR];   // Line buffer
    char d_tok[MAX_STR];  // Description
    char m_tok[MAX_STR];  // Macro mode
    
    
    printf( "Loading human description of macro modes, file: %s\n",path);
    YPrintf("Loading human description of macro modes, file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    while (fgets(line, MAX_STR, fd) != NULL)
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



// Load bias settings = Load file with commanded biases (pds.bias?)
// 
// Output : bias_s     : 2D array of integers, size bias_cnt_s x 3. Contains content of rows where second column is NOT "*Mode*".
//                       [i][0] = First  file column value = time (time_t).
//                       [i][1] = Second file column value = voltage bias (high byte=P1, low byte=P2)
//                       [i][2] = Third  file column value = current bias (high byte=P1, low byte=P2)
// Output : mode_s     : 2D array of integers, size mode_cnt_s x 2. Contains content of rows where second column IS "*Mode*".
//                       [i][0] = First file column value = time (time_t, ConvertUtc2Timet).
//                       [i][1] = THIRD file column value. (Second column value ="*Mode*")
// Output : bias_cnt_s
// Output : mode_cnt_s
// 
int LoadBias(unsigned int ***bias_s,unsigned int ***mode_s,int *bias_cnt_s,int *mode_cnt_s,char *path)
{
// Excerpt from pds.bias:
// ----------------------
// # TIME|TAB|DENSITY P1P2|TAB|EFIELD P2P1|
// #
// 2004-03-21T20:24:39.000 *Mode*  0x0011
// 2004-05-08T11:42:50.000 0xd0d0  0x0000
// 2004-05-08T16:47:51.000 0xd0d0  0x0000
// 2004-05-08T21:30:40.387 *Mode*  0x000c

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
    
    printf( "Loading bias file: %s\n",path);
    YPrintf("Loading bias file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    
    //=======================
    // Count bias table rows
    //=======================
    while(fgets(line,255,fd) != NULL)
    {
        if (line[0] == '\n') continue;   // Empty line.
        if (line[0] == '#')  continue;   // Remove comments.
        if (line[0] == ' ')  continue;   // Ignore whitespace line.
        if (strstr(line,"*Mode*")==NULL)
            bias_cnt++;
        else
            mode_cnt++;
    }
    rewind(fd); // Rewind index label to start
    
    
    
    bias=CallocIntMatrix(bias_cnt,3);   // Allocate a matrix of integers.
    mode=CallocIntMatrix(mode_cnt,2);

    
    
    //========================================
    // Fill "bias" and "mode" and with values
    //========================================
    bias_cnt=0;
    mode_cnt=0;
    while (fgets(line,255,fd) != NULL)
    {
        if (line[0] == '\n') continue;   // Empty line.
        if (line[0] == '#')  continue;   // Remove comments.
        if (line[0] == ' ')  continue;   // Ignore whitespace line.
        Separate(line,l_tok,m_tok,'\t',1);   // Separate at first occurrence of a tab character.
        Separate(line,m_tok,r_tok,'\t',2);   // Separate at second occurrence of a tab character.
        ConvertUtc2Timet(l_tok, &t);         // Convert time to seconds
        
        if(strstr(m_tok,"*Mode*")==NULL)
        {
            sscanf(m_tok,"%x",&de);
            sscanf(r_tok,"%x",&ef);
            
            bias[bias_cnt][0] = (unsigned int) t; // Safe since time_t is a unsigned int!
            bias[bias_cnt][1] = de;
            bias[bias_cnt][2] = ef;
            bias_cnt++;
        }
        else
        {
            sscanf(r_tok,"%x",&mde);
            
            mode[mode_cnt][0] = (unsigned int) t; // Safe since time_t is a unsigned int!
            mode[mode_cnt][1] = mde;
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



/* Load list of macros that should only be present in EDITED but not in CALIB.
 * This feature is intended for calibration macros.
 * 
 * path : Path to text file to load.
 * exclude : List of macro numbers.
 * 
 * NOTE: Ignores lines beginning with "#", LF (empty lines), and whitespace.
 */
int LoadExclude(unsigned int **exclude, char *path) // Load exclude file
{
    FILE *fd;
    char line[256]; // Line buffer
    unsigned int macro;
    unsigned int *tmp_e;
    
    int excl_cnt=0;
    
    printf( "Loading macro exclude file: %s\n",path);
    YPrintf("Loading macro exclude file: %s\n",path);
    
    if((fd=fopen(path,"r"))==NULL)
    {
        perror(path);
        return -1;
    }
    
    // Count exclude table lines
    while(fgets(line,255,fd) != NULL)
    {
        if (line[0] == '\n') continue;  // Empty line.
        if (line[0] == '#' ) continue;  // Remove comments.
        if (line[0] == ' ' ) continue;  // Ignore whitespace line.
        excl_cnt++;
    }
    
    rewind(fd); // Rewind index label to start
    
    tmp_e=(unsigned int *)CallocArray(excl_cnt,sizeof(unsigned int)); // Allocate 
    
    excl_cnt=0;
    while(fgets(line,255,fd) != NULL)
    {
        if (line[0] == '\n') continue;  // Empty line.
        if (line[0] == '#' ) continue;  // Remove comments.
        if (line[0] == ' ' ) continue;  // Ignore whitespace line.
        sscanf(line,"%x",&macro);
        tmp_e[excl_cnt]=macro;
        excl_cnt++;
    }
    fclose(fd);
    
    *exclude=tmp_e;
    return excl_cnt;
}



/* Erik P G Johansson 2015-03-25: Created function
 * 
 * Load file with time intervals for which data should be excluded.
 * Every line in the data exclude file specifies one time interval.
 *
 * NOTE: Time is given in the form of spacecraft clock counts,
 * like SPACECRAFT_CLOCK_START_COUNT and SPACECRAFT_CLOCK_STOP_COUNT.
 * The reasons for using this format are:
 * 1) Makes bugs more unlikely
 * 2) One wants to use a time designation that is as absolute/permanent as possible
 *    and therefore independent of time conversions, in particular time corrections,
 *    and therefore as close to the packet data times as possible.
 *    If one had not and the time conversions changed, then one would have to update
 *    the data exclude list.
 * 3) The code only partly contains functions for conversion from human-readable
 *    strings to spacecraft clock counter?
 *
 * NOTE: Current implementation does not return any information in the event of failure
 * making soft error harder for caller.
 * NOTE: One could remove time intervals that are outside the time limits of the current
 * mission phase to speed up comparisons, but that has not been implemented (2015-03-26).
 * Doubtful how useful that would be.
 */
int LoadDataExcludeTimes(data_exclude_times_type **dataExcludeTimes, char *depath) {
// PROPOSAL: Read macro ID.
//      PRO: Can include the 910 P1 HF, 710 P2 HF data exclusions.
//          CON: Can not, since that applies to all times.
//              PRO: Can do, since can give LONG time interval.

    FILE   *fd;
    char   line[MAX_STR];   // Line buffer
    int    i = 0;
    int    scrc_begin = -1;    // SCRC = Spacecraft reset counter
    double sccd_begin = 0.0;
    double sccd_end   = 0.0;
    char sccs_begin[MAX_STR];
    char sccs_end[MAX_STR];
    char utc_begin[MAX_STR];
    char utc_end[MAX_STR];
    char probe_constraint_str[MAX_STR];
    int  probe_constraint = -1;
    char data_type_constraint_str[MAX_STR];
    int  data_type_constraint = -1;
    data_exclude_times_type dataExcludeTimes_temp;

    YPrintf("Loading data exclude times file: %s\n", depath);  // Print to "pds system log".
    
    *dataExcludeTimes = (data_exclude_times_type*) NULL;   // Default value to be returned to the caller in case of error.

    if (OpenFileCountDataRows(depath, &fd, &(dataExcludeTimes_temp.N_intervals))) {
        YSPrintf("ERROR: LoadDataExcludeTimes: Can not find file: \"%s\"\n", depath);
        return -1;
    }

    dataExcludeTimes_temp.scrc_begin_list           = (int *)    CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(int));
    dataExcludeTimes_temp.sccd_begin_list           = (double *) CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(double));
    dataExcludeTimes_temp.sccd_end_list             = (double *) CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(double));
    dataExcludeTimes_temp.probe_constraint_list     = (int *)    CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(int));
    dataExcludeTimes_temp.data_type_constraint_list = (int *)    CallocArray(dataExcludeTimes_temp.N_intervals, sizeof(int));

    i = 0;
    YPrintf("Ingested data exclude time intervals:\n");  // Print to "pds system log".
    while(fgets(line, MAX_STR, fd) != NULL)  // NOTE: "line" will end with a \n.
    {
        if (line[0] == '\n') continue; // Ignore empty line.
        if (line[0] == '#')  continue; // Ignore comments.
        if (line[0] == ' ')  continue; // Ignore whitespace line
        
        //============================
        // Interpret one line of data
        //============================
        // NOTE: This defines the columns and part of the file format/syntax.
//         if (sscanf(line, " %s %s ", l_tok, r_tok) != 2)   // Whitespace represent any sequence of whitespace and tab (incl. empty).
        if (sscanf(line, " %[^,], %[^,], %[^,], %[^,]", sccs_begin, sccs_end, probe_constraint_str, data_type_constraint_str) != 4)
        {
            YSPrintf("ERROR: LoadDataExcludeTimes: Can not interpret line in data exclude times file (sscanf): \"%s\"\n", line);
            fclose(fd);   return -1;
        }
        TrimWN(sccs_begin);   // Remove leading and trailing whitespace, remove CR (if any). Can be important depending on the exact chosen file format (future PDS compliant).
        TrimWN(sccs_end);
        TrimWN(probe_constraint_str);
        TrimWN(data_type_constraint_str);   // Necessary to deal with end-of-line?
        
        // Convert SCCS --> SCCD.
        if (ConvertSccs2Sccd(sccs_begin, &scrc_begin, &sccd_begin)) {
            YSPrintf("ERROR: LoadDataExcludeTimes: Can not interpret interval BEGINNING in data exclude times file: \"%s\"\n", sccs_begin);
            fclose(fd);   return -10;
        }
        if (ConvertSccs2Sccd(sccs_end, NULL, &sccd_end)) {    // NOTE: Does not read S/C clock reset counter.
            YSPrintf("ERROR: LoadDataExcludeTimes: Can not interpret interval END in data exclude times file: \"%s\"\n", sccs_end);
            fclose(fd);   return -11;
        }
        
        // Convert SCCD --> UTC -- Only for log messages.
        if (ConvertSccd2Utc(sccd_begin, NULL, utc_begin)) {
            YSPrintf("ERROR: LoadDataExcludeTimes: Can not convert interval BEGINNING in data exclude times file: \"%f\"\n", sccd_begin);
            fclose(fd);   return -12;
        }
        if (ConvertSccd2Utc(sccd_end, NULL, utc_end)) {
            YSPrintf("ERROR: LoadDataExcludeTimes: Can not convert interval END in data exclude times file: \"%f\"\n", sccd_end);
            fclose(fd);   return -13;
        }

        // ASSERTIONS
        if (sccd_begin > sccd_end) {
            YSPrintf("ERROR: LoadDataExcludeTimes: Found time interval that runs backwards (sccd_begin > sccd_end) in data exclude times file.\n");
            fclose(fd);   return -20;
        }
        
        // Translate probe_constraint_str --> probe_constraint + ASSERTION
        if      (!strcmp(probe_constraint_str, "ALL_PROBES")) {   probe_constraint = PROBE_CONSTRAINT_NONE;  }
        else if (!strcmp(probe_constraint_str, "P1"))         {   probe_constraint = SENS_P1;      }
        else if (!strcmp(probe_constraint_str, "P2"))         {   probe_constraint = SENS_P2;      }
        else if (!strcmp(probe_constraint_str, "P3"))         {   probe_constraint = SENS_P1P2;    }
        else {
            YSPrintf("ERROR: Illegal probe_constraint_str=\"%s\" at i=%i.\n", probe_constraint_str, i);
            fclose(fd);   return -21;
        }
        // Translate data_type_constraint_str --> data_type_constraint + ASSERTION
        if      (!strcmp(data_type_constraint_str, "ALL_DATA_TYPES")) {   data_type_constraint = DATA_TYPE_CONSTRAINT_NONE;    }
        else if (!strcmp(data_type_constraint_str, "SWEEPS"))         {   data_type_constraint = DATA_TYPE_CONSTRAINT_SWEEP;   }
        else if (!strcmp(data_type_constraint_str, "LF"))             {   data_type_constraint = DATA_TYPE_CONSTRAINT_LF;      }
        else if (!strcmp(data_type_constraint_str, "HF"))             {   data_type_constraint = DATA_TYPE_CONSTRAINT_HF;      }
        else {
            YSPrintf("ERROR: Illegal data_type_constraint_str=\"%s\" at i=%i.\n", data_type_constraint_str, i);
            fclose(fd);   return -22;
        }

        dataExcludeTimes_temp.scrc_begin_list[i]           = scrc_begin;
        dataExcludeTimes_temp.sccd_begin_list[i]           = sccd_begin;
        dataExcludeTimes_temp.sccd_end_list[i]             = sccd_end;
        dataExcludeTimes_temp.probe_constraint_list[i]     = probe_constraint;
        dataExcludeTimes_temp.data_type_constraint_list[i] = data_type_constraint;

        // Print table contents to "pds system log".
        // NOTE: Preceded by "header row.
        // Useful for double-checking that the code interprets the file correctly.
        //YPrintf("line = %s");
        YPrintf("    %-18s %-18s %18f %18f %26s %26s %1i %1i\n", sccs_begin, sccs_end, sccd_begin, sccd_end, utc_begin, utc_end, probe_constraint, data_type_constraint);

        i++;
    }
    
    // Check that the two different counts of time intervals agree to detect bugs, especially when changing the file format.
    // NOTE: Could possibly remove this code.
    if (i != dataExcludeTimes_temp.N_intervals) {
        YSPrintf("ERROR: LoadDataExcludeTimes: Implementation bug. Can not obtain the number of time intervals.\n");  // Print to "pds system log".
        fclose(fd);   return -30;
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
 * Checks whether a SCI LBL/TAB file pair should be created at all depending on the time
 * covered by the files, and other parameters.
 * NOTE: Function uses (and should use) TM times, i.e. not corrected for ADC20 group delay.
 * NOTE: Not intended for HK.
 * 
 * 
 * ARGUMENTS
 * =========
 * INPUT  : curr. Function only uses TM time designations.
 * OUTPUT : *shouldExcludeFilePair    : false = Do not exclude data.
 *                                      true  = Exclude data.
 * RETURN VALUE: 0 (no error), -1 (error).
 *
 * 
 * ASSUMES: Spacecraft reset counter is the same for all times involved in the algorithm (or something in that direction...).
 * 
 * 
 * ALGORITHM FOR TIME
 * ==================
 * Excludes data if-and-only-if
 *          sccd_end >= sccd_exclude_begin
 *      and
 *          sccd_begin <= sccd_exclude_end.
 *      Note the less-than-or-EQUAL.
 *--------------------------------------------------------------------------------------------*/
int DecideWhetherToExcludeData(
    data_exclude_times_type *dataExcludeTimes,
    curr_type curr,
    int param_type,
    sweep_type *sw_info,
    unsigned int macro_id,
    int dop,
    int *shouldExcludeFilePair)
{
    int i = -1;

    const double sccd_file_begin = curr.seq_time_TM;
    const double sccd_file_end   = curr.stop_time_TM;
    const int writing_P1_data = (curr.sensor==SENS_P1   || dop==1);
    const int writing_P2_data = (curr.sensor==SENS_P2   || dop==2);
    const int writing_P3_data = (curr.sensor==SENS_P1P2 && dop==0);
    
    //================================================================================================================================
    void PrintExcludedFilePairData_LOCAL() {
        // PROPOSAL: Print more info: probe, macro ID, ...
        char sccs_begin[MAX_STR];
        char sccs_end  [MAX_STR];
        char utc_begin[MAX_STR];
        char utc_end  [MAX_STR];
        
        ConvertSccd2Sccs(sccd_file_begin, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_begin, FALSE);   // No error handling needed.
        ConvertSccd2Sccs(sccd_file_end,   ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_end,   FALSE);
        
        if ((ConvertSccd2Utc(sccd_file_begin, utc_begin, NULL) != 0) ||
            (ConvertSccd2Utc(sccd_file_end,   utc_end,   NULL) != 0)) {
            YSPrintf("ERROR: DecideWhetherToExcludeData: Can not convert either sccd_file_begin=%f or sccd_file_end=%f.\n", sccd_file_begin, sccd_file_end);
            ExitPDS(1);
        }
        
        CPrintf("DecideWhetherToExcludeData: File pair (TAB+LBL) for below parameters should be excluded:\n");
        CPrintf("    (Note: Stated times not modified for group delay, ADC20_DELAY_S.)\n");
//         CPrintf("    sccd_file_begin = %f\n", sccd_file_begin);
//         CPrintf("    sccd_file_end   = %f\n", sccd_file_end  );
        CPrintf("    SPACECRAFT_CLOCK_START_COUNT = sccs_begin = %s\n", sccs_begin);
        CPrintf("    SPACECRAFT_CLOCK_STOP_COUNT  = sccs_end   = %s\n", sccs_end  );
        CPrintf("    START_TIME                   = utc_begin  = %s\n", utc_begin);
        CPrintf("    STOP_TIME                    = utc_end    = %s\n", utc_end  );
        CPrintf("    macro_id    = 0x%04x\n", macro_id);
        CPrintf("    param_type  = %i\n",     param_type);
        CPrintf("    dop         = %i\n",     dop);
        CPrintf("    curr.sensor = %i\n",     curr.sensor);
    }
    //================================================================================================================================



    // ASSERTION
    if (dataExcludeTimes == NULL) {
        YSPrintf( "ERROR: DecideWhetherToExcludeData: Can not find data in dataExcludeTimes.\n");
        ExitPDS(1);   // return -1;  ??
    }



    //========================================================================================================
    // Check if the file data overlaps with a specific data exclusion time interval.
    // If it does, then it is not necessary to check with other data exclusion intervals.
    // NOTE: Uses ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER since no reset counter is submitted to the function.
    //========================================================================================================
    for (i=0; i<dataExcludeTimes->N_intervals; i++) {
        if (   (ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER == dataExcludeTimes->scrc_begin_list[i])
            && (sccd_file_end   >= dataExcludeTimes->sccd_begin_list[i])
            && (sccd_file_begin <= dataExcludeTimes->sccd_end_list[i])
        )
        {
            const int probe_nbr =
                writing_P1_data * SENS_P1 +
                writing_P2_data * SENS_P2 +
                writing_P3_data * SENS_P1P2;
            
            const int data_type =
                (param_type==SWEEP_PARAMS) * DATA_TYPE_CONSTRAINT_SWEEP +
                (param_type==NO_PARAMS   ) * DATA_TYPE_CONSTRAINT_HF +
                (param_type==ADC20_PARAMS) * DATA_TYPE_CONSTRAINT_LF;

            const int probe_constraint     = dataExcludeTimes->probe_constraint_list[i];
            const int data_type_constraint = dataExcludeTimes->data_type_constraint_list[i];
            if  (
                    (
                            (probe_constraint == PROBE_CONSTRAINT_NONE)
                        ||
                            (probe_constraint == probe_nbr)
                    )
                &&
                    (
                            (data_type_constraint == DATA_TYPE_CONSTRAINT_NONE)
                        ||
                            (data_type_constraint == data_type)
                    )
                )
            {
                CPrintf("DecideWhetherToExcludeData: Matching data exclusion rule (i=%i).\n", i);
                PrintExcludedFilePairData_LOCAL();
                *shouldExcludeFilePair = TRUE;
                return 0;
            }   // if
        }   // if
    }   // for

    //===========================================
    // Exclude files for specific cases of data:
    // (1) macro 710 P2 HF
    // (2) macro 910 P1 HF
    //===========================================
    if ((param_type!=SWEEP_PARAMS) && (param_type!=ADC20_PARAMS)) {
        // CASE: HF data (not sweep, not ADC20). Conditions taken from the filenaming code.
        // param_type=NO_PARAMS might work instead judging from the source code, since param_type only appears to
        // only take on three values.
        if ((writing_P1_data) && (macro_id==0x0910)) {
            CPrintf("DecideWhetherToExcludeData: Matching macro 910 P1 HF.\n");
            PrintExcludedFilePairData_LOCAL();
            *shouldExcludeFilePair = TRUE;
            return 0;
        }
        if ((writing_P2_data) && (macro_id==0x0710)) {
            CPrintf("DecideWhetherToExcludeData: Matching macro 710 P2 HF.\n");
            PrintExcludedFilePairData_LOCAL();
            *shouldExcludeFilePair = TRUE;
            return 0;
        }
    }   // if
    
    //==============================================================
    // Exclude all FINE sweeps from CALIB (but keep them in EDITED)
    //==============================================================
    if (EXCLUDE_FINE_SWEEPS_FROM_CALIB) {
        if (calib && (param_type==SWEEP_PARAMS) && (!strcmp(sw_info->resolution,"FINE"))) {
            // IMPLEMENTATION NOTE: Calls strcmp as seldomly as possible.
            // IMPLEMENTATION NOTE: Important exclamation mark/negation for "strcmp".
            CPrintf("DecideWhetherToExcludeData: Excluding FINE sweep from CALIB datasets.\n");
            *shouldExcludeFilePair = TRUE;
            return 0;
        }
    }



    *shouldExcludeFilePair = FALSE;
    return 0;
}



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
//  int n_entries;        // Number of entries
//  unsigned int *SCET;   // Time correlated OBT from which correlation below is valid
//  double *offset;       // Offset
//  double *gradient;     // Gradient
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
    int N_bytes;
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
            
            
            ConvertUtc2Timet(newdate,&etime);
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
    //  int n_entries;        // Number of entries
    //  unsigned int *SCET;   // Time correlated OBT from which correlation below is valid
    //  double *offset;       // Offset
    //  double *gradient;     // Gradient
    // } tc_type;
    //
    tcp->n_entries=(tlen/TIME_CORR_PTOT_SIZE); // Total number of time correlation packets
    
    // Allocate storage
    tcp->SCET     = (double *) CallocArray(tcp->n_entries, sizeof(double));
    tcp->offset   = (double *) CallocArray(tcp->n_entries, sizeof(double));
    tcp->gradient = (double *) CallocArray(tcp->n_entries, sizeof(double));
    
    n=0;
    while(!feof(fd) && n<tcp->n_entries)
    {
        len=0;
        // Read file loop, most of the time only one read will be done
        // but if an error occurs we do try again to read the rest..just good practice!
        while(len<TIME_CORR_PTOT_SIZE && !feof(fd))
        {
            len+=fread(&buff[len],1,TIME_CORR_PTOT_SIZE-len,fd);
            nanosleep(&nap,NULL); // Be nice                         
        }
        
        tcp->SCET[n]=DecodeDDSTime2Timet(buff); // Get time of DDS packet
        
        N_bytes=(buff[8]<<24 | buff[9]<<16 | buff[10]<<8 | buff[11]);
        
        if(N_bytes!=TIME_CORR_P_SIZE)
        {
            YPrintf("Corrupted time packet length: %d\n",N_bytes);
            return -3;
        }
        
        tcp->gradient[n] = GetDBigE(&buff[18]);
        tcp->offset[n]   = GetDBigE(&buff[26]);
        n++;
    }
    
    fclose(fd);
    return 0;
}



/**
 * Load all macro descriptions (.mds files) into memory.
 *
 * NOTE: Iterates over all possible filenames (that fit the filename pattern) and tries to load each one of them.
 * NOTE: *.mds files: The left-most numbers appear to be ignored, but a TAB is still required before the actual "variable assignment".
 * NOTE: Adds prefix "ROSETTA:" to all keys (in property lists).
*/
int LoadMacroDesc(prp_type macs[][MAX_MACROS_INBL],char *home) // Load all macro descriptions
{
    FILE *mac_fd;    // File desc
    int m_bl,m_n;    // macro block & macro number
    char path[256];  // Path + filename
    char line[256];  // Line buffer

    int macro_descr_error = 0;
    int state=0;     // State variable: 0=Not found <START> tag yet. 1=Found <START> tag, but not <END> tag. 2=Found <START> tag and an <END> tag afterwards.
    char l_tok[80];  // line nr token
    char t_tok[80];  // temp    token
    char n_tok[80];  // name    token
    char v_tok[80];  // value   token
    
    int n_macs=0;
    
    printf("Loading detailed macro descriptions\n");
    for(m_bl=0;m_bl<MAX_MACRO_BLCKS;m_bl++)   // All macro blocks
    {
        for(m_n=0;m_n<MAX_MACROS_INBL;m_n++)   // All macro numbers
        {
            sprintf(path, "%sPRG_B%d_M%d.mds", home, m_bl, m_n);
            if((mac_fd=fopen(path,"r"))==NULL) {
                continue;
            }
            YPrintf("Loading detailed macro desc: %s, Length: %d\n", path, FileLen(mac_fd));
            
            
            n_macs++;
            while (fgets(line,255,mac_fd) != NULL)
            {
                if(line[0] == '\n') continue;   // Empty line.
                if(line[0] == '#' ) continue;   // Remove comments.
                
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
                        
                        Separate(line, l_tok, t_tok, '\t', 1);    // NOTE: Requires (exactly one) TAB between first and second column.
                        Separate(t_tok, n_tok, v_tok, '=', 1);
                        //TrimWN(l_tok);   // Meaningless? l_tok is never used(?)
                        TrimWN(n_tok);
                        TrimWN(v_tok);

                        /** 1) Specifically check for values NOT beginning with quote. An old version of MEDS, used for generating .mds files,
                         * generates faulty .mds files without quotes (plus other things).
                         * pds can crash(!) if a faulty macro description (with unquoted values) is used by the data.
                         * 2) Checks for .mds keywords generated with older version of MEDS. The keywords have since been replaced (E-FIELD --> EFIELD).
                         * Not substituting them leads to pds not being able to explicitly read the value and not putting the keyword in the LBL files
                         * (empirically, from testing to generate otherwise identical data sets).
                         */
                        if ((v_tok[0] != '"') || !strcmp(n_tok, "LAP_P1_E-FIELD_FIX_DURATION") || !strcmp(n_tok, "LAP_P2_E-FIELD_FIX_DURATION")) {
                            macro_descr_error = 1;
                        }

                        sprintf(line, "ROSETTA:%s", n_tok);   // New PDS standard! Add prefix "ROSETTA:" to key value.
                        if((&macs[m_bl][m_n])!=NULL) {
                            // Print what was read.
                            //YPrintf("   %-30s = %s\n", n_tok, v_tok);     // NOTE: Does not print prefix "ROSETTA:" since it only repeats.

//                             YSPrintf("LoadMacroDesc: 8\n");   // DEBUG
                            Append(&macs[m_bl][m_n],line, v_tok);
//                             YSPrintf("LoadMacroDesc: 9\n");   // DEBUG
                        }
                        
                        break;
                }    // switch
            }    // while
            
            if(state==0) YPrintf("Warning: couldn't find <START> tag\n");
            if(state==1) YPrintf("Warning: couldn't find <END> tag\n");
            if(macro_descr_error) {
                YPrintf("    WARNING: Erroneous file contents. Hint: Could be .mds files generated with old version of MEDS.\n");
            }

            state = 0; // Restore state
            macro_descr_error = 0;
            fclose(mac_fd);
        }    // for
    }    // for

    return n_macs;
}



// Run shell command.
// NOTE: There is some code that could be replaced by a call to this function (search for "popen").
// NOTE: Current implementation does not return any error code.
void RunShellCommand(char *command_str)
{
    struct timespec dose={0, DOSE_TIME};
    FILE *pipe_fp;
    char tstr[PATH_MAX];    // NOTE: Buffer length should probably be the same in the fread/fgets command.
//     int len;
    
    /* Documentation "If mode is r, when the child process is started, its file descriptor STDOUT_FILENO shall be the writable end of the pipe,
     * and the file descriptor fileno(stream) in the calling process, where stream is the stream pointer returned by popen(), shall be
     * the readable end of the pipe.
     */
    pipe_fp = popen(command_str, "r");   // Runs shell command(s)
    while (!feof(pipe_fp))               // Read out stdout from executing shell commands
    {
        
        // Inherited implementation copied from other calls to popen.
        // Only prints one character of output for unknown reason.
        // /Erik P G Johansson 2016-03-23
//         if ((len=fread(tstr, 1, PATH_MAX, pipe_fp)>0))
//         {   
//             printf("len = %i\n", len);
//             tstr[len]='\0';             // Ensure null terminated string!
//             printf("%s",tstr);          // Dump stdout from commands
//             YPrintf("%s",tstr);         // Dump stdout from commands
//         }
        if (fgets(tstr, PATH_MAX, pipe_fp)) {
//             printf("%s\n",tstr);            // Dump stdout from commands
            YPrintf("%s",tstr);             // Dump stdout from commands
        }
        
        nanosleep(&dose,NULL);          // Prevent hogging
    }
    pclose(pipe_fp);                    // Close pipeline
}



// Function for telling whether a filename is a CALIB_MEAS file.
// A separate function is convenient since there is an exception to the pattern.
//
// ARGUMENTS AND RETURN VALUE
// ==========================
// INPUT : calib_meas_filename_pattern = Filename (not path) pattern that works with "Match" function.
// INPUT : filename
// RETURN VALUE : TRUE = "filename" represents a CALIB_MEAS file.
int FilenameMatchesCalibMeas(char *calib_meas_filename_pattern, char *filename)
{
    return (!Match(calib_meas_filename_pattern, filename) && strcmp(filename, CALIB_MEAS_FILE_EXCEPTION_LBL));
}



/*
 * Reads measured offset calibration (CALIB_MEAS) files and the offset calibration exceptions list (CALIB_MEAS_EXCEPT).
 * The offset calibration exceptions file contains manually selected time intervals which specify
 * calibrations to use for specific time intervals when the default algorithm does not apply.
 * Returns data in data structure "m".
 *
 * 
 * ARGUMENTS
 * =========
 * rpath    : Path to directory where the CALIB_MEAS files are (LBL+TAB).
 * fpath    : The part after the last "/" is used as a filename pattern (for LBL files!). All CALIB_MEAS.LBL filenames must
 *            match the pattern. The rest of the string/path is unused(!).
 * pathocel : Path to offset calibration exceptions (OCE) LBL file.
 * pathocet : Path to offset calibration exceptions (OCE) TAB file.
 * m        : Structure in which the calibration data is to be stored.
 *
 * 
 * NOTE: See naming conventions.
 * NOTE: Does not update the CALIB_MEAS LBL keywords.
 * NOTE: This code does not decide on which algorithm to use for selecting a calibration. It only constructs and fills
 *       the data structure used by that algorithm.
 * NOTE: The function REWRITES the CALIB_MEAS LBL files (CALIB_MEAS, CALIB_MEAS_EXCEPT) with some updated
 *       information. This should ideally maybe be done elsewhere.
 * NOTE: The function _used_to_ read ADC16/ADC20 conversion factors from LBL files. -- REMOVED (no longer reads any calibration factors).
 * NOTE: The function does not care about whether EDITED/CALIB (does not use the global "calib" variable).
 */
int InitCalibMeas(char *rpath, char *fpath, char *pathocel, char *pathocet, m_type *m)
{
// Fredrik Johansson 2014-09-04: Completely rewritten to accommodate for scandir (and alphanumerical sorting).
// PROPOSAL: Iterate over files only once and increase array sizes as needed as in InitCalibCoeff.
    
    prp_type mc_lbl;            // Linked property/value list for measured data offset and conversion to volts/ampere.
//     property_type *property;

    char file_path[PATH_MAX];           // Temporary variable for a file path.
    char *filename_pattern;
    struct dirent **dir_entry_list;     // (Pointer to array of pointers.)
    struct dirent *dentry;              // Directory entry
    int N_calibrations;
    int i=0;
    int N_dir_entries;
    int i_calib;



    // Get the name of the file/directory (the part of the path after the last "/"). Is used as a filename pattern for measured data calib files.
    // NOTE: The rest of the string fpath is never used in this function(!).
    filename_pattern = basename(fpath);

    // Read files in CALIB directory.
    // FKJN sort it alphanumerically first. 2/9 2014
    N_dir_entries = scandir(rpath, &dir_entry_list, 0, alphasort);
    if (N_dir_entries < 0)
    {
        YPrintf("Could not open/scan calibration directory\n");
        return -1;
    }

    //====================================================================================
    // Count the number of matching files. (Needed to allocate arrays of the right size.)
    //====================================================================================
    N_calibrations = 0;
    for(i=0; i<N_dir_entries; i++)
    {
        dentry=dir_entry_list[i];
        if (FilenameMatchesCalibMeas(filename_pattern, dentry->d_name))
        {
            N_calibrations++;
        }
        dentry=dir_entry_list[i];  // Get next filename.
    }

    if(N_calibrations==0)
    {
        YPrintf("No offset or factor calibration label files found\n");
        return -2;
    }

    //=========================================================================================================================
    // Allocate memory (arrays) for the calibration data structure (one component per offset calibration/CALIB_MEAS file pair)
    //=========================================================================================================================
    m->N_calib_meas = N_calibrations;
    if((m->CD=(c_type *)CallocArray(m->N_calib_meas,sizeof(c_type)))==NULL)
    {
        YPrintf("Error allocating memory for array of calibration table structures\n");
        return -4;
    }
    if((m->calib_meas_data=(calib_meas_file_type *)CallocArray(m->N_calib_meas,sizeof(calib_meas_file_type)))==NULL)
    {
        YPrintf("Error allocating memory for array of calibration info structures\n");
        return -5;
    }



    //====================================================================================
    // Iterate over all filenames - Read conversion factors and offset calibration tables
    //====================================================================================
    i_calib=0;
    for(i=0; i<N_dir_entries; i++)
    {
        dentry=dir_entry_list[i];
        if (FilenameMatchesCalibMeas(filename_pattern, dentry->d_name))
        {
            // CASE: Filename matches pattern, UNLESS it is a specific calibration data product CALIB_MEAS_FILE_EXCEPTION_LBL
            // that should never be used, and always be kept. ==> CALIB_MEAS_FILE_EXCEPTION_LBL must never be deleted.

            //================================================
            // Read and update (rewrite) calibration LBL file
            //================================================
            sprintf(file_path, "%s%s", rpath, dentry->d_name);   // Construct full path
            if (ReadLabelFile(&mc_lbl, file_path)<0)             // Read offset and TM calibration file
            {
                return -6;
            }

            //===============================================
            // Extract calibration factors from the LBL file
            //===============================================
            // ADC20 calibration factors used to be read from CALIB_MEAS file labels but are no more.
            // The corresponding C variables are set to NaN (they are never used).
            //FindP(&mc_lbl, &property, "ROSETTA:LAP_VOLTAGE_CAL_16B",       1, DNTCARE);
            //sscanf(property->value, "\"%le\"", &m->CF[i_calib].v_cal_16b);
            //FindP(&mc_lbl, &property, "ROSETTA:LAP_VOLTAGE_CAL_20B",       1, DNTCARE);
            //sscanf(property->value, "\"%le\"", &m->CF[i_calib].v_cal_20b);
            
            //FindP(&mc_lbl, &property, "ROSETTA:LAP_CURRENT_CAL_16B_G1",    1, DNTCARE);
            //sscanf(property->value, "\"%le\"", &m->CF[i_calib].c_cal_16b_hg1);
            //FindP(&mc_lbl, &property, "ROSETTA:LAP_CURRENT_CAL_20B_G1",    1, DNTCARE);
            //sscanf(property->value, "\"%le\"", &m->CF[i_calib].c_cal_20b_hg1);
            
            //FindP(&mc_lbl, &property, "ROSETTA:LAP_CURRENT_CAL_16B_G0_05", 1, DNTCARE);
            //sscanf(property->value, "\"%le\"", &m->CF[i_calib].c_cal_16b_lg);
            //FindP(&mc_lbl, &property, "ROSETTA:LAP_CURRENT_CAL_20B_G0_05", 1, DNTCARE);
            //sscanf(property->value, "\"%le\"", &m->CF[i_calib].c_cal_20b_lg);

            //=======================================================
            // Read calibration tables into array of data structures
            //=======================================================
            if(ReadTableFile(&mc_lbl, &m->CD[i_calib], rpath, "Reading CALIB_MEAS TAB file")<0)
            {
                // ExitPDS() will free m->CF and m->CD memory at exit.
                FreePrp(&mc_lbl); // Free linked property/value list for measured data offset
                return -7;
            }

            m->calib_meas_data[i_calib].LBL_filename          = strdup(dentry->d_name);
            m->calib_meas_data[i_calib].calibration_file_used = FALSE;
            m->calib_meas_data[i_calib].intervals             = NULL;

            i_calib++;
            FreePrp(&mc_lbl);  // Free linked property/value list for measured data offset
        }
        dentry=dir_entry_list[i];
    }
    
    FreeDirEntryList(dir_entry_list, N_dir_entries);



    char tstr[MAX_STR];

    //====================================================
    // Read offset calibration exceptions list - TAB file
    //====================================================
    FILE *fd;
    if((fd=fopen(pathocet, "r"))==NULL) {
        sprintf(tstr, "Can not open offset calibration exceptions table file \"%s\"\n", pathocet);
        YPrintf(tstr);
        perror(tstr);
        return -10;
    }

    YPrintf("Reading offset calibration exceptions table file: %s\n", pathocet);
    char line[MAX_STR];   // Line buffer
    while (fgets(line, MAX_STR, fd) != NULL)
    {
        // NOTE: Ignores a wider syntax than PDS (the format) permits.
        //if (line[0] == '\n') continue;   // Ignore empty line.
        //if (line[0] == '\r') continue;   // Ignore empty line ending with CR+LF.
        //if (line[0] == '#' ) continue;   // Ignore comments.

        TrimWN(line);   // Convert CR and LF to whitespace, then trim leading and trailing whitespace.
        char str_t_begin[256], str_t_end[256], LBL_filename[256];

        // NOTE: The format specifier %s will read over comma. Must therefore specify something like
        // %[^,] or %[^, ], %[^, "] instead (read ALL characters EXCEPT comma, or whitespace, or double quote).
        // Cf regular expressions.
        // NOTE: It is better trim whitespace after sscanf for time strings so that they can contain whitespace.
        // NOTE: Whitespace represents any sequence of whitespace and tab, incl. none at all.
        if (sscanf(line, " %[^,], %[^,], \"%[^, \"]\" ", str_t_begin, str_t_end, LBL_filename) != 3)
        {
            sprintf(tstr, "Error interpreting line in offset calibration exceptions table file (sscanf): \"%s\"\n", line);
            YPrintf(tstr);
            perror(tstr);

            fclose(fd);
            return -11;
        }
        TrimWN(str_t_begin);
        TrimWN(str_t_end);

        TrimWN(LBL_filename);   // Remove leading and trailing whitespace, remove CR (if any). Can be important depending on the exact chosen file format (future PDS compliant).

        time_t t_begin, t_end;
        if ((ConvertUtc2Timet(str_t_begin, &t_begin) < 0) || (ConvertUtc2Timet(str_t_end, &t_end) < 0)) {
            sprintf(tstr, "Error interpreting time in offset calibration exceptions file: \"%s\"\n", line);
            YPrintf(tstr);
            perror(tstr);

            fclose(fd);
            return -12;
        }
        if (difftime(t_end, t_begin) <= 0)
        {
            sprintf(tstr, "The stated time interval in offset calibration exceptions file has non-positive length: \"%s\"\n", line);
            YPrintf(tstr);
            perror(tstr);

            fclose(fd);
            return -13;
        }

        // Print the strings read and parsed by the code. Indentation to connect it to previous log message "title".
        YPrintf("   Read strings \"%s\", \"%s\", \"%s\"\n", str_t_begin, str_t_end, LBL_filename);

        //=============================================================================
        // Add information about one (offset calibration exception) time interval to
        // the (linked) list of such intervals for the corresponding calibration file.
        // ----------------------------------------------------------------------------
        // NOTE: Checks that the file names in the offset calibration exceptions list
        // actually match real offset calibration LBL files. This checks for
        // misspellings in the TAB files, which is very important!!
        //=============================================================================
        // Find index by searching for matching filename.
        int found_matching_file = FALSE;
        for (i=0; i<m->N_calib_meas; i++) {
            if (!strcmp(LBL_filename, m->calib_meas_data[i].LBL_filename)) {
                found_matching_file = TRUE;
                break;
            }
        }
        if (found_matching_file) {
            // Add link first in linked list.
            calib_meas_interval_type *new_interval = malloc(sizeof *new_interval);
            new_interval->next = m->calib_meas_data[i].intervals;
            m->calib_meas_data[i].intervals = new_interval;
            new_interval->t_begin = t_begin;
            new_interval->t_end   = t_end;
        } else  {
            sprintf(tstr, "Error interpreting filename in offset calibration exceptions file: \"%s\"\n", LBL_filename);
            YPrintf(tstr);
            perror(tstr);

            fclose(fd);
            return -14;
        }

    }
    fclose(fd);
    
    return 0;
}  // InitCalibMeas



// Free all the allocated memory associated with the list of directory entries returned from a call to scandir.
//
// NOTE: Will also deallocate the array pointed to by dir_entry_list.
void FreeDirEntryList(struct dirent **dir_entry_list, int N_dir_entries)
{
    int i;
    for (i = 0; i < N_dir_entries; i++)
    {
        // IMPLEMENTATION NOTE: One does not need to free dir_entry_list[i]->d_name since it is presumably not separatly allocated on the heap;
        // "The character array d_name is of unspecified size, but the number of bytes preceding the terminating null byte shall not exceed {NAME_MAX}."
        free(dir_entry_list[i]);
    }
    free(dir_entry_list);   // Free pointer to array of pointers
}



/* Given a path (p->mcpath) and mission phase abbreviation (m->mission_phase_abbrev),
 * initialize an instance of mp_type with data from the mission calendar file.
 * Function previously called "GetMissionP".
 * 
 * ARGUMENTS
 * =========
 * OUTPUT: m
 * 
 * NOTE: Could use global variable mp & pds but requires them through arguments instead to reduce dependence on global variables.
 */
// int InitMissionPhaseStructFromMissionCalendar(mp_type *m, pds_type *p)
int InitMissionPhaseStructFromMissionCalendar(mp_type *m, char *mission_calendar_path, int DPL_number, float data_set_version)
{
    // NOTE: Does not set DPL_number, although it fits in with the function (DPL number is set outside of any function).
    //  PROPOSAL: Correct for?
    FILE *fd;
    
    char nline[256];  // Input new line assume shorter than 256 characters
    char dataset_duration_days_str[6]; // Duration of period
    char sdate[11];   // Date from mission calendar in string form
    char mission_phase_abbrev[5];
    int stat;         // Just a status variable
    int dataset_duration_days;

//     if((fd=fopen(p->mcpath,"r"))==NULL)
    if((fd=fopen(mission_calendar_path,"r"))==NULL) 
    {
        printf("ERROR: Could not open mission calendar file\n");
        YPrintf("Could not open mission calendar file\n");
        return -1;
    }
    nline[255]='\0';
    while(fgets(nline,255,fd)!= NULL)
    {
        if(nline[0] == '\n') continue;  // Skip empty lines.
        if(nline[0] == '#')  continue;  // Remove any comments.
        
        // Extract values from columns, TWO columns at a time.
        // NOTE: Some column values are quoted, some are not (1st, 5th, 7th are quoted).
        Separate(nline, m->mission_phase_name,  mission_phase_abbrev,      ':', 1);  // Get mission phase name (incl. quotes) and abbreviation.
        Separate(nline, sdate,                  dataset_duration_days_str, ':', 3);  // Get Duration and Date string.
        Separate(nline, m->target_name,         m->target_name_dsi,        ':', 5);
        Separate(nline, m->target_type,         m->target_name_dsn,        ':', 7);

        sdate[10]='\0';                 // Null terminate
        mission_phase_abbrev[4]='\0';   // Null terminate
        m->target_name_dsi[5]='\0';     // Null terminate

        // Trim strings of whitespace (all but starting date).
        TrimWN(mission_phase_abbrev);
        TrimWN(dataset_duration_days_str);
        TrimWN(m->mission_phase_name);
        TrimWN(m->target_name);
        TrimWN(m->target_name_dsi);
        TrimWN(m->target_type);
        TrimWN(m->target_name_dsn);

        if(!strcmp(m->mission_phase_abbrev, mission_phase_abbrev)) // Matching mission phase abbreviation
        {
            // Get new time from mission calendar, convert to seconds
            if((stat=ConvertUtc2Timet(sdate,&(m->t_start)))<0) {
                CPrintf("    Error mission phase time conversion: %02d\n",stat);
            }

            sscanf(dataset_duration_days_str, "%d", &dataset_duration_days);

            // Compute end time. NOTE: Does not take leap seconds (e.g. 2015-06-30, 23:59.60) that occurred in time interval
            // into account. However, these are time_t variables which might not use leap seconds anyway.
            m->t_stop = m->t_start + dataset_duration_days*24*3600;

//             // Would like to execute these commands here but SPICE is not initialized yet.
//             printf("InitMissionPhaseStructFromMissionCalendar: 1 \n");    // DEBUG
//             ConvertTimet2Sccd_SPICE(m->t_start, NULL, &(m->sccd_start_data));   
//             ConvertTimet2Sccd_SPICE(m->t_stop,  NULL, &(m->sccd_stop_data ));
//             printf("InitMissionPhaseStructFromMissionCalendar: 2 \n");    // DEBUG
            
            char* descr;
            if(calib) {
                descr = "CALIB";
            } else {
                descr = "EDITED";
            }
            DeriveDSIandDSN(
                m->data_set_id, m->data_set_name,
                m->target_name_dsi, DPL_number, m->mission_phase_abbrev, descr, data_set_version, m->target_name_dsn);
            
            fclose(fd); // Close mission calendar
            return 0;   // Ok, Returns using previous ID and name, exactly what we want! 
        }
    }
    fclose(fd);
    
    printf("Could not find the mission phase in the mission calendar file.\n");
    YPrintf("    Could not find the mission phase in the mission calendar file.\n");
    return -3; // No phase found
}  // InitMissionPhaseStructFromMissionCalendar



/* Derive DATA_SET_ID and DATA_SET_NAME keyword values, INCLUDING QUOTES!
 * 
 * ARGUMENTS
 * =========
 * OUTPUT: DATA_SET_ID      : PDS DATA_SET_ID   including quotes.
 * OUTPUT: DATA_SET_NAME    : PDS DATA_SET_NAME including quotes.
 * INPUT:  data_set_version : Data set version as a float (not string)
 */
void DeriveDSIandDSN(
    char* DATA_SET_ID, char* DATA_SET_NAME,
    char* target_name_dsi, int DPLNumber, char* mpAbbreviation, char* descr, float data_set_version, char* target_name_dsn)
{
    sprintf(DATA_SET_ID,   "\"RO-%s-RPCLAP-%d-%s-%s-V%3.1f\"",              target_name_dsi, DPLNumber, mpAbbreviation, descr, data_set_version);
    sprintf(DATA_SET_NAME, "\"ROSETTA-ORBITER %s RPCLAP %d %s %s V%3.1f\"", target_name_dsn, DPLNumber, mpAbbreviation, descr, data_set_version);
}



// Dump macros. This is for debugging.
/*
void TestDumpMacs()
{
    int i,j;
    for(i=0;i<10;i++) {
        for(j=0;j<8;j++) {
            DumpPrp(&macros[i][j]);
        }
    }
}
//*/



/* Update certain fields in LBL/CAT files in a given directory, recursively.
 * This is in practice meant to be used for updating LBL/CAT (maybe TXT) files copied from templates.
 *
 * ARGUMENT
 * ========
 * dir_path         : Path to directory.
 * filename_pattern : Search pattern that selects the filenames of those files which will be updated. E.g. "*.LBL".
 *
 * NOTE: Does NOT follow symlinks.
 * NOTE: This does not work for *.TXT files where the first part is ODL keywords & values, and the last part is pure text,
 *       since ReadLabelFile & WriteUpdatedLabelFile can not handle that file format.
 * NOTE: ReadLabelFile can not handle comments, as in some *.CAT files, and can therefore not handle CATALOG/.
 * Even if it could, it would still alter PSA supplied .CAT files (with comments) which should not(?) be altered.
 * Could maybe alter only RPCLAP_*.CAT?
 */
int UpdateDirectoryODLFiles(const char *dir_path, const char *filename_pattern, int update_PUBLICATION_DATE)
{
    int i;
    int N_files;

    struct dirent **dir_entry_list;
    struct dirent *dir_entry;



    // Scan directory
    N_files=scandir(dir_path, &dir_entry_list, 0, alphasort);  // Returns sorted list, although sorting is unnecessary.
    if (N_files < 0)
    {
        char tstr[MAX_STR];
        sprintf(tstr, "Failed to scan directory for files: \"%s\"\n", dir_path);
        YPrintf(tstr);
        perror(tstr);
        return -1;
    }

    // Iterate over files/directories located in directory.
    for (i=0; i<N_files; i++)
    {
        dir_entry = dir_entry_list[i];

        // DT_REG : Regular file (not directory)
        if ((dir_entry->d_type==DT_REG) && !fnmatch(filename_pattern, dir_entry->d_name, 0))
        {
            // CASE: Object is a matching file (not directory).
            
            char file_path[PATH_MAX];
            sprintf(file_path, "%s/%s", dir_path, dir_entry->d_name); // Construct full path

            UpdateODLFile(file_path, NULL, 0);    // UPDATE LBL FILE
        }
        else if ((dir_entry->d_type==DT_DIR) && strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, ".."))  // DT_DIR : Directory
        {
            // CASE: Object is a directory (but not "." or "..").
            
            char subdir_path[PATH_MAX];
            sprintf(subdir_path, "%s/%s", dir_path, dir_entry->d_name);
            int status = UpdateDirectoryODLFiles(subdir_path, filename_pattern, update_PUBLICATION_DATE);    // NOTE: RECURSIVE CALL
            if (status<0) {
                // Free allocated memory.
                // NOTE: Uncertain if this code actually frees everything it should.
                for (i = 0; i < N_files; i++)   // Need to free array of all “returns”.. some other time.
                {
                    free(dir_entry_list[i]);
                }
                free(dir_entry_list);

                return status;
            }
        }
    }

    FreeDirEntryList(dir_entry_list, N_files);
    return 0;
}



/* Update one ODL file by reading it and then writing it with data set-dependent standard modifications.
 * Can optionally return the contents.
 * 
 *
 * ARGUMENTS
 * =========
 * odl_prp : Iff non-null: Will be filled with the contents of the ODL file.
 *          NOTE: The caller does not need to pre-initialize it with InitP, but is allowed to,
 *                since InitP does not allocate any memory.
 *
 * 
 * NOTE: Based on ReadLabelFile and WriteUpdatedLabelFile ==> Can not handle ODL files with...
 * (1) text after "END" e.g. *.TXT files.
 * (2) ODL comments e.g. *.CAT files. Even if it could, it would still alter PSA supplied
 * .CAT files (with comments) which should not(?) be altered. Could maybe alter only RPCLAP_*.CAT?
 */
int UpdateODLFile(char *file_path, prp_type *odl_prp, int update_PUBLICATION_DATE)
{
    prp_type temp_odl_prp;
    
    YPrintf("Reading and updating ODL file: %s\n", file_path);
    InitP(&temp_odl_prp);
    
    // Read file
    if (ReadLabelFile(&temp_odl_prp, file_path) < 0) // Read offset and TM calibration file
    {
        YPrintf("Error when modifying file \"%s\"\n", file_path);
        FreePrp(&temp_odl_prp);
        return -1;
    }
    
    // Write file
    if (WriteUpdatedLabelFile(&temp_odl_prp, file_path, update_PUBLICATION_DATE) < 0)
    {
        YPrintf("Error when modifying file \"%s\"\n", file_path);
        FreePrp(&temp_odl_prp);
        return -2;
    }
    
    // Dispose of content of (original) ODL file.
    if (odl_prp==NULL) {
        FreePrp(&temp_odl_prp);
    } else {
        *odl_prp = temp_odl_prp;
    }
    
    return 0;
}



/* Write updated ODL (TXT/LBL/CAT) file.
 *
 * The function sets certain label keyword values IF THEY ARE ALREADY PRESENT, 
 * since not all ODL files neither do, nor should, contain all of these keywords.
 * The function is in practice used for updating ODL files copied from the template directory.
 * 
 * NOTE: PUBLICATION_DATE should not be set to the same value everywhere. Therefore optional.
 * NOTE: Does not set VOLUME_ID, VOLUME_NAME which need  extra code to be derived and only applies to VOLDESC.CAT (?).
 * NOTE: Does not set DATA_SET_RELEASE_DATE  which needs extra code to be derived and only applies to DATASET.CAT (?).
 * 
 *
 * ARGUMENTS
 * =========
 * update_PUBLICATION_DATE : true iff keyword PUBLICATION_DATE should be updated.
 * 
 * 
 * RETURN VALUE
 * ============
 * 0 = No error; -1 = Error
 */
int WriteUpdatedLabelFile(prp_type *lb_data, char *name, int update_PUBLICATION_DATE)
{
    // PROPOSAL: Separate (1) updating variables and (2) writing file into separate functions.
    
    SetP(lb_data, "DATA_SET_ID",        mp.data_set_id,         1);
    SetP(lb_data, "DATA_SET_NAME",      mp.data_set_name,       1);  
    SetP(lb_data, "PRODUCER_ID",        PDS_PRODUCER_ID,        1);  
    SetP(lb_data, "PRODUCER_FULL_NAME", PDS_PRODUCER_FULL_NAME, 1);
    SetP(lb_data, "MISSION_PHASE_NAME", mp.mission_phase_name,  1);  
    SetP(lb_data, "TARGET_NAME",        mp.target_name,         1);   // Needed for DATASET.CAT.
    if (update_PUBLICATION_DATE) {
        SetP(lb_data, "PUBLICATION_DATE",   pds.ReleaseDate,          1); // Set publication date of data set
    }
    
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



/* Read label file
 *
 * 
 * ARGUMENTS
 * =========
 * OUTPUT: lb_data : Does not need to (should not) be initialized by caller.
 * 
 * 
 * RETURN VALUE : 0=success, -1=failure
 *
 *
 * NOTE: The function is implicitly used for modifying LBL/CAT files from the template directory
 * by first reading a file into linked list, then modifying the linked property values list,
 * then writing the linked list as LBL file.
 * NOTE: I _think_ this code can handle LBL and CAT files
 * with multiple line values. This is important for being able to modify *.CAT files
 * with long texts in the form of "values".   /Erik P G Johansson 2015-04-27.
 * NOTE: Can not handle PDS comments.
 * NOTE: Will ignore file contents after "END".
 *
 */
int ReadLabelFile(prp_type *lb_data, char *file_path)
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

    // Dynamic buffer for keywords that cross several lines
    // such as description fields. They can be very large.
    char *bcross;
    
    InitP(lb_data);   // Initialize linked property/value list
    
    bcross  = NULL;
    msubone = (MAX_STR-1);
    

    // Open label file, for description of calibration table
    if((fd=fopen(file_path, "r"))==NULL)
    {
        YPrintf("Couldn't open label file:  %s\n", file_path);
        return -1;
    }
    
    while(fgets(line,msubone,fd)!= NULL)   // Reads line into variable "line". Appears to include the ending line feed.
    {
        if(line[0] == '\n') continue;     // Empty line.
        if(line[0] == '\r') continue;     // Empty line ending with CR+LF.
        if(line[0] == '#') continue;      // Remove comments.
        if(line[0] == '/') continue;      // Remove comments.
        
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
        DumpPrp(lb_data);
    }
    
    fclose(fd);
    return 0;
}



/*
 * Read "generic" calibration TABLE file described in "lbl_data" and consisting of arbitrary number of numeric columns
 * (PDS types ASCII_INTEGER, ASCII_REAL).
 * The table file's name is taken from ^TABLE in lbl_data, and the directory is taken from the function argument.
 *
 * I try to use the description in the label file as much as possible
 * but it could probably be done better. Function uses the LBL file for finding columns and can thus handle
 * a variable number of columns. Uses START_BYTE=column start (but not BYTES=column width),
 * DATA_TYPE (to distinguish ASCII_INTEGER, ASCII_REAL).
 *
 * 
 * ARGUMENTS
 * =========
 * lbl_data     : LBL file description from which some data will be taken, incl. TAB filename.
 * cal          : Structure which will contain the data read from file.
 *                cal->C[i][j] = Value at row i (i=0=first row), column j (j=0=first column) in TAB file.
 * dir_path     : Path to parent _directory_ (not path to file!)
 * msg          : String to be used in log message. Colon and file path is added after the string.
 * Return value : 0=(Seems like) success, Negative=Failure
 */
int ReadTableFile(prp_type *lbl_data, c_type *cal, char *dir_path, char *msg)
{
    int i,j;
    char file_path[PATH_MAX];
    
    // The following arrays are taken to be large enough for their purposes
    char format[256];  // A vector indicating column format, thus max 256 columns. We have 4 in reality.
    char line[8192];   // Temporary buffer  to read a row into.
    int start[256];    // A vector indicating start positions of columns
    
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
    
    int startpos;  // Start position of a column
    int tmp1;      // Temporary data value
    double tmp2;   // Temporary data value
    
    // Interpreting label file data
    FindP(lbl_data,&property1, "^TABLE",     1, DNTCARE);   // Get file name (TAB file name).
    FindP(lbl_data,&property2, "START_TIME", 1, DNTCARE);   // Get start time
    FindP(lbl_data,&property3, "ROWS",       1, DNTCARE);   // Number of rows
    FindP(lbl_data,&property4, "COLUMNS",    1, DNTCARE);   // Number of columns
    strncpy(line,property1->value,8192);                    // Copy so we don't modify the real property list
    TrimQN(line);                                           // Trim quotes away.
    sprintf(file_path,"%s%s",dir_path,line);                // Construct file_path = dir_path + filename

    //YPrintf("Reading table file: %s\n", line);
//     YPrintf("%s: %s\n", msg, line);          // Only prints the TAB filename (not path), as fetched from lbl_data/the LBL file data.
    YPrintf("%s: %s\n", msg, file_path);     // Prints the TAB path+filename (filename as fetched from lbl_data/the LBL file data).
    
    // Open calibration table file
    if((fd=fopen(file_path,"r"))==NULL)
    {
        YPrintf("Couldn't open table file:  %s\n",file_path);
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
        strcpy(cal->valid_utc, property2->value); // Copy valid from time
    } else {
        strcpy(cal->valid_utc, "2003-01-01T00:00:00.000");
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
                return -2; // Error. We only expect integer or double for now!
            }
        }
        
        FindP(lbl_data,&property6,"START_BYTE",j+1,DNTCARE); // Get start byte in line
        sscanf(property6->value,"%d",&start[j]);
    }
    
    // Read and parse table rows
    i=0;   // i=Row index.
    while(fgets(line,255,fd)!= NULL)
    {
        for(j=0;j<ncols;j++)    // j=Column index
        {
            startpos=start[j]-1; // Compute start position of column
            if(format[j]==0) // CASE: Read integer
            {
                sscanf(&line[startpos],"%d",&tmp1);
                tmp2=(float)tmp1;
            }
            
            if(format[j]==1) { // CASE: Read float
                sscanf(&line[startpos],"%le",&tmp2);
            }
            
            cal->C[i][j]=tmp2; // Store data in matrix
        }
        i++; // Next row
    } 
    
    
    if(debug>2) // If debugging level is larger than 2 
    {
        printf("Valid from: %s\n", cal->valid_utc);
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



/*
 * Update DATASET.CAT with some values.
 * 
 * NOTE: Could use global variables mp & pds but requires them through arguments instead to reduce dependence on global variables.
 * Hence the *_arg argument names.
 * 
 * 
 * RETURN VALUE
 * ============
 * 0=No error; -1=Error
 * 
 */
int UpdateDATASET(mp_type *mp_arg, pds_type *pds_arg) {
    
    char DATASET_path[MAX_STR];
    char utc[MAX_STR];
    prp_type DATASET_contents;

    int status = 0;
    
    sprintf(DATASET_path, "%sCATALOG/DATASET.CAT", pds.apathpds);  // Get full path    
    
    status = ReadLabelFile(&DATASET_contents,DATASET_path);              // Read catalog keywords into property value pair list
    if (status < 0) {
        return -1;
    }
    
    //ConvertTimet2Utc((double) mp_arg->t_start, utc, FALSE);          // Decode raw time into PDS compliant UTC time
    ConvertSccd2Utc(mp_arg->sccd_start_data, utc, NULL);
    YPrintf("Mission phase start         : %s\n", utc);
    printf( "Mission phase start         : %s\n", utc);
    SetP(&DATASET_contents,"START_TIME", utc, 1);                      // Set START_TIME
    
    //ConvertTimet2Utc((double) mp_arg->t_stop, utc, FALSE);           // Decode raw time into PDS compliant UTC time
    ConvertSccd2Utc(mp_arg->sccd_stop_data, utc, NULL);
    YPrintf("Mission phase stop          : %s\n\n", utc);
    printf( "Mission phase stop          : %s\n",   utc);
    SetP(&DATASET_contents,"STOP_TIME", utc, 1);                       // Set STOP_TIME
    
    SetP(&DATASET_contents,"DATA_SET_RELEASE_DATE", pds_arg->ReleaseDate, 1); // Set DATA_SET_RELEASE_DATE. NOTE: pds.ReleaseDate also used for keyword PUBLICATION_DATE.
    
    status = WriteUpdatedLabelFile(&DATASET_contents, DATASET_path, 1);         // Write back label file with new info. NOTE: This function may update more keywords.
    FreePrp(&DATASET_contents);                                        // Free property value list
    if (status < 0) {
        return -1;    // NOTE: Return after having freed the property.
    }
    
    return 0;
}



// Return the bias mode for a given "dop" value as it used for when writing to file.
//
// dop : See "WritePTAB_File".
// NOTE: The return type is chosen to agree with pds.h:curr_type_def#bias_mode1/2.
// NOTE: If the assertion error is ever triggered, and pds does NOT exit, then it might fill up the logs with far too many log messages.
//
char GetBiasMode(curr_type *curr, int dop) {
    if (curr->sensor==SENS_P1 || dop==1) {
        return curr->bias_mode1;
    } else if (curr->sensor==SENS_P2 || dop==2) {
        return curr->bias_mode2;
    } else if (curr->sensor==SENS_P1P2 && dop==0) {
        if (curr->bias_mode1 != curr->bias_mode2) {
            CPrintf("ASSERTION ERROR: GetBiasMode: curr->bias_mode1 != curr->bias_mode2 for dop=0.\n");
            YPrintf("ASSERTION ERROR: GetBiasMode: curr->bias_mode1 != curr->bias_mode2 for dop=0.\n");
            ExitPDS(1);
        }
        return curr->bias_mode1;
    }
    
    CPrintf("ERROR: GetBiasMode: Can not determine bias mode (curr->sensor=%i).\n", curr->sensor);
    return -1;
}



/*
 * Function to select which bias voltage-dependent current offsets (calibration data, CALIB_MEAS) to use for a given point in time.
 *
 * NOTE: The function will set the flag mc->calibration_used.
 *
 * t_data   : The time for which we have science data that needs to be calibrated.
 * UTC_data : The time for which we have science data as a UTC string.
 * mc       : Calibration data.
 * Return value : An index into the corresponding arrays in "mc".
 */
int SelectCalibrationData(time_t t_data, char *UTC_data, m_type *mc)
{
    /*
     * Algorithm for selecting the calibration nearest in time.
     *
     * ASSUMES: Calibrations are sorted in time (increasing).
     *
     * IMPLEMENTATION NOTE: This is potentially unnecessarily slow since every calibration data time stamp must be converted:
     * mc->CD[i].valid_utc: UTC --> time_t --> scalar variable
     * (and t_data: time_t --> scalar variable)
     *
     * VARIABLE NAMING CONVENTION:
     *    t_*  = Variable of type time_t.
     *    tp_* = Variable of type double, representing the number of seconds after a common arbitrary reference epoch (same epoch for all tp_* variables).
     */

    //######################################################################################################
    /* FUNCTION: Return number of seconds since an arbitrary epoch (a constant reference point in time).
     *
     * ASSUMES: The refence epoch variable "t_ref" has been initialized.
     * RATIONALE: According the time_t documentation, one should really avoid doing arithmetic with time_t variables since its meaning
     * depends on the library implementation. There is also no proper(?) way of converting from time_t to a scalar time (e.g. seconds
     * after some universal reference time, e.g. 1970-01-01, 00:00.00) more than difftime.
     * --
     * QUESTION: Overkill since POSIX & UNIX systems should use time_t=seconds since 1970 anyway, and other pds code uses that assumption anyway?
     */
    time_t t_ref;                                     // Variable used by function.
    ConvertUtc2Timet(mc->CD[0].valid_utc, &t_ref);    // Initialize ref_time - Set it to an arbitrary valid time.
    inline double GetSecondsTime(time_t t)    {   return difftime(t, t_ref);   }   // difftime : Return time difference in seconds between two time_t variables.
    //######################################################################################################

    double tp_data;
    time_t t_calib;        // Calibration time for current calibration.
    int i;

    tp_data = GetSecondsTime(t_data);

    //============================================================================================================
    // Check if t_data is covered by any of pre-defined time intervals in the offset calibration exceptions list.
    // If so, use that result and return/exit.
    //============================================================================================================
    for (i=0; i<mc->N_calib_meas; i++)   // Iterate over calibrations.
    {
        calib_meas_interval_type *interval = mc->calib_meas_data[i].intervals;

        while (interval!=NULL) {
            // NOTE: Covers equals at both interval beginning and interval end.
            if (   (GetSecondsTime(interval->t_begin) <= tp_data                        )
                && (                          tp_data <= GetSecondsTime(interval->t_end)) )
            {
//                 printf(" mc->calib_meas_data[i].LBL_filename = %s\n", mc->calib_meas_data[i].LBL_filename);    // DEBUG
                mc->calib_meas_data[i].calibration_file_used = TRUE;
                return i;                                            // RETURN / EXIT FUNCTION
            }
            interval = interval->next;
        }
    }


    //=========================================================
    // Derive the offset calibration which is closest in time.
    //=========================================================
    /* IMPLEMENTATION NOTE: Rounds off data time to the middle of the day (noon) so that all data for any given day uses the same calibration.
     * ==> Every day (midnight-to-midnight) gets the same data time. This is to ensure that any change of used calibration (due to the
     * default algorithm) only happens at midnight.
     */
    double tp_calib1, tp_calib2;   // Times of two subsequent calibrations.
    ConvertUtc2Timet_midday( UTC_data,            &t_data  );   tp_data   = GetSecondsTime(t_data);
    ConvertUtc2Timet(        mc->CD[0].valid_utc, &t_calib );   tp_calib2 = GetSecondsTime(t_calib);   // Initial value to start algorithm.
    for (i=0; i+1<mc->N_calib_meas; i++)   // Iterate over PAIRS of subsequent calibrations {i,i+1}. Can also be seen as iterating over (time) middle points between calibrations.
    {
        tp_calib1 = tp_calib2;                 // Set first calibration time.

        ConvertUtc2Timet(mc->CD[i+1].valid_utc, &t_calib);
        tp_calib2 = GetSecondsTime(t_calib);   // Set second calibration time.

        double tp_calib_middle = (tp_calib1 + tp_calib2) / 2.0;   // Middle between the two calibration times.
        // If tp_calib1 is closest to tp_data, then if statement will be triggered
        // If tp_calib2 is closest to tp_data, then the if statement in the next loop iteration will be triggered.
        // This is effectively equivalent to choosing the calibration closest to tp_data.
        if (tp_data <= tp_calib_middle) {
            break;
        }
    }

//     printf(" mc->calib_meas_data[i].LBL_filename = %s\n", mc->calib_meas_data[i].LBL_filename);    // DEBUG
    mc->calib_meas_data[i].calibration_file_used = TRUE;
    return i;   // NOTE: Returns zero if there is only one calibration (i.e. no iterations in the for loop). The for loop will still initialize i=0.
}



/* To be run when pds is finished using CALIB_MEAS.
 * 
 * The function:
 * (1) deletes unused CALIB_MEAS files, incl. CALIB_MEAS_EXCEPT files
 * (2) deallocates the CALIB_MEAS data structure
 * In other words, the content of "m" should not be used again after being used as an argument to this function.
 * 
 * ASSUMES: Assumes that the function can still call the log function YPrintf.
 * (This is important information for when to call this function during the shutdown process.)
 *
 *
 * ARGUMENTS
 * =========
 * pathocel : Path to CALIB_MEAS_EXCEPT LBL file
 * pathocet : Path to CALIB_MEAS_EXCEPT TAB file
 * cpath    : Path to calibration directory
 * m        : Data structure used for determining files to delete. The criterion is m->calib_meas_data[i].calibration_file_used==FALSE.
 *
 * 
 * NOTE: Does not update the CALIB_MEAS LBL keywords. Assumes that other code does that.
 * NOTE: The algorithm deletes files with calibration_file_used==FALSE, and deletes CALIB_MEAS_EXCEPT when calibration_file_used==FALSE
 * for __all__ dated CALIB_MEAS files. The effect should be that:
 * (1) for EDITED, it deletes all CALIB_MEAS files for EDITED (incl. CALIB_MEAS_EXCEPT).
 * (2) for CALIB it deletes all unused CALIB_MEAS files (incl. CALIB_MEAS_EXCEPT).
 * If CALIB_MEAS functionality is not used (CALIB_COEFF functionality is used instead), then calibration_file_used==FALSE
 * for all dated CALIB_MEAS files, which means the files are automatically deleted. This function can thus be called also then.
 * NOTE: Returns on error to delete file. Will not continue to try to delete other files.
 */
// PROPOSAL: Better name, not "Destroy".
//      Ex: Destruct, Destructor (cf Constructor), Done
int DestroyCalibMeas(char *cpathd, char *pathocel, char *pathocet, m_type *m)
{
//     int exit_code = 0;
    int i;
    int removed_all_calib_meas_files = TRUE;   // Initial value until proven false.
    
//     YPrintf("DestroyCalibMeas: m->N_calib_meas = %i\n", m->N_calib_meas);   // DEBUG

    for (i=0; i<m->N_calib_meas; i++)   // Iterate over CALIB_MEAS files (which are registered in data structure).
    {
        // Set LBL_file_path.
        char LBL_file_path[PATH_MAX];
        sprintf(LBL_file_path, "%s/%s", cpathd, m->calib_meas_data[i].LBL_filename);

        // Set TAB_filename, TAB_file_path.
        prp_type lbl_info;
        if (ReadLabelFile(&lbl_info, LBL_file_path)<0)  // Read offset and TM calibration LBL file
        {
            FreePrp(&lbl_info); // Free linked property/value list
//             exit_code = -6;    break;
            return -6;
        }
        property_type *property;
        char TAB_filename[PATH_MAX], TAB_file_path[PATH_MAX];
        FindP(&lbl_info, &property, "^TABLE", 1, DNTCARE);
        strncpy(TAB_filename, property->value, PATH_MAX);
        TrimQN(TAB_filename);
        sprintf(TAB_file_path, "%s/%s", cpathd, TAB_filename);

        if (m->calib_meas_data[i].calibration_file_used) {
            // NOTE: Extra whitespace in log message so that the log messages line up nicely.
            YPrintf("Keeping  used   calibration files: %s, %s\n", m->calib_meas_data[i].LBL_filename, TAB_filename);  // Print only filenames (not entire paths).
            removed_all_calib_meas_files = FALSE;
        } else {
            //=======================
            // Delete files on disk.
            //=======================
            YPrintf("Deleting unused calibration files: %s, %s\n", m->calib_meas_data[i].LBL_filename, TAB_filename);  // Print only filenames (not entire paths).
            if (remove(TAB_file_path)!=0) {
                YPrintf("Error when deleting file \"%s\"\n", TAB_file_path);
//                 exit_code = -1;    break;
                return -1;
                // NOTE: Does not return from function yet.
            }
            if (remove(LBL_file_path)!=0) {
                YPrintf("Error when deleting file \"%s\"\n", LBL_file_path);
//                 exit_code = -1;    break;
                return -1;
                // NOTE: Does not return from function yet.
            }

            FreePrp(&lbl_info); // Free linked property/value list
        }
    }   // for

    
    
    //==============================
    // Delete CALIB_MEAS_EXCEPT file
    //==============================
//     YPrintf("removed_all_calib_meas_files = %i\n", removed_all_calib_meas_files);    // DEBUG
    //if (!calib || removed_all_calib_meas_files) {
    if (removed_all_calib_meas_files) {
        // NOTE: Prints entire paths.
        YPrintf("Deleting unused OCE calibration files: \"%s\"\n", pathocet);
        YPrintf("                                       \"%s\"\n", pathocel);
        if ((remove(pathocel)!=0) || (remove(pathocet)!=0)) {
            YPrintf("Error when deleting files \"%s\", \"%s\"\n");
//             exit_code = -2;
            return -2;
            // NOTE: Does not return from function yet.
        }
    }



    //==========================================
    // Deallocate "m" (entire m_type structure)
    //==========================================
    // Deallocate m->CD
    if (m->CD!=NULL && m->N_calib_meas!=0)
    {
        // Deallocate array of arrays.
        for (i=0; i<m->N_calib_meas; i++) {
            FreeDoubleMatrix(m->CD[i].C, m->CD[i].rows, m->CD[i].cols);
        }
        free(m->CD);
    }
    // Deallocate m->calib_meas_data
    if (m->calib_meas_data!=NULL && m->N_calib_meas!=0)
    {
        for (i=0; i<m->N_calib_meas; i++)
        {
            free(m->calib_meas_data[i].LBL_filename);
            
            // Deallocate linked list of time intervals.
            calib_meas_interval_type *next_ci = m->calib_meas_data[i].intervals;
            while (next_ci != NULL) {
                calib_meas_interval_type *prev_ci = next_ci;
                next_ci = next_ci->next;
                free(prev_ci);
            }
        }
        free(m->calib_meas_data);
    }

//     return exit_code;
    return 0;
}



/*===============================================================================================================================
 * Write TAB file, EDITED or CALIB.
 * Convert from TM units to physical units (i.e. calibrate) for CALIB.
 *
 * ARGUMENTS
 * =========
 * sweep_type   sw_info;           // Sweep info structure steps, step height, duration, ...
 * adc20_type   a20_info;          // ADC 20 info structure resampling, moving average, length, ...
 * m_type *mc                      // CALIB_MEAS
 *
 * We use the mode information from command logs here since it also contain off information.
 *
 * NOTE: Conversion from TM units to physical units plus calibration is done here.
 * NOTE: Uses global variable "f_conv" (for fine sweeps).
 *
 * Uncertain what "dop" means, and compared to curr->sensor. Compare "dop" in WritePLBL_File.
 *    NOTE: Check how dop is set in calls to WritePTAB_File and WritePLBL_File. It is always a literal.
 *    NOTE: Check how dop is used in WritePLBL_File (very little).
 *    NOTE: "dop" seems different from curr_type_def.sensor. sensor==0 has different meaning at the very least.
 *    NOTE: In this function, whenever dop is used together with something else in a (boolean) condition (many of those),
 *          it is one of these three conditions:
 *      P1  (curr->sensor==SENS_P1   || dop==1)
 *      P2  (curr->sensor==SENS_P2   || dop==2)
 *      P3  (curr->sensor==SENS_P1P2 && dop==0)
 *
 *    Erik P G Johansson's GUESS: curr-->sensor refers to the probe(s) for which there is data.
 *    Erik P G Johansson's GUESS: The combination of curr.sensor and "dop" determines which probe is being written to disk.
 *       curr.sensor=SENS_P1
 *          dop=0, 1 : P1
 *       curr.sensor=SENS_P2
 *          dop=0, 2 : P2
 *       curr.sensor=SENS_P1P2
 *          dop=0 : P3
 *          dop=1 : P1
 *          dop=2 : P2
 *       <=> (This is equivalent to...)
 *       dop=0
 *          curr.sensor=SENS_P1   : P1 
 *          curr.sensor=SENS_P2   : P2
 *          curr.sensor=SENS_P1P2 : P3
 *       dop=1
 *          curr.sensor=SENS_P1, SENS_P1P2 : P1
 *       dop=2
 *          curr.sensor=SENS_P2, SENS_P1P2 : P2
 * 
 * NOTE: Uses global variable "pds" and maybe others.
 ===============================================================================================================================*/
int WritePTAB_File(
    unsigned char *buff,
    char *fname,
    int data_type,
    int N_tmsmp,
    int id_code,
    int N_bytes,
    sweep_type *sw_info,
    adc20_type *a20_info,
    curr_type *curr,
    int param_type,
    int ADC16_P1_insmp_per_tmsmp,
    int ADC16_P2_insmp_per_tmsmp,
    int dop,
    m_type *mc,
    unsigned int **bias,
    int nbias,
    unsigned int **mode,
    int nmode,
    int N_non_tsweep_tmsmp,
    int N_plateau_tmsmp)
{
    char file_path[PATH_MAX];        // Temporary string

    char current_sample_utc_corrected[MAX_STR];
    char first_sample_utc_TM[MAX_STR];   // TM=Time according to TM (no group delay).

    int sw_bias_step_size_TM = 3;               // Current sweep step, default value just there to get rid of compilation warning.

    double t_delta_s;                // Time since first sample. Unit: seconds
    double current_sample_sccd_corrected;   // Time of current sample (current row) as SCCD.

    int i_sample;                    // Sample number.
    int j,l,m;
    int k_tsweep_tmsmp;              // Sample number during the true sweep plateaus (not the initial samples, which number may vary). Used for setting/deriving sweep bias.

    int meas_value_TM;               // Measured value (i.e. not bias value) in TM units. NOTE: Integer.
    int sw_bias_voltage_TM = 0;      // Bias voltage during a sweep (sw) in TM units.

    int macro_id; 
    int current_TM;                  // Current variable in TM units (TC or TM).
    int voltage_TM;                  // Voltage variable in TM units (TC or TM).

    int ibias;                       // Temporary current variable
    int vbias;                       // Temporary voltage variable

    int ibias1;                      // Temporary current variable
    int vbias1;                      // Temporary voltage variable
    int ibias2;                      // Temporary current variable
    int vbias2;                      // Temporary voltage variable

    char bias_mode = GetBiasMode(curr, dop);      // Name and type analogous to curr_type_def#bias_mode1/2.

    // double ADC_offset =0.0;	// due to ADC errors around 0 (for 16bit data, possibly 20bit data also), we need to correct small offset
    
    double vcalf = NAN;             // vcalf=Voltage Calibration Factor. Basic conversion TM-to-physical units. Includes ADC20 truncation factor.
    double ccalf = NAN;             // ccalf=Current Calibration Factor. Basic conversion TM-to-physical units. Includes ADC20 truncation factor.

    // Current/voltage calibration factors, but always for ADC16. Useful for converting offsets TM-->physical units.
    double vcalf_ADC16 = 0.0/0.0;    // Voltage calibration factor for ADC16.
    double ccalf_ADC16 = 0.0/0.0;    // Current calibration factor for ADC16.

    double utime;                   // Current time in UTC for test of extra bias settings. Interpreted as time_t.
    time_t utime_old;               // Previous value of utime in algorithm for detecting commanded bias.
    // Time of current data. Used for selecting calibration and commanded bias, not the samples. TM=Time according to TM (no group delay) NOTE: No subseconds since time_t.
    time_t t_first_sample_TM;       

    int i_calib=0;                  // Valid index into structure with calibration data.

    // BDCO = Bias-Dependent Current Offsets (stemming from CALIB_MEAS or CALIB_COEFF).
    // Indices: [i][j], i=probe (0=P1, 1=P2), j=bias voltage TM value (0-255).
    // PROPOSAL: Better name
    //      PROPOSAL: bdc_offsets
    //      PROPOSAL: bd_current_offsets
    double bdco[2][256];

    property_type *property1;       // Temporary property1
    
    int ADC20_moving_average_enabled;    // ADC20 Moving average boolean. Used for correcting for flight s/w bug.
    
    const int is_ADC20_nontrunc =       // True if-and-only-if current data is nontruncated ADC20.
        (data_type==D20 ) |
        (data_type==D201) |
        (data_type==D202);
    
    /*======================================================================================================
     * There is a bug in the flight software implementation of moving average for ADC20 data.
     * 
     * Intended implementation in flight software:
     *      Moving average = (SUMMA x_i) / N
     * The actual implementation in flight s/w (with bug), inferred from data (not flight s/w source code),
     * and as it is believed as of 2017-05-30:
     *      Moving average = (x_? + SUMMA x_i) / N
     *      x_? = Unknown sample
     * 
     * N = ROSETTA:LAP_P1P2_ADC20_MA_LENGTH
     * NOTE: N = 1 is equivalent to moving average disabled. ==> No bug.
     * 
     * All ADC20 data should be multiplied by this variable before adding/subtracting any offsets.
     * This corrects for the difference (factor) between intended and actual flight software
     * implementation. The variable should have value equal to one when moving average is disabled.
     * See the variable's assignment.
     ======================================================================================================*/
    double ADC20_moving_average_bug_TM_factor = 0.0/0.0;

    /*==================================================================================================================
     * Offset that is to be SUBTRACTED from every measured CALIB value (density and E-field, ADC16 and ADC20, 4 kHz and 8 kHz).
     * Used for calibration purposes. See its assignment. Expressed in ADC16 TM units.
     * NOTE: Same value is used for all samples in the TAB file, i.e. it can not be used for ADC16 nonnegative offset
     * which depends on the sample value. Hence the prefix "global_".
     * 
     * PROPOSAL: Include voltage bias measured current offset.
     *      CON: vbias not set at the time the variable is assigned.
     ==================================================================================================================*/
    double global_calib_offset_ADC16TM = 0.0/0.0;



    int extra_bias_setting;



    ConvertSccd2Utc(curr->seq_time_TM, first_sample_utc_TM, NULL);   // First convert spacecraft time to UTC to get time calibration right.
    ConvertUtc2Timet(first_sample_utc_TM, &t_first_sample_TM);    // Convert back to seconds.

    /*===================================================================================================================
     * (1) Determine whether ADC20 moving average is enabled, and
     * (2) set the ADC20 flight software moving average bug compensation factor.
     * 
     * Not certain how the moving average length variable is set for non-ADC20 data. Therefore require both 
     * (1) ADC20 data, AND (2) moving average length > 1, before concluding that moving average is enabled.
     * This makes the values always valid (i.e. can be used when moving average can not be applied, i.e. for ADC16 data).
     * 
     * NOTE: This should not be confused with another flight software bug that randomizes the last four bits in
     * non-truncated ADC20 data when the moving average is used.
     ===================================================================================================================*/
    if ((param_type==ADC20_PARAMS) && (a20_info->N_MA_length_insmp != 1)) {
        ADC20_moving_average_enabled = TRUE;
        ADC20_moving_average_bug_TM_factor = (a20_info->N_MA_length_insmp) / (a20_info->N_MA_length_insmp + 1.0);   // NOTE: Force floating-point division.
    } else {
        ADC20_moving_average_enabled = FALSE;
        ADC20_moving_average_bug_TM_factor = 1.0;
    }



    /* Declare constants which replace the conditions in many "if" statements. This clarifies the code.
     * NOTE: For unknown reasons, the expressions represented by the constants here are not used for sweeps,
     * Possibly because "param_type==SWEEP_PARAMS" implies that P3 is not used and therefore "dop" does not need to be read.
     */
    const int writing_P1_data = (curr->sensor==SENS_P1   || dop==1);
    const int writing_P2_data = (curr->sensor==SENS_P2   || dop==2);
    const int writing_P3_data = (curr->sensor==SENS_P1P2 && dop==0);

    const int is_high_gain_P1 = !strncmp(curr->gain1, "\"GAIN 1\"", 8);    // GAIN 1 <==> High gain
    const int is_high_gain_P2 = !strncmp(curr->gain2, "\"GAIN 1\"", 8);
    const int uses_8kHz_filter = (curr->afilter == 8);

    // Derive constants needed for determining saturation.
    double saturation_phys_min;
    double saturation_phys_max;
    int saturation_TM_1;
    int saturation_TM_2;
    set_saturation_limits(
        &saturation_phys_min,
        &saturation_phys_max,
        &saturation_TM_1,
        &saturation_TM_2,
        is_ADC20_nontrunc, 
        (is_high_gain_P1 && writing_P1_data) || (is_high_gain_P2 && writing_P2_data) || (is_high_gain_P1 && writing_P3_data),
        bias_mode==E_FIELD);  // NOTE: Handling P3 case just for safety. P3 does not presently utilize saturation detection.
    
    if(calib)
    {
        //=============
        // CASE: CALIB
        //=============

        // Figure out which calibration data to use for the given time of the data.
        if (CALIB_COEFF_ENABLED)
        {
            double cc_coeff_array[2*N_CALIB_COEFFS];
            if (GetCalibCoeff(pds.cpathd, &calib_coeff_data, curr->seq_time_TM, cc_coeff_array)) {
                YPrintf("WritePTAB_File: Can not obtain CALIB_COEFF coefficients for time SCCD=curr->seq_time_TM=%f, first_sample_utc_TM=%s\n", curr->seq_time_TM, first_sample_utc_TM);
                return -1;
            }
            
            const double p1 = cc_coeff_array[CALIB_COEFF_P_P1];
            const double q1 = cc_coeff_array[CALIB_COEFF_Q_P1];
            const double r1 = cc_coeff_array[CALIB_COEFF_R_P1];
            const double s1 = cc_coeff_array[CALIB_COEFF_S_P1];
            
            const double p2 = cc_coeff_array[CALIB_COEFF_P_P2];
            const double q2 = cc_coeff_array[CALIB_COEFF_Q_P2];
            const double r2 = cc_coeff_array[CALIB_COEFF_R_P2];
            const double s2 = cc_coeff_array[CALIB_COEFF_S_P2];
            
            // Calculate offsets from CALIB_COEFF coefficients using third-order polynomial.
            for (j=0; j<256; j++) {
                bdco[0][j] = p1*pow(j-s1, 3) + q1*(j-s1) + r1;
                bdco[1][j] = p2*pow(j-s2, 3) + q2*(j-s2) + r2;
            }
        }
        else
        {
            i_calib = SelectCalibrationData(t_first_sample_TM, first_sample_utc_TM, mc);
            if (debug >= 1) {
                CPrintf("    Using calibration file %i: %s\n", i_calib, mc->calib_meas_data[i_calib].LBL_filename);
            }
            for (j=0; j<256; j++) {
                bdco[0][j] = mc->CD[i_calib].C[j][1];
                bdco[1][j] = mc->CD[i_calib].C[j][2];
            }
        }



        //################################################################################################################
        //################################################################################################################
        // Find the correct calibration factors & offsets depending on E-FIELD/DENSITY, GAIN, ADC16/ADC20, 4/8 kHz FILTER
        //################################################################################################################
        //################################################################################################################
        {
            /*=====================================================
            * Offset to remove from all ADC16 8 kHz-filtered data.
            =====================================================*/
            double calib_ADC16_8kHz_offset_ADC16TM = 0.0/0.0;   // Temporary variable.
            if ((data_type==D16) && uses_8kHz_filter) {
                if      (writing_P1_data) { calib_ADC16_8kHz_offset_ADC16TM = CALIB_8KHZ_P1_OFFSET_ADC16TM; }
                else if (writing_P2_data) { calib_ADC16_8kHz_offset_ADC16TM = CALIB_8KHZ_P2_OFFSET_ADC16TM; }
                else if (writing_P3_data) { calib_ADC16_8kHz_offset_ADC16TM = CALIB_8KHZ_P1_OFFSET_ADC16TM - CALIB_8KHZ_P2_OFFSET_ADC16TM; }
                else {   YPrintf("Bug. Reached unreachable statement 1.");   ExitPDS(2);   }
            } else {
                calib_ADC16_8kHz_offset_ADC16TM = 0.0;
            }
            
            /*==============================================================================================
            * Offset to remove from all ADC20 data. /Functionality added by Erik P G Johansson 2015-06-02.
            * 
            * NOTE: Assumes that C_ADC20 is equal for both probes, but P3 is not used with ADC20 data(?).
            ==============================================================================================*/
            double calib_ADC20_offset_ADC16TM = 0.0/0.0;   // Temporary variable.
            if (data_type!=D16) {
                if      (writing_P1_data) { calib_ADC20_offset_ADC16TM = CALIB_ADC20_P1_OFFSET_ADC16TM; }
                else if (writing_P2_data) { calib_ADC20_offset_ADC16TM = CALIB_ADC20_P2_OFFSET_ADC16TM; }
                else if (writing_P3_data) { calib_ADC20_offset_ADC16TM = CALIB_ADC20_P1_OFFSET_ADC16TM - CALIB_ADC20_P2_OFFSET_ADC16TM; }
                else {   YPrintf("Bug. Reached unreachable statement 2.");   ExitPDS(2);   }
            } else {
                calib_ADC20_offset_ADC16TM = 0.0;
            }
            
            global_calib_offset_ADC16TM = calib_ADC16_8kHz_offset_ADC16TM + calib_ADC20_offset_ADC16TM;
        }
        

        
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
                if (writing_P1_data) {
                    //============
                    //  CASE: P1
                    //============
                    if (is_high_gain_P1) {   ccalf = CALIB_ADC16_FACTOR_CURRENT_G1;      }
                    else                 {   ccalf = CALIB_ADC16_FACTOR_CURRENT_G0_05;   }
                } else if (writing_P2_data) {
                    //============
                    //  CASE: P2
                    //============
                    if (is_high_gain_P2) {   ccalf = CALIB_ADC16_FACTOR_CURRENT_G1;      }
                    else                 {   ccalf = CALIB_ADC16_FACTOR_CURRENT_G0_05;   }
                } else if (writing_P3_data) {
                    //============
                    //  CASE: P3
                    //============
                    // NOTE: USES P1 to determine high/low gain for P3 for now!!! Undetermined what one should really use.
                    if (is_high_gain_P1) {   ccalf = CALIB_ADC16_FACTOR_CURRENT_G1;      }
                    else                 {   ccalf = CALIB_ADC16_FACTOR_CURRENT_G0_05;   }
                }
                else {   YPrintf("Bug. Reached unreachable statement 3.");   ExitPDS(2);   }
                
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
                
                if (writing_P1_data) {
                    //============
                    //  CASE: P1
                    //============
                    if (is_high_gain_P1) {
                        ccalf       = CALIB_ADC16_FACTOR_CURRENT_G1 / 16.0 * ADC_RATIO_P1;                        
                        ccalf_ADC16 = CALIB_ADC16_FACTOR_CURRENT_G1;
                    } else {
                        ccalf       = CALIB_ADC16_FACTOR_CURRENT_G0_05 / 16.0 * ADC_RATIO_P1;                        
                        ccalf_ADC16 = CALIB_ADC16_FACTOR_CURRENT_G0_05;
                    }

                    if(data_type==D20T || data_type==D201T) { // If using P1 ADC20 truncated data, compensate calibration factors for this.
                        ccalf *= 16;
                    }
                } else if (writing_P2_data) {
                    //============
                    //  CASE: P2
                    //============
                    if (is_high_gain_P2) {
                        ccalf       = CALIB_ADC16_FACTOR_CURRENT_G1 / 16.0 * ADC_RATIO_P2;                        
                        ccalf_ADC16 = CALIB_ADC16_FACTOR_CURRENT_G1;
                    } else {
                        ccalf       = CALIB_ADC16_FACTOR_CURRENT_G0_05 / 16.0 * ADC_RATIO_P2;                        
                        ccalf_ADC16 = CALIB_ADC16_FACTOR_CURRENT_G0_05;
                    }

                    if(data_type==D20T || data_type==D202T) { // If using P2 ADC20 truncated data, compensate calibration factors for this.
                        ccalf *= 16;
                    }
                }
                else {   YPrintf("Bug. Reached unreachable statement 4. This combination of settings is never supposed to have been used on the instrument.");   ExitPDS(2);   }
                
                // Other alternative than above shouldn't be possible, if so keep default ccalf.
            }
            
            ccalf *= ADC20_moving_average_bug_TM_factor;
        }   // if(bias_mode==DENSITY)
        else
        {
            //====================
            // CASE: E-FIELD MODE
            //====================
            vcalf_ADC16 = CALIB_ADC16_FACTOR_VOLTAGE;
            
            if(data_type==D16) {
                //=================
                //   CASE: ADC16
                //=================
                vcalf = CALIB_ADC16_FACTOR_VOLTAGE;
            } else {
                //=================
                //   CASE: ADC20
                //=================
                if      (writing_P1_data) {   vcalf = CALIB_ADC16_FACTOR_VOLTAGE / 16.0 * ADC_RATIO_P1;   }
                else if (writing_P2_data) {   vcalf = CALIB_ADC16_FACTOR_VOLTAGE / 16.0 * ADC_RATIO_P2;   }

                // If using ADC20 truncated data, compensate calibration factors for this. NOTE: Odd condition with "||"?
                if (data_type==D20T || data_type==D201T || data_type==D202T) {
                    vcalf *= 16;   // Increase cal factor by 16 for truncated ADC20 data.
                }
            }
            
            vcalf *= ADC20_moving_average_bug_TM_factor;
        }
    }   //  if(calib) ...
    

    
    strcpy(file_path, pds.spaths);  // Copy data path
    strcat(file_path, fname);       // For now put in root path, so add file name!
    
    CPrintf("    Writing PDS Data table file: %s\n",fname);
    if((pds.stable_fd=fopen(file_path, "w"))==NULL)                       // Open table file
    {
        CPrintf("    Couldn't open PDS TAB data file: %s!!\n", file_path);
        return -1;
    }
    
    if (id_code==D_SWEEP_P2_LC_16BIT_BIP || id_code==D_SWEEP_P1_LC_16BIT_BIP) { // Log compression used
        LogDeComp(buff,N_bytes,ilogtab); // Decompress log data in buff result returned in buff
    }
        
    if(param_type==SWEEP_PARAMS)
    {
        if((sw_info->formatv ^(sw_info->formatv<<1)) & 0x2) {   // Decode the four sweep types
            sw_bias_step_size_TM = -sw_info->height;    // Store height of step and set sign to a down sweep 
        } else {
            sw_bias_step_size_TM =  sw_info->height;   // Store height of step and set sign to a up sweep
        }
        sw_bias_voltage_TM = sw_info->start_bias;         // Get start bias
    }

    //============
    // Set biases
    //============
    if(writing_P1_data)
    {
        vbias  = curr->vbias1;
        vbias1 = curr->vbias1;
        ibias  = curr->ibias1;
        ibias1 = curr->ibias1;
    }

    if(writing_P2_data)
    {
        vbias  = curr->vbias2;
        vbias2 = curr->vbias2;
        ibias  = curr->ibias2;
        ibias2 = curr->ibias2;
    }

    // BUGFIX: "if" statement needed to interpret data for P3 (E field), macro 700, for 2006-12-19.
    // /Erik P G Johansson 2016-03-09
    if(writing_P3_data)
    {
        // P3=P1-P2 difference
        ibias1 = curr->ibias1;
        ibias2 = curr->ibias2;
        vbias1 = curr->vbias1;
        vbias2 = curr->vbias2;
    }

    utime_old=0;



    //#######################################################
    //#######################################################
    // ITERATE OVER ALL SAMPLES (I.E. OVER ROWS IN TAB FILE)
    //#######################################################
    //#######################################################
    for(k_tsweep_tmsmp=0,i_sample=0,j=0; i_sample<N_tmsmp; i_sample++)   // NOTE: ONLY increments i_sample.
    {
        // Convert data from signed 16 bit and signed 20 bit to native signed integer
        // In case of alternating 20 bit P1 and P2, this will be split into two label
        // and tab files.
        switch(data_type)
        {
            case D20:
                // Put together 8+8+4 bits to signed 20 bit number
                if(dop==2) // Doing probe 2, skip probe 1
                    j+=2;
                //==================================================================================================
                // EDIT FKJN 2015-02-12. Error in LAP flight software implementation of moving average of
                // non-truncated ADC20 data makes the lowest 4 bits garbage (~random). ==> Set those bits to zero.
                //==================================================================================================
                meas_value_TM=buff[j]<<12 | buff[j+1]<<4 | (((buff[N_tmsmp*2+(i_sample>>1)])>>(4*((i_sample+1)%2))) & 0x0F);
                SignExt20(&meas_value_TM); // Convert 20 bit signed to native signed
                if(ADC20_moving_average_enabled) {
                    meas_value_TM = meas_value_TM & 0xFFFF0;  // Clear the last 4 bits (in 20-bit TM data) since a moving-average bug in the flight software renders them useless.
                }
                j+=2;
                if(dop==1) { // Doing probe 1, skip probe 2
                    j+=2;
                }
                break;
                
            case D201:
            case D202:
                // Put together 8+8+4 bits to signed 20 bit number.
                //==================================================================================================
                // EDIT FKJN 2015-02-12. Error in LAP flight software implementation of moving average of
                // non-truncated ADC20 data makes the lowest 4 bits garbage (~random). ==> Set those bits to zero.
                //==================================================================================================
                meas_value_TM=buff[j]<<12 | buff[j+1]<<4 | (((buff[N_tmsmp*2+(i_sample>>1)])>>(4*((i_sample+1)%2))) & 0x0F);
                SignExt20(&meas_value_TM); // Convert 20 bit signed to native signed
                if(ADC20_moving_average_enabled) {
                    // Clear the last 4 bits (in 20-bit TM data) since a moving-average bug in the flight software renders them useless (~random).
                    meas_value_TM = meas_value_TM & 0xFFFF0;
                }
                j+=2;
                break;
                
            case D20T:
                // Put together 8+8 bits to 16 bit number
                if(dop==2) {// Doing probe 2, skip probe 1
                    j+=2;
                }
                meas_value_TM=((short int)(buff[j]<<8 | buff[j+1]));
                j+=2;
                if(dop==1) {// Doing probe 1, skip probe 2
                    j+=2;
                }
                break;
                
            case D16:
            default:
                meas_value_TM=((short int)(buff[j]<<8 | buff[j+1])); // Convert 16 bit into native
                j+=2;
        }

        //==========================================================
        // Derive different measures of time for the current sample.
        //==========================================================
        t_delta_s = i_sample * curr->sec_per_tmsmp;         // Calculate current time relative to first time (first sample). Unit: Seconds.                
        current_sample_sccd_corrected = curr->seq_time_corrected + t_delta_s;              // Calculate SCCD.
        ConvertSccd2Utc(current_sample_sccd_corrected, NULL, current_sample_utc_corrected);   // Decode raw time to UTC.
        
        //==============================================================================
        // Check for commanded bias (outside of macro lopp). If found, then set biases.
        //==============================================================================
        if(nbias>0 && bias!=NULL)
        {
            // Figure out if any extra bias settings have been done outside of macros.
            utime = (unsigned int)t_first_sample_TM + t_delta_s;   // Current time in raw UTC format
            
            extra_bias_setting=0;
            for(l=(nbias-1);l>=0 && extra_bias_setting==0;l--)   // Go through all extra bias settings (iterate backwards in time).
            {
                if(bias[l][0]<=utime && bias[l][0]>utime_old)   // Find any bias setting before current time.
                {
                    extra_bias_setting=1;
                    
                    // Check if any mode change happened after the found bias setting, but before the current time.
                    for(m=0;m<nmode;m++)
                    {
                        utime_old=utime;
                        if(mode[m][0]>bias[l][0] && mode[m][0]<=utime)
                        {
                            // CASE: A mode change happened (1) after the found bias setting, and (2) before current data (utime).
                            extra_bias_setting=0;
                            break;
                        }
                    }
                    break;   // Do NOT continue iterating over other bias settings.
                }
            }
            
            /*=================================================================================================================
            * FKJN 2014-09-25: Extra bias commands are only allowed for certain macros!!! That's three exclamation marks.
            * We need to find the macro, compare to a list of macros and decide if we should let it pass or not.
            * If a bias command is issued on forbidden macros, the bias will change for maximum one Macro Loop (see Meds),
            * but pds will not know when it changes back.
            * 
            * FKJN 2014-10-31 added macro 515 & 807
            *===============================================================================================================*/
            if(extra_bias_setting)
            {
                FindP(&comm,&property1,"INSTRUMENT_MODE_ID",1,DNTCARE);	  // tstr4 is now macro ID on form "MCID0X%04x" we need the 4 numerals.
                
                /* BUG FIX: Old code interpreted macro ID number in string as a decimal number when it should have
                * been interpreted as a hexadecimal number. Old code should not have been a problem as long as
                * (1) one does not need to check for macro numbers containing letters, and
                * (2) if the code failed in a good way for non-decimal numbers which it seemed to do.
                * /Erik P G Johansson 2015-12-07
                */            
                char* tempstr = strndup(property1->value+6,4);   // Extract 4 (hex) digits. (Remove non-digit characters "MCID0X".)
                sscanf(tempstr, "%x", &macro_id);    // Interpret string as a HEXADECIMAL representation of a number.
                free(tempstr);
                
                if( macro_id == 0x505 || macro_id == 0x506 || macro_id == 0x604 || macro_id == 0x515 || macro_id == 0x807 )
                {
                    extra_bias_setting = 0;
                    YPrintf("Forbidden bias setting found at %s (not corrected for delay). Macro %x\n", first_sample_utc_TM, macro_id);
                    // Add some way of detecting that this is the first macro loop with extra_bias_settings?
                }
            }
            
            if(extra_bias_setting)
            {
                vbias1 = ((bias[l][1] & 0xff00)>>8);  // Override macro present voltage bias p1.
                vbias2 =  (bias[l][1] & 0xff);        // Override macro present voltage bias p2.
                ibias2 = ((bias[l][2] & 0xff00)>>8);  // Override macro present current bias p1.
                ibias1 =  (bias[l][2] & 0xff);        // Override macro present current bias p2.
                /* The above lines were corrected by aie@irfu.se 2012-08-22 as the current bias is permuted 
                 * in the bias command. Original code:
                 *      curr->ibias1=((bias[l][2] & 0xff00)>>8);  // Override macro present current bias p1 
                 *      curr->ibias2= (bias[l][2] & 0xff);        // Override macro present current bias p2 
                 */
                
                CPrintf("    Extra bias setting applied at: %s \n", current_sample_utc_corrected);
                CPrintf("      Density P1: 0x%02x P2: 0x%02x\n",vbias1,vbias2);
                CPrintf("      E_Field P1: 0x%02x P2: 0x%02x\n",ibias1,ibias2);
                
                // Override biases
                if(writing_P1_data)
                {
                    vbias=vbias1;
                    ibias=ibias1;
                }
                
                // Override biases
                if(writing_P2_data)
                {
                    vbias=vbias2;
                    ibias=ibias2;
                }
                
                extra_bias_setting=0;
            }
        }   // if(nbias>0 && bias!=NULL)
        
        /*========================================================================================================
        * Offset to be subtracted from every CALIB sample (not EDITED). Analogous to global_calib_offset_ADC16TM
        * except that it (potentially) varies for every sample, hence the prefix "local_".
        * Should contain the offsets in global_calib_offset_ADC16TM.
        =======================================================================================================*/
        double local_calib_offset_ADC16TM = global_calib_offset_ADC16TM;
        
        /* Subtract offset for ADC16 non-negative values.
         * See comments on ADC16_EDITED_NONNEGATIVE_OFFSET_ADC16TM / ADC16_CALIB_NONNEGATIVE_OFFSET_ADC16TM. */
        if(data_type==D16) {
            if(meas_value_TM>=0) {
                if (!calib) {
                    // CASE: EDITED
                    meas_value_TM = meas_value_TM - ADC16_EDITED_NONNEGATIVE_OFFSET_ADC16TM;    // NOTE: Subtraction from actual measured value.
                } else {
                    // CASE: CALIB
                    
                    // Subtract ADC16 nonnegative values offset using local_calib_offset_ADC16TM instead of meas_value_TM,
                    // since the latter is an integer variable that can not handle decimal values.
                    // NOTE: Addition to offset which will later be subtracted from actual measured value.
                    local_calib_offset_ADC16TM = local_calib_offset_ADC16TM + ADC16_CALIB_NONNEGATIVE_OFFSET_ADC16TM;    
                }
            }
        }
        
        /*============================================================================================================
        * Set variables "current_TM" and "voltage_TM" to bias and measured values
        * (Update variable "k_tsweep_tmsmp" for the above purpose)
        * -----------------------------------------------------------------------
        * NOTE: This is the only place where "current_TM" and "voltage_TM" are set.
        * Always measured current in density mode. Current bias in E field mode for P1, P2 data (undefined for P3).
        * Always measured voltage in E field mode. Voltage bias in density mode for P1, P2 data (undefined for P3).
        ===========================================================================================================*/
        if(bias_mode==DENSITY)
        {
            //====================
            // CASE: DENSITY MODE
            //====================
            current_TM = meas_value_TM; // Set sampled current value in TM units
            if(param_type==SWEEP_PARAMS)
            {   
                //=============
                // CASE: Sweep
                //=============
                // Handle sweep stepping, and changing sweep direction.
                // IMPLEMENTATION NOTE: Note that N_non_tsweep_tmsmp is set such that the last bias value of the last bias plateau is omitted.
                // This is due to bug in RPCLAP according to pds source code comment (by Reine Gill). /Erik P G Johansson 2018-07-30.
                if (i_sample<N_non_tsweep_tmsmp) {
                    //==========================================================
                    // CASE: Current sample is not part of a true sweep plateau
                    //==========================================================
                    voltage_TM = vbias;   // Set initial voltage bias before sweep starts. Not defined for P3.
                }
                else
                {
                    //======================================================
                    // CASE: Current sample is part of a true sweep plateau
                    //======================================================
                    voltage_TM = sw_bias_voltage_TM;   // Set value used before changing the bias, prevents start bias value to be modified before used.
                    k_tsweep_tmsmp++;
                    if (!(k_tsweep_tmsmp%N_plateau_tmsmp)) {   // Every new step set a new bias voltage
                        sw_bias_voltage_TM += sw_bias_step_size_TM;     // Change bias
                    }
                    if (sw_info->formatv & 0x1)   // If up-down or down-up sweep, check if direction shall change
                    {
                        if(k_tsweep_tmsmp==(sw_info->N_tsweep_bias_steps*N_plateau_tmsmp/2)) {   // Time to change direction ? 
                            sw_bias_step_size_TM = -sw_bias_step_size_TM;         // Change direction
                        }
                    }
                }
            }
            else {
                //=================
                // CASE: Not sweep
                //=================
                voltage_TM = vbias; // Set FIX Density bias in TM unit. Not defined for P3.
            }
        }
        else
        {
            //====================
            // CASE: E-FIELD MODE
            //====================
            current_TM = ibias;           // Set FIX Current bias in TM units. Not defined for P3.
            voltage_TM = meas_value_TM;   // Set sampled voltage value in TM units.
        }
            
        pthread_testcancel();



        //#########################
        //#########################
        // WRITE TO DISK (ONE ROW)
        //#########################
        //#########################
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
                double ccurrent;
                ccurrent  = ccalf * ((double)(current_TM));    // Factor calibration
                ccurrent -= ccalf_ADC16 * local_calib_offset_ADC16TM;

                if(param_type==SWEEP_PARAMS)
                {
                    //=====================
                    // CASE: SWEEP (ADC16)
                    //=====================
                    
                    // NOTE: No occurrence of SENS_P1P2 in the probe data conditions (unknown why).
                    if(strcmp(sw_info->resolution,"FINE"))
                    {
                        //=========================
                        // CASE: NOT(!) FINE SWEEP
                        //=========================
                        double cvoltage;
                        if      (writing_P1_data)   {   ccurrent -= ccalf_ADC16 * bdco[0][voltage_TM];   cvoltage = v_conv.C[voltage_TM][1];   }
                        else if (writing_P2_data)   {   ccurrent -= ccalf_ADC16 * bdco[1][voltage_TM];   cvoltage = v_conv.C[voltage_TM][2];   }
                        ccurrent = handle_saturation(
                            ccurrent,
                            current_TM,
                            saturation_phys_min,
                            saturation_phys_max,
                            saturation_TM_1,
                            saturation_TM_2);
                        fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,   ccurrent,   cvoltage);
                    }
                    else
                    {
                        //==================
                        // CASE: FINE SWEEP
                        //==================                                
                        // NOTE/BUG: Offset and factor calibration, voltage is not entirely correct here!!
                        // We should perhaps have a calibration mode for fine sweeps in space
                        // but it would be rather many 4096.
                        
                        // Edit FKJN 2014-09-02: Here we need to convert a number from 0-4096 (p1_fine_offs*256 + voltage)
                        // to a number from 0-255 if we want to use the same offset calibration file.
                        double cvoltage;
                        // NOTE: f_conv.C[ ... ][i] : [i] refers to column i+1 in the file from which the data is read (i.e. not probe i).
                        if      (writing_P1_data)   {   ccurrent -= ccalf_ADC16 * bdco[0][voltage_TM];   cvoltage = f_conv.C[ (sw_info->p1_fine_offs*256+voltage_TM) ][2];   }
                        else if (writing_P2_data)   {   ccurrent -= ccalf_ADC16 * bdco[1][voltage_TM];   cvoltage = f_conv.C[ (sw_info->p2_fine_offs*256+voltage_TM) ][3];   }
                        ccurrent = handle_saturation(
                            ccurrent,
                            current_TM,
                            saturation_phys_min,
                            saturation_phys_max,
                            saturation_TM_1,
                            saturation_TM_2);
                        fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,   ccurrent,   cvoltage);
                    }
                }   // if(param_type==SWEEP_PARAMS)
                else
                {
                    //===============================
                    // CASE: NOT SWEEP (ADC16/ADC20)
                    //===============================
                    if (writing_P1_data || writing_P2_data) {
                        double cvoltage;
                        if      (writing_P1_data)   {   ccurrent -= ccalf_ADC16 * bdco[0][vbias1];   cvoltage  = v_conv.C[vbias1][1];   }
                        else if (writing_P2_data)   {   ccurrent -= ccalf_ADC16 * bdco[1][vbias2];   cvoltage  = v_conv.C[vbias2][2];   }
                        ccurrent = handle_saturation(
                            ccurrent,
                            current_TM,
                            saturation_phys_min,
                            saturation_phys_max,
                            saturation_TM_1,
                            saturation_TM_2);
                        fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,   ccurrent,   cvoltage);
                    }
                    else if(writing_P3_data)
                    {
                        ccurrent -= ccalf_ADC16 * (bdco[0][vbias1] - bdco[1][vbias2]);
                        // NOTE: NO SATURATION DETECTION, since method does not work for P3.
                        
                        // Write ONE CURRENT (difference), TWO VOLTAGES (one per probe).
                        fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e,%14.7e\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,
                            ccurrent,   v_conv.C[vbias1][1],   v_conv.C[vbias2][2]);
                    }

                }   // if(param_type==SWEEP_PARAMS) ... else ...

            }   // if(bias_mode==DENSITY)
            else // Assume bias mode is E_FIELD no other possible
            {
                //====================
                // CASE: E-FIELD MODE
                //====================
                double cvoltage;
                cvoltage  = vcalf * ((double) voltage_TM);
                cvoltage -= vcalf_ADC16 * local_calib_offset_ADC16TM;
                if(writing_P3_data) {
                    //==========
                    // CASE: P3
                    //==========
                    // NOTE: No saturation detection, since method does not work for P3.
                    fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e,%14.7e\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,
                            i_conv.C[ibias1][1],   i_conv.C[ibias2][2],   cvoltage); // Write time, calibrated currents 1 & 2, and voltage
                }
                else
                {
                    //================
                    // CASE: P1 or P2
                    //================
                    double ccurrent;
                    if      (writing_P1_data) {   ccurrent = i_conv.C[ibias1][1];   }
                    else if (writing_P2_data) {   ccurrent = i_conv.C[ibias2][2];   }
                    cvoltage  = handle_saturation(
                        cvoltage,
                        voltage_TM,
                        saturation_phys_min,
                        saturation_phys_max,
                        saturation_TM_1,
                        saturation_TM_2);
                    fprintf(pds.stable_fd,"%s,%016.6f,%14.7e,%14.7e\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,   ccurrent,   cvoltage);
                }
            }   // if(bias_mode==DENSITY) ... else ...
        }   // if(calib)
        else
        {
            //###################
            // CASE: EDITED data
            //###################
            if(writing_P3_data)
            {
                //==========
                // CASE: P3
                //==========
                // For difference data P1-P2 we need to add two bias vectors. They can be different!
                if(bias_mode==DENSITY) {
                    fprintf(pds.stable_fd,"%s,%016.6f,%7d,%7d,%7d\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,
                            current_TM,   vbias1,   vbias2);    // Write TWO VOLTAGE bias vectors.
                } else {
                    fprintf(pds.stable_fd,"%s,%016.6f,%7d,%7d,%7d\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,
                            ibias1,   ibias2,   voltage_TM);    // Write TWO CURRENT bias vectors.
                }
            }
            else
            {
                //================
                // CASE: P1 or P2
                //================
                // Line width (incl. CR+LF): 26+1+16 + 1+7+1+7 + 2
                fprintf(pds.stable_fd,"%s,%016.6f,%7d,%7d\r\n",current_sample_utc_corrected,current_sample_sccd_corrected,
                        current_TM,   voltage_TM); // Write time, current and voltage
            }
        }   // if(calib) ... else ...
    }   // for(k_tsweep_tmsmp=0,i_sample=0,j=0;i_sample<N_tmsmp;i_sample++)    // Iterate over all samples
    
    fclose(pds.stable_fd);
    pthread_testcancel();



    if(debug>1) // If debugging level is larger than 1, dump common and LAP-dictionary PDS parameters.
    {
        DumpPrp(&comm); // Debugging  
        DumpPrp(&dict); // Debugging  
    }
    
    return 0;
}   // WritePTAB_File



/* Determine values used for detecting saturation in the current data using function "handle_saturation".
 *
 * This function is separated from "handle_saturation" (called once per sample) in order to only derive these values once
 * per TAB file instead of once per sample, and hence (hopefully) speed up pds.
 * 
 * ARGUMENTS
 * =========
 * Return value: x_phys_min
 * Return value: x_phys_max
 * Return value: x_TM_saturation_1
 * Return value: x_TM_saturation_2
 * is_ADC20_nontrunc : True if-and-only-if data type is non-truncated ADC20 data.
 */
void set_saturation_limits(
    double* x_phys_min,
    double* x_phys_max,
    int* x_TM_saturation_1,
    int* x_TM_saturation_2,
    int is_ADC20_nontrunc, int is_high_gain, int is_Efield)
{    
    // TODO-NEED-INFO: How specify argument value/expression that signifies NON-truncated ADC20 data?!

    // NOTE: High/low gain only exists for density mode.
    //  PROPOSAL: ADC20 saturation value truncated.

    // Set x_TM_saturation_1/2
    if (is_ADC20_nontrunc) {
        *x_TM_saturation_1 = SATURATION_ADC20_NONTRUNC_TM_VALUE_1;
        *x_TM_saturation_2 = SATURATION_ADC20_NONTRUNC_TM_VALUE_2;
    } else {
        *x_TM_saturation_1 = SATURATION_ADC16_ADC20_TRUNC_TM_VALUE_1;
        *x_TM_saturation_2 = SATURATION_ADC16_ADC20_TRUNC_TM_VALUE_2;
    }

    // Set x_phys_min/max
    if (is_Efield)
    {
        *x_phys_min = SATURATION_EFIELD_MIN;
        *x_phys_max = SATURATION_EFIELD_MAX;
    }
    else
    {
        if (is_high_gain) {
            *x_phys_min = SATURATION_DENSITY_HG_MIN;
            *x_phys_max = SATURATION_DENSITY_HG_MAX;
        } else {
            *x_phys_min = SATURATION_DENSITY_LG_MIN;
            *x_phys_max = SATURATION_DENSITY_LG_MAX;
        }
    }
}



/* Replace calibrated value with special value if it is deemed to be saturated.
 * 
 * NOTE: When using min-max limits, values only count as saturated if they are strictly
 * OUTSIDE the min-max value range (e.g. x_phys<x_phys_min).
 * NOTE: Function/algorithm does not work for P3.
 */
double handle_saturation(
    double x_phys,
    int    x_TM,
    double x_phys_min,
    double x_phys_max,
    int x_TM_saturation_1,
    int x_TM_saturation_2)
{
    // IMPLEMENTATION NOTE:
    // Uses regular if-then-else, not #ifdef-#else-#endif, to
    // (1) make USE_SPICE easier to convert to a C variable (e.g. for CLI argument flag),
    // (2) ensure compilation checking of code for both cases.
    if (USE_SATURATION_LIMITS)
    {
        if (   (x_phys < x_phys_min)
            || (x_phys > x_phys_max)
            || (x_TM   == x_TM_saturation_1)
            || (x_TM   == x_TM_saturation_2))
        {
            return SATURATION_TAB_CONSTANT;
        } else {
            return x_phys;
        }
    }
    else
    {
        // CASE: No saturation testing.
        return x_phys;
    }
}



/*================================================================================================================================================
 * WRITE LABEL FILE (.LBL)
 * 
 * Uncertain what "dop" refers to and what the difference compared to "curr-->sensor" is. See "WritePTAB_File" (assuming it has the same meaning).
 * NOTE: This function only has very little dependence on "dop". Compare "WritePTAB_File".
 * 
 * NOTE: This function does not have a MISSING_CONSTANT value for SATURATION_TAB_CONSTANT and therefore does not technically produce proper
 * PDS-compliant LBL files.
 ================================================================================================================================================*/
int WritePLBL_File(
    char *path,
    char *fname,
    curr_type *curr,
    int N_tmsmp,
    int id_code,
    int dop,
    int N_non_tsweep_tmsmp,
    int param_type)
{
    char fullname[PATH_MAX];
    char tstr1[256];
    char tstr2[256];
    
    char bias_mode = GetBiasMode(curr, dop);      // Name and type analogous to curr_type_def#bias_mode1/2.
    
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
                    /*
                     * BUG FIX: The tstr1 value end up in an unquoted PDS keyword/attribute value and those must not contain
                     * dash.
                     * Source: "Planetary Data Systems Standards Reference", Version 3.6, p12-11, section 12.3.4+12.4.2.
                     */
                    strcpy(tstr1, "P1_P2");   // Difference
                    diff=1;   // Must be 16 bit diff data.
                    // The LAP instrument permits using 20 bit data for P3 but that is for some reason not an interesting case
                    // and Anders Eriksson states (2016-03-10) that it will never be used in practice.
                }
                if(dop==1)
                    strcpy(tstr1,"P1");
                if(dop==2)
                    strcpy(tstr1,"P2");
                break;
        }


        //====================================
        // Derive the row width (incl. CR+LF)
        //====================================
        // Sum up row bytes going from left to right, more or less.
        //
        // Example row, EDITED, P1/P2 (not P3):
        // 2016-05-05T04:37:52.610109,421043787.239258,    507,      8<CR><LF>
        //               26          1       16       1  7    1   7    1   1   = 59+2 = 61 bytes/row
        //row_bytes=59;
        row_bytes = 26+1+16 +2;   // = 45 = 59-14; UTC string + comma + decimalized spacecraft clock + CR+LF.
        if(calib) // If we do calibration
        {
            //##################
            // CASE: CALIBRATED
            //##################
            //row_bytes+=16; // Calibrated data has wider columns.
            row_bytes += 2*(1+14); // += 30; Add TWO columns: CALIBRATED current + voltage.

            if(diff) // Any 16 bit difference data ?
            {
                //row_bytes+=15; // Extra calibrated bias column current or voltage
                row_bytes += 1+14;   // Add ONE extra column: CALIBRATED current OR voltage.
                columns++;     // One extra column
            }
        }
        else
        {
            //##############
            // CASE: EDITED
            //##############

            row_bytes += 2*(1+7);   // += 16; Add TWO columns: EDITED current + voltage.

            if(diff)
            {
                //row_bytes+=7; // Extra bias column current or voltage
                row_bytes += 1+7;   // Add ONE extra column: EDITED current OR voltage.
                columns++;    // One extra column
            }
        }
        sprintf(tstr2,"%02d",row_bytes);

        
        // if-else looks unnecessary.
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
                    InsertTopQV(&dict, tstr2, N_non_tsweep_tmsmp); 
                    // InsertTopQV(&dict,"ROSETTA:LAP_INITIAL_SWEEP_SMPLS",N_non_tsweep_tmsmp);   // Original line.
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
                fprintf(pds.slabel_fd,"ROWS               = %d\r\n",N_tmsmp);
                fprintf(pds.slabel_fd,"COLUMNS            = %d\r\n",columns);
                
                
                fprintf(pds.slabel_fd,"ROW_BYTES          = %d\r\n",row_bytes);
                
                strcpy(tstr2,IDList[id_code]);      // Get ID code name as description
                TrimWN(tstr2);                      // Remove trailing whitespace
                fprintf(pds.slabel_fd,"DESCRIPTION        = \"%s\"\r\n",tstr2); // Add it
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN, first one
                fprintf(pds.slabel_fd,"NAME        = UTC_TIME\r\n");
                fprintf(pds.slabel_fd,"DATA_TYPE   = TIME\r\n");
                fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte); 
                fprintf(pds.slabel_fd,"BYTES       = 26\r\n"); start_byte+=(26+1);
                fprintf(pds.slabel_fd,"DESCRIPTION = \"UTC TIME\"\r\n");
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN
                fprintf(pds.slabel_fd,"NAME        = OBT_TIME\r\n");
                fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                fprintf(pds.slabel_fd,"BYTES       = 16\r\n"); start_byte+=(16+1);
                fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                fprintf(pds.slabel_fd,"UNIT        = SECONDS\r\n");
                fprintf(pds.slabel_fd,"FORMAT      = \"F16.6\"\r\n");
                fprintf(pds.slabel_fd,"DESCRIPTION = \"SPACECRAFT ONBOARD TIME SSSSSSSSS.FFFFFF (TRUE DECIMAL POINT)\"\r\n");
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN

                //============================================================
                // Create 1 or 2 CURRENT columns (2 columns iff. P3, E field)
                //============================================================
                if(calib)
                {
                    //#############
                    // CASE: CALIB
                    //#############

                    if(diff && bias_mode==E_FIELD)
                    {
                        //#####################################
                        // CASE: We have difference data P1-P2
                        //#####################################

                        // We need an extra current bias column.
                        fprintf(pds.slabel_fd,"NAME        = P1_CURRENT\r\n");
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        fprintf(pds.slabel_fd,"BYTES       = 14\r\n");start_byte+=(14+1);
                        fprintf(pds.slabel_fd,"UNIT        = AMPERE\r\n");
                        fprintf(pds.slabel_fd,"FORMAT      = \"E14.7\"\r\n");
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CALIBRATED CURRENT BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN
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
                    //##############
                    // CASE: EDITED
                    //##############

                    if(diff && bias_mode==E_FIELD) // We need an extra current bias column
                    {
                        fprintf(pds.slabel_fd,"NAME        = P1_CURRENT\r\n");  
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        //fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                        fprintf(pds.slabel_fd,"BYTES       = 7\r\n");start_byte+=(7+1);
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CURRENT BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN
                        fprintf(pds.slabel_fd,"NAME        = P2_CURRENT\r\n");  
                    }
                    else
                        fprintf(pds.slabel_fd,"NAME        = %s_CURRENT\r\n",tstr1);  
                    
                    fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                    fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                    //fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                    fprintf(pds.slabel_fd,"BYTES       = 7\r\n");start_byte+=(7+1);

                    if(bias_mode==E_FIELD)
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CURRENT BIAS\"\r\n");
                    else
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"MEASURED CURRENT\"\r\n");
                }
                
                fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN
                


                //=================================================================
                // Create 1 or 2 VOLTAGE columns (2 columns iff. P3, density mode)
                //=================================================================
                if(calib)
                {
                    //#############
                    // CASE: CALIB
                    //#############

                    if(diff && bias_mode==DENSITY)
                    {
                        //#####################################
                        // CASE: We have difference data P1-P2
                        //#####################################

                        // We need an extra voltage bias column.
                        fprintf(pds.slabel_fd,"NAME        = P1_VOLTAGE\r\n");
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_REAL\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        fprintf(pds.slabel_fd,"BYTES       = 14\r\n");start_byte+=(14+1);
                        fprintf(pds.slabel_fd,"UNIT        = VOLT\r\n");
                        fprintf(pds.slabel_fd,"FORMAT      = \"E14.7\"\r\n");
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"CALIBRATED VOLTAGE BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN
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
                    //##############
                    // CASE: EDITED
                    //##############
                    if(diff && bias_mode==DENSITY)
                    {
                        //#####################################
                        // CASE: We have difference data P1-P2
                        //#####################################

                        // We need an extra voltage bias column
                        fprintf(pds.slabel_fd,"NAME        = P1_VOLTAGE\r\n");
                        fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                        fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                        //fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                        fprintf(pds.slabel_fd,"BYTES       = 7\r\n");start_byte+=(7+1);
                        fprintf(pds.slabel_fd,"DESCRIPTION = \"VOLTAGE BIAS\"\r\n");
                        fprintf(pds.slabel_fd,"END_OBJECT = COLUMN\r\n");
                        
                        fprintf(pds.slabel_fd,"OBJECT     = COLUMN\r\n");          // OBJECT = COLUMN
                        fprintf(pds.slabel_fd,"NAME        = P2_VOLTAGE\r\n"); 
                    }
                    else
                        fprintf(pds.slabel_fd,"NAME        = %s_VOLTAGE\r\n",tstr1);  
                    
                    fprintf(pds.slabel_fd,"DATA_TYPE   = ASCII_INTEGER\r\n");
                    fprintf(pds.slabel_fd,"START_BYTE  = %d\r\n",start_byte);
                    //fprintf(pds.slabel_fd,"BYTES       = 6\r\n");start_byte+=(6+1);
                    fprintf(pds.slabel_fd,"BYTES       = 7\r\n");start_byte+=(7+1);
                    
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
// Do nothing if they contain no data.
void FreeBuffs(buffer_struct_type *b0,
               buffer_struct_type *b1,
               buffer_struct_type *b2,
               buffer_struct_type *b3)
{
    YPrintf("Freeing up circular buffers\n");
    if(b0->data!=NULL) FreeBuffer(b0); // Free circular buffer
    if(b1->data!=NULL) FreeBuffer(b1); // Free circular buffer
    if(b2->data!=NULL) FreeBuffer(b2); // Free circular buffer
    if(b3->data!=NULL) FreeBuffer(b3); // Free circular buffer
}



// Get data from circular buffer
// 
// bs   : Circular buffer
// buff : Destination buffer
// len  : Number of bytes to retrieve
//
// NOTE: Effectively a wrapper around GetB but this one waits for the circular buffer to be filled with (enough) data
// if the requested amount of data can not be retrieved immediately.
int GetBuffer(buffer_struct_type *bs, unsigned char *buff, int len)
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
            sched_yield();        // nanosleep is not good here, can't delay less than us resolution
        }
    }
    return 0;
}



// Return data for one HK packet.
//
// ARGUMENTS
// =========
// ch   : Circular buffer, source of data
// buff : Destination buffer for (some) packet data.
// Return values:
// sccd : Time derived from HK packet. Raw, spacecraft clock count (double; true decimals)
// 
// NOTE: Does NOT RETURN until it has all the requested data.
// NOTE: Some buffer data is thrown away.
int GetHKPacket(buffer_struct_type *ch, unsigned char *buff, double *sccd)
{
    unsigned int N_bytes; // Length variable
    
    // Get data
    GetBuffer(ch,buff,16);
    
    N_bytes=((buff[4]<<8) | buff[5])-9-2;     // Get LAP HK data length
    HPrintf("HK packet, length: %d\n",N_bytes);
    
    if(N_bytes>LAP_HK_LEN) {
        N_bytes=LAP_HK_LEN; // Packet to long, force standard length. Add warning in the future...
    }
    
    *sccd = DecodeSCTime2Sccd(&buff[6]);            // Decode S/C time into raw time
    
    GetBuffer(ch,buff,2);                    // Skip 2 bytes
    
    GetBuffer(ch,buff,N_bytes);  // Get data from circular HK buffer
    
    return 0;
}     



// Reads packet from (arbitrary!) circular buffer, logs its existence but otherwise ignores it.
//
void DumpTMPacket(buffer_struct_type *cs,unsigned char packet_id)
{
    unsigned int N_bytes;
    char utc[32];
    double sccd;
    unsigned char buff[14];
    
    GetBuffer(cs,buff,14); // Get 14 bytes from circular buffer
    
    N_bytes=((buff[2]<<8) | buff[3])-9;           // Get "data" length
    PPrintf("    Packet ID: 0x0d%02x , Data length: %d Discarding\n",packet_id,N_bytes);
    sccd = DecodeSCTime2Sccd(&buff[4]);       // Decode S/C time into raw time
    ConvertSccd2Utc(sccd, utc, NULL);        // Decode raw time to PDS compliant date format
    PPrintf("    SCET Time: %s OBT Raw Time: %014.3f\n", utc, sccd);
    Forward(cs,N_bytes);// Move forward, thus skip packet
}



// Test synchronisation ahead.
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
        Append(p,"DESCRIPTION","\"SPACECRAFT ONBOARD TIME SSSSSSSSS.FFFFFF (TRUE DECIMAL POINT)\"");
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
        Append(p,"BYTES","5");
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
        char tempstr[MAX_STR];
        
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
        Append(p,"PRODUCT_ID",            "\"INDEX\"");
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
        Append(p,"NAME",          "PRODUCT_ID"); // This I think is weird, however PVV complains with FILE_NAME
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
        Append(p,"DESCRIPTION",   "\"An identifier unique for this data set\"");
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
        
        // Count index table lines! Assume all lines are shorter than dummy line.
        // It is needed in a keyword below
        while(fgets(dline,PATH_MAX,pds.itable_fd) != NULL) {
            index_cnt++;
        }
        
        rewind(pds.ilabel_fd); // Rewind index label to start
        
        sprintf(tstr,"%d",index_cnt);
        SetP(p,"FILE_RECORDS",tstr,1);             // Set number of file records in index label file
        SetP(p,"ROWS",tstr,1);                     // Set number of rows
        SetP(p,"DATA_SET_ID",m->data_set_id,1);    // Set DATA SET ID 
        
        GetCurrentUtc0(tstr);  // Get current UTC time     
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
 * Routine for reading a row in text table. Each column is separated by "separator".
 * Effetively reads out the strings in two columns.
 *
 * Input  : str       : String to parsed/analyzed.
 * Output : left      : The substring bounded by the (k_sep-1)'th (or string beginning) and k_sep'th     occurrance of "separator".
 * Output : right     : The substring bounded by the k_sep'th                           and (k_sep+1)'th occurrance of "separator" (or end-of-string).
 * Input  : separator : 
 * Input  : k_sep     : The occurrence of "separator" that is to be used. k_sep=1 <==> First occurrance.
 * Return value : min(occurs, <nbr of separators in str>)
 */
int Separate(char *str, char *left, char *right, char separator, int k_sep)
{
    int len;
    int i;
    int i_sep = 0;
    char *lpos;
    char *rpos;
    
    lpos = str; 
    rpos = str;
    
    len = strlen(str);
    if(str!=NULL)
    {
        for(i=0; i<(len-1); i++)
        {
            if(str[i]==separator)
            {
                lpos = rpos;
                rpos = &str[i+1];
                i_sep++;         // Increment occurrence of separator.
            }
            
            if(i_sep==k_sep)
            {
                // Copy substring at lpos to (rpos-1) to "left".
                for(;lpos<(rpos-1);) {
                    *(left++)=*(lpos++);
                }
                *left='\0';
                
                // Copy substring at rpos to (before next separator or LF) to "right".
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
    
    // ASSERTIONS
    if (str==NULL)            return -1;
    if ((len=strlen(str))<=0) return -1;
    
    // Convert all line feeds and carriage returns to whitespace.
    for(i=0;i<len;i++) {
        if(str[i]=='\n' || str[i]=='\r') {
            str[i]=' ';
        }
    }
    
    pos=str;   // Pointer to beginning of current string (at the current point in algorithm).
    
    // Remove all initial whitespace.
    nlen=len;
    for(i=0;i<len;i++)
    {
        if(state==0)
        {
            if(str[i]==' ') {
                nlen--;
                continue;
            } else {
                state=1;   // Effectively stop checking for initial whitespace and start moving remainder of string.
            }
        }
        *(pos++)=str[i];
    }
    
    // Remove all trailing whitespace.
    for(i=nlen-1;i>=0;i--)
    {
        if(str[i]==' ') {
            continue;
        } else {
            str[i+1]='\0'; //Set new end of string
            break;
        }
    }
    
    return 0;
}



// Trim away (1) leading and trailing quotes, and (2) all newlines (and CR and LF).
// Or something very similar.
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
    for(i=0;i<len;i++) // Remove all leading quotes
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
        *(pos++)=str[i];   // Copy/"move" character N step back. (pos is incremented AFTER returning its value.)
    }
    
    for(i=nlen-1;i>=0;i--)  // Remove all trailing quotes. Iterate from the back to the front.
    {
        if(str[i]=='\"') 
        {
            str[i]='\0';
            continue;
        }
        else
        {
            if(str[i]==' ') continue;
            str[i+1]='\0';   // Set new end of string
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

    // NOTE: Documentation: "The pointer returned by readdir() points to data which may be overwritten by another call to readdir()
    // on the same directory stream. This data is not overwritten by another call to readdir() on a different directory stream."
    // ==> Results do (presumably) not need to be deallocated.
    dentry=readdir(de);  // Get first entry
    
    // Do a linear search through all filenames.
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
// MODIFIES the path string to make sure it ends with "/".
// 
// ARGUMENTS
// =========
// INPUT:  path: Path to directory
// OUTPUT: path: Path to directory that ends with "/".
// 
// NOTE: Should only be used for DIRECTORY PATHS.
// NOTE: "info_txt" is only used for log messages. NOT printed to logs, but stdout/stderr.
int SetupPath(char *info_txt,char *path)
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
    
    // If mkdir is not thread safe this might cause problems. If so add mutex lock.
    err=mkdir(npath,0775); 
    
    if(!err || err==EEXIST) return 0; // If no error or it already exist, return ok!
    
    return err; // Else we return the error code.
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
    
    GetCurrentUtc0(strp); // Get universal time
    sprintf(name,"Default_%s.lap",strp);
    
    if((una_dir = opendir(pds.uapath))==NULL) // Open UnAccepted_Data directory
        return -1;
    
    rewinddir(una_dir);       // Rewind
    dentry=readdir(una_dir);  // Get first entry
    
    while(dentry!=NULL) // Do a linear search through all filenames to find the last one.
    {  
        if(!fnmatch("UNA_*_*.lap",dentry->d_name,0)) // Match filename to pattern
        {
            strcpy(lastm,dentry->d_name);            // Get matching file name
            lastm[9]='\0';                           // We only care about number part
            if(sscanf(&lastm[4], "%d", &tmp))        // Get number of file
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



/*
 * Find out if a string matches a pattern string.
 * Intended for filenames matched against filename patterns.
 *
 * stra : Pattern string. "#" matches any digit 0-9 in "strb".
 * strb : String
 * Return value : Returns 0 (!, compare Strcmp) if-and-only-if "strb" matches "stra".
 */
int Match(char *stra, char *strb)
{
    int len;
    int i;
    
    if ((len=strlen(stra)) != strlen(strb)) {
        return -1;   // No match - String lengths differ.
    }
    
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


/* Construct a string of HK data that can be written as row/line to a HK TAB file.
 *
 *
 * ARGUMENTS
 * =========
 * b                    : ?? Bitstream buffer? HK packet?
 * line                 : Must be a pointer to a buffer of at least 165 characters.
 * sccd                 : Spacecraft clock double.
 * OUTPUT:
 * utc_3decimals_return : UTC that is derived from sccd and that is RETURNED to the caller. The content of the string is thus overwritten.
 *                        This value is returned so that the caller does need to derive the value, which can then be used to minimize
 *                        the number of calls to SPICE, which speeds up pds.
 * macro_id             : Will be set to macro ID number, as returned from the HK bit stream. Return value only included for debugging reasons(?).
 *
 *
 * HOUSE KEEPING EXAMPLE ROW
 * =========================
 * NOTE 1: Longest strings used, thus DISABLED is 8 characters
 *         and ENABLED is 7 so if enabled we put in " ENABLED"
 *         with an initial white space.
 *
 * NOTE 2: We use delimiters to make it easier for other software to
 *         quickly read the file for testing purposes.
 *  
 * NOTE 3: Line is terminated with both carriage return and line feed
 *         as in a DOS system (This is not strictly needed anymore).
 *
 * Examle of a HK line with positions displayed vertically, time is in format description.
 *
 * Beginning of line:
 * ---------------------------------------------------------------------------------------------------
 * 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * 000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999
 * 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
 * YYYY-MM-DDThh:mm:ss.ffffff,FFFFFFFFFFFFFFFF,P,E,DISABLED,DISABLED,1,MIXED 0,DISABLED,+-05,+-32,DENS 
 *
 * Continuation of line:
 * -------------------------------------------------------------------------------------------------------------------
 * 1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111222222222222222
 * 0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000000000
 * 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345677778888
 * ITY,DENSITY,DENSITY,DENSITY,DENSITY,DENSITY,LAP,DISABLED,RX,DENSITY,FLOAT,RX,DENSITY,FLOAT,255,255,00000,00<CR><LF>
 */
void AssembleHKLine(unsigned char *b, char *line, double sccd, char *utc_3decimals_return, unsigned int *macro_id)
{
    char ldlmode_str[4][8]={"NONE   ","MIXED 0"," NORMAL","MIXED 1"};
    char tstr[MAX_STR]; // Temporary string
    
    int temp;
    int f22dedc;
    int f11dedc;
    double t;

    ConvertSccd2Utc(sccd, utc_3decimals_return, tstr);    // Decode raw time/SCCD to UTC.

    // Start assembly of table line described above
    sprintf(line,"%s,%016.6f,%1d,%1d,", tstr, sccd, GetBitF(b[0],3,5), GetBitF(b[0],3,2));    // Set time and pmac and emac.

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
    
    *macro_id = (b[6]<<8) | b[7];   // Put together high and low bytes.
//     HPrintf("0: *macro_id = (base 10) %i\n",   *macro_id);            // DEBUG. Debugging INSTRUMENT_MODE_ID bug.
//     HPrintf("1: *macro_id = (base 16) %04x\n", *macro_id);            // DEBUG. Debugging INSTRUMENT_MODE_ID bug.
    
    if(temp) {
        t  = ((b[8]<<8 | b[9]) ^ 0x8000);   // If temp is off this is just a sample from ADC20 probe 2.
    } else {
            t  = ((b[8]<<8 | b[9]) ^ 0x8000)*T_SCALE+T_OFFSET;   // CALIB values educated guesses!!
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
        *val=-*val;      // Make it negative X number of bits.
    }
    // If not negative do nothing
}

// Returns double from 64 bit big-endian data
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


// Decodes S/C time 
//
// buff        : TM bytes
// Return value: SSCD. Raw S/C time as a raw, spacecraft clock count (double; true decimals).
//
// Function name until 2017-07-05: DecodeSCTime
double DecodeSCTime2Sccd(unsigned char *buff)
{
    unsigned int full_s; // Full seconds
    double frac_s;       // Fractional seconds
    
    full_s = ((buff[0]<<24) | buff[1]<<16 | buff[2]<<8 | buff[3]);   // Full seconds
    frac_s = ((double)(buff[4]<<2 | buff[5]>>6))/1024.0;             // Fractional seconds
    
    return(full_s+frac_s);
}



// Decodes lap S/C time 
//
// NOTE: The two highest bits are truncated away fitting the time into five bytes.
//
// buff         : TM bytes
// Return value : Raw S/C time as a double. (NOTE: No reset count.)
//
// Function name until 2017-07-05: DecodeLAPTime
double DecodeLAPTime2Sccd(unsigned char *buff)
{
    unsigned int full_s; // Full seconds
    double frac_s;       // Fractional seconds
    
    // Seconds since spacecraft clock reset
    full_s = ((buff[0]<<22) | buff[1]<<14 | buff[2]<<6 | buff[3]>>2);   // Full seconds
    frac_s = ((double)((buff[3] & 0x3)<<8 | buff[4]))/1024.0;           // Fractional seconds
    
    return(full_s+frac_s); // Return raw time in seconds
}



// Given raw S/C time this function returns 
// a PDS compliant string for this time.
// Time is estimated UTC thus no calibration
// is done.
// A negative return code means an error occured
// 
// NOTE: It appears that this function is not used as of 2015-06-22. /Erik P G Johansson 2015-06-22
/*
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
*/



// See ConvertSccd2Utc_nonSPICE/SPICE
int ConvertSccd2Utc(double sccd, char *utc_3decimals, char *utc_6decimals)
{
    int exit_code;
    
    // Use regular if-then-else, not #ifdef-#else-#endif, to
    // (1) make USE_SPICE easier to convert to a C variable (e.g. for CLI argument flag),
    // (2) ensure compilation checking of code for both cases.
    if (USE_SPICE) {
        exit_code = ConvertSccd2Utc_SPICE   (sccd, utc_3decimals, utc_6decimals);
    } else {
        exit_code = ConvertSccd2Utc_nonSPICE(sccd, utc_3decimals, utc_6decimals);
    }
    
    return exit_code;
}



/* Decodes and correlates SCCS time (Reine Gill: "raw" time) into UTC time.
 * 
 * Input:  sccd          : SCCD. Spacecraft clock count (double; true decimals)
 * Output: utc_3decimals : If NULL, then ignore it. If non-NULL, then write UTC with 3 decimals of fractional seconds.
 * Output: utc_6decimals : If NULL, then ignore it. If non-NULL, then write UTC with 6 decimals of fractional seconds.
 * Return value          : Always zero.
 * 
 * NOTE/BUG: Function uses ConvertTimet2Utc ==> Does not handle leap seconds if using non-SPICE version of that function.
 * 
 * NOTE: The function uses the global tcp structure! 
 * 1) Can do this since it's the only user.
 * 2) It only reads! No thread conflicts.
 * 
 * NOTE: The function and its input value is used by WritePTAB_File to produce the first two columns in (science data) TAB files:
 *      utc:  UTC_TIME (DESCRIPTION = "UTC TIME")
 *      sccd: OBT_TIME (DESCRIPTION = "SPACECRAFT ONBOARD TIME SSSSSSSSS.FFFFFF (TRUE DECIMAL POINT)")
 *     
 * NOTE: Implementation uses TCORR, not SPICE kernels.
 * 
 * NOTE/BUG: Function uses ConvertTimet2Utc which in turn does not appear to use leap seconds. This is contradictive,
 * since experience tells, that the time conversion errors that DVAL-NG complains about are generally smaller
 * than seconds, which hints that the function somehow does handle leap seconds.
 * 
 * ASSUMES: Global variable sec_epoch has been set, although it is only used if there is no (applicable) TCORR information.
 * 
 * Function name until 2017-07-05: DecodeRawTime
 */
int ConvertSccd2Utc_nonSPICE(double sccd, char *utc_3decimals, char *utc_6decimals)
{
    // PROPOSAL: Error on not finding any relevant TCORR information.
    
    double craw;
    double gradient;
    double offset;
    int i;
    double correlated;
    double tmp=0.0;
    
    
    // Default coefficients in case no time calibration data is found.
    gradient=1.0; 
    offset=sec_epoch;

    /* Go through all entries (i.e. time intervals, each of which represents a linear time conversion function).    
     * NOTE: The algorithm is built to handle the cases of
     * (1) there being no linear conversion functions.
     * (2) no matching time interval. */
    for(i=0; i<tcp.n_entries; i++) 
    {
        // Try converting time (sccd-->correlated) and see if "correlated" ends up outside of range.
        // If it is not outside of range, then use that conversion function.
        correlated = sccd*tcp.gradient[i]+tcp.offset[i]; // Compute UTC time using all gradient offset candidates
        
        if(i < (tcp.n_entries-1)) {   // If we are not at the end
            tmp = tcp.SCET[i+1];      // Use next SCET to test the upper validity limit
        } else {
            tmp = 1e100;   // No next SCET. Last correlation packet valid until ground station produces new ones.
        }
        
        if (correlated>=tcp.SCET[i] && correlated<tmp) // Test if in valid range
        {
            gradient = tcp.gradient[i]; 
            offset   = tcp.offset[i];
            break;
        }
    }
    
    craw = sccd*gradient + offset; // Compute correlation. If no time calibration data found default coefficients are used.
    
    //ConvertTimet2Utc(craw, utc, use_6_digits); // Compute a PDS date string
    if (utc_3decimals != NULL) {
        ConvertTimet2Utc(craw, utc_3decimals, FALSE);
    }
    if (utc_6decimals != NULL) {
        ConvertTimet2Utc(craw, utc_6decimals, TRUE);
    }
    
    return 0;
}



/* SPICE-based implementation of ConvertSccd2Utc_nonSPICE.
 * Should have identical interface except where explicitly stated to be different.
 * See ConvertSccd2Utc_nonSPICE for interface details.
 * 
 * Return value :  0 = No error
 *                -1 = Error, in particular illegal SCCD.
 * 
 * NOTE: Reacts on errors by exiting pds, rather than returning error code.
 *       Most (all?) calls to this function do not check the error code anyway.
 * NOTE: This function is THREAD-SAFE and uses the SPICE mutex.
 */ 
int ConvertSccd2Utc_SPICE(double sccd, char *utc_3decimals, char *utc_6decimals)
{
    SpiceChar sccs[MAX_STR];
    SpiceChar utc_temp[MAX_STR];
    SpiceDouble et;
    
//     printf("ConvertSccd2Utc_SPICE - BEGIN\n");   // DEBUG

    ConvertSccd2Sccs(sccd, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs, FALSE);         // Convert SCCD-->SCCS

    pthread_mutex_lock(&protect_spice);

    scs2e_c(ROSETTA_SPICE_ID, sccs, &et);    // Convert SCCS-->et    
    if (CheckSpiceError("SPICE failed to convert SCCS (spacecraft clock count string)-->et, probably because of out-of-range SCCS.", FALSE, FALSE)) {
        // NOTE: Will not exit PDS for this error. This error may occur "legitimately" because of errors in the data,
        // or the (calling) state machine not being synched to the byte stream (it makes the wrong assumption of where to
        // find what data in the byte stream), which leads to strange SSCDs being used. Failure here is used
        // to detect that the state machine is out of sync.
        pthread_mutex_unlock(&protect_spice);
        return -1;
    }

    // Convert et-->UTC
    // 
    // NOTE: UTC string written to temporary buffer, then copied to the caller's buffers.
    // ----------------------------------------------------------------------------------
    // Allowing et2utc to write directly to the caller's string buffers
    // crashes the program, presumably because et2utc requires
    // a string buffer of known length and then overwrites it completely, whereas
    // "utc_3decimals" and "utc_6decimals" are buffers of unknown length.
    if (utc_3decimals != NULL) {
        et2utc_c(et, "ISOC", 3, MAX_STR, utc_temp);
        CheckSpiceError("SPICE failed to convert et-->UTC.", TRUE, FALSE);   // Exit PDS on error.
        strcpy(utc_3decimals, utc_temp);
    }
    if (utc_6decimals != NULL) {
        et2utc_c(et, "ISOC", 6, MAX_STR, utc_temp);
        CheckSpiceError("SPICE failed to convert et-->UTC.", TRUE, FALSE);   // Exit PDS on error.
        strcpy(utc_6decimals, utc_temp);
    }
    
    pthread_mutex_unlock(&protect_spice);

//     printf("ConvertSccd2Utc_SPICE - END\n");   // DEBUG
    return 0;
}



/* Inverse function to ConvertSccd2Utc_* (made for testing/comparing ConvertSccd2Utc_*).
 * 
 * ARGUMENTS
 * =========
 * OUTPUT : reset_counter : Will be set to the spacecraft clock reset counter. Ignored if NULL.
 * 
 * NOTE: Will exit pds on SPICE error.
 * Initially only meant to be used for testing purposes (therefore no non-SPICE version). Now used.
 */
void ConvertUtc2Sccd_SPICE(char *utc, int *reset_counter, double *sccd)
{
    double et;
    char sccs[MAX_STR];
    char tstr[MAX_STR];
    
    pthread_mutex_lock(&protect_spice);

//     printf("ConvertUtc2Sccd_SPICE - BEGIN\n");   // DEBUG
//     printf("utc = %s\n", utc);   // DEBUG
    
    utc2et_c(utc, &et);
    sprintf(tstr, "SPICE failed to convert UTC (\"%s\")-->et.", utc);
    CheckSpiceError(tstr, TRUE, TRUE);

//     printf("et = %g\n", et);   // DEBUG
    
    sce2s_c(ROSETTA_SPICE_ID, et, MAX_STR, sccs);
    CheckSpiceError("SPICE failed to convert et-->SCCS.", TRUE, TRUE);
    
//     printf("sccs = %s\n", sccs);   // DEBUG
    
    ConvertSccs2Sccd(sccs, reset_counter, sccd);    // reset_counter : Ignored if NULL.
//     printf("ConvertUtc2Sccd_SPICE - END\n");   // DEBUG
    
    pthread_mutex_unlock(&protect_spice);
}



/*
 * Convert
 * from (1) spacecraft clock count double (true decimals; Reine Gill: RAW time)
 * to   (2) spacecraft clock count string (false decimals).
 * 
 * NOTE: Due to the fact that the period . in e.g.
 *      SPACECRAFT_CLOCK_START/STOP_COUNT="1/21339876.237"
 * is not a decimal mark (NOT specified in PDS) but now specified
 * in PSA to be a fraction of 2^16 thus decimal .00123 seconds is stored as
 * 0.123*2^16 ~ .81
 * 
 * We run the raw time into this function to convert back to fractions
 * and create an appropriate string. Reason for not converting
 * the DecodeSCTime2Sccd is that other parts of the code rely on this function.
 * In our .TAB files we have a column with UTC and a column with OBT
 * but with higher precision we can not use the 2^16 fractions there
 * it need to be a real_ascii with a true decimal point.
 *  
 * Input  : sccd       : SCCD. Spacecraft clock count double (i.e. true decimals).
 * Input  : n_resets   : Reset count.
 * Output : sccs       : SCCS. Spacecraft clock count string (i.e. false decimals). QUOTED.
 * Input  : quote_sccs : TRUE/FALSE for whether to quote the SCCS.
 *
 * Function name until 2017-07-05: Raw2OBT_Str
 */
int ConvertSccd2Sccs(double sccd, int n_resets, char *sccs, int quote_sccs)
{
    unsigned int sec;
    unsigned int frac;
    
    sec  = (unsigned int) (sccd);   // Truncate seconds
    frac = (unsigned int) (((sccd-sec)*65536.0)+0.5);      // Get fractions back in terms of 2^16

    if (quote_sccs) {
        sprintf(sccs,"\"%d/%010d.%d\"",n_resets,sec,frac);    // Compile raw S/C time string
    } else {
        sprintf(sccs,  "%d/%010d.%d",  n_resets,sec,frac);    // Compile raw S/C time string
    }
    
    return sec;
}



/**--------------------------------------------------------------------------------------------------
 * Erik P G Johansson 2015-03-25: Created function.
 *
 * Converts
 * from STRING representation of spacecraft clock counter (false decimals; "OBT_Str", "sccs")
 * to   DOUBLE representation of spacecraft clock counter (true decimals; "raw", Reine Gill: "rawTime").
 * 
 * ARGUMENTS
 * =========
 * Input  : sccs
 * Output : reset_counter : Assigned to the value of the spacecraft clock reset counter. Ignored if NULL.
 * Output : sccd
 *
 * NOTE: Opposite conversion of ConvertSccd2Sccs (hence the name) with the difference that this function
 * does accepts strings both with and without surrounding quotes.
 * The former is used when reading the data exclusion file, the latter is used for reading
 * SPACECRAFT_CLOCK_START/STOP_COUNT values in property lists (property_type).
 * 
 * Function name until 2017-07-05: OBT_Str2Raw
--------------------------------------------------------------------------------------------------*/
int ConvertSccs2Sccd(char *sccs, int *reset_counter, double *sccd)
{
    int    reset_counter_temp;
    double sec, false_frac;
    char   s[MAX_STR], s_temp[MAX_STR];
    
    // Trick to remove surrounding quotes from string, but only if there are any.
    if (sccs[0] == '"') {
        Separate(sccs, s_temp, s, '"', 1);   // s_temp value is never used.
    } else {
        strncpy(s, sccs, MAX_STR);
    }
    
    // Parse numbers in string.
    // (1) Use doubles instead of integers (except for reset counter) to ensure that
    // the variables can represent large enough integers. (C's "int" is not guaranteed to have many bits.)
    // (2) Use two separate doubles for "seconds" and "fractions" since double can represent all (not too
    //    large) integers exactly, but not all decimal numbers.
    if (   (3 != sscanf(s,      "%i/%[^.].%lf", &reset_counter_temp, s_temp, &false_frac))
        || (1 != sscanf(s_temp, "%lf",          &sec)))
    {
        // Error messages are unnecessary if all the calls to this function give error messages instead (which they do).
        // If the calling code also gives context in the error messages, that is even better.
//         YPrintf("ERROR: ConvertSccs2Sccd: Can not interpret spacecraft clock counter string: \"%s\"\n", sccs);
//         printf( "ERROR: ConvertSccs2Sccd: Can not interpret spacecraft clock counter string: \"%s\"\n", sccs);
        return -1;
    }
    
    // Assign values to "parameters".
    *sccd          = sec + false_frac/65536.0;
    if (reset_counter != NULL) {
        *reset_counter = reset_counter_temp;
    }
    
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



/* Converts a time_t (in the form of a double) to a UTC string. Handles the decimals in the double which a time_t can not do.
 * (Uncertain if conversion is entirely correct, e.g. w.r.t. leap seconds).
 * 
 * Input  : raw          : A time_t value in the form of a double.
 * Input  : use_6_digits : False=3 decimal digits; True=6 decimal digits (fractional seconds)
 * Output : utc          : Set to a string "YYYY-MM--DDThh:mm:ss.xxx" (3 decimals) or "YYYY-MM-DDThh:mm:ss.xxxxxx" (6 decimals).
 *
 * NOTE/BUG: Implementation uses gmtime_r. ==> Does not appear to care about leap seconds since gmtime_r does not.
 * Still experience tells, that the time conversion errors that DVAL-NG complains about are generally smaller
 * than seconds, which hints that the function somehow does handle leap seconds.
 * 
 * IMPLEMENTATION NOTE: Not implemented with two potential return UTC values (like ConvertSccd2Utc) since the rounding
 * means that no processing can be saved that way.
 * 
 * Function name until 2017-07-05: Scet2Date_2
 */
// PROPOSAL: Eliminate the backup code for the case that "gmtime_r" fails. It is dangerous.
int ConvertTimet2Utc(double raw, char *utc, int use_6_decimals)
{     
    double s;            // Seconds and fractional seconds
    struct tm bt;        // Broken down time
    time_t t;            // Time since 1970 in secs to current S/C time

    unsigned int full_s; // Full seconds
    double       frac_s; // Fractional seconds
    unsigned int d,h,m;  // Days, hours and minutes

    // Compile time since 1970 jan 1 00:00:00 to current S/C time.
    // t = raw+sec_epoch; // Full secs + seconds from 1970 to reset of S/C clock.

    // Round depending on the number of digits that will be displayed. Otherwise sprintf will do it (randomly).
    // line edited 10/7 2014. FJ. We need to make sure seconds are rounded properly.
    if(use_6_decimals) {
        raw = floor(1000*1000*raw+0.5)/(1000*1000);
    } else {
        raw = floor(1000*raw+0.5)/1000;
    }


    t=raw;   // Convert double-->time_t.
    
    // Use gmtime_r to stay POSIX compliant and to be consistent.
    full_s = raw;            // double-->unsigned int (truncate decimals)
    frac_s = raw - full_s;   // Obtain fractional seconds.

    // Compile plausible date, time and year, use thread-safe version of gmtime.
    if((gmtime_r(&t,&bt))==NULL)  
    {
        // CASE: Error in previous call to "gmtime_r".
        // NOTE: Does not take leap seconds into account!!
        d=full_s/86400;            // Full number of days.
        h=(full_s-=d*86400)/3600;  // Remaining hours.
        m=(full_s-=h*3600)/60;     // Remaining minutes. 
        s=(full_s- m*60)+frac_s;   // Remaining seconds.
        // Couldn't resolve corresponding UTC date of S/C time
        sprintf(utc, "CCYY-MM-DDT%02d:%02d:%06.3f",h,m,s);
        return -1; 
    }

    // Compile PDS compliant time string.
    if(use_6_decimals) {
        // Use 6-digit fractional seconds.
        sprintf(utc, "%4d-%02d-%02dT%02d:%02d:%09.6f", bt.tm_year+1900, bt.tm_mon+1, bt.tm_mday, bt.tm_hour, bt.tm_min, ((double)bt.tm_sec)+frac_s);
    } else {
        // Use 3-digit fractional seconds, PDS standard.
        sprintf(utc, "%4d-%02d-%02dT%02d:%02d:%06.3f", bt.tm_year+1900, bt.tm_mon+1, bt.tm_mday, bt.tm_hour, bt.tm_min, ((double)bt.tm_sec)+frac_s);
    }

    return 0;
}



/* Convert time_t --> SCCD (via SPICE).
 * NOTE: Probably not very fast due to converting via UTC strings.
 * NOTE: Somewhat limited in precision due to UTC strings 6 decimals.
 * 
 * ARGUMENTS
 * =========
 * OUTPUT : reset_counter : Will be set to the spacecraft clock reset counter. Ignored if NULL.
 */
void ConvertTimet2Sccd_SPICE(time_t t, int *reset_counter, double *sccd)
{
//     printf("ConvertTimet2Sccd_SPICE - BEGIN\n");    // DEBUG
//     printf("t = %i\n", (int) t);    // DEBUG
    
    char utc[MAX_STR];
    ConvertTimet2Utc(t, utc, TRUE);
//     printf("utc = %s\n", utc);    // DEBUG
    
    ConvertUtc2Sccd_SPICE(utc, reset_counter, sccd);
//     printf("ConvertTimet2Sccd_SPICE - END\n");   // DEBUG
}



/*
 * Converts UTC --> time_t.
 *
 * NOTE/BUG: Implementation uses mktime. ==> Does not handle leap seconds.
 * 

 * NOTE: Handling of timezone seems hackish/suspect.
 * QUESTION: How does this function handle leap seconds?! (Probably not, if one interprets time_t=seconds since 1970.)
 *
 * Input  : sdate : UTC string on format "YYYY-MM-DD" (only date) or "YYYY-MM-DD hh:mm:ss" (no second decimals).
 *                  NOTE: The function will ignore characters after the final "ss" (two-digit seconds) and
 *                  will therefore ignore decimals in seconds. time_t can not contain fractions (subsecond) anyway.
 *                  Can thus handle PDS compliant UTC time strings "YYYY-MM-DDThh:mm:ss.xxx..." (albeit while ignoring of second decimals).
 *                  NOTE: If no hour-minute-seconds are given, then they will taken as zero (i.e. midnight at beginning
 *                  of day) as default value.
 *                  NOTE: In reality, the function ignores the characters between the number fields and
 *                  they can therefore be set arbitrarily (only their absolute positions are important).
 * Output : t     : time_t
 *                  NOTE: Empirically (playing with TimeOfDatePDS/ConvertUtc2Timet, pds' default compiler), time_t appears to correspond to seconds
 *                  after 1970-01-01 00:00.00, but only for times beginning at 1970-01-01 01:00.00 (!).
 * Return value : 0=Success; Negative value indicates error.
 * 
 * 01234567890123456789012
 * CCYY-MM-DDThh:mm:ss.fff
 * 
 * Function name until 2017-07-05: TimeOfDatePDS
 */
int ConvertUtc2Timet(char *sdate, time_t *t)
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
    
    strncpy(year,   sdate,4);      // Copy year from sdate
    strncpy(month, &sdate[5], 2); // Copy month from sdate
    strncpy(mday,  &sdate[8], 2);  // Copy day of month to mday
    
    
    year[4]  = '\0';   // Terminate
    month[2] = '\0';   // Terminate
    mday[2]  = '\0';   // Terminate  
    
    // Set default values (midnight) in case they are not supplied by the caller (sdate).
    atime.tm_sec  = 0;
    atime.tm_min  = 0;
    atime.tm_hour = 0;
    
    if((len=strlen(sdate))>10) // If we have a full PDS time string with more than just YYYY-MM-DD (year-month-day). Assume hour-minute-second.
    {
        strncpy(hour,&sdate[11], 2);    // Copy HOURS from sdate
        hour[2] = '\0';                  // Terminate
        if(!sscanf(hour, "%d", &(atime.tm_hour))) {
            return -2;    // Error couldn't resolve minutes
        }

        strncpy(min, &sdate[14], 2);      // Copy MINUTES from sdate
        min[2] = '\0';                   // Terminate
        if(!sscanf(min, "%d", &(atime.tm_min))) {
            return -1;    // Error couldn't resolve minutes
        }
        
        strncpy(sec, &sdate[17], 2); // Copy SECONDS to sec
        sec[2] = '\0';               // Terminate
        if(!sscanf(sec, "%d", &(atime.tm_sec))) {
            return -3;   // Error couldn't resolve seconds
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
        return -7;       // Err only do time after 1970, could be 2000.
    }
    
    atime.tm_year -= 1900;   // Get number of years since 1900, that's what mktime wants 
    atime.tm_mon  -= 1;      // Month ranges from 0 to 11 and not as usual 1 to 12
    
    atime.tm_wday  = 0;      // Day of week doesn't matter here
    atime.tm_yday  = 0;      // Day in year doesn't matter here
    atime.tm_isdst = 0;      // Daylight saving unknown

    // Calculates UTC time in seconds since 1970 1 Jan 00:00:00.
    // NOTE: mktime interprets "atime" as a local time and hence ADDS a timezone shift (despite that what we input is UTC).
    if((*t=mktime(&atime))<0) {
        return -8;              // Error couldn't calculate time
    }
    
    *t=*t-timezone; // Compensates for assuming that the input time was in the local computer timezone.
    // timezone is defined as negative number for Sweden so thus it needs to be a minus sign.
    
    return 0; // Ok!
}



/*
 * Convert
 * from UTC string (without fractional seconds), YYYY-MM-DDThh:mm:ss,
 * to   number of seconds since epoch 1970.
 *
 * NOTE/BUG: Implementation uses mktime ==> Does not account for leap seconds.
 * ASSUMES: Relies on time_t being interpreted as number of seconds (which is unwise).
 * NOTE: There is another function "ConvertUtc2Timet" that at least superficially can do the job of this function.
 *       Not sure why that one has not been used instead.
 *
 * Return value : Time as time_t, number of seconds. NOTE: Integer; no fractional seconds.
 * 
 * Function name until 2017-07-05: E2Epoch
 */
unsigned int ConvertUtc2Timet_2(char *utc)
{
    struct tm at; // Broken down time structure
    time_t t;
    
    // Put year,month,day,hour,minutes and seconds in structure
    sscanf(utc,"%4d-%2d-%2dT%2d:%2d:%2d",&at.tm_year,&at.tm_mon,&at.tm_mday,&at.tm_hour,&at.tm_min,&at.tm_sec); 
    
    at.tm_mon--;      // Month ranges from 0 to 11 and not as usual 1 to 12
    at.tm_year -= 1900; // Get number of years since 1900, that's what mktime wants 
    
    at.tm_wday  = 0;     // Day of week doesn't matter here
    at.tm_yday  = 0;     // Day of year doesn't matter here
    at.tm_isdst = 0;    // Daylight saving unknown
    
    t = mktime(&at);   // Calculates UTC time in seconds since 1970 1 Jan 00:00:00
    
    t += at.tm_gmtoff; // Add number of second east of UTC to get UTC.
    return t;
}



// Like TimeOfDatePDS, but always sets the time of day to 12:00:00.
//
// NOTE/BUG: Uses ConvertUtc2Timet. ==> Does not handle leap seconds if using non-SPICE-based version of that function.
int ConvertUtc2Timet_midday(char *sdate, time_t *t)
{
    char tstr[20];
    const char * midday_str = "12:00.00";

    // YYYY-MM-DD hh:mm.ss  (length: 19 bytes excl. \0)
    // 0123456789012345678
    //
    // Construct new string with time of day overwritten.
    strncpy(tstr, sdate, 10);           // Copy YYYY-MM-DD to bytes 0-9.
    strncpy(&(tstr[11]), midday_str, 8);   // (Skip byte 10.) Copy hh:mm.ss to bytes 11-18.
    tstr[19] = '\0';

    return ConvertUtc2Timet(tstr, t);
}



/*
 * Returns CURRENT time as a string on format CCYY-MM-DDThh:mm:ss .
 * "0" in the function name = zero decimals (on the second counter).
 *
 * Assumes that ltime has enough space for the result, at least 20 chars.
 * 
 * NOTE/BUG: Uses gmtime_r which does not handle leap seconds.
 *
 * Function name until 2017-07-05: GetUTime
*/
int GetCurrentUtc0(char *ltime) 
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
// First X=|x|+1 is done to get all values in the range 1 to 32768.
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
// L(00459)=149   > Choosing this value for inverse will be least wrong in an average sense. Thus L_Inverse(149)=459
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
// First X=|x|+1 is done to get all values in the range 1 to 32768.
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

/**
 * Traverse through DDS archive
 * If the end is reached, then it will wake up every 10th second
 * and look for new data.
 *
 * NOTE: Will search all DDS files regardless over mission phase time coverage.
 * ==> Still a bit slow for very short data sets.
 */
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
    
    // Make it work with messy input DDS Archive 050113.
    while(1)
    {
        af=fts_open(path_arrays,FTS_LOGICAL,Compare);   // Get handle with which one (using fts_read) can iterate over files in directory tree.
        if(af!=NULL)
        {
            do
            {
                fe=fts_read(af);  // Get next file
                if(fe!=NULL)      // Exists ?
                {
                    // fe->fts_accpath;   /* "access path" */
                    // Ignore directories not starting with "20".
                    if(fe->fts_accpath[skip_len]=='2' && fe->fts_accpath[skip_len+1]=='0')
                    {
                        len=strlen(fe->fts_name);
                        if(len==DDS_TM_FILE_LEN)
                        {
                            // NOTE: The DDS directory as it is organized at IRF-U also contains files with paths and names on the form
                            //    .../ddsData/archive/2016/08/12/rpc160812Dds00_TLM_0000_00h00m19s_23h56m44s.tm,
                            // and
                            //    .../ddsData/archive/2016/08/12/sorted/rpc160812Sci00_00h00m07s_23h59m47s.tm
                            // pds matches filenames and therefore only reads the latter. Location does not seem to matter (except for directory prefix "20").
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
                                                slen=0; // Sum of lengths
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
                // Here we can do a graceful exit! if Ctrl-C is pressed
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
        
        // Here we can do a graceful exit!
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

// Decode a DDS file.
// 
// NOTE: Uses global variable mp  (mp.t_start, mp.t_stop (time_t)).
void ProcessDDSFile(unsigned char *ibuff,int len,struct stat *sp,FTSENT *fe)
{
    char tmp_str[32];
    unsigned int N_bytes;
    unsigned short int gsid; // Ground station ID
    unsigned short int vch;  // Virtual channel
    unsigned char SLE;
    unsigned char TQ;
    unsigned char *endp;
    
    static double old=0.0;   // Previous DDS packet time
    static double scet=1.0;  // DDS Packet time
    
    double eps=1e-5; // Epsilon. Can't have too small value.
    
    int toggle=1; // Just print one message to the log, until time is current
    
    int i;
    
    endp=ibuff+len;
    do
    {
        N_bytes=(ibuff[8]<<24 | ibuff[9]<<16 | ibuff[10]<<8 | ibuff[11]);
        
        if(N_bytes>len)
        {
            DPrintf("Corrupted length\n");
            break;
        }
        
        if(scet>=old) // As long as we at least have the same DDS time or move forward in time.
        {
            old=scet; // Remember old time
            toggle=1; // Indicate monotonic time
        }
        
        scet = DecodeDDSTime2Timet(ibuff); // Get time of DDS packet
        
        // Determine if data lies within the specified data set (time interval).
        if(mp.t_start>0)
            if(scet<mp.t_start || scet>mp.t_stop)
            {
                ibuff+=(18+N_bytes); // Skip DDS packet
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
                ibuff+=(18+N_bytes); // Skip DDS packet, most probably done this one before!
                continue;
            }      
            
            ConvertTimet2Utc(scet,tmp_str,0);
            DPrintf("SCET: %s\n",tmp_str);
            
            DPrintf("Length: %d\n",N_bytes);
            
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
            
            // CRC=crc16(ibuff,N_bytes);
            // DPrintf("Data CRC: 0x%x\n",CRC);
            
            InB(&cbtm,ibuff,N_bytes);                // Store data in cicular S/C TM buffer
            
            ibuff+=N_bytes;                          // Go forward
            
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
    
    GetCurrentUtc0(tmp_str);   // Get universal time string.
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
// in a DDS archive hierarchy.
// Function assumes that it is a true DDS archive
// hierarchy. Start time is in seconds)
// 
// NOTE: Derives UTC time from filename or path(?), then converts ~UTC-->time_t with mktime (disregarding leap seconds?).
// 
time_t DDSFileStartTime(FTSENT *f) 
{
    //           0123456789012345678901234567890
    // Example?:  2016/09/30/rpc160930dds00_tlm_sci____00h00m07s_10h39m16s.tm
    char tstr[64];   // Temporary string
    struct tm at;    // Broken down time structure
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
    
    t+=at.tm_gmtoff; // Add number of second east of UTC to get UTC
    return t;
}



/*
 * Returns DDS Time from a DDS packet.
 *
 * ibuff        : Byte stream.
 * Return value : time_t value as a double, i.e. number of seconds since 1970-01-01 00:00.00 (judging from how the value is used).
 *                Conversions to UTC should probably assume that leap seconds are included in this cound.
 * 
 * NOTE: Used by (1) ProcessDDSFile.
 *               (2) LoadTimeCorr (TCORR, non-SPICE-kernel information on how to convert "spacecraft clock count"
 *                   to "Earth second count").
 * 
 * NOTE: time_t tends to be interpreted with gmtime/gmtime_r which does not take leap seconds into account. Therefore kind of approximate.
 * 
 * Function name until 2017-07-05: DDSTime
 */
double DecodeDDSTime2Timet(unsigned char *ibuff)
{
    unsigned int secs;
    unsigned int usecs;
    
    // We assume that we run on a little endian system! Linux i386 platform
    secs  = (ibuff[0]<<24 | ibuff[1]<<16 | ibuff[2]<<8 | ibuff[3]);
    usecs = (ibuff[4]<<24 | ibuff[5]<<16 | ibuff[6]<<8 | ibuff[7]);
    
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
// It's not bad to use them.

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

// Allocates double-typed 2D array of size [rows][cols].
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

// Allocate 2D array/matrix of integers.
// Return pointer to array (length "rows") of pointer to arrays (length "cols").
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



void TimeConversion_TEST()
{
    // Example: RO-C-RPCLAP-2-TF5-EDITED-V0.1___BACKUP_2017-07-06_17.35.07___befSPICE/DATA/EDITED/2016/SEP/D19/RPCLAP160919_001S_RDS24NS.LBL
    // START_TIME = 2016-09-19T23:58:44.481
    // STOP_TIME  = 2016-09-19T23:58:45.307
    // SPACECRAFT_CLOCK_START_COUNT = "1/0432950235.15680"
    // SPACECRAFT_CLOCK_STOP_COUNT  = "1/0432950236.4278"
    
    if (FALSE) {
        char sccs[MAX_STR];
        double enc_sclk;
        double et;
        char utc[MAX_STR];
        
        enc_sclk = 432950236.4278*65536;    // DOES NOT SEEM TO WORK;
        sct2e_c(ROSETTA_SPICE_ID, enc_sclk, &et);
        et2utc_c(et, "ISOC", 6, MAX_STR, utc);
        printf("enc_sclk = %f\n", enc_sclk);
        printf("et       = %f\n", et);
        printf("utc      = %s\n", utc);
        // enc_sclk = 28373826694532.300781
        // et       = 564411412.872962
        // utc      = 2017-11-20T00:55:44.690127
        
        strcpy(sccs, "1/0432950235.15680");
        scs2e_c(ROSETTA_SPICE_ID, sccs, &et);
        et2utc_c(et, "ISOC", 6, MAX_STR, utc);
        printf("sccs = %s\n", sccs);
        printf("et   = %f\n", et);
        printf("utc  = %s\n", utc);
        // sccs = 1/0432950235.15680
        // et   = 527601592.666569
        // utc  = 2016-09-19T23:58:44.484171
    }

    /*=====================================
     * Compare
     *      ConvertSccd2Utc_nonSPICE, and
     *      ConvertSccd2Utc_SPICE
    =====================================*/
    if (FALSE) {
//         double sccd_array[MAX_STR];
        const int N_seq = 10000;
        const double sccd_seq_1=1e8, sccd_seq_2=4.025e8;
        const double sccd_diff_threshold_1 = 0.001;
        const double sccd_diff_threshold_2 = 1.0;

        int i = 0, N_timestamps = -1;
        char sccs_array[1024*10][100];    // 2016-09-30T12:00:00.000000 ==> 26 tecken.
        int N_over_threshold_1 = 0;
        int N_over_threshold_2 = 0;

        // Add timestamps linearly distributed between two specified timestamps.
        int j = 0;
        for (j=0; j<N_seq; j++) {
            ConvertSccd2Sccs(
                sccd_seq_1 + j/(1.0*N_seq-1) * (sccd_seq_2-sccd_seq_1),
                ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
        }

        // Add timestamps on and around leap second 2015-06-30.
//         const double sccd_ls2015 = 394329522.327700;    // Approximately leap second 2017-06-30 23:59:60 (according to SPICE).
//         ConvertSccd2Sccs(sccd_ls2015-100, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015  -2, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015  -1, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015,     ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015  +1, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015  +2, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015  +5, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015 +10, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
//         ConvertSccd2Sccs(sccd_ls2015+100, ROSETTA_SPACECRAFT_CLOCK_RESET_COUNTER, sccs_array[i++], FALSE);
        
        N_timestamps = i;
        
        printf("SCCS --> SCCD --> (non-SPICE) UTC, (SPICE) UTC --> SCCD, SCCD --> SCCD abs.diff\n");
        for (i=0; i<N_timestamps; i++) {
            char utc1[MAX_STR], utc2[MAX_STR];
            double sccd, sccd1, sccd2;
//             int reset_counter;
            
            ConvertSccs2Sccd(sccs_array[i], NULL, &sccd);
            ConvertSccd2Utc_nonSPICE(sccd, NULL, utc1);    // Requires loaded TCORR data structures!
            ConvertSccd2Utc_SPICE   (sccd, NULL, utc2);
            ConvertUtc2Sccd_SPICE(utc1, NULL, &sccd1);
            ConvertUtc2Sccd_SPICE(utc2, NULL, &sccd2);
            double sccd_diff = fabs(sccd1-sccd2);
            printf("%-18s --> %16f --> %s, %s --> %16f, %16f --> %f\n", sccs_array[i], sccd, utc1, utc2, sccd1, sccd2, sccd_diff);
            
            if (sccd_diff >= sccd_diff_threshold_1) {   N_over_threshold_1++;   }            
            if (sccd_diff >= sccd_diff_threshold_2) {   N_over_threshold_2++;   }
        }
        printf("Number of tested timestamps: %i\n", N_timestamps);
        printf("Fraction of diffs > threshold=%f: %f\n", sccd_diff_threshold_1, (1.0*N_over_threshold_1)/N_timestamps);
        printf("Fraction of diffs > threshold=%f: %f\n", sccd_diff_threshold_2, (1.0*N_over_threshold_2)/N_timestamps);
    }



    /* Compare UTC-->time_t-->UTC (ConvertUtc2Timet, ConvertTimet2Utc)
     */
    if (FALSE) {
        char * utc_array[MAX_STR];
        time_t t, t_midday;
        int i = 0;

        utc_array[i++] = "1970-01-01 00:00:00"; 
        utc_array[i++] = "1970-01-01 00:00:01";
        utc_array[i++] = "1970-01-01 00:59:59";
        utc_array[i++] = "1970-01-01 01:00:00";
        utc_array[i++] = "1970-01-01 01:00:01";
        utc_array[i++] = "1970-01-01 12:00:00";
        utc_array[i++] = "1970-01-01 23:59:59";
        utc_array[i++] = "1970-01-02 00:00:00";
        utc_array[i++] = "1970-01-02 00:00:01";

        const int i1=i;
        utc_array[i++] = "2015-06-30 21:59:59";
        utc_array[i++] = "2015-06-30 21:59:60";    // Possibly leap second if happens to be something fishy with the time zone.
        utc_array[i++] = "2015-06-30 22:00:00";
        
        utc_array[i++] = "2015-06-30 22:59:59";
        utc_array[i++] = "2015-06-30 22:59:60";    // Possibly leap second if happens to be something fishy with the time zone.
        utc_array[i++] = "2015-06-30 23:00:00";
        
        utc_array[i++] = "2015-06-30 23:59:59";
        utc_array[i++] = "2015-06-30 23:59:60";    // Leap second in UTC. 
        utc_array[i++] = "2015-07-01 00:00:00";
        
        utc_array[i++] = "2016-01-01 12:00:00";
        utc_array[i++] = "2016-01-01 23:00:00";
        utc_array[i++] = "2016-01-02 00:00:00";
        utc_array[i++] = "2016-01-02 00:00:01";
        utc_array[i++] = "2016-01-02 12:00:00";
        const int i2=i;
        //const int N=i;

        printf("UTC                 ==> time_t     time_t midday ==> UTC (non-midday)\n");
        for (i=i1; i<i2; i++) {
            char utc[MAX_STR];
            
            ConvertUtc2Timet(       utc_array[i], &t);
            ConvertUtc2Timet_midday(utc_array[i], &t_midday);
            ConvertTimet2Utc(t, utc, TRUE);
            printf("%19s ==> %10i, %10i ==> %s\n", utc_array[i], (int) t, (int) t_midday, utc);
        }
    }
}



//##################################################################################################################
// Alternative "main" function that can be temporarily used instead of the real one for testing purposes.
// The real main function can, as its first command, call this function, which will then exit pds.
// This is useful for having test code that has access to other pds-internal functions.
//##################################################################################################################
// PROPOPOSAL: Separate out tests into separate functions.
int main_TEST(int argc, char* argv[]) {
    printf("###################################################################################\n");
    printf("The normal main() function has been DISABLED in this executable. This is test code.\n");
    printf("###################################################################################\n");
    ProtectPlnkInit();
    
    // NOTE: SPICE may or may not have been initialized by the core pds code, depending on from where main_TEST was called.
//     InitSpice("/home/erjo/ROSETTA_SPICE_KERNELS_spiftp.esac.esa.int___ROS_V040___birra.TM");
//     erract_c("SET", 99999, "DEFAULT");
    
//     FindNearestInSortedArray_TEST();
//     TimeConversion_TEST();
//     CalibCoeff_TEST();

//     char tstr[MAX_STR];
//     double et;
//     char *utc = "2010-07-12T00:00:00.000";
//     printf("utc = %s\n", utc);   // DEBUG
//     utc2et_c(utc, &et);
//     CheckSpiceError(tstr, TRUE, TRUE);
    
    ExitPDS(255);

    return 0;
}
