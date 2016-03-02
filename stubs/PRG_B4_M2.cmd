#LapCommandFile
#
#File: PRG_B4_M2.cmd
#
#
MACRO  Dummy Command : 8100 0502 0000 : Macro ID tag 0x0502
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : Set moving average and 20 bit ADC parameters
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Sets up Density Mode P1 & P2
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 00cc : Sets fix bias 0, Duration 4096 samples on p1 & p2
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter offsets for P1 and P2
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets resampling masks for 16bit adc
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Do Comp : b805 0000 0000 : Differance sensor 1 - sensor 2
***Wait 1s
MACRO  Set Subheader : b710 0000 0000 : Sets sub header and id code 0x10
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0007 0000 : Go 7 steps back in macro
***Wait 1s
