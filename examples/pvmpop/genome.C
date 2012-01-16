/* ----------------------------------------------------------------------------
  genome.C
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   This file contains the information needed to evaluate an operate on a 
genome in these examples.  We define routines for sending and receiving genome
data as well as routines for modifying the genetic algorithm's population.
   At some point we should add better error checking to the pvm status returns.
---------------------------------------------------------------------------- */
#include <pvm3.h>
#include <unistd.h>
#include "genome.h"

// When this flag is defined, the initialize and evaluate function dump a 
// string to cerr that tells which host they're running on.  You can then look
// at this info in the pvm log file.
//#define DEBUG

int PackIndividual(GAGenome& g);
int UnpackIndividual(GAGenome& g);


// The first few routines depend on the type of genome that you are using.
// If you want to change the genome type, be sure to modify these functions.
// Technically these should be member functions of a genome class that knows
// how to pack and unpack itself.  Perhaps some other day...



// This is a simple one-max-like objective function.  We just try to set every
// other bit in the binary string.
float
GenomeEvaluator(GAGenome& g) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
#ifdef DEBUG
  char buf[255];
  gethostname(buf, 255);
  cerr << "evaluating on " << buf << "\n";
#endif
  float score=0.0;
  for(int i=0; i<genome.length(); i++){
    if(genome.gene(i) == 0 && i%2 == 0)
      score += 1.0;
    else if(genome.gene(i) == 1 && i%2 != 0)
      score += 1.0;
  }
//  sleep(2);

  return score;
}


// Send the bits of the genome to the task that requested them.  First we send
// the number of bits, then we send the bits themselves.  Note that we can 
// handle genomes of varying lengths with this setup.  We also pack the score
// and stuff that in as well (so that they don't have to do an eval at the
// other end).  If we did this as a member function we could save the hassle
// of an extra copy of the bits...
//   Returns negative number (error code) if failure.
int
PackIndividual(GAGenome& g) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
  static int* bits = 0;
  static int nbits = 0;
  int status = 0;;

  if(nbits < genome.length()){
    nbits = genome.length();
    delete [] bits;
    bits = new int [nbits];
  }

  int length = genome.length();
  for(int i=0; i<length; i++)
    bits[i] = genome.gene(i);

  status = pvm_pkint(&length, 1, 1);
  status = pvm_pkint(bits, length, 1);

  float score = g.score();
  status = pvm_pkfloat(&score, 1, 1);

  return status;
}


// Receive the bits from the specified task.  Stuff the genome with the data.
// Returns a negative number if there was a transmission failure.
int
UnpackIndividual(GAGenome& g) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
  int length = 0;
  float score = 0.0;
  static int nbits = 0;
  static int* bits = 0;
  int status = 0;

  status = pvm_upkint(&length, 1, 1);
  if(nbits < length){
    nbits = length;
    delete [] bits;
    bits = new int [nbits];
  }
  status = pvm_upkint(bits, length, 1);
  
  genome.length(length);	               // resize the genome
  genome = bits;			       // stuff it with the bits

  status = pvm_upkfloat(&score, 1, 1);     // get the score from process
  g.score(score);			   // set the score on the genome

  return status;
}














// This should eventually use a genome member function rather than an external.
// When we pack/unpack a population we also stuff its statistics.
int
SendPopulation(int toid, const GAPopulation& pop) {
  int status = 0;
  int psize = pop.size();
  status = pvm_initsend(PvmDataDefault);
  status = pvm_pkint(&psize, 1, 1);
  for(int i=0; i<pop.size() && status>=0; i++)
    status = PackIndividual(pop.individual(i));
  status = pvm_send(toid, MSG_INCOMING_POPULATION);
  return status;
}


// This assumes that the original population contains at least one individual
// from which to grow.  If it does not, the data in the buffer will be ignored.
int
RecvPopulation(GAPopulation& pop) {
  int status = 0;
  int psize = 0;
  status = pvm_upkint(&psize, 1, 1);
  pop.size(psize);
  for(int i=0; i<pop.size() && status>=0; i++)
    status = UnpackIndividual(pop.individual(i));
  return status;
}



int
SendStatistics(int toid, const GAStatistics& s) {
  int status = 0;
  unsigned long int val;
  val = s.numsel;
  status = pvm_pkulong(&val, 1, 1);
  val = s.numcro;
  status = pvm_pkulong(&val, 1, 1);
  val = s.nummut;
  status = pvm_pkulong(&val, 1, 1);
  val = s.numrep;
  status = pvm_pkulong(&val, 1, 1);
  val = s.numeval;
  status = pvm_pkulong(&val, 1, 1);
  val = s.numpeval;
  status = pvm_pkulong(&val, 1, 1);

  status = pvm_send(toid, MSG_INCOMING_STATISTICS);
  return status;
}

int
RecvStatistics(GAStatistics& s) {
  int status = 0;
  unsigned long int val;
  status = pvm_upkulong(&val, 1, 1);
  s.numsel = val;
  status = pvm_upkulong(&val, 1, 1);
  s.numcro = val;
  status = pvm_upkulong(&val, 1, 1);
  s.nummut = val;
  status = pvm_upkulong(&val, 1, 1);
  s.numrep = val;
  status = pvm_upkulong(&val, 1, 1);
  s.numeval = val;
  status = pvm_upkulong(&val, 1, 1);
  s.numpeval = val;
  return status;
}








// Send the specified number of individuals from the current population to
// the specified task.
int 
SendMigration(int toid, GAGeneticAlgorithm& ga, int count) {
  int status = 0;
  status = pvm_initsend(PvmDataDefault);
  status = pvm_pkint(&count, 1, 1);
  for(int i=0; i<count && status>=0; i++)
    status = PackIndividual(ga.population().best(i));
  status = pvm_send(toid, MSG_INCOMING_MIGRATION);
  return status;
}



// Receive a bunch of individuals from a task.  To do this, we clone the GA's
// population, stuff the immigrants into the population, trash the worst 
// individuals to bring the population size back down to what it was, then
// stick the population back into the GA.
//   This implementation is really inefficient, but you get the idea of how
// to do this...
int 
RecvMigration(GAGeneticAlgorithm& ga) {
  int status = 0;

  GAPopulation pop(ga.population());
  GAGenome *tmpind = ga.population().individual(0).clone();

  int count = 0;
  status = pvm_upkint(&count, 1, 1);
  for(int i=0; i<count && status>=0; i++) {
    status = UnpackIndividual(*tmpind);
    pop.add(*tmpind);
  }
  for(int j=0; j<count; j++)
    pop.destroy();		// default behavior is to destroy worst

  ga.population(pop);
  delete tmpind;

  return status;
}

