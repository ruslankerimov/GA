/* ----------------------------------------------------------------------------
  slave.C
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   Each slave evolves its own population.  Between steps it checks to see if
any messages have appeared for it.  After each step it sends a message that 
says that it has completed a generation.  A slave can receive two kinds of 
messages, one to send individuals (and where to send them), and another to
receive them (and from where to receive them).  The master controls the
migrations by telling the slaves when and where to migrate individuals.  
   To control the actual migration algorithm, you can modify the section of
code that merges the received individuals into the population.  You can also
determine which individuals will be selected to migrate to another population.
---------------------------------------------------------------------------- */
#include <pvm3.h>
#include <ga/GASimpleGA.h>
#include "genome.h"

int
main(int, char** argv) {
  int status = 0;
  int mytid = pvm_mytid();
  int masterid = pvm_parent();
  if(mytid < 0 || masterid < 0) {
    cerr << "\n" << argv[0] << ": Couldn't get slave/master IDs.  Aborting.\n";
    exit(1);
  }

  GA1DBinaryStringGenome genome(GENOME_LENGTH,GenomeEvaluator);
  GASteadyStateGA ga(genome);

  status = pvm_initsend(PvmDataDefault);
  status = pvm_send(masterid, MSG_READY);

  int done = 0;
  while(!done){
    int bufid = pvm_recv(-1, -1);
    int ival;
    if(bufid >= 0) {
      int bytes, msgtag, tid;
      status = pvm_bufinfo(bufid, &bytes, &msgtag, &tid);
      switch(msgtag) {
      case MSG_DONE:
	done = 1;
	break;

      case MSG_SET_POPULATION_SIZE:
	ival = gaDefPopSize;
	status = pvm_upkint(&ival, 1, 1);
	ga.populationSize(ival);
	break;

      case MSG_INITIALIZE:
	ga.initialize();
	break;

      case MSG_STEP:
	ival = 0;
	status = pvm_upkint(&ival, 1, 1);
	for(int i=0; i<ival; i++)
	  ga.step();
	ival = ga.generation();
	  status = pvm_initsend(PvmDataDefault);
	status = pvm_pkint(&ival, 1, 1);
	status = pvm_send(masterid, MSG_STEP_COMPLETE);
	break;

      case MSG_INCOMING_MIGRATION:
	RecvMigration(ga);
	break;

      case MSG_SEND_MIGRATION:
	{
	  int toid = 0, count = 0;
	  status = pvm_upkint(&toid, 1, 1);
	  status = pvm_upkint(&count, 1, 1);
	  SendMigration(toid, ga, count);
	}
	break;

      case MSG_SEND_POPULATION:
	SendPopulation(masterid, ga.population());
	break;

      case MSG_SEND_STATISTICS:
	SendStatistics(masterid, ga.statistics());
	break;

      default:
	cerr << argv[0] << ": unknown msgtag: " << msgtag << "\n";
	break;
      }
    }
    else {
      cerr << argv[0] << ": error from pvm_recv: " << bufid << "\n";
    }
  }

  pvm_exit();
  return 0;
}
