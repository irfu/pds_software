#LapCommandFile
#
#File: PRG_B5_M3.cmd
#
#
MACRO  Dummy Command : 8100 0603 0000 : Macro ID tag 0x0603
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Normal
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Calib A20 & A16, 8KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Density mode (+-32V) P1 and P2 Gain 1.0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : No MA                
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 d0d0 0088 : Sets fix bias 20 V and duration 256 samples p1 & p2
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets resampling masks for 16bit adc
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Holds until p1 & p2 done
***Wait 1s
MACRO  Do Comp : b805 0000 0000 : P1-P2
***Wait 1s
MACRO  Set Subheader : b710 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Fill out buffer with data from source p1
***Wait 1s
MACRO  Resampling : 9a00 007f 0000 : Keep every 128 sample p1 (For resampling or sweeping)
***Wait 1s
MACRO  Denisty Sweep : 8cc0 0338 000a : Coarse Density sweep probe 1, V, 1.4s [16V..-32V].
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
MACRO  Resampling : 9a00 0000 007f : Keep every 128 sample p2 (For resampling or sweeping)
***Wait 1s
MACRO  Denisty Sweep : 8cc0 0338 0012 : Coarse Density sweep probe 2, V, 1.4s [16V..-32V].
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
MACRO  AQP Hold : b000 0300 0000 : Hold 3 AQP
***Wait 1s
MACRO  Goto : b100 0019 0000 : Go 25 steps back in macro
***Wait 1s
