#LapCommandFile
#
#File: PRG_B5_M15.cmd
#
#
MACRO  Dummy Command : 8100 0615 0000 : Macro ID tag 0x0615
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets Normal Telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03e7 : Calib A20 & A16, 4KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ea 008b : P2 E-field float, P1 Density gain 1, Boot strap on
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : E-Field current bias 0
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c73 0700 0007 : 1792 smpls, dowsampled 128 times, truncated
***Wait 1s
MACRO  Denisty Fix Bias : 8d05 f87f 0000 : Density bias P1 +30V, sweep enabled (GOTO goal)
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter P1 and P2 ADC16 OFF
***Wait 1s
MACRO  Resampling : 9a00 003f 0000 : Keep every 64 sample p1
***Wait 1s
MACRO  Denisty Sweep : 8c08 00f5 0008 : Coarse Density sweep probe 1, 0.83s [-30V..30V]
***Wait 1s
MACRO  Denisty Fix Bias : 8d07 f87f 0000 : Turn off time series, sweeps allowed
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Hold until prb 1 full
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : 9  DENSITY SWEEP P1 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Transfers Prb 1 if full
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until ADC20
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b731 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc20 source 
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 f87f 0000 : Density bias P2 0V (ignored), P1 -30V, no 16 bit, terminate sweep
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until 20 ADC
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b731 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from  adc20 source 
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until 20 ADC
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b731 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from  adc20 source 
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until 20 ADC
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b731 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from  adc20 source 
***Wait 1s
MACRO  Set Filter : 9b01 0000 0040 : Set filter 1172Hz P2
***Wait 1s
MACRO  E Fix Bias : 8e04 7f7f 000a : Sets up P2 efield bias (ignored) and duration 160
***Wait 1s
MACRO  Resampling : 9a00 0000 0007 : Downsampling 8 times ADC16 P2
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b202 0000 0000 : Holds until sampling is finnished on ADC 16 P2
***Wait 1s
MACRO  Set Subheader : b738 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Fill out buffer with data from source p2
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b731 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc 20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 0030 0000 : Go 48 steps back in macro
***Wait 1s
