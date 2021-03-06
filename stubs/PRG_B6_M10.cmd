#LapCommandFile
#
#File: PRG_B6_M10.cmd
#
#
MACRO  Dummy Command : 8100 0710 0000 : Macro ID tag 0x0710
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets Normal Telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03e7 : Calib A20 & A16, 4KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00d5 0099 : P1 Efloat, P2 density (GOTO target)
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter P1 and P2 ADC16 OFF
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c82 0706 0008 : P2 A20 1798 smpls, down 256x, trunc
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : Sets up efield bias
***Wait 1s
MACRO  Denisty Fix Bias : 8d05 7f08 0000 : Bias P2 -30V
***Wait 1s
MACRO  Resampling : 9a00 0000 003f : Keep every 64 sample P2
***Wait 1s
MACRO  Denisty Sweep : 8c14 0175 0010 : Coarse Density sweep probe 2, / , 0.4s [-27V..28V]
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b202 0000 0000 : Hold until P2 A16 buffer full
***Wait 1s
MACRO  Set Subheader : b70a 0000 0000 : 10   DENSITY SWEEP P2 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Transfers Prb 1 if full
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until ADC20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b718 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc20 source 
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ff 0089 : Sets up relays and muxes E-field floating
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c83 0706 0008 : 1798 smpls, dowsampled 256 times, truncated
***Wait 1s
MACRO  E Fix Bias : 8e04 7f7f 0000 : Sets up efield bias and duration disallow sweep
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until 20 ADC
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
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
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
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
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from  adc20 source 
***Wait 1s
MACRO  Set Filter : 9b03 0040 0040 : Set filter 1172Hz P1 & P2
***Wait 1s
MACRO  E Fix Bias : 8e04 7f7f 0404 : Sets up efield bias and duration 64
***Wait 1s
MACRO  Resampling : 9a00 0007 0007 : Downsampling 8 times ADC16 P1 & P2
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c23 01e0 0002 : Set moving average and 20 bit ADC parameters, downsampling 4
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Holds until sampling is finnished on ADC 16 P1 & P2
***Wait 1s
MACRO  Set Subheader : b737 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Fill out buffer with data from source p1
***Wait 1s
MACRO  Set Subheader : b738 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Fill out buffer with data from source p2
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc 20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 0038 0000 : Go 55 steps back in macro
***Wait 1s
