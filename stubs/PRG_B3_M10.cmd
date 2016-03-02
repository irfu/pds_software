#LapCommandFile
#
#File: PRG_B3_M10.cmd
#
#
MACRO  Dummy Command : 8100 0410 0000 : Macro ID tag 0x0410
***Wait 1s
MACRO  Set Telemetry Rate : 8203 0000 0000 : Sets burst telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : ADC CR Bipolar 8KHz Run/Calibrate
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00d5 0099 : P1 E-field float, P2 Density gain 1, Boot strap on
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : E-Field current bias 0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Set filter offsets for P1 and P2   
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c0f 0660 0000 : 20 Bit ADC P1 & P2 1632 Samples, No moving average
***Wait 1s
MACRO  Denisty Fix Bias : 8d05 7f08 0000 : Density bias P2 -30V, sweep enabled
***Wait 1s
MACRO  Resampling : 9a00 0000 003f : Resampling mask 0x3f for P2, we are sweeping P2
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
MACRO  Set Subheader : b730 0000 0000 : Sets sub header and ID code 0x30 E-P1 D-P2 truncated ADC 20
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
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 000f 0000 : Go 15 steps back in macro
***Wait 1s
