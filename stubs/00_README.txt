
Footnote 1: pds (which reads the *.mds files), requires that all variable values are surrounded by quotes. An older version of MEDS (the macro editor) which generates *.mds files, mistakenly generates *.mds files WITHOUT quotes. Add quotes manually to prevent pds from hanging/crashing when reading new *.mds files. There is a newer version of MEDS which does add quotes.

Footnote 2: 
a) Most *.mds files contain the variable name "LAP_P1_EFIELD_FIX_DURATION" and some contain the variable name "LAP_P1_E-FIELD_FIX_DURATION" INSTEAD.
b) The forgotten bash/sed script "repl2" in this directory appears to replace the former with the latter in files.
It appears, based on this, that one variable name has been replaced by the other manually in the past (after generation in MEDS). It also appears that this has not caused pds to crash since pds searches for the former variable name, and the latter variable name only occurs in macros which do not use this variable, and if pds does not ask for it, it does not notice the name change and does not crash.

/Erik P G Johansson, Fredrik Johansson 2014-10-30
/Erik P G Johansson, update 2014-12-08



Footnote 1:
One needs to replace
"LAP_P1P2_ADC20_DOWNSAMPLE      = 0x0000"
with
"LAP_P1P2_ADC20_DOWNSAMPLE      = 0x0001"
in *.mds ? Newest version of MEDS should give the correct value.
(See old script "repl" in pds/stubs/.)

Footnote 2:
MEDS can erroneously translate different commands to the same command in .mds files, giving the appearance of setting the bias when it does not, thus fooling pds into believing the bias is set when it is not. One can therefore have commands
"LAP_VBIAS1 ... = "0x007f"
(and similar for probe 2) that should be removed UNLESS they represent genuine commands to set the bias to 0x007f TM units (approximately 0 Volt for voltage). It should be possible to distinguish a genuine setting of bias from a false one by looking at the exact command bytecode (.cmd file). If bit 1 (second-least significant bit, "worth 2") in the second byte is set, then no bias is set.
It is uncertain if the same problem exists for "LAP_IBIAS1 ... = "0x007f" commands, but it seems likely that the same bit in the bytecode (.cmd file) works the same.

/Erik P G Johansson 2014-12-08, 2014-12-16, quoting Reine Gill.



When I have modified *.mds files, I have made sure to keep one version of the original files as PRG_Bx_Mx.mds.unmodified files (x=number). These are not read by pds.

/Erik P. G. Johansson, 2014-12-11



PRG_B4_M5.cmd,
PRG_B4_M6.cmd,
PRG_B7_M7.cmd appear to be missing.
/Erik P G Johansson, 2014-12-15



pds only reads *.mds files using a command
"sprintf(path,"%sPRG_B%d_M%d.mds",home,m_bl,m_n)" (v3.06)
i.e. it only reads *.mds files with the exact "correct" filename, NOT all *.mds files.
/Erik P G Johansson, 2014-12-16



Judging from the source code, pds requires that *.mds files contain exactly one tab (optionally padded with whitespaces) between first and second column ("line numbers" and "variable name"). Otherwise those particular commands will not be read.
/Erik P G Johansson, 2015-04-09



PRG_B8_M4.cmd/.mds are manually derived from PRG_B5_M24.cmd/.mds as the former macro (904) should be identical to the latter (624).
/Erik P G Johansson, 2014-12-17



