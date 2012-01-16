/* ----------------------------------------------------------------------------
  slave.C
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   Example program to illustrate use of GAlib with PVM.  This example uses a
master-slave configuration to parallelize the genetic algorithm.  In this case,
the master controls the evolution and farms out the task of evaluating single
genomes to each of the slaves.
   This is the slave component.  We wait for messages from the master then act
on the content.  We expect two kinds of messages:  initialize and evaluate.  
When we get an initialize request, we initialize our genome then send the data
to the master process.  When we get an evaluate request, we grab the data from 
the master then invoke the genome's evaluator on the new data, then send only
the score to the master process.  We can also receive a 'finished' message
that tells us to shut down and exit.
---------------------------------------------------------------------------- */
#include <pvm3.h>
#include "genome.h"

int
main(int argc, char** argv) {
  if(argc > 1) {
    cerr << "\n" << argv[0] << ": This program takes no arguments.\n";
    exit(1);
  }

  int mytid = pvm_mytid();
  int masterid = pvm_parent();
  if(mytid < 0 || masterid < 0) {
    cerr << "\n" << argv[0] << ": Couldn't get slave/master IDs.  Aborting.\n";
    exit(1);
  }

// create the genome (we'll resize it later on)
  GARandomSeed();
  GA1DBinaryStringGenome genome(1);
  genome.evaluator(GenomeEvaluator);
  genome.initializer(GenomeInitializer);

// send an "I'm ready" message then wait for instructions from the master
  SendReady(masterid);

  int done = 0;
  int status, bufid, bytes, msgtag, tid;
  while(!done){
    bufid = pvm_recv(masterid, -1);
    if(bufid >= 0) {
      status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
      if(msgtag == MSG_DONE) {
	done = 1;
      }
      else if(msgtag == MSG_GENOME_DATA) {
	RecvGenomeData(genome);
	SendGenomeScore(genome, masterid);
	SendReady(masterid);
      }
      else if(msgtag == MSG_GENOME_INITIALIZE) {
	RecvGenomeInitialize(genome);
	SendGenomeData(genome, masterid);
	SendReady(masterid);
      }
      else {
	cerr << argv[0] << ": unknown msgtag: " << msgtag << "\n";
      }
    }
    else {
      cerr << argv[0] << ": error from pvm_recv: " << bufid << "\n";
    }
  }

  pvm_exit();
  return 0;
}
