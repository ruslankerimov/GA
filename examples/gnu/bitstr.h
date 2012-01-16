/* ----------------------------------------------------------------------------
  bitstr.h
  mbwall 13sep95
  Copyright 1995 Massachusetts Institute of Technology

  This code can be freely distributed and modified under the terms of the GNU
  public license.   See the COPYING file for details.

 DESCRIPTION:
   Header file for the bitstring example.  This code uses the BitString object
from the GNU class library for the derivation of a new class of genomes.  Here
we define the genome and all of the basic operators that it needs to function.
---------------------------------------------------------------------------- */
#ifndef _bitstr_h_
#define _bitstr_h_

#include <ga/genome.h>
#include "BitString.h"

// This is the class definition for the BitString genome.  It is derived from
// the Genome class and the GNU BitString class.  We define here only the
// additional methods that we'll need in order to use it a genome in GAlib.
//   The identity definition is to take care of the polymorphic nature of 
// GAlib genomes.  You can use any number above 200 when defining your own
// genome type.  Anything under 200 is reserved for use in GAlib internals.
//   I have defined a 'gene' method here as a convenience routine.  It simply
// calls the BitString's bit access member function to determine if a bit is
// set or not.  When you call 'gene' with a second argument, it sets the
// specified bit in the bitstring.
//   Unlike the binary string genomes included in GAlib, this genome is not
// resizable.

class BitStringGenome : public GAGenome, public BitString {
public:
  GADefineIdentity("BitStringGenome", 201);
  static void UniformInitializer(GAGenome&);
  static int UniformMutator(GAGenome&, float);
  static float Comparator(const GAGenome&, const GAGenome&);
  static int UniformCrossover(const GAGenome&, const GAGenome&,
			      GAGenome*, GAGenome*);
public:
  BitStringGenome(unsigned int x, GAGenome::Evaluator f=0, void * u=0);
  BitStringGenome(const BitStringGenome& orig) { copy(orig); }
  BitStringGenome& operator=(const GAGenome& orig)
    { copy(orig); return *this; }
  virtual ~BitStringGenome() {}
  virtual GAGenome *clone(GAGenome::CloneMethod) const;
  virtual void copy(const GAGenome&);

  int write (ostream& os) const { printon(os); return os.fail() ? 1 : 0; }
  int equal(const GAGenome & c) const {
    BitStringGenome & b = (BitStringGenome&)c;
    return ((BitString&)*this == (BitString&)b ? 1 : 0);
  }

  int gene(unsigned int x) const { return test(x); }
  int gene(unsigned int x, int b) 
    { _evaluated = gaFalse; assign(x,b); return test(x); }
};


#endif
