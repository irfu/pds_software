RPC LAP PDS SOFTWARE DOCUMENTATION
===========================================================

pds_a.b.c   , a.b = Version Number


COMPILATION:
------------

To compile the pds program do as follows:

First time do $> make clean
Then do       $> make


See "CONFIGURING PDS" below for more details.

COMMAND LINE: 

 pds  [-h] [-c pds.conf] [-a pds.anomalies] [-b pds.bias] [-e pds.exclude] [-m pds.modes] [-calib] [-debug level] -mp mission_phase_abbreviation

VERSION: 2.96

AUTHOR: Reine Gill
DATE: 091026

DESCRIPTION:
------------
Loads DDS files from a DDS archive and then filters, decodes and converts this into a LAP PDS archive.
The -mp option is the only required option and it acts as a filter selecting a certain mission phase.
The mission phase is abbreviated as in the Mission_Calendar.txt file in the pds source directory.

If execution is aborted with Ctrl-C the program will gracefully finish the current DDS file and exit.

Data that is not fully understood will be stored in the UnAccepted_Data path and can be streamed directly to 
a LAP gse for analysis. A chunk of accepted data preeceding the unaccepted data will also be dumped 
every time we find data we do not understand. If the problem is related to macro settings some errors can be 
corrected in the pds.anomalies file. The pds.anomalies file sets precedence to parameters in macro descriptions
instead of the parameters inside the data.

The new archives are created in the generated archive directory (defined in pds.conf). The start directory
of an archive is the same as the data set id for the archive. Dervived from mission phase, DPL numer etc.
If you try to generate the same archive twice thus on top of the old an error will occur.


Note0: pds will be used to both create calibrated and edited data, now because of the
       way ESA implements the PDS system we must not have CALIBRATED EDITED or DERIVED data
       in the same PDS archive. The ESA way to implement it is to create a separate PDS archive 
       for each of the data types EDITED, CALIBRATED and DERIVED. Therefore this software will
       be made to run in two modes EDITED and CALIBRATED mode, producing two separate
       archives. DERIVED data will be produced by software working on the CALIBRATED archive
       probably Matlab software. The default mode is to generate EDITED data, see -calib option below. 

Note1: If needed to abort use Ctrl-C, this will cleanly stop this program at a convenient starting point
       this might take a long time, depending on the size of the current dds file. Since it completes this 
       before quitting. Logfiles of the current run are also compressed. If you do not intend to complete 
       the archive later, use killall pds to abort. Stopping and completing later is not recommended.



OPTIONS:
--------

 -c pds.conf
    This is a configuration file for the PDS system, if left out the default is $HOME/pds.conf
    See $LAP_PDS_HOME/pds_config for a template file.

pds.config contains the following:
---------------------------------------------------------------------------------------------------------
% pds.conf
% Note that comments are preceeded by % and not #
% as usual. And # is interpreted as any numerical 
% character.
%
01                                           % Number of times the spacecraft clock has been reset
2003-01-01T00:00:00                          % Date and time of last spacecraft clock reset
/usr/local/src/gse/pds/stubs                 % Path to Macro descriptions for the LAP instrument
1.0                                          % The PDS data set version, first version is 1.0
Reine Gill (IRFU), first release             % Label revision note
2009-09-22                                   % Release date
/usr/local/src/gse/pds/Mission_Calendar.txt  % Path and file name to the mission calendar
/data/LAP_ARCHIVE/                           % Path to Our generated PDS archives
/opt/ddsData                                 % Path to Our Mirror of the RPC DDS archive
/data/LAP_ARCHIVE/logs                       % All activity,progress and status of this pds program is logged here
/data/LAP_ARCHIVE/UnAccepted_Data            % Path to unaccepted data that the PDS program did not understand
RPCLAP030101_CALIB_VBIAS.LBL                 % Filename of coarse voltage bias calibration data
RPCLAP030101_CALIB_FINE.LBL                  % Filename of fine voltage bias calibration data
RPCLAP030101_CALIB_IBIAS.LBL                 % Filename of current bias calibration data
RPCLAP######_CALIB_MEAS.LBL                  % Filename of offset calibration data of measured data, note that date is flexible
ROS_AUX_TIME_C_C_####-##-##T##:##:##Z_####-##-##T##:##:##Z % Filname of time correlation packets, note dates are flexible
RPCLAP030101_CALIB_FRQ_D_P1.LBL              % Filename of density frequency response probe 1
RPCLAP030101_CALIB_FRQ_D_P2.LBL              % Filename of density frequency response probe 2
RPCLAP030101_CALIB_FRQ_E_P1.LBL              % Filename of e-field frequency response probe 1
RPCLAP030101_CALIB_FRQ_E_P2.LBL              % Filename of e-field frequency response probe 2


 -a pds.anomalies, if it's left out the default is $HOME/pds.anomalies 
    This is an anomaly file containing times of anomalies and how to correct them.
    The method of correction is to set higher priority on macro information.
    So in case of mismatch the macro information is used. Currently the default
    is to trust the data information more than the macro description. One can choose
    to override with the macro id in the data stream or provide a macro id manualy in the file.
    In special cases one can also construct a special macro description 
    just to correct a specific problem.
    See $LAP_PDS_HOME/pds_config for a template file.

    Note: The anomaly file shold be manually sorted in time with the future at the end
          and the past at the start.

-b pds.bias, if it's left out the default is $HOME/pds.bias. 
   All bias settings are normally done through the macros. If any extra
   bias settings are done..it is not reflected in the pds archive because
   the fix bias setting is not in the science data. Therefore it has to
   be included here in the pds.bias file. See$LAP_PDS_HOME/pds_config 
   for a template file.

-e pds.exclude, if it's left out the default is $HOME/pds.exclude.
   In calibration mode all macros in this file will be excluded.
   Typical macros to exclude are calibration macros, it does not
   make sense to calibrate them since they are the basis for 
   the calibration. 

-m pds.modes, if it's left out the default is $HOME/pds.modes.
   Contains a short human readable description of each mode
   thus for each LAP macro.

 -calib 
   Produces calibrated output data, default if -calib is left out is to produce edited data.
   pds.conf file contains the file names for the current calibration data.
   For offset calibration data it contains a file name with date replaced by XXXXXX.
   Since there will be many files with different "valid from" dates to perform offset calibration.   
 
 -debug level
   Turns on debugging higher level number prints more information.
   Currently level above 4 doesn't add anything new.
   Setting level to 0 or removing option turns debugging off.

 -mp mission_phase_abreviation
   The -mp option acts as a filter selecting a certain mission phase. The mission phase is abbreviated as
  in the Mission_Calendar.txt file in the source directory.
 
  An example would be: pds -calib -mp CVP
  This would generate the Commisioning Verification Phase calibrated PDS archive.


PRODUCED FILE TYPES:
--------------------
 
Label files: .LBL                 Contains description of the data in the .TAB file.
Table files: .TAB                 Contains data.
UnAccepted data: UNA*.LAP         Data that this PDS program couldn't accept needs manual attention!!
                                  Is not a part of the finnished PDS archive.
 
CONFIGURING PDS:
---------------

0) An environment variable to the PDS home directory:
   EX: export LAP_PDS_HOME=/home/rg/gse/pds

1) Add $LAP_PDS_HOME/bin to the execution path $PATH.

2) A configuration file as descibed above default is: -c pds.conf.
   And default location is: $HOME

3) LAP MACRO descriptions (Created by the Meds program) 
   recides in the $LAP_PDS_HOME/stubs.

4) A mission calendar file named Mission_Calendar.txt 
   describing mission phases. Default location $LAP_PDS_HOME.
   KEEP THIS UP TO DATE!!!

5) Anomaly file as described above default is: -a pds.anomalies
   And default location is: $HOME

6) File with extra bias settings outside macros, default is: -b pds.bias
   And default location is: $HOME
   Bias files are generated from TC history with: run_mpb pds.bias	

7) File with macros to exclude in calibration mode such as calibration macros.
   Default is: -e pds.exclude
   And default location is: $HOME	

8) File with short human descriptions of macros, default is: -m pds
   And default location is: $HOME	

9) File with time intervals for which data should be excluded/omitted from
   the archives (pds.dataexcludetimes).
   
10) If we are producing a calibrated archive!
   Calibration data files must exist in the CALIB directory
   and they must be in the format of the calibration templates
   see below.

The configuration files above can be found in  $LAP_PDS_HOME/pds_config/ copying or linking this  to $HOME is a good start.
pds.conf might be individual for different users so linking this is not recommended.


LOGGING:
--------

The following log files exists in the logging directory, not part of final PDS archive. 
Currently the logs tend to get very large so after each run they are compressed to save space. 
For this to work external shell commands cd, tar and gzip must exist.


NAME                        DESCRIPTION                                      FILE DESCRIPTOR
--------------------------------------------------------------------------------------------
0pds_system.log             * Overall program status                         pds.ylog_fd
1pds_dds_packet_filter.log  * Decoding of dds packets                        pds.dlog_fd
2pds_dds_last_state.log     * Last decoded dds file                          pds.ddss_fd
3pds_dds_progress.log       * List of all previously decoded dds files       pds.ddsp_fd
4pds_rpc_packet_filter.log  * Filtering of S/C TM packets                    pds.plog_fd
5pds_science_decode.log     * Info about science decoding                    pds.clog_fd
6pds_hk_decode.log          * Info about HK decoding                         pds.hlog_fd


NOTE ABOUT THREADS:
-------------------
Threads are used in this program to get efficent IO and data crunching at the same time.
It was also from the beginning intended to run continiously on a data stream, producing
a single PDS archive for the whole mission. This was not allowed so it was reworked to
operate on single mission phases, however the stream architecture was kept.

This gives us the bonus of running better on a multithreded CPU system, or multiple CPUs. 

Protected regions are created with code like the one below:

static pthread_mutex_t protect=PTHREAD_MUTEX_INITIALIZER; 

and: 

pthread_mutex_lock(&protect);
 -=< CRITICAL REGION >=-
 pthread_mutex_unlock(&protect);

Around all critical regions!!


Also note that ANSI IO functions (such as fwrite) are thread safe, we also use reentrant functions when they are needed.


TO BE DONE:
-----------

 *] We need a new instrument mode for offset calibration of gain 0.05 data and this must be implemented in the PDS system
    currently we only have a mode for gain 1.0

