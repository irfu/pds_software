#LapCommandFile
#
#File: PRG_B1_M4.cmd
#
#
MACRO  Dummy Command : 8100 0204 0000 : Macro ID tag 0x0204
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets burst telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 009b : Density mode and Boot Strap
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 00bc : Density bias 0,Duration P1 2048 smpls,P2 4096 smpls 
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filters A16 Off!
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : Set no moving average!
***Wait 1s
MACRO  Resampling : 9a00 007f 0000 : Set resampling mask for sweep on p1
***Wait 1s
MACRO  Denisty Sweep : 8cf8 01f8 0009 : Coarse Density sweep probe 1, V, 6.6s [30V..-30V].
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : Set SubHeader Sweep P1 raw bipolar
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Sets sweep parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b702 0000 0000 : Set SubHeader P2 raw 16 bit
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  Resampling : 9a00 0000 003f : Set resampling mask for sweep on p2
***Wait 1s
MACRO  Denisty Sweep : 8ce8 01d8 0011 : Coarse Density sweep probe 2, V, 5.8s [26V..-26V].
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Set SubHeader P1 raw 16 bit
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b70a 0000 0000 : Set SubHeader Sweep P2 raw bipolar
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Sets sweep parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 0014 0000 : Go 20 steps back in macro
***Wait 1s
