#LapCommandFile
#
#File: PRG_B5_M4.cmd
#
#
MACRO  Dummy Command : 8100 0604 0000 : Macro ID tag 0x0604
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets Burst Telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Calib A20 & A16, 8KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Density mode (+-32V) P1 and P2 Gain 1.0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c23 06fd 0001 : 1789 smpls moving average cutoff frequency 8.55 Hz, dowsampled 2 times 
***Wait 1s
MACRO  Resampling : 9a00 00ff 0000 : Keep every 256 sample p1
***Wait 1s
MACRO  Denisty Sweep : 8cd0 00d8 000a : Coarse Density sweep probe 1, \ , 6.6s [20V..-32V].
***Wait 1s
MACRO  Denisty Fix Bias : 8d07 7f7f 0000 : Turn off time series
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Hold until prb 1 full
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : 9   DENSITY SWEEP P1 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Transfers Prb 1 if full
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until ADC20
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b703 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc20 source 
***Wait 1s
MACRO  Resampling : 9a00 0000 00ff : Keep every 256 sample p2
***Wait 1s
MACRO  Denisty Sweep : 8cd0 00d8 0012 : Coarse Density sweep probe 2, \ , 6.6s [20V..-32V]
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
MACRO  Sample Hold : b204 0000 0000 : Holds until 20 ADC
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b703 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from  adc20 source 
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 d0d0 7373 : Density bias 20V ,Duration P1 & P2 1840 smpls
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets resampling masks for 16bit adc
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Holds until sampling is finnished on ADC 16 P1 P2
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Fill out buffer with data from source p1
***Wait 1s
MACRO  Set Subheader : b702 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Fill out buffer with data from source p2
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b703 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc 20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 0028 0000 : Go 40 steps back in macro
***Wait 1s
