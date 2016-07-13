#LapCommandFile
#
#File: PRG_B5_M13.cmd
#
#
MACRO  Dummy Command : 8100 0613 0000 : Macro ID tag 0x0613
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets Normal Telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03e7 : Calib A20 & A16, 4KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Density mode (+-32V) P1 and P2 Gain 1.0
***Wait 1s
MACRO  Denisty Fix Bias : 8d05 f8f8 0000 : Bias P1 P2 30V
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c73 0700 0007 : 1792 smpls, dowsampled 128 times, truncated
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter P1 and P2 ADC16 OFF
***Wait 1s
MACRO  Resampling : 9a00 003f 0000 : Keep every 64 sample p1
***Wait 1s
MACRO  Denisty Sweep : 8c14 0175 0008 : Coarse Density sweep probe 1, / , 0.4s [-27V..28.0V].
***Wait 1s
MACRO  Denisty Fix Bias : 8d07 f8f8 0000 : Turn off time series, sweeps allowed
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
MACRO  Resampling : 9a00 0000 003f : Keep every 64 sample p2
***Wait 1s
MACRO  Denisty Sweep : 8c14 0175 0010 : Coarse Density sweep probe 2, / , 0.4s [-27V..28V]
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
MACRO  Denisty Fix Bias : 8d04 f8f8 0000 : Density bias P1 P2 30V, no 16 bit, terminate sweep
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
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
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
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
MACRO  Set Filter : 9b02 0040 0000 : Set filter 1172Hz P1
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 f8f8 0b00 : Density bias P1 P2 30V,Duration P1 176 smpls
***Wait 1s
MACRO  Resampling : 9a00 0007 0000 : Downsampling 8 times ADC16 P1
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Holds until sampling is finnished on ADC 16 P1
***Wait 1s
MACRO  Set Subheader : b723 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Fill out buffer with data from source p1
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
MACRO  Goto : b100 0035 0000 : Go 53 steps back in macro
***Wait 1s
