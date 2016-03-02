#LapCommandFile
#
#File: PRG_B1_M3.cmd
#
#
MACRO  Dummy Command : 8100 0203 0000 : Macro ID tag 0x0203
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets normal telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ea 0083 : P1 Density gain 1, P2 E-field current bias,Boot strap on
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : E-Field current bias 0
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 : Density voltage bias 0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter offsets for P1 and P2   
***Wait 1s
MACRO  Resampling : 9a00 007f 0000 : Resampling mask 0x7f for p1, we are sweeping p1
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c01 0100 0000 :  20 bit ADC P2 Truncated, 256 Samples, No moving average
***Wait 1s
MACRO  Denisty Sweep : 8caf 0338 0009 : Coarse Density Sweep probe 1, V, 1.4s [12V..-12V]
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP, Sampling allowed
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Hold until sampling on p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Sets sweep or filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Hold until A20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b719 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets sweep or filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty temporary buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  AQP Hold : b000 0300 0000 : Hold 3 AQP
***Wait 1s
MACRO  Goto : b100 000d 0000 : Go 13 steps back in macro
***Wait 1s
