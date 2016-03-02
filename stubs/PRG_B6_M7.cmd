#LapCommandFile
#
#File: PRG_B6_M7.cmd
#
#
MACRO  Dummy Command : 8100 0707 0000 : Macro ID tag 0x0707
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets Normal Telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Calib A20 & A16, 8KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 00ad : Open offset current calibration
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 7f7f 0000 : Sets fix bias 0V and duration 0
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c63 0700 0006 : 1792 smpls, dowsampled 64 times, truncated
***Wait 1s
MACRO  Denisty Fix Bias : 8d06 7f7f 0000 : Turn off time series, sweeps off, bias ignored
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until ADC20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b703 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc20 source 
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until 20 ADC
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
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
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
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
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b703 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from  adc20 source 
***Wait 1s
MACRO  Denisty Fix Bias : 8d06 7f7f 0606 : Duration P1 & P2 96 smpls, ignore bias
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
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
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
