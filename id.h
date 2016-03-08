/* id.h
 * 
 * NOTE: This code is meant to be used by shell script "doid" to automatically generate "id.c" which means it can not be edited arbitrarily.
 * 
 * NOTE: Through "doid", the macro variable NAMES here are used to set the strings in "IDList" in "id.c" which values are then to some extent
 * parsed/interpreted(!) in "pds_X.XX.c", despite that they are "human-readable"! One should therefore NOT arbitrarily change the variable names here.
 */

#ifndef ID_H
#define ID_H

#define UNDEFINED_0x00                  0x00 // UNDEFINED
#define D_P1_RAW_16BIT                  0x01 // DENSITY P1 RAW 16 BIT
#define D_P2_RAW_16BIT                  0x02 // DENSITY P2 RAW 16 BIT
#define D_P1P2INTRL_TRNC_20BIT_RAW_BIP  0x03 // DENSITY P1 & P2 INTERLEAVED TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define D_P1_TRNC_20_BIT_RAW_BIP        0x04 // DENSITY P1 TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define D_P2_TRNC_20_BIT_RAW_BIP        0x05 // DENSITY P2 TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define D_P1P2INTRL_20BIT_RAW_BIP       0x06 // DENSITY P1 & P2 INTERLEAVED 20 BIT ADCS RAW BIPOLAR
#define D_P1_20_BIT_RAW_BIP             0x07 // DENSITY P1 20 BIT ADCS RAW BIPOLAR
#define D_P2_20_BIT_RAW_BIP             0x08 // DENSITY P2 20 BIT ADCS RAW BIPOLAR
#define D_SWEEP_P1_RAW_16BIT_BIP        0x09 // DENSITY SWEEP P1 RAW 16 BIT BIPOLAR
#define D_SWEEP_P2_RAW_16BIT_BIP        0x0a // DENSITY SWEEP P2 RAW 16 BIT BIPOLAR
#define D_SWEEP_P1_LC_16BIT_BIP         0x0b // DENSITY SWEEP P1 LOG COMPRESSION 16 BIT BIPOLAR
#define D_SWEEP_P2_LC_16BIT_BIP         0x0c // DENSITY SWEEP P2 LOG COMPRESSION 16 BIT BIPOLAR
#define UNDEFINED_0x0d                  0x0d // UNDEFINED
#define UNDEFINED_0x0e                  0x0e // UNDEFINED
#define UNDEFINED_0x0f                  0x0f // UNDEFINED
#define D_DIFF_P1P2                     0x10 // DENSITY DIFFERENCE P1 - P2 RAW 16 BIT
#define E_DIFF_P1P2                     0x11 // E_FIELD DIFFERENCE P1 - P2 RAW 16 BIT
#define UNDEFINED_0x12                  0x12 // UNDEFINED
#define UNDEFINED_0x13                  0x13 // UNDEFINED
#define UNDEFINED_0x14                  0x14 // UNDEFINED
#define E_P1_16BIT_RAW                  0x15 // E-FIELD P1 16 BIT RAW
#define E_P2_16BIT_RAW                  0x16 // E-FIELD P2 16 BIT RAW
#define E_P1P2INTRL_TRNC_20BIT_RAW_BIP  0x17 // E-FIELD P1 & P2 INTERLEAVED TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define E_P1_TRNC_20_BIT_RAW_BIP        0x18 // E-FIELD P1 TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define E_P2_TRNC_20_BIT_RAW_BIP        0x19 // E-FIELD P2 TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define E_P1P2INTRL_20BIT_RAW_BIP       0x1a // E-FIELD P1 & P2 INTERLEAVED 20 BIT ADCS RAW BIPOLAR
#define E_P1_20_BIT_RAW_BIP             0x1b // E-FIELD P1 20 BIT ADCS RAW BIPOLAR
#define E_P2_20_BIT_RAW_BIP             0x1c // E-FIELD P2 20 BIT ADCS RAW BIPOLAR
#define UNDEFINED_0x1d                  0x1d // UNDEFINED                           
#define UNDEFINED_0x1e                  0x1e // UNDEFINED
#define D_P1_RAW_16BIT_D2               0x1f // DENSITY P1 RAW 16 BIT DOWNSAMPLED 2 TIMES
#define D_P2_RAW_16BIT_D2               0x20 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 2 TIMES
#define D_P1_RAW_16BIT_D4               0x21 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 4 TIMES
#define D_P2_RAW_16BIT_D4               0x22 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 4 TIMES
#define D_P1_RAW_16BIT_D8               0x23 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 8 TIMES
#define D_P2_RAW_16BIT_D8               0x24 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 8 TIMES
#define D_P1_RAW_16BIT_D16              0x25 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 16 TIMES
#define D_P2_RAW_16BIT_D16              0x26 // DENSITY P1 RAW 16 BIT DOWNSAMPLED 16 TIMES
#define UNDEFINED_0x27                  0x27 // UNDEFINED
#define UNDEFINED_0x28                  0x28 // UNDEFINED
#define GENERIC_ID_P1                   0x29 // GENERIC => DATA P1 IS NOT FULLY TRANSPARENT
#define GENERIC_ID_P2                   0x2a // GENERIC => DATA P2, NOT FULLY TRANSPARENT 
#define GENERIC_ID_20BIT                0x2b // GENERIC => DATA 20 BIT NOT FULLY TRANSPARENT 
#define GENERIC_LDL_P1                  0x2c // GENERIC => DATA LDL P1 NOT FULLY TRANSPARENT 
#define GENERIC_LDL_P2                  0x2d // GENERIC => DATA LDL P2 NOT FULLY TRANSPARENT
#define GENERIC_LDL_20BIT               0x2e // GENERIC => DATA LDL 20 BIT NOT FULLY TRANSPARENT
#define UNDEFINED_0x2f                  0x2f // UNDEFINED
#define E_P1_D_P2_INTRL_20_BIT_RAW_BIP  0x30 // E-FIELD P1 & DENSITY P2 INTERLEAVED TRUNCATED 20 BIT ADCS RAW BIPOLAR
#define UNDEFINED_0x31                  0x31 // UNDEFINED
#define UNDEFINED_0x32                  0x32 // UNDEFINED
#define E_P1_RAW_16BIT_D2               0x33 // E-FIELD P1 RAW 16 BIT DOWNSAMPLED 2 TIMES
#define E_P2_RAW_16BIT_D2               0x34 // E-FIELD P2 RAW 16 BIT DOWNSAMPLED 2 TIMES
#define E_P1_RAW_16BIT_D4               0x35 // E-FIELD P1 RAW 16 BIT DOWNSAMPLED 4 TIMES
#define E_P2_RAW_16BIT_D4               0x36 // E-FIELD P2 RAW 16 BIT DOWNSAMPLED 4 TIMES
#define E_P1_RAW_16BIT_D8               0x37 // E-FIELD P1 RAW 16 BIT DOWNSAMPLED 8 TIMES
#define E_P2_RAW_16BIT_D8               0x38 // E-FIELD P2 RAW 16 BIT DOWNSAMPLED 8 TIMES
#define E_P1_RAW_16BIT_D16              0x39 // E-FIELD P1 RAW 16 BIT DOWNSAMPLED 16 TIMES
#define E_P2_RAW_16BIT_D16              0x3a // E-FIELD P2 RAW 16 BIT DOWNSAMPLED 16 TIMES
#define UNDEFINED_0x3b                  0x3b // UNDEFINED
#define P1_TRANSMITTER                  0x3c // INDICATE P1 TRANSMITTS, EXPECT NEW SUBHEADER AFTER THIS.
#define P2_TRANSMITTER                  0x3d // INDICATE P2 TRANSMITTS, EXPECT NEW SUBHEADER AFTER THIS.
#define UNDEFINED_0x3e                  0x3e // UNDEFINED
#define UNDEFINED_0x3f                  0x3f // UNDEFINED
#define UNDEFINED_0x40                  0x40 // UNDEFINED
#define UNDEFINED_0x41                  0x41 // UNDEFINED
#define UNDEFINED_0x42                  0x42 // UNDEFINED
#define UNDEFINED_0x43                  0x43 // UNDEFINED
#define UNDEFINED_0x44                  0x44 // UNDEFINED
#define MACRO_ID_TAG                    0x45 // A MACRO ID TAG IS PRESENT IN THE DATA STREAM
#define SECOND_ID_01                    0x46 // EXPECT A SECOND SUBHEADER AND A SECOND ID AFTER THIS
#endif /* ID_H */
