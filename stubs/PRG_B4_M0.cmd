#LapCommandFile
#
#File: PRG_B4_M0.cmd
#
#
MACRO  Dummy Command : 8100 0500 0000 : Macro ID tag 0x0500
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets normal telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Set Density, Gain 1.0, Boot strap on
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0077 : Set fix density bias & durations
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : No 16 bit ADC filters
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : No MA or 20 Bit ADC
***Wait 1s
MACRO  Resampling : 9a00 00ff 0000 : Sets resampling masks for sweep on p1
***Wait 1s
MACRO  Denisty Sweep : 8ce0 0369 0009 : Setup sweep on p1 [24V..-24V]  5.4 s
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b70b 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Sets sweep or filter parameters in data stream
***Wait 1s
MACRO  Do Comp : b801 0001 0000 : Run log compression p1
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b702 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  Resampling : 9a00 0000 00ff : Sets resampling masks for p2
***Wait 1s
MACRO  Denisty Sweep : 8ce0 0369 0011 : Setup sweep on p2 [24V..-24V]  5.4 s
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b70c 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Sets sweep or filter parameters in data stream
***Wait 1s
MACRO  Do Comp : b801 0002 0000 : Run log compression p2
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  AQP Hold : b000 0400 0000 : Hold 4 AQP
***Wait 1s
MACRO  Goto : b100 0017 0000 : Go 23 steps back in macro
***Wait 1s
