#LapCommandFile
#
#File: PRG_B1_M6.cmd
#
#
MACRO  Dummy Command : 8100 0206 0000 : Macro ID tag 0x0206
***Wait 1s
MACRO  AQP Hold : b000 0111 0000 : AQP hold 1 until active LDL phase, allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b72c 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b72d 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold until A20 finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b72e 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty Temporary buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 000c 0000 : Go 12 steps back in macro
***Wait 1s
