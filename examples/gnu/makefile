# makefile for compiling some of g++ library components with GAlib extensions
# Copyright 1995 Massachusetts Institute of Technology
# mbwall 10sep95
#
#   This code can be freely distributed and modified under the terms of the GNU
# public license.   See the COPYING file for details.

CC_INC_DIR=/usr/include/CC
INC_DIRS= -I. -I../..
LIB_DIRS= -L. -L../../ga
CCFLAGS= +w +pp -O -g $(INC_DIRS)
C++C= DCC

LIB= libbitstr.a
SRCS= gnuex.C bitstr.C
OBJS= gnuex.o bitstr.o
GNUSRCS= AllocRing.cc Obstack.cc BitString.cc builtin.cc bitand.c bitany.c bitblt.c bitclear.c bitcopy.c bitcount.c bitinvert.c bitlcomp.c bitset1.c bitxor.c error.cc
GNUOBJS= AllocRing.o  Obstack.o  BitString.o  builtin.o  bitand.o bitany.o bitblt.o bitclear.o bitcopy.o bitcount.o bitinvert.o bitlcomp.o bitset1.o bitxor.o error.o

.SUFFIXES: .o .c
.SUFFIXES: .o .cc
.SUFFIXES: .o .C
.c.o:
	$(C++C) $(CCFLAGS) -c $<
.cc.o:
	$(C++C) $(CCFLAGS) -c $<
.C.o:
	$(C++C) $(CCFLAGS) -c $<

gnuex: $(OBJS) $(LIB)
	$(C++C) $(PF) $(OBJS) -o $@ $(LIB_DIRS) -lbitstr -lga -lm

$(LIB): $(GNUOBJS)
	ar rv $(LIB) $?
	@echo $(LIB) is now up-to-date
.c.a:;
.C.a:;

clean:
	rm -rf gnuex $(LIB) *~ *.bak *.out *.o core bog.dat

depend:
	makedepend -I$(CC_INC_DIR) $(INC_DIRS) $(SRCS) $(GNUSRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
