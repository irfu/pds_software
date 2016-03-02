#LapCommandFile
#
#File: PRG_B5_M5.cmd
#
#
MACRO  Dummy Command : 8100 0605 0000 : Macro ID tag 0x0605
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets Burst Telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Calib A20 & A16, 8KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Density mode (+-32V) P1 and P2 Gain 1.0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : No MA                
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 d0d0 b0b0 : Density bias 20V ,Duration P1 & P2 2816 smpls
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets resampling masks for 16bit adc
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Holds until p1 & p2 done
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Fill out buffer with data from source p1
***Wait 1s
MACRO  Set Subheader : b702 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Fill out buffer with data from source p2
***Wait 1s
MACRO  Resampling : 9a00 001f 0000 : Keep every 32 sample p1 (For resampling or sweeping)
***Wait 1s
MACRO  Denisty Sweep : 8cf8 00f8 000a : Coarse Density sweep probe 1, \ , 6.6s [30V..-30V].
***Wait 1s
MACRO  Denisty Fix Bias : 8d07 7f7f 0000 : Turn off time series
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Hold until prb 1 full
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : 9   DENSITY SWEEP P1 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Transfers Prb 1 if full
***Wait 1s
MACRO  Resampling : 9a00 0000 001f : Keep every 32 sample p2
***Wait 1s
MACRO  Denisty Sweep : 8cf8 00f8 0012 : Coarse Density sweep probe 2, \ , 6.6s [30V..-30V].
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b202 0000 0000 : Hold until prb 2 full
***Wait 1s
MACRO  Set Subheader : b70a 0000 0000 : 10  DENSITY SWEEP P2 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Transfers Prb 2 if full
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 0019 0000 : Go 25 steps back in macro
***Wait 1s
