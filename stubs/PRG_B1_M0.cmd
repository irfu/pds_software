#LapCommandFile
#
#File: PRG_B1_M0.cmd
#
#
MACRO  Dummy Command : 8100 0200 0000 : Macro ID Tag 0x0200
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Put Start header and time code in data stream
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Sample hold until p1 & p2 finished
***Wait 1s
MACRO  Set Subheader : b729 0000 0000 : Set id to generic id 41
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Set Subheader : b72a 0000 0000 : Set id to generic id 42
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Sample hold until A20 sampling is finished
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b72b 0000 0000 : Set id to generic id 43
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty tmp buffer
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 000c 0000 : Go 12 steps back in macro
***Wait 1s
