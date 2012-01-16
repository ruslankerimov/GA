/* ----------------------------------------------------------------------------
  PVMDemeGA.h
  mbwall jan96
  Copyright (c) 1995-1996 Massachusetts Institute of Technology
                          all rights reserved

  Header for the deme (parallel population) genetic algorithm class.
  This genetic algorithm lets you specify a number of individuals to migrate
from one population to another at the end of each generation.  You can specify
how many populations to maintain.  Each population evolves using a steady-state
genetic algorithm.  At the end of each generation, the specified number of 
individuals migrate from one population to the next (we use the loop migration
topology in this implementation).
  You can modify the migration method by deriving a new class from this one and
redefine the migration method.  If you want to use a different kind of genetic
algorithm for each population then you'll have to modify the mechanics of the
step method.
---------------------------------------------------------------------------- */
#ifndef _PVMDemeGA_h_
#define _PVMDemeGA_h_

#include <pvm3.h>
#include <ga/gademe.h>

class PVMDemeGA : public GADemeGA {
public:
  GADefineIdentity("PVMDemeGA", 238);

  PVMDemeGA(const GAGenome&);
  PVMDemeGA(const GAPopulation&);
  PVMDemeGA(const PVMDemeGA&);
  PVMDemeGA& operator=(const PVMDemeGA&);
  virtual ~PVMDemeGA();
  virtual void copy(const GAGeneticAlgorithm&);

  virtual void initialize(unsigned int seed=0);
  virtual void step();
  virtual void migrate();
  PVMDemeGA & operator++() { step(); return *this; }

  const GAPopulation& population(unsigned int i) const;
  const GAStatistics& statistics() const {return stats;}
  const GAStatistics& statistics(unsigned int i) const {return pstats[i];}

public:
  int spawn(const char*);
  void reap();
  int status() const { return _status; }

protected:
  int _mid;                     // id of the master task
  int *_tid;                    // id of each slave task
  int _ntid, _Ntid;		// how many we have, how many we requested
  int _status;                  // status of last pvm function called
  int _nhosts;                  // number of hosts we can see
  int _narch;                   // number of architectures we can see

  int tid2idx(int) const;
  int ntasks() const { return _ntid; }
  int nreq() const { return _Ntid; }
  int collect();
};

#ifndef NO_STREAMS
inline ostream & operator<< (ostream & os, PVMDemeGA & arg)
{arg.write(os); return(os);}
inline istream & operator>> (istream & is, PVMDemeGA & arg)
{arg.read(is); return(is);}
#endif

#endif
