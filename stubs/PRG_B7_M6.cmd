#LapCommandFile
#
#File: PRG_B7_M6.cmd
#
#
MACRO  Dummy Command : 8100 0806 0000 : Macro ID tag 0x0806
***Wait 1s
MACRO  Set Telemetry Rate : 8201 0000 0000 : Sets minimum telemetry rate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 003b : Density mode and Boot Strap
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0000 : Density voltage bias 0
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c52 02c0 0005 : 704 smpls, Trunc A20, 0.83 Hz MA filter, resmpl 2 Hz.
***Wait 1s
MACRO  AQP Hold : b000 0111 0000 : AQP hold 1, active LDL, allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold untilA20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Subheader Trunc A20 P1 RAW Bipolar
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets MA filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty Temporary buffer
***Wait 1s
MACRO  AQP Hold : b000 0700 0000 : Hold 7 AQPs, no sampling
***Wait 1s
MACRO  AQP Hold : b000 0111 0000 : AQP hold 1, active LDL, allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold untilA20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Subheader Trunc A20 P1 RAW Bipolar
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets MA filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty Temporary buffer
***Wait 1s
MACRO  AQP Hold : b000 0700 0000 : Hold 7 AQPs, no sampling
***Wait 1s
MACRO  AQP Hold : b000 0111 0000 : AQP hold 1, active LDL, allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold untilA20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Subheader Trunc A20 P1 RAW Bipolar
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets MA filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty Temporary buffer
***Wait 1s
MACRO  AQP Hold : b000 0700 0000 : Hold 7 AQPs, no sampling
***Wait 1s
MACRO  AQP Hold : b000 0111 0000 : AQP hold 1, active LDL, allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold untilA20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Subheader Trunc A20 P1 RAW Bipolar
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets MA filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty Temporary buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  AQP Hold : b000 0700 0000 : Hold 7 AQPs, no sampling
***Wait 1s
MACRO  Goto : b100 001e 0000 : Go 20 steps back in macro
***Wait 1s
