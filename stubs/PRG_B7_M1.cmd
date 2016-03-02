#LapCommandFile
#
#File: PRG_B7_M1.cmd
#
#
MACRO  Dummy Command : 8100 0801 0000 : Macro ID tag 0x0801
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets normal telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Sets up ADC control register 8KHz filter
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ff 0089 : E-Field P1 & P2, Float Bias
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 : Sets fix bias 0 V and duration 0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : No resampling
***Wait 1s
MACRO  E Fix Bias : 8e0c 8080 0000 : Does nothing
***Wait 1s
MACRO  E Fix Bias : 8e06 8080 0c0c : Sets duration 192 smpls p1 & p2 (bias ignored)
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Hold until sampling on p1 finished
***Wait 1s
MACRO  Set Subheader : b715 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  E Fix Bias : 8e06 8080 0000 : Ensures no 16 bit following AQPs (Bias value ignored)
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b717 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0025 0000 : Go 37 steps back in macro
***Wait 1s
