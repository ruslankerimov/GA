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

#include <ga/gabase.h>
#include <ga/binstr1.h>

#define GENOME_LENGTH 64

float GenomeEvaluator(GAGenome&);
int RecvMigration(GAGeneticAlgorithm& ga);
int SendMigration(int toid, GAGeneticAlgorithm& ga, int count);
int RecvPopulation(GAPopulation&);
int SendPopulation(int toid, const GAPopulation&);
int RecvStatistics(GAStatistics&);
int SendStatistics(int toid, const GAStatistics&);

const int MSG_DONE                =1;
const int MSG_INITIALIZE          =2;
const int MSG_STEP                =3;
const int MSG_RECEIVE_MIGRATION   =4;
const int MSG_SEND_MIGRATION      =5;
const int MSG_SEND_POPULATION     =6;
const int MSG_SEND_STATISTICS     =7;
const int MSG_INCOMING_MIGRATION  =8;
const int MSG_INCOMING_POPULATION =9;
const int MSG_INCOMING_STATISTICS =10;
const int MSG_STEP_COMPLETE       =11;
const int MSG_SET_POPULATION_SIZE =12;
const int MSG_READY               =100;

#endif
