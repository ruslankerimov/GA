/* ----------------------------------------------------------------------------
  genome.C
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   This file contains the information needed to evaluate an operate on a 
genome in these examples.
---------------------------------------------------------------------------- */
#include <pvm3.h>
#include <unistd.h>
#include "genome.h"

int id2idx(int tid, PVMData& data) {
  int idx = -1;
  for(int i=0; i<data.nreq && idx == -1; i++)
    if(tid == data.tid[i]) idx = i;
  return idx; 
}


// When this flag is defined, the initialize and evaluate function dump a 
// string to cerr that tells which host they're running on.  You can then look
// at this info in the pvm log file.
//#define DEBUG


// The initializer and evaluation functions will vary depending on your 
// problem.  Here we have a couple of dummy functions - the initializer just
// calls the default binary string initializer and the objective function 
// just tries to set the string to alternating bits, then sleeps for awhile.
// Both of these include a gethostname that is useful for debugging purposes
// to see which CPUs are doing the work during the parallelization.
void
GenomeInitializer(GAGenome& g) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
#ifdef DEBUG
  char buf[255];
  gethostname(buf, 255);
  cerr << "initializing on " << buf << "\n";
#endif
  GA1DBinaryStringGenome::UniformInitializer(genome);
}


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








// The send/receive routines will change depending on the type of genome that
// you plan to use.  These are written to work with the binary string genome,
// so all they do is send a sequence of bits over the pvm.


// Send/receive the information we need to configure and initialize a genome.
// In this case, send the length the genome should be, but we let the slave do
// the initialization.  If you wanted your slaves to read data from a file, you
// could pass the filename here.
int
SendGenomeInitialize(GAGenome& g, int tid) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
  int status = 0;
  int length = genome.length();
  status = pvm_initsend(PvmDataDefault);
  status = pvm_pkint(&length, 1, 1);
  status = pvm_send(tid, MSG_GENOME_INITIALIZE);
  return status;
}


// Receive the configuration data, configure the genome, then do the 
// initialization.
int 
RecvGenomeInitialize(GAGenome& g) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
  int status = 0;
  int length = 1;
  status = pvm_upkint(&length, 1, 1);
  genome.length(length);
  genome.initialize();
  return status;
}


// Send the bits of the genome to the task that requested them.  First we send
// the number of bits, then we send the bits themselves.  Note that we can 
// handle genomes of varying lengths with this setup.
// Returns negative number (error code) if failure.
int
SendGenomeData(GAGenome& g, int tid) {
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

  status = pvm_initsend(PvmDataDefault);
  status = pvm_pkint(&length, 1, 1);
  status = pvm_pkint(bits, length, 1);
  status = pvm_send(tid, MSG_GENOME_DATA);

  return status;
}


// Receive the bits from the specified task.  Stuff the genome with the data.
// Returns a negative number if there was a transmission failure.
int
RecvGenomeData(GAGenome& g) {
  GA1DBinaryStringGenome& genome = (GA1DBinaryStringGenome&)g;
  int length = 0;
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

  return status;
}


// Send only the score of the genome to the specified task.
int
SendGenomeScore(GAGenome& g, int tid) {
  int status = 0;
  float score = g.score();
  status = pvm_initsend(PvmDataDefault);
  status = pvm_pkfloat(&score, 1, 1);
  status = pvm_send(tid, MSG_GENOME_SCORE);

  return status;
}


// Receive the score and set it on the genome.
int
RecvGenomeScore(GAGenome& g) {
  int status = 0;	
  float score = 0.0;
  status = pvm_upkfloat(&score, 1, 1);     // get the score from process
  g.score(score);			   // set the score on the genome

  return status;
}


int
SendReady(int tid) {
  int status = 0;
  status = pvm_initsend(PvmDataDefault);
  status = pvm_send(tid, MSG_READY);
  return status;
}







// The population initializer invokes the genomes' initializers just like the
// standard population initializer, but here we farm out the genomes to the
// slaves before invoking the initialization.  Farm out the genomes and give
// the slaves the initialize command rather than the evaluate command.
void
PopulationInitializer(GAPopulation& pop) {
  PVMDataPtr data = (PVMDataPtr)pop.userData();
  int* index = new int [data->nreq];
  int done = 0, outstanding = 0, next = 0;
  int bufid, status, bytes, msgtag, tid, who;

  while(!done) {
// If we have a genome that needs to be initialized and one of the slaves is
// available, then ask the slave to configure a genome and send us back the
// configured, initialized genome.
    if(next < pop.size() && (bufid=pvm_nrecv(-1, MSG_READY)) != 0) {
      if(bufid > 0) {
	status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
	status = SendGenomeInitialize(pop.individual(next), tid);
	if(status >= 0) {
	  if((who = id2idx(tid, *data)) >= 0) {
	    index[who] = next; next++;
	    outstanding++;
	  }
	  else {
	    cerr << "PopInit: bogus tid mapping: " << tid << "\n";
	  }
	}
	else {
	  cerr << "PopInit: error sending initialize command to: " << tid;
	  cerr << "  genome " << next << " will be inited by next slave\n";
	  cerr << "  error code is: " << status << "\n";
	}
      }
      else {
	cerr << "PopInit: error from pvm_nrecv: " << bufid << "\n";
      }
    }

// If we have requests for initialization outstanding and a slave has posted
// a message stating that it will provide genome data, then get the data from
// the slave and stuff it into the appropriate genome in the population.
    if(outstanding > 0 && (bufid=pvm_nrecv(-1, MSG_GENOME_DATA)) != 0) {
      if(bufid > 0) {
	status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
	if((who = id2idx(tid, *data)) >= 0) {
	  if(index[who] >= 0) {
	    status = RecvGenomeData(pop.individual(index[who]));
	    if(status >= 0) {
	      index[who] = -1;
	      outstanding--;
	    }
	    else {
	      cerr << "PopInit: error receiving data from: " << tid;
	      cerr << "  error code is: " << status << "\n";
	    }
	  }
	  else {
	    cerr << "PopInit: index conflict from tid " << tid << "\n";
	  }
	}
	else {
	  cerr << "PopInit: bogus tid mapping: " << tid << "\n";
	}
      }
      else {
	cerr << "PopInit: error from pvm_nrecv: " << bufid << "\n";
      }
    }

    if(next == pop.size() && outstanding == 0) done = 1;
    if(next > pop.size()) {
      cerr << "bogus value for next: " << next;
      cerr << "  popsize is: " << pop.size() << "\n";
    }
  }

  delete [] index;
}


//   This population evaluator is the administrator for the parallelization.
// It looks around to see when slaves are available to evaluate a genome.  As
// soon as a slave is available and a genome needs to be evaluated, this 
// routine sends it off.  When a slave is finished, it posts a message to 
// say so and this routine gets the message and grabs the results from the 
// slave that posted the message.
//   An index of -1 means that the slave has no assignment.  The first int in 
// the stream of stuff is always the ID of the slave (0-nslaves) that is 
// sending the information.  After that it is either nothing (the slave just 
// reported that it is ready for another genome) or it is a float (the score 
// of the genome that was assigned to the slave).
void 
PopulationEvaluator(GAPopulation& pop) {
  PVMDataPtr data = (PVMDataPtr)pop.userData();
  int* index = new int [data->nreq];
  int done = 0, outstanding = 0, next = 0;
  int bufid, status, bytes, msgtag, tid, who;

  while(!done) {
// If we have a genome that needs to be evaluated and one of the slaves is
// ready to evaluate it, send the genome to the slave.
    if(next < pop.size() && (bufid=pvm_nrecv(-1, MSG_READY)) != 0) {
      if(bufid > 0) {
	pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
	status = SendGenomeData(pop.individual(next), tid);
	if(status >= 0) {
	  if((who = id2idx(tid, *data)) >= 0) {
	    index[who] = next; next++;
	    outstanding++;
	  }
	  else {
	    cerr << "PopEval: bogus tid mapping: " << tid << "\n";
	  }
	}
	else {
	  cerr << "PopEval: error sending data to: " << tid;
	  cerr << "  error code is: " << status << "\n";
	}
      }
      else {
	cerr << "PopEval: error from pvm_nrecv: " << bufid << "\n";
      }
    }

// If we have any genomes waiting for their evaluation and any slaves have 
// posted a message stating that they have a finished score ready for us, get
// the score from the slave and stuff it into the appropriate genome.
    if(outstanding > 0 && (bufid=pvm_nrecv(-1, MSG_GENOME_SCORE)) != 0) {
      if(bufid > 0) {
	pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
	if((who = id2idx(tid, *data)) >= 0) {
	  if(index[who] >= 0) {
	    status = RecvGenomeScore(pop.individual(index[who]));
	    if(status >= 0) {
	      index[who] = -1;
	      outstanding--;
	    }
	    else {
	      cerr << "PopEval: error receiving score from: " << tid;
	      cerr << "  error code is: " << status << "\n";
	    }
	  }
	  else {
	    cerr << "PopEval: index conflict from tid " << tid << "\n";
	  }
	}
	else {
	  cerr << "PopEval: bogus tid mapping: " << tid << "\n";
	}
      }
      else {
	cerr << "PopEval: error from pvm_nrecv: " << bufid << "\n";
      }
    }

    if(next == pop.size() && outstanding == 0) done = 1;
    if(next > pop.size()) {
      cerr << "bogus value for next: " << next;
      cerr << "  popsize is: " << pop.size() << "\n";
    }
  }

  delete [] index;
}
