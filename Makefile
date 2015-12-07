
# Compiles PDS software for LAP
# =====================================================================
#
# dds2rpc 	Reads archive structure and feeds pds (and or a gse)
# 
# pds		Recives data from dds2rpc and converts
#               this into a pds archive
#
# OASWLIB.f 	Was included to get ESA orbit and attitude software
#               as well as time functions. It required lots of
#               fortran libraries to work though. We use
#               the intel fortran compiler 8.0.
#
# NOTE: pds is only written to be run as a 32-bit application. Therefore
# it has to be compiled as 32-bit also on 64-bit systems, e.g. spis 
# and birra, using the gcc -m32 flag.
# /Erik P G Johansson 2015-04-27

# gcc options:
#       -Wall
#           This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the
#           warning), even in conjunction with macros.  This also enables some language-specific warnings described in C++ Dialect Options and Objective-C
#           and Objective-C++ Dialect Options.
#	   ...
#
#       -ggdb
#           Produce debugging information for use by GDB.  This means to use the most expressive format available (DWARF 2, stabs, or the native format if
#           neither of those are supported), including GDB extensions if at all possible.
#
#       -m32
#       -m64
#       -mx32
#           Generate code for a 32-bit or 64-bit environment.  The -m32 option sets "int", "long", and pointer types to 32 bits, and generates code that
#           runs on any i386 system.
#
#           The -m64 option sets "int" to 32 bits and "long" and pointer types to 64 bits, and generates code for the x86-64 architecture.  For Darwin
#           only the -m64 option also turns off the -fno-pic and -mdynamic-no-pic options.
#
#           The -mx32 option sets "int", "long", and pointer types to 32 bits, and generates code for the x86-64 architecture.
#
#======================================================================

VER = 3.09
CC	= gcc
#OBJ1    = pds_$(VER).o plnk.o cirb.o id.o OASWlib.o 
OBJ1    = pds_$(VER).o plnk.o cirb.o id.o
EF      = -lefence
CFLAGS  = -Wall -ggdb -m32       # -m32 : Force compilation as 32-bit application.
# Libraries required by Fortran 
FLIBS   = -lifport -lunwind -lcxa -lifcore #$(EF)
# Fortran includes
FINCL   = /opt/intel_fc_80/lib


all: pds 

#edit FJ 10/7 2014 , added math lib <math.h>, by using -lm after(!!) objects
pds : $(OBJ1) pds.h id.h esatm.h nice.h
	$(CC) $(CFLAGS) $(OBJ1) -lm -o bin/$@ -lpthread -D_GNU_SOURCE -D_XOPEN_SOURCE=500 

#OASWlib.o : OASWlib.f
#	ifort -w -c $< -o $@ -Iinc -cxxlib-gcc

id.o    : id.c id.h
	$(CC) -c $(CFLAGS) id.c
id.c    : id.h
	./doid $<

%.o: %.c nice.h pds.h id.h esatm.h plnk.h cirb.h plnkdec.h cirbdec.h
	$(CC) $(CFLAGS)  -c $< -o $@ -D_GNU_SOURCE -D_XOPEN_SOURCE=500 
clean:
	rm -f *.o 
	rm -f id.c
	rm *~

