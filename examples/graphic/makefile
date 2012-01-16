# Makefile for GAlib 2.x - example programs
# Copyright (c) 1994-1996 Massachusetts Institute of Technology
# mbwall 17jan96 all rights reserved

# You'll probably have to modify the XLIBS to include the right X libraries
# for your system.  For example, solaris typically need Xmu, some SUNs need PW,
# and HP boxes are really messed up.  On Athena's SUNs you must also add the
# library -lgen when using MOTIF.

# use these libs if you have MOTIF.  You might need to add -lPW to XLIBS.
MOTIF=-DUSE_MOTIF
XLIBS= -lXm -lXt -lX11

# use these if you're compiling with the athena widget set (no motif)
#MOTIF=
#XLIBS= -lXaw -lXt -lX11

# the CC include directory is only for use by makedepend.  linux needs the
# /usr/X11R6/lib, but others might need it as well.
CC_INC_DIR= -I/usr/include/CC
INC_DIRS= -I../..
LIB_DIRS= -L../../ga -L/usr/X11R6/lib

# If you have SGI's DCC compiler...
#C++C= DCC
#CCFLAGS= -ptv +w +pp -O -g $(MOTIF) $(INC_DIRS)

# If you're using the gnu compiler...
C++C= g++
CCFLAGS= -Wall -O -g $(MOTIF) $(INC_DIRS)

.SUFFIXES: .o .C
.C.o:
	$(C++C) $(CCFLAGS) -c $<

all: gaview tspview

gaview: gaview.o 
	$(C++C) -o $@ $@.o $(LIB_DIRS) $(XLIBS) -lga -lm

tspview: tspview.o 
	$(C++C) -o $@ $@.o $(LIB_DIRS) $(XLIBS) -lga -lm

clean:
	rm -rf *~ *.bak *.out *.pixie *.o core ii_files test* bog.dat
	rm -f gaview tspview

depend:
	makedepend $(CC_INC_DIR) $(INC_DIRS) gaview.C tspview.C

# DO NOT DELETE THIS LINE -- make depend depends on it.
