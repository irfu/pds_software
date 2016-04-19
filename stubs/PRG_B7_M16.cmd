#LapCommandFile
#
#File: PRG_B7_M18.cmd
#
#
MACRO  Dummy Command : 8100 0816 0000 : Macro ID tag 0x0816
***Wait 1s
MACRO  Set Telemetry Rate : 8202 0000 0000 : Sets normal telemetry rate
***Wait 1s
MACRO  ADC Control Register : 8b00 0000 03ff : Sets up ADC control register 8KHz filter
***Wait 1s
MACRO  Set Moving Avrg & ADC Params : 9c62 0706 0005 : P1 20 bit 1798 smpls, down 32 times, truncated
***Wait 1s
MACRO  Set Relays & Muxes : 8a01 00c0 003b : Density P1, P2 Connected to MIP new LDL method
***Wait 1s
MACRO  E Fix Bias : 8e00 7f7f 0000 : Sets E-fix bias to 0 and duration 0
***Wait 1s
MACRO  Set Filter : 9b00 0000 0000 : Digital realtime filters off!
***Wait 1s
MACRO  Denisty Fix Bias : 8d05 f87f 0100 : P1 bias +30V, 16 smpls, sweep allowed
***Wait 1s
MACRO  Resampling : 9a00 003f 0000 : Keep every 64th sample P1 (for sweeping)
***Wait 1s
MACRO  Denisty Sweep : 8c08 00f5 0008 : Sweep P1 (-30V to 30V) Step 0.25V, 64cyc/step
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Set Start Header : b600 0000 0000 : Sets start header and time code
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Holds until prb 1 is full
***Wait 1s
MACRO  Set Subheader : b709 0000 0000 : 9 DENSITY SWEEP P1 RAW 16 BIT BIPOLAR
***Wait 1s
MACRO  Set Parameters : ba01 0000 0000 : Adds sweep parameters
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finished on ADC20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Sets sub header and ID code P1 dens A20 trunc bip
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  Resampling : 9a00 0000 0000 : Sets no resampling for 16bit ADCs
***Wait 1s
MACRO  Denisty Fix Bias : 8d04 f87f 0100 : P1 bias +30V, 16 samples, sweep disallowed
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b201 0000 0000 : Hold until sampling on p1 finished
***Wait 1s
MACRO  Set Subheader : b701 0000 0000 : Sets sub header and ID code D_P1_16BIT_RAW
***Wait 1s
MACRO  Fillout Buffer : b301 0000 0000 : Empty p1 to buffer
***Wait 1s
MACRO  Denisty Fix Bias : 8d06 f87f 0000 : Ensures no 16 bit following AQPs
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Sets sub header and ID code D_P1_20_BIT_RAW_BIP
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Sets sub header and ID code P1 dens A20 trunc bip
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Sets sub header and ID code P1 dens A20 trunc bip
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  AQP Hold : b000 0101 0000 : AQP hold 1 allow sampling
***Wait 1s
MACRO  Sample Hold : b204 0000 0000 : Holds until sampling is finnished on ADC 20
***Wait 1s
MACRO  A 20 Moving (& Grooving) Avrg : b500 0000 0000 : Do MA filtering/resampling on ADC 20
***Wait 1s
MACRO  Set Subheader : b704 0000 0000 : Sets sub header and ID code P1 dens A20 trunc bip
***Wait 1s
MACRO  Set Parameters : ba02 0000 0000 : Sets 20 bit parameters in data stream
***Wait 1s
MACRO  Fillout Buffer : b303 0000 0000 : Fill out buffer with data from source ADC20
***Wait 1s
MACRO  FullOut Buffer : b400 0000 0000 : Indicate out buffer is full
***Wait 1s
MACRO  Goto : b100 002d 0000 : Go 45 steps back in macro
***Wait 1s
