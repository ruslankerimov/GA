/* ----------------------------------------------------------------------------
  master.C
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   Example program to illustrate use of GAlib with PVM.  This example uses a
master-slave configuration to parallelize the genetic algorithm.  In this case,
the master controls the evolution and farms out the task of evaluating single
genomes to each of the slaves.  Initialization of each genome is also 
distributed (in case you have a CPU-intensive initializer).
  This method of parallelization is effective only if the length of time it 
takes to evaluate a genome is longer than the time it takes to transmit the
genome data from the master to the slave.  If this is not the case then try the
single population per processor version of parallelization.
  Be careful about mixing and matching parallel implementations and various
GAlib components - they do *not* all interoperate as you might expect.  For 
example, many of the default GAlib methods do a fair amount of caching, so if
you parallelize one component without doing all the others, you might end up
slowing everything down.
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <iostream.h>
#include <time.h>
#include <pvm3.h>
#include <ga/ga.h>
#include "genome.h"

#define SLAVE_NAME "slave"	// name of the compiled slave program

int StartupPVM(const char*, PVMData&);
int ShutdownPVM(PVMData&);

int
main(int argc, char** argv) {
  cout << "This program tries to fill a 1DBinaryStringGenome with\n";
  cout << "alternating 1s and 0s using a simple genetic algorithm.  It runs\n";
  cout << "in parallel using PVM.\n\n";
  cout.flush();

  GAParameterList params;
  GASimpleGA::registerDefaultParameters(params);
  params.set(gaNpopulationSize, 150);
  params.set(gaNnGenerations, 100);
  params.set(gaNscoreFilename, "bog.dat");
  params.set(gaNflushFrequency, 10);
  params.set(gaNscoreFrequency, 1);
  params.parse(argc, argv);

  int usepvm = 1;
  int length = 32;
  PVMData data;			// our own PVM data structure used by pops
  data.nreq = 5;		// by default we want this many slaves to run

  for(int i=1; i<argc; i++){
    if(strcmp("nopvm", argv[i]) == 0){
      usepvm = 0;
      continue;
    }
    else if(strcmp("len", argv[i]) == 0 || strcmp("l", argv[i]) == 0){
      if(++i >= argc){
        cerr << argv[0] << ": genome length needs a value.\n";
        exit(1);
      }
      else{
        length = atoi(argv[i]);
        continue;
      }
    }
    else if(strcmp("nslaves", argv[i]) == 0 || strcmp("ns", argv[i]) == 0){
      if(++i >= argc){
        cerr << argv[0] << ": number of slaves needs a value.\n";
        exit(1);
      }
      else{
        data.nreq = atoi(argv[i]);
        continue;
      }
    }
    else {
      cerr << argv[0] << ":  unrecognized arguement: " << argv[i] << "\n\n";
      cerr << "valid arguements include standard GAlib arguments plus:\n";
      cerr << "  nopvm\t\tdo not use pvm\n";
      cerr << "  nslaves n\tnumber of slave processes (" << data.nreq << ")\n";
      cerr << "  len l\t\tlength of bit string (" << length << ")\n";
      cerr << "\n";
      exit(1);
    }
  }

  if(usepvm && StartupPVM(argv[0], data)) exit(1);

  GA1DBinaryStringGenome genome(length, GenomeEvaluator);
  GAPopulation pop(genome,1);
  if(usepvm){
    pop.initializer(PopulationInitializer);
    pop.evaluator(PopulationEvaluator);
    pop.userData((void*)&data);
  }
  GASimpleGA ga(pop);
  ga.parameters(params);

  time_t tmStart = time(NULL);

  cout << "initializing the GA...\n"; cout.flush();
  ga.initialize();
  cout << "evolving the solution "; cout.flush();
  while(!ga.done()){
    ga.step();
    if(ga.generation() % 10 == 0){
      cout << ga.generation() << " ";
      cout.flush();
    }
  }
  ga.flushScores();

  time_t tmFinish = time(NULL);

  genome = ga.statistics().bestIndividual();
  cout << "\nThe evolution took " << tmFinish-tmStart << " seconds.\n";
  cout << "The GA found an individual with a score of "<<genome.score()<<"\n";
  if(length < 80) cout << genome << "\n";

  if(usepvm) ShutdownPVM(data);

  return 0;
}






// Set up the PVM stuff.  Register this task then set up all of the slaves.
// Return 1 if a problem, 0 if everything went ok.
int
StartupPVM(const char* prog, PVMData& d) {
  int i;

  d.masterid = pvm_mytid();
  
  int nhost, narch;
  struct pvmhostinfo* hostp;
  int status = pvm_config(&nhost, &narch, &hostp);
  if(status == PvmSysErr) {
    cerr<<"\n" << prog << ": PVM not responding. Have you started the PVM?\n";
    return 1;
  }
  
  d.tid = new int [d.nreq];	// task IDs for the slaves
  d.ntasks = pvm_spawn(SLAVE_NAME, (char**)0, 0, "", d.nreq, d.tid);
  if(d.ntasks <= 0) {
    cerr << prog << ": Error spawning slaves.\n";
    cerr << "  Error codes of failed spawns are:\n";
    for(i=0; i<d.nreq; i++) {
      cerr << "    slave "; cerr.width(3);
      cerr << i << ": " << d.tid[i] << "\n";
    }
    pvm_exit();
    return 1;
  }
  else if(d.ntasks < d.nreq) {
    cerr << prog << ": Spawned only "<<d.ntasks<<" of "<<d.nreq<<"\n";
    cerr << "  Error codes of failed spawns are:\n";
    for(i=0; i<d.nreq; i++) {
      cerr << "    slave "; cerr.width(3); 
      cerr << i << ": " << d.tid[i] << "\n";
    }
  }
  else {
    cerr << prog << ": Spawned " << d.nreq << " slave processes...\n";
  }
  
  return 0;
}


// Now shut down all of the PVM stuff.  Tell all of the slaves to quit.  We
// can do this by sending the quit message or just by killing them all.  We'll
// just kill them all rather than telling them to quit nicely (sinister grin).
int
ShutdownPVM(PVMData& d) {
  for(int i=0; i<d.ntasks; i++)
    pvm_kill(d.tid[i]);
  delete [] d.tid;

  pvm_exit();			// leave the PVM

  return 0;
}
