/* ----------------------------------------------------------------------------
  bitstr.C
  mbwall 13sep95
  Copyright 1995 Massachusetts Institute of Technology

  This code can be freely distributed and modified under the terms of the GNU
  public license.   See the COPYING file for details.

 DESCRIPTION:
  Source file for the BitString genome (derived from the GNU BitString object).
---------------------------------------------------------------------------- */
#include <ga/random.h>
#include "bitstr.h"


// Set all the initial values to zero.  We do NOT call the initialize method at
// this point - initialization must be done explicitly by the user of the
// genome (eg when the population is created or reset).  If we called the
// initializer routine here then we could end up with multiple initializations
// and/or calls to dummy initializers (for example when the genome is 
// created with a dummy initializer and the initializer is assigned later on).
BitStringGenome::
BitStringGenome(unsigned int l, GAGenome::Evaluator f, void * u) :
GAGenome(UniformInitializer, UniformMutator, Comparator),
BitString() {
  crossover(UniformCrossover); evaluator(f); ud=u;
  for(int i=0; i<l; i++)
    *this += 1;
}


// This is the class-specific copy method.  It will get called by the super
// class since the superclass operator= is set up to call ccopy (and that is
// what we define here - a virtual function).  We should check to be sure that
// both genomes are the same class.
void
BitStringGenome::copy(const GAGenome & orig) {
  if(&orig == this) return;
  if(!sameClass(orig)){
    GAErr(GA_LOC, className(), "copy", gaErrObjectTypeMismatch);
    return;
  }
  GAGenome::copy(orig);
  BitStringGenome &bsg = (BitStringGenome &)orig;
  BitString::operator=(bsg._substr(0,bsg.length()));
}


// The clone method basically does the same thing as the copy method, but here
// we return a pointer to a completely new genome (the copy method just fills
// the current genome with the copied contents).  It is the responsibility of 
// the caller to free the memory returned by this routine.
//   This implementation does not make use of the clone method flag.
GAGenome *
BitStringGenome::clone(GAGenome::CloneMethod) const {
  return new BitStringGenome(*this);
}







// These are the default operators used by the BitStringGenome.

// The random initializer sets the bits in the bit string randomly to 0 or 1.
// It uses the GARandomBit function to do this (GARandomBit is pretty efficient
// in terms of its calls to your system's random function).
void 
BitStringGenome::UniformInitializer(GAGenome & c) {
  BitStringGenome &genome=(BitStringGenome &)c;
  for(int i=genome.length()-1; i>=0; i--)
    genome.gene(i, GARandomBit());
}


// Randomly pick bits in the bit string then flip their values.  We try to be
// smart about the number of times we have to call any random functions.  If
// the requested likliehood is small enough (relative to the number of bits in
// the genome) then we must do a weighted coin toss on each bit in the genome.
// Otherwise, we just do the expected number of flips (note that this will not
// guarantee the requested mutation rate, but it will come close when the
// length of the bit string is long enough).
int 
BitStringGenome::UniformMutator(GAGenome & c, float pmut) {
  BitStringGenome &genome=(BitStringGenome &)c;
  register int n, i;
  if(pmut <= 0.0) return(0);

  float nMut = pmut * (float)(genome.length());
  if(nMut < 1.0){		// we have to do a flip test on each bit
    nMut = 0;
    for(i=genome.length()-1; i>=0; i--){
      if(GAFlipCoin(pmut)){
	genome.gene(i, genome.gene(i) ? 0 : 1);
	nMut++;
      }
    }
  }
  else{				// only flip the number of bits we need to flip
    for(n=1; n<nMut; n++){
      i = GARandomInt(0, genome.length()-1); // the index of the bit to flip
      genome.gene(i, genome.gene(i) ? 0 : 1);
    }
  }
  return (int)nMut;
}


// The comparator returns a number between 0 and 1 to indicate how similar 
// two genomes are.  A 0 indicates that they are identical (zero diversity)
// whereas a 1 indicates completely different.
//   This implementation assumes that the genomes are the same size.
float
BitStringGenome::Comparator(const GAGenome& a, const GAGenome& b) {
  BitStringGenome& sis = (BitStringGenome&)a;
  BitStringGenome& bro = (BitStringGenome&)b;

  float count=0;
  for(int i=sis.length()-1; i>=0; i--)
    if(sis.gene(i) == bro.gene(i)) count += 1.0;

  return count / sis.length();
}


// This is a an implementation of a uniform crossover for the binary string.
// It assumes that the the genomes are all the same length.
int
BitStringGenome::UniformCrossover(const GAGenome& a, const GAGenome& b, 
				  GAGenome* c, GAGenome* d) {
  BitStringGenome& mom=(BitStringGenome &)a;
  BitStringGenome& dad=(BitStringGenome &)b;

  int n = 0;

  if(c && d){
    BitStringGenome& sis=(BitStringGenome &)*c;
    BitStringGenome& bro=(BitStringGenome &)*d;
    for(int i=sis.length()-1; i>=0; i--){
      if(GARandomBit()){
	sis.gene(i, mom.gene(i));
	bro.gene(i, dad.gene(i));
      }
      else{
	sis.gene(i, dad.gene(i));
	bro.gene(i, mom.gene(i));
      }
    }
    n = 2;
  }
  else {
    BitStringGenome& sis = (c ? (BitStringGenome&)*c : (BitStringGenome&)*d);
    for(int i=sis.length()-1; i>=0; i--)
      sis.gene(i, (GARandomBit() ? mom.gene(i) : dad.gene(i)));
    n = 1;
  }

  return n;
}

