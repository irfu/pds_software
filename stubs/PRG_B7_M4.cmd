#LapCommandFile
#
#File: PRG_B7_M4.cmd
#
#
MACRO  Dummy Command : 8100 0804 0000 : Macro ID tag 0x0804
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets burst telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Sets up ADC control register 8KHz filter
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c02 0706 0000 : P1 20 bit 1798 smpls, truncated
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 003b : Density P1, P2 Connected to MIP new LDL method
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : Sets E-fix bias to 0 and duration 0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : No resampling
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 a8a8 9700 : Sets P1 density bias 10V and duration 2416 smpls
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Hold until sampling on p1 finished
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code D_P1_16BIT_RAW
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (&Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Sets sub header and ID code D_P1_TRNC_20_BIT_RAW_BIP
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 000b 0000 : Go 11 steps back in macro
***Wait 1s
