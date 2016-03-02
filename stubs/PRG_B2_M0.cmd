#LapCommandFile
#
#File: PRG_B2_M0.cmd
#
#
MACRO  Dummy Command : 8100 0300 0000 : Macro ID tag 0x0300
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Burst
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ff 0080 : E-field mode p1 & p2 and boot strap.
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03e7 : Calib A20 & A16, 4KHz filt, BiPolar
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 :  0  Fix Dbias also stops any previous sweeps
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : Default Configuration
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c0f 06fd 0000 : 1789 samples p1+p2,full 20 bit.
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Put Start header and time code in data stream
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Hold until ADC20 on Prb 1&2 full
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do 20 bit filtering and packing of bits
***Wait 1s
MACRO  Set Subheader : b71a 0000 0000 : 26  E-FIELD P1 & P2 INTERLEAVED20 BIT ADCS RAW BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Adds MA resampling/filter params
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Transfers Temp buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 0008 0000 : Loop back 8 steps
***Wait 1s
