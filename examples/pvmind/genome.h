/* ----------------------------------------------------------------------------
  genome.h
  mbwall 5dec95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology

 DESCRIPTION:
   This file contains the information needed to evaluate an operate on a 
genome in these examples.
---------------------------------------------------------------------------- */
#ifndef _genome_h_
#define _genome_h_

#include <ga/ga.h>

float GenomeEvaluator(GAGenome&);
void GenomeInitializer(GAGenome&);
void PopulationInitializer(GAPopulation&);
void PopulationEvaluator(GAPopulation&);

int SendGenomeData(GAGenome&, int);
int RecvGenomeData(GAGenome&);
int SendGenomeScore(GAGenome&, int);
int RecvGenomeScore(GAGenome&);
int SendGenomeInitialize(GAGenome&, int);
int RecvGenomeInitialize(GAGenome&);

int SendReady(int);

typedef struct _PVMData {
  int masterid;
  int* tid;		// task ids of all slave processes
  int ntasks;		// number of slave tasks that are running
  int nreq;		// number of tasks that we asked for
} PVMData, *PVMDataPtr, **PVMDataHdl;
 
const int MSG_READY=10;
const int MSG_DONE=100;
const int MSG_GENOME_DATA=50;
const int MSG_GENOME_SCORE=51;
const int MSG_GENOME_INITIALIZE=52;

#endif
