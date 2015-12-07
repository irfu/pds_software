// ESA packet ID Definitions for LAP(first word).
#define HBYTE   0x0d   // High byte is always the same (msb).
#define ACK     0x61   // Acknowledge.
#define HK      0x64   // House Keeping.
#define EVENT   0x67   // Events
#define DUMP    0x69   // Memory Dump
#define SCIENCE 0x6c   // Science data.
#define PIU_ACK       0x31
#define PIU_HK        0x34  
#define PIU_EVENT     0x37  
#define PIU_DUMP      0x39
#define PIU_SCIENCE   0x3c  
#define IES_ACK       0x41  
#define IES_HK        0x44  
#define IES_EVENT     0x47  
#define IES_DUMP      0x49
#define IES_SCIENCE   0x4c
#define ICA_ACK       0x51  
#define ICA_HK        0x54  
#define ICA_EVENT     0x57  
#define ICA_DUMP      0x59  
#define ICA_SCIENCE   0x5c  
#define MIP_ACK       0x71  
#define MIP_HK        0x74  
#define MIP_SCIENCE   0x7c  
#define MAG_ACK       0x81  
#define MAG_HK        0x84  
#define MAG_SCIENCE   0x8c  
