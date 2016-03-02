#LapCommandFile
#
#File: PRG_B2_M3.cmd
#
# EE-Mode
MACRO  Dummy Command : 8100 0303 0000 : Macro ID tag 0x0303
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : Set moving average and 20 bit ADC parameters
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00ff 0080 : Sets up relays and muxes
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 : Sets fix bias and duration
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 00bb : Sets up efield bias and duration
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter offsets for P1 and P2
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets resampling masks for 16bit adc:s
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold AQP:S or (LDL AQPs),(Sample on/off)
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Holds until sampling is finnished on ADC X
***Wait 1s
MACRO  Set Subheader : b715 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1
***Wait 1s
MACRO  Set Subheader : b716 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0008 0000 : Go 8 steps back in macro
***Wait 1s


