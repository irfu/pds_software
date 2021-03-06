#LapCommandFile
#
#File: PRG_B3_M0.cmd
#
#
MACRO  Dummy Command : 8100 0400 0000 : Macro ID tag 0x0400
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets normal telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : Set moving average and 20 bit ADC parameters
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ff 0080 : E-Field P1 & P2, Current Bias
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 : Sets fix bias 0 V and duration 0
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0099 : Sets up efield bias and duration 512 smpls
***Wait 1s
MACRO  Set Filter : 9b03 0000 0000 : Digital Filter Cut Off 4688 Hz p1 and p2
***Wait 1s
MACRO  Resampling : 9a00 0001 0001 : Downsampling 2 times p1 and p2
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Hold until sampling on p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b733 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b734 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  AQP Hold : b000 0800 0000 : AQP hold 8 don't sample
***Wait 1s
MACRO  Goto : b100 0009 0000 : Go 9 steps back in macro
***Wait 1s
