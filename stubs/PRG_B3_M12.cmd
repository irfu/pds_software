#LapCommandFile
#
#File: PRG_B3_M12.cmd
#
#
MACRO  Dummy Command : 8100 0412 0000 : Macro ID tag 0x0412
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets burst telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03e7 : Calib A20 & A16, 4 kHz, bipolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00d5 0099 : P1 E-field float, P2 Density gain 1, Boot strap on
***Wait 1s
MACRO  E Fix Bias : 8e00 7f08 0000 : E-Field current bias 0
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c03 0706 0000 : A20 P1P2 1798 smpls, truncated
***Wait 1s
MACRO  Denisty Fix Bias : 8d05 7f08 0000 : Density bias P2 -30V, sweep enabled (GOTO goal)
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Turn off A16 filters P1 P2
***Wait 1s
MACRO  Resampling : 9a00 0000 003f : Downsampling 64x for P2 sweep
***Wait 1s
MACRO  Denisty Sweep : 8c08 00f5 0010 : Coarse Density sweep probe 2, V, 0.83s [-30V..30V] 
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Hold until A20 finnished sampling
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Needed even if no resampling filtering done
***Wait 1s
MACRO  Set Subheader : b732 0000 0000 : Sets sub header and ID code 0x32 E-P1 D-P2 trunc A20
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets MA parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Empty Temporary to buffer
***Wait 1s
MACRO  Sample Hold : b202 0000 0000 : Hold until p2 finnished sampling
***Wait 1s
MACRO  Set Subheader : b70a 0000 0000 : Sets sub header and ID code 10 Density sweep probe 2
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Sets sweep parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Empty p2 to buffer
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 7f08 0000 : Density bias P1 0V (ignored), P2 -30V, stop sweep
***Wait 1s
MACRO  Set Filter : 9b03 0040 0040 : Set filter 1172Hz P1 & P2
***Wait 1s
MACRO  Denisty Fix Bias : 8d06 7f08 2a2a : Sets A16 P1 P2 duration 672 smpls (bias ignored)
***Wait 1s
MACRO  Resampling : 9a00 0007 0007 : Downsampling 8 times A16 P1 & P2
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Holds until sampling is finished on A16 P1 & P2
***Wait 1s
MACRO  Set Subheader : b737 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Fill out buffer with data from source P1
***Wait 1s
MACRO  Set Subheader : b724 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Fill out buffer with data from source P2
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b732 0000 0000 : Sets sub header and ID code
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets filter parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from adc 20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  Goto : b100 001f 0000 : Go 31 steps back in macro
***Wait 1s
