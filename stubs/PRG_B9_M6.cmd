#LapCommandFile
#
#File: PRG_B9_M6.cmd
#
#
MACRO  Dummy Command : 8100 0a06 0000 : Macro ID tag 0x0a06
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Sets up ADC control register
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 003b : Sets up relays and muxes
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 00c0 : Sets fix bias and duration 4096 samples P1
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter offsets for P1 and P2
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : Set moving average and 20 bit ADC parameters
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets resampling masks for 16bit adc
***Wait 1s
MACRO  AQP Hold : b000 0111 0000 : AQP hold 1 until active LDL phase, allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Sample hold until p1 finished
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0006 0000 : Go N steps back in macro
***Wait 1s
