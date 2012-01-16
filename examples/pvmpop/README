mbw 5dec95

To build the programs in this directory, use aimk (part of the PVM package).
You should be able to simply type 'aimk' to build the programs or
'aimk install' to build the programs then put them into your PVM bin 
directories.  Be sure you edit the Makefile first to work with your compiler
and to specify the location of the GAlib headers and library.  You can use
'aimk clean' just as you would use 'make clean'.

This example shows how to use GAlib with pvm to do parallel processing of your
genetic algorithms.  In this example we use a master-slave configuration.  The
master controls the overall evolution and the slaves chug away.  Using PVM you
can specify on which machines the slaves should run (see the PVM documentation
for more details about that).

Note that this configuration is particularly useful for problems in which the
objective function takes a long time to run relative to the time it takes to
transfer a single genome from the master to the slave (or vice versa).

Before you run this example, you must install PVM3 (I have tested with version
3.3.10 - you'll have to check the PVM documentation for PVM incompatibilities).
The makefile in this directory assumes that you have your environment properly
configured as described in the PVM documentation (see the makefile for details)

For more details about PVM, see 
 http://www.netlib.org/pvm3/index.html
 http://www.epm.ornl.gov/pvm/pvm_home.html







OK, so you don't want to chug through the PVM documentation?  Here's the 
quick and dirty PVM config (assuming that PVM is on your system already):

1) add this to your .cshrc file (you may have to change the value of 
   PVM_ROOT to match your system's configuration)

# -----------------------------------------------------------------------------
# pvm additions for .cshrc file (mbw dec95)
#
# These are for using the parallel virtual machine codes.  The first tells
# where PVM is located, the second tells where XPVM is located.
setenv PVM_ROOT /nfs/lancet/local/pvm3
setenv XPVM_ROOT /nfs/lancet/local/pvm3/xpvm

# These set up the environment so we can find the pvm man pages and run pvm
# programs without typing in the explicit path to each executable.
setenv PVM_ARCH `$PVM_ROOT/lib/pvmgetarch`
set path=($path $PVM_ROOT/lib)
set path=($path $PVM_ROOT/bin/$PVM_ARCH $HOME/pvm3/bin/$PVM_ARCH)

# The MANPATH variable must be defined already for this to work.
setenv MANPATH ${MANPATH}:$PVM_ROOT/man
# -----------------------------------------------------------------------------


2) create a pvm directory for your programs

cd
mkdir pvm3 pvm3/bin


3) create a hosts file that defines your virtual machine - just put the
   hostnames of all the machines you'll be using into a file, then when you
   start pvm, pass the name of the hosts file and pvm will use those hosts
   to set up the PVM.  I keep a file called ~/pvm3/hosts with my default
   host configuration.


4) start pvm

pvm ~/pvm3/hosts


5) run your program in a separate shell (do not type this at the pvm prompt)

master ngen 100 nslaves 25


6) to shut down pvm, type 'halt' at the pvm prompt (if you just type 'quit'
   then you'll leave the pvm prompt but pvmd will still be running)





IMPORTANT NOTES
  To shut down all of the processes spawned in your PVM, type 'reset' at the
pvm prompt.  If you just control-C the master then it will die but the slaves
will continue to run (unless you do some signal handling in the master).  Don't
forget to 'halt' the PVM when you are finished running everything.  For help
with PVM commands, type 'help' at the PVM prompt.
  To see the cout/cerr messages from your spawned (slave) processes, look in 
the file /var/tmp/pvml.XXXXXX where XXXXXX is your uid.  On some systems the
pvm log file may be located in /tmp/pvml.XXXXXX
