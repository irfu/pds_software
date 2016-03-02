#LapCommandFile
#
#File: PRG_B2_M2.cmd
#
#
MACRO  Dummy Command : 8100 0302 0000 : Macro ID tag 0x0302
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets burst  telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ea 0083 : P1 Density gain 1, P2 E-field current bias,Boot strap on
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : E-Field current bias 0
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 a8a8 00bb : Density voltage bias 10 V
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter offsets for P1 and P2   
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : No resampling
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 :  No moving average
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP, Sampling allowed
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Hold until sampling on p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b716 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0008 0000 : Go 8 steps back in macro
***Wait 1s
