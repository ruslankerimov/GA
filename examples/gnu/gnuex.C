/* ----------------------------------------------------------------------------
  gnuex.C
  mbwall 13sep95
  Copyright 1995 Massachusetts Institute of Technology

  This code can be freely distributed and modified under the terms of the GNU
  public license.   See the COPYING file for details.

 DESCRIPTION:
   This example illustrates the use of a custom data structure (in this case
the BitString object from the GNU g++ class library) with GAlib.  This example
uses a steady-state GA to evolve a bit string with alternating 1s and 0s.  It
uses a uniform crossover and bitflip mutation, both of which are custom written
for use with the GNU BitString object.
   You can specify which type of genome you would like to use - the GNU bit
string or the binary string built in to GAlib (I did this for some simple speed
comparisons to get an idea of how efficiently each was implemented).  In my
simple tests I found the GAlib binary string object to be a bit faster than
the GNU bit string (but the gnu bit string is a much more robust, more
compact, etc implementation).
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <iostream.h>
#include <ga/ga.h>		// this is the include file for the ga library

// When this is defined, use the GNU BitString otherwise we use the 
// GA1DBinaryString that is built in to GAlib.
#define USE_GNU	

#ifdef USE_GNU
#include "bitstr.h"		// this header contains the genome we defined
#define GENOME_TYPE    BitStringGenome
#else
#define GENOME_TYPE    GA1DBinaryStringGenome
#endif


float Objective(GAGenome &);

int
main() {
  cout << "This program tries to fill a bit string with\n";
  cout << "alternating 1s and 0s using a steady-state GA,\n";
  cout << "uniform crossover, and bitflip mutation.\n\n"; cout.flush();

// Declare variables for the GA parameters and set them to some default values.

  int length   = 512;
  int popsize  = 30;
  int ngen     = 1000;
  float pmut   = 0.001;
  float pcross = 0.9;
  int which = 
    GAStatistics::Maximum | GAStatistics::Minimum | GAStatistics::Mean;

// Now create the GA and run it.  First we create a genome of the type that
// we want to use in the GA.  The ga doesn't use this genome in the
// optimization - it just uses it to clone a population of genomes.

  GENOME_TYPE genome(length, Objective);

// Now that we have the genome, we create the genetic algorithm and set
// its parameters - number of generations, mutation probability, and crossover
// probability.  We tell the ga to keep the single best genome of all the 
// generations, then tell the GA to evolve itself.

  GASteadyStateGA ga(genome);
  ga.populationSize(popsize);
  ga.nGenerations(ngen);
  ga.pMutation(pmut);
  ga.pCrossover(pcross);
  ga.nBestGenomes(1);		// how many of the 'best' we should keep
  ga.scoreFilename("bog.dat");	// where to dump the statistics
  ga.flushFrequency(50);	// how often to dump the statistics
  ga.selectScores(which);

#ifndef USE_GNU
  GA1DBinStrUniformCrossover cro;
  ga.crossover(cro);
#endif
  ga.evolve();

  cout << "best individual is:\n" << ga.statistics().bestIndividual() << "\n";
  cout << "generational data are in 'bog.dat'\n";

  return 0;
}
 



// This is the objective function.  All it does is check for alternating 0s and
// 1s.  If the gene is odd and contains a 1, the fitness is incremented by 1.
// If the gene is even and contains a 0, the fitness is incremented by 1.  No
// penalties are assigned.
float
Objective(GAGenome & c)
{
  GENOME_TYPE & genome = (GENOME_TYPE &)c;
  float score=0.0;
  for(int i=0; i<genome.length(); i++){
    if(genome.gene(i) == 0 && i%2 == 0)
      score += 1.0;
    if(genome.gene(i) == 1 && i%2 != 0)
      score += 1.0;
  }
  return score;
}
