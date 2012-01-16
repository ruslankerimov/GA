/* ----------------------------------------------------------------------------
  master.C
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   Example program to illustrate use of GAlib with PVM.  This example uses a
master-slave configuration to parallelize the genetic algorithm.  In this case,
the master controls the evolution by specifying migrations between populations
on various processes.
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <iostream.h>
#include "PVMDemeGA.h"
#include "genome.h"

int
main(int argc, char** argv) {
  cout << "This program tries to fill a 1DBinaryStringGenome with\n";
  cout << "alternating 1s and 0s using a simple genetic algorithm.  It runs\n";
  cout << "in parallel using PVM and a population on each process.\n\n";
  cout.flush();

  GA1DBinaryStringGenome genome(GENOME_LENGTH);
  PVMDemeGA ga(genome);
  ga.parameters(argc, argv);
  ga.parameters("settings.txt");
  if(ga.spawn("slave") < 0) exit(1);

  cout << "initializing..." << endl;
  ga.initialize();
  cout << ga.statistics().bestIndividual() << endl;
  cout << "evolving..." << endl;
  while(!ga.done()){
    ga.step();
    cout << ga.statistics().bestIndividual() << endl;
  }
  ga.flushScores();

  cout << "\nThe GA found an individual with a score of ";
  cout << ga.statistics().bestIndividual().score() << endl;

  return 0;
}
