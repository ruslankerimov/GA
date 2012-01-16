/* ----------------------------------------------------------------------------
  DemeGA.C
  mbwall 28jul94
  Copyright (c) 1995-1996 Massachusetts Institute of Technology
                          all rights reserved

   Souce file for the deme-based genetic algorithm object.
---------------------------------------------------------------------------- */
#include <string.h>
#include <ga/garandom.h>
#include "PVMDemeGA.h"
#include "genome.h"

#define DEBUG

PVMDemeGA::PVMDemeGA(const GAGenome& c) : GADemeGA(c) {
  _status = 0;
  _Ntid = _ntid = _nhosts = _narch = 0;
  _tid = 0;
  _mid = 0;
}
PVMDemeGA::PVMDemeGA(const GAPopulation& p) : GADemeGA(p) {
  _status = 0;
  _Ntid = _ntid = _nhosts = _narch = 0;
  _tid = 0;
  _mid = 0;
}
PVMDemeGA::PVMDemeGA(const PVMDemeGA& orig) : GADemeGA(orig) {
  _status = 0;
  _Ntid = _ntid = _nhosts = _narch = 0;
  _tid = 0;
  _mid = 0;
  copy(orig);
}
PVMDemeGA::~PVMDemeGA(){
  reap();			// just in case
}
PVMDemeGA&
PVMDemeGA::operator=(const PVMDemeGA& orig){
  if(&orig != this) copy(orig); 
  return *this;
}
void
PVMDemeGA::copy(const GAGeneticAlgorithm& g){
  reap();			// need to fix this behavior
  GADemeGA::copy(g);
}







/*
int
GADemeGA::populationSize(int i, unsigned int value){
  if(value < 1){
    GAErr(className(), "populationSize", gaErrBadPopSize);
    value = 1;
  }
  if(i == ALL)
    for(unsigned int ii=0; ii<npop; ii++)
      deme[ii]->size(value);
  else
    deme[i]->size(value);

  if(_mid) {
    for(int j=0; j<_ntid; j++) {
      int psize = deme[j]->size();
      _status = pvm_pkint(&psize, 1, 1);
      _status = pvm_send(_tid[j], MSG_SET_POPULATION_SIZE);
    }
  }

  return value;
}
*/







// To initialize this genetic algorithm, we make sure all of our spawns are
// not evolving then we tell them all to initialize themselves.  Then we wait
// and copy their populations into our own.  Update our stats based upon the
// harvested populations.
void
PVMDemeGA::initialize(unsigned int seed) {
  GARandomSeed(seed);

  if(_mid == 0) return;

#ifdef DEBUG
  cerr << "sending initialize command to slaves...\n";
#endif

  for(int j=0; j<_ntid; j++) {
    _status = pvm_initsend(PvmDataDefault);
    _status = pvm_send(_tid[j], MSG_INITIALIZE);
  }

  collect();

  for(int i=0; i<_ntid; i++) {
    pstats[i].reset(*deme[i]);
    pop->individual(i).copy(deme[i]->best());
  }

  pop->touch();
  stats.reset(*pop);
}


// To evolve the genetic algorithm, we loop through all of our populations and
// tell each process to evolve its population for a certain number of 
// generations.  Then allow the migrator to do its thing.  Each process is
// supposed to keep track of the statistics for its population, so we reap 
// those as well.
void
PVMDemeGA::step() {
  if(_mid == 0) return;

#ifdef DEBUG
  cerr << "sending step command to slaves...\n";
#endif

  for(int j=0; j<_ntid; j++) {
    int nsteps = 10;
    _status = pvm_initsend(PvmDataDefault);
    _status = pvm_pkint(&nsteps, 1, 1);
    _status = pvm_send(_tid[j], MSG_STEP);
  }

#ifdef DEBUG
  cerr << "waiting for slaves to step...\n";
#endif

  int flag = _ntid;
  while(flag > 0) {
    int bufid = pvm_recv(-1, -1);
    if(bufid >= 0) {
      int bytes, msgtag, tid;
      _status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
      switch(msgtag) {
      case MSG_STEP_COMPLETE:
	flag--;

#ifdef DEBUG
	cerr << "  tid " << tid << " has finished step\n";
#endif
	break;

      default:
	cerr << className() << ": step:\n";
	cerr << "  unexpected msgtag: " << msgtag << "\n";
	break;
      }
    }
    else {
      cerr << className() << ": step:\n";
      cerr << "  error from pvm_recv: " << bufid << "\n";
    }
  }

  migrate();

// Now update the statistics and individuals in our local populations.  We copy
// all of the distributed individuals into our own populations then do the
// statistics updates.  Since we copy, we don't force any new evaluations.  If
// you don't need to keep the master up-to-date, then comment out this section
// and just let the slaves run on their own.

  collect();

  for(unsigned int jj=0; jj<npop; jj++)
    pstats[jj].update(*deme[jj]);

  stats.numsel = stats.numcro = stats.nummut = stats.numrep = stats.numeval=0;
  for(unsigned int kk=0; kk<npop; kk++) {
    pop->individual(kk).copy(deme[kk]->best());
    stats.numsel += pstats[kk].numsel;
    stats.numcro += pstats[kk].numcro;
    stats.nummut += pstats[kk].nummut;
    stats.numrep += pstats[kk].numrep;
    stats.numeval += pstats[kk].numeval;
  }

  pop->touch();
  stats.update(*pop);
  for(unsigned int ll=0; ll<npop; ll++)
    stats.numpeval += pstats[ll].numpeval;
}


// The migrator tells the spawned populations when, where, and how many 
// individuals to move around.  This example does a single migration from one
// randomly chosen population to another randomly chosen population.  It
// chooses randomly the number of individuals to migrate.
//   One simple way to get your own migration algorithm into this is to derive
// a genetic algorithm from this class then define your own migrate method.
void
PVMDemeGA::migrate() {
  if(_ntid == 1) return;

  int fromidx, toidx;
  toidx = fromidx = GARandomInt(0, _ntid-1);
  while(fromidx == toidx) { fromidx = GARandomInt(0, _ntid-1); }

  int fromid = _tid[fromidx];
  int toid = _tid[toidx];
  int count = GARandomInt(1, deme[fromidx]->size());
  _status = pvm_initsend(PvmDataDefault);
  _status = pvm_pkint(&toid, 1, 1);
  _status = pvm_pkint(&count, 1, 1);
  _status = pvm_send(fromid, MSG_SEND_MIGRATION);

#ifdef DEBUG
  cerr << "told task " << fromid;
  cerr << " to migrate " << count << " individuals to task " << toid << "\n";
#endif
}


// Gather the population data from each of our distributed populations.
int
PVMDemeGA::collect() {
#ifdef DEBUG
  cerr << "sending request for populations...\n";
#endif

  for(int j=0; j<_ntid; j++) {
    _status = pvm_initsend(PvmDataDefault);
    _status = pvm_send(_tid[j], MSG_SEND_POPULATION);
  }

#ifdef DEBUG
  cerr << "waiting for populations from slaves...\n";
#endif

  int flag = _ntid;
  while(flag > 0) {
    int bufid = pvm_recv(-1, -1);
    if(bufid >= 0) {
      int bytes, msgtag, tid;
      _status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
      int which = tid2idx(tid);

      switch(msgtag) {
      case MSG_INCOMING_POPULATION:
	_status = RecvPopulation(*deme[which]);
	flag--;

#ifdef DEBUG
	cerr << "  received pop from tid " << tid << " (" << which << ")\n";
#endif
	break;

      default:
	cerr << className() << ": collect:\n";
	cerr << "  unexpected msgtag: " << msgtag << "\n";
	break;
      }
    }
    else {
      cerr << className() << ": collect:\n";
      cerr << "  error from pvm_recv: " << bufid << "\n";
    }
  }

  return _status;
}









// Hook up to the pvm and spawn the slave processes.
int
PVMDemeGA::spawn(const char* slavename) {
  _mid = pvm_mytid();
  if(_mid < 0) {
    cerr << "\n" << className() << ": spawn:\n";
    cerr << "  Bad ID for master task. Have you started the PVM?\n";
    return _status = _mid;
  }

  struct pvmhostinfo* hostp;
  _status = pvm_config(&_nhosts, &_narch, &hostp);
  if(_status == PvmSysErr) {
    cerr << "\n" << className() << ": spawn:\n";
    cerr << "  PVM not responding. Have you started the PVM?\n";
    return _status;
  }
  
  _Ntid = npop;
  _tid = new int [_Ntid];		// task IDs for the slaves
  char sn[32];				// PVM is not const-safe...
  strcpy(sn, slavename);
  _ntid = pvm_spawn(sn, (char**)0, 0, "", _Ntid, _tid);
  if(_ntid <= 0) {
    cerr << className() << ": spawn:\n  Error spawning slaves.\n";
    cerr << "  Error codes of failed spawns are:\n";
    for(int i=0; i<_Ntid; i++) {
      cerr << "    slave "; cerr.width(3); cerr<<i<<": "<<_tid[i]<<"\n";
    }
    pvm_exit();
    return _status = -1;
  }
  else if(_ntid < _Ntid) {
    cerr << className() << ": spawn:\n  ";
    cerr << "  Spawned only "<<_ntid<<" of "<<_Ntid<<"\n";
    cerr << "  Error codes of failed spawns are:\n";
    for(int i=0; i<_Ntid; i++) {
      if(_tid[i] < 0) {
	cerr << "    slave "; cerr.width(3); cerr<<i<<": "<<_tid[i]<<"\n";
      }
    }
  }
  else {
    cerr << className() << ": spawn:\n";
    cerr << "  Spawned " << _Ntid << " slave processes...\n";
  }

#ifdef DEBUG
  cerr << "waiting for response from slaves...\n";
#endif

  int flag = _ntid;
  while(flag > 0) {
     int bufid = pvm_recv(-1, -1);
    if(bufid >= 0) {
      int bytes, msgtag, tid;
      _status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
      int which = tid2idx(tid);

      switch(msgtag) {
      case MSG_READY:
#ifdef DEBUG
	cerr << "  slave " << tid << " (" << which << ") is alive\n";
#endif
	flag--;
	break;

      default:
	cerr << className() << ": spawn:\n";
	cerr << "  unexpected msgtag: " << msgtag << "\n";
	break;
      }
    }
    else {
      cerr << className() << ": spawn:\n";
      cerr << "  error from pvm_recv: " << bufid << "\n";
    }
  }

#ifdef DEBUG
  cerr << "slaves appear to be up and running.\n";
#endif

  return _status;
}

// Reap all of our spawned processes and detach ourselves from the PVM.
void
PVMDemeGA::reap() {
  for(int j=0; j<_Ntid; j++)
    if(_tid[j] > 0) pvm_kill(_tid[j]);
  delete [] _tid; 
  _tid = 0;
  _ntid = _Ntid = 0;
  if(_mid > 0) pvm_exit();
  _mid = 0;
}


int
PVMDemeGA::tid2idx(int taskid) const {
  int idx = -1;
  for(int i=0; i<_ntid && idx == -1; i++)
    if(taskid == _tid[i]) idx = i;
  return idx; 
}
