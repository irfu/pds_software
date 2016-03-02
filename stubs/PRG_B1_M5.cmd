#LapCommandFile
#
#File: PRG_B1_M5.cmd
#
#
MACRO  Dummy Command : 8100 0205 0000 : Macro ID tag 0x0205
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets burst telemetry rate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Density mode and Boot Strap
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 : Density voltage bias 0
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c0f 06fd 0000 : 1789 samples p1+p2 full 20 bit
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold until A20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do no MA filtering/resampling on ADC 20, needed though!
***Wait 1s
MACRO  Set Subheader : b706 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Set MA filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty temporary buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0008 0000 : Go 8 steps back in macro
***Wait 1s
