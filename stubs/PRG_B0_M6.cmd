#LapCommandFile
#
#File: PRG_B0_M6.cmd
#PROG_TRNSMT_P1.cmd
#
***Wait 1s
MACRO  Dummy Command : 8100 0106 0000 : Macro ID tag 0x0106
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Normal
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Calib A20 & A16, 8KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009f : Density P2 Gain 1.0 and P1 to Transmitter
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0008 :  0     volts p1 & p2 256 samples on p2
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : No MA                
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Default no resampling.
***Wait 1s
MACRO  Transmitter : 9E00 0000 5001 : Start transmitter 1KHz with LTR01.
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP:s with sampling.
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Put Start header and time code in data stream
***Wait 1s
MACRO  Sample Hold : b202 0000 0000 : Hold until Prb 1 full
***Wait 1s
MACRO  Set Subheader : b73c 0000 0000 : Set id to 60 p1 to transmitter.
***Wait 1s
MACRO  Set Subheader : b702 0000 0000 : Set id to 1 raw data p2.
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Transfers Prb 2 if full
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  AQP Hold : b000 0200 0000 : Hold 2 AQP:s without sampling.
***Wait 1s
MACRO  Goto : b100 0008 0000 : Go back 8 steps
***Wait 1s

