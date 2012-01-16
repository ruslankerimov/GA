GNU BitString object and GAlib
mbwall 19sep95

This directory contains some objects from the GNU library (libg++) that can be
used with GAlib.  In particular, I have implemented a set of GAlib operators
for use with the GNU BitString object.  See the makefile for specifics, but
basically you can compile a mini-library using the code in this directory then
link to that as well as GAlib when you compile your program.

The GAlib-specific files are bitstr.C, bitstr.h, and gnuex.C.  bitstr.C and
bitstr.h define the new genome class, and gnuex.C contains the main program
that runs the GA.

I had to modify the GNU files a bit in order to make them work cross-platform.
No major changes, just tweaks to the includes and removal of libg++ 
dependencies that I don't need for this example.

This code has been tested on various UNIX machines.  I do not know if or how
well it work on DOS and/or Mac platforms.  At this point I do not have time to
try to compile GNU code on those machines (especially when the code does such
lowlevel bit operations).

COPYRIGHT and LICENSING ISSUES

The code in this directory is protected under the terms of the GNU public 
license (see the file COPYING for details).  Under the terms of that agreement,
all of the code in this directory is free for any use.  It is included with
GAlib as an example of how to integrate GAlib with other data structures.

Please see the copyright notices in each file for specific ownership.  

As works that use the GNU library (or parts thereof), the GAlib-specific files 
are Copyright MIT, but they are available for copying and distribution under 
the terms of the GNU public license agreement, not the terms of the general 
GAlib licensing agreement.
