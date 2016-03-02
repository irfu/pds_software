#LapCommandFile
#
#File: PRG_B2_M7.cmd
#
#
MACRO  Dummy Command : 8100 0307 0000 : Macro ID tag 0x0307
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Normal
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03e7 : Calib A20 & A16, 4KHz filt, BiPolar
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 0000 009b : Density (+-5V) P1 & P2 gain 1.0
***Wait 1s
MACRO  Denisty Fix Bias : 8d00 7f7f 0088 :  0     volts p1 & p2 fix len = 256
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c00 0000 0000 : No MA                
***Wait 1s
MACRO  Resampling : 9a00 007f 0000 : Keep every 128 sample p1 (For resampling or sweeping)
***Wait 1s
MACRO  Denisty Sweep : 8caf 0338 bb0d : Fine sweep P1, Offset 11,1.4 s
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Put Start header and time code in data stream
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Hold until prb 1 & prb 2 full
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : 9   DENSITY SWEEP P1 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Transfers Prb 1 if full
***Wait 1s
MACRO  Set Subheader : b702 0000 0000 : 2 DENSITY P2 RAW 16 BIT
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Transfers Prb 2 if full
***Wait 1s
MACRO  Resampling : 9a00 0000 007f : Keep every 128 sample p2 (For resampling or sweeping)
***Wait 1s
MACRO  Denisty Sweep : 8caf 0338 bb15 : Fine sweep P2, Offset 11,1.4 s
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : Default Hold 1 AQP
***Wait 1s
MACRO  Sample Hold : b203 0000 0000 : Hold until prb 1 & prb 2 full
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : 1 DENSITY P1 RAW 16 BIT
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Transfers Prb 1 if full
***Wait 1s
MACRO  Set Subheader : b70a 0000 0000 : 10  DENSITY SWEEP P2 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep params
***Wait 1s
MACRO  Fillout Buffer : b302 0000 0000 : Transfers Prb 2 if full
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Out buffer full
***Wait 1s
MACRO  AQP Hold : b000 0600 0000 : Hold 6 AQP
***Wait 1s
MACRO  Goto : b100 0015 0000 : Go back 21 steps
***Wait 1s




