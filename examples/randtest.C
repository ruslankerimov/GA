/* ----------------------------------------------------------------------------
  randtest.C
  mbwall 9may98
  Copyright (c) 1998 Matthew Wall, all rights reserved

 DESCRIPTION:
   Program to test the random number generator in GAlib.
---------------------------------------------------------------------------- */
#include <string.h>
#include <math.h>
#include <ga/garandom.h>
#include <ga/std_stream.h>

#define cout STD_COUT
#define cerr STD_CERR

static const int HIST_SIZE=501;
static const int NFLIPS=10000;
static const long int NUM_CHECKS=1000000L;
static const int RCHI=100;

int
main(int argc, char **argv)
{
  cerr << "This program checks the random number generators in GAlib.\n";
  cerr << "These are not rigorous statistical tests, but they should\n";
  cerr << "give you a quick indication of whether or not the library is\n";
  cerr << "working properly.  Command-line options include:\n\n";
  cerr << "  [+-]chi    - do the chi square test (default)\n";
  cerr << "  [+-]hist   - do the histogram tests.  Histograms should be\n";
  cerr << "               nearlyflat, i.e. each number has an equal chance\n";
  cerr << "               of beingchosen.\n";
  cerr << "  [+-]bnds   - do the bounds tests.  All numbers generated\n";
  cerr << "               should be within the limits specified in the\n";
  cerr << "               test.\n";
  cerr << "  [+-]means  - do the means tests.  A few thousand invocations\n";
  cerr << "               of each random number function are made, and the\n";
  cerr << "               averages of these calls are displayed with the\n";
  cerr << "               number that should have been found.\n";
  cerr << "      seed n - specify the seed number to use for the RNG.  You\n";
  cerr << "               should get the same results every time if you\n";
  cerr << "               specify the same seed every time.  A seed of 0\n";
  cerr << "               tells the lib to pick its own seed based upon the\n";
  cerr << "               current time.\n";
  cerr << "\n"; 
  cerr.flush();

  int dohist = 0;
  int dobnds = 0;
  int domeans = 0;
  int dochisq = 1;
  int seed = 0;
  int i;

  for(i=1; i<argc; i++){
    if(strcmp("seed", argv[i]) == 0){
      if(++i >= argc) {
	cerr << "You must enter a number when specifying a random seed.\n";
	exit(1);
      }
      else {
	seed = atoi(argv[i]);
      }
    }
    else if(strcmp("+chi", argv[i]) == 0){
      dochisq = 1;
    }
    else if(strcmp("-chi", argv[i]) == 0){
     dochisq  = 0;
    }
    else if(strcmp("+hist", argv[i]) == 0){
      dohist = 1;
    }
    else if(strcmp("-hist", argv[i]) == 0){
      dohist = 0;
    }
    else if(strcmp("+bnds", argv[i]) == 0){
      dobnds = 1;
    }
    else if(strcmp("-bnds", argv[i]) == 0){
      dobnds = 0;
    }
    else if(strcmp("+means", argv[i]) == 0){
      domeans = 1;
    }
    else if(strcmp("-means", argv[i]) == 0){
      domeans = 0;
    }
  }

// Tell us which RNG we're using...

  cout << argv[0] << ": Random Number Test\n";
  cout << "Using the " << GAGetRNG() << " random number generator (RNG).\n";

  // initialize the RNG by calling the seed routine with our seed

  if(seed) {
    cout << "Using specified random seed " << seed << "\n";
  } else {
    cout << "No random seed specified\n";
  }

  GARandomSeed(seed);

  unsigned int libseed = GAGetRandomSeed();
  cout << "Library thinks the random seed is " << libseed << "\n";


  // the basic chi square test for randomness of a RNG
  // must do it more than once since it might be wrong about one in ten
  // times.  The error is r*t/N - N and should be within 2*sqrt(r) of r.

  if(dochisq) {
    cout << "\n";
    cerr << "chi-square test...\n";
    cout << "running the chi-square test for randomness of the RNG...\n";
    cout << " (there will be some failures of the chi-square test)\n";

    int ii;
    double elimit = 2*sqrt((double)RCHI);
    double chisq = 0.0;
    long int NCHI=1000;	// NCHI should be >= 10 * RCHI
    long int f[RCHI];

    cerr << "  integer test (" << NCHI << ")...\n";
    cout << "  integer test (" << NCHI << "):  chi-squared should be within ";
    cout << elimit << " of " << RCHI << "\n"; cout.flush();
    for(ii=0; ii<10; ii++) {
      memset(f, 0, RCHI * sizeof(long int));
      for(i=0; i<NCHI; i++)
	f[GARandomInt(0,RCHI-1)]++;
      for(i=0; i<RCHI; i++)
	chisq += ((double)f[i] - (double)NCHI / (double)RCHI) * 
	  ((double)f[i] - (double)NCHI / (double)RCHI);
      chisq *= (double)RCHI / (double)NCHI;
      cout << "    run #" << ii << "\t" << chisq << "\t";
      if(fabs(chisq - RCHI) > elimit) cout << "***failed***";
      cout << "\n";
    }
    cout.flush();

    NCHI = 10000;
    cerr << "  integer test (" << NCHI << ")...\n";
    cout << "  integer test (" << NCHI << "):  chi-squared should be within ";
    cout << elimit << " of " << RCHI << "\n"; cout.flush();
    for(ii=0; ii<10; ii++) {
      memset(f, 0, RCHI * sizeof(long int));
      long int i;
      long int t;
      for(i=0; i<NCHI; i++)
	f[GARandomInt(0,RCHI-1)]++;
      for(i=0, t=0; i<RCHI; i++)
	t += f[i] * f[i];
      chisq = (double)RCHI*(double)t/(double)NCHI - (double)NCHI;
      cout << "    run #" << ii << "\t" << chisq << "\t";
      if(fabs(chisq - RCHI) > elimit) cout << "***failed***";
      cout << "\n";
    }
    cout.flush();

    NCHI = 10000;
    cerr << "  integer test (" << NCHI << ")...\n";
    cout << "  integer test (" << NCHI << "):  chi-squared should be within ";
    cout << elimit << " of " << RCHI << "\n"; cout.flush();
    for(ii=0; ii<10; ii++) {
      memset(f, 0, RCHI * sizeof(long int));
      long int i;
      long int t;
      for(i=0; i<NCHI; i++)
	f[GARandomInt(0,RCHI-1)]++;
      for(i=0, t=0; i<RCHI; i++)
	t += f[i] * f[i];
      chisq = (double)RCHI*(double)t/(double)NCHI - (double)NCHI;
      cout << "    run #" << ii << "\t" << chisq << "\t";
      if(fabs(chisq - RCHI) > elimit) cout << "***failed***";
      cout << "\n";
    }
    cout.flush();

    NCHI = 100000;
    cerr << "  integer test (" << NCHI << ")...\n";
    cout << "  integer test (" << NCHI << "):  chi-squared should be within ";
    cout << elimit << " of " << RCHI << "\n"; cout.flush();
    for(ii=0; ii<10; ii++) {
      memset(f, 0, RCHI * sizeof(long int));
      long int i;
      long int t;
      for(i=0; i<NCHI; i++)
	f[GARandomInt(0,RCHI-1)]++;
      for(i=0, t=0; i<RCHI; i++)
	t += f[i] * f[i];
      chisq = (double)RCHI*(double)t/(double)NCHI - (double)NCHI;
      cout << "    run #" << ii << "\t" << chisq << "\t";
      if(fabs(chisq - RCHI) > elimit) cout << "***failed***";
      cout << "\n";
    }
    cout.flush();
  }

  // histograms of the tests

  if(dohist) {
    cout << "\n";
    cerr << "histograms...\n";
    cout << "plotting histograms of calls to random number functions...\n";

    int i, j, histogram[HIST_SIZE];

    memset(histogram, 0, HIST_SIZE*sizeof(int));
    cout << "\n100 random integers in [0, 1] with GARandomInt():\n";
    for(i=0; i<100; i++)
      histogram[GARandomInt()]++;
    for(i=0; i<=1; i++){
      cout << i << "\t";
      for(j=0; j<histogram[i]; j++)
	cout << ".";
      cout << "\n";
    }
    cout.flush();
    
    memset(histogram, 0, HIST_SIZE*sizeof(int));
    cout << "\n100 random integers in [0, 1] with GARandomInt(0,1):\n";
    for(i=0; i<100; i++)
      histogram[GARandomInt(0,1)]++;
    for(i=0; i<=1; i++){
      cout << i << "\t";
      for(j=0; j<histogram[i]; j++)
	cout << ".";
      cout << "\n";
    }
    cout.flush();
    
    memset(histogram, 0, HIST_SIZE*sizeof(int));
    cout<<"\n10000 random integers in [-20, -10] with GARandomInt(-20,-10):\n";
    for(i=0; i<10000; i++)
      histogram[GARandomInt(-20,-10)+20]++;
    for(i=0; i<=10; i++){
      cout << (i-20) << "\t";
      for(j=0; j<histogram[i]; j++)
	if(j%20 == 0) cout << ".";
      cout << "\n";
    }
    cout.flush();
    
    memset(histogram, 0, HIST_SIZE*sizeof(int));
    cout << "\n300 random integers in [0, 5] with GARandomInt(0,5):\n";
    for(i=0; i<300; i++)
      histogram[GARandomInt(0,5)]++;
    for(i=0; i<=5; i++){
      cout << i << "\t";
      for(j=0; j<histogram[i]; j++)
	cout << ".";
      cout << "\n";
    }
    cout.flush();
    
    memset(histogram, 0, HIST_SIZE*sizeof(int));
    cout << "\n10000 random integers in [0, 100] with GARandomInt(0,100):\n";
    for(i=0; i<10000; i++)
      histogram[GARandomInt(0,100)]++;
    for(i=0; i<=100; i++){
      cout << i << "\t";
      for(j=0; j<histogram[i]; j++)
	if(j%2 == 0) cout << ".";
      cout << "\n";
    }
    cout.flush();
    
    memset(histogram, 0, HIST_SIZE*sizeof(int));
    cout<<"\n10000 random integers in [-10, 100] with GARandomInt(-10,100):\n";
    for(i=0; i<10000; i++)
      histogram[GARandomInt(-10,100)+10]++;
    for(i=0; i<=110; i++){
      cout << (i-10) << "\t";
      for(j=0; j<histogram[i]; j++)
	if(j%2 == 0) cout << ".";
      cout << "\n";
    }
    cout.flush();
  }

// calculate means on various types

  if(domeans) {
    cout << "\n";
    cerr << "means test...\n";
    cout << "check for averages of repeated biased coin tosses...\n";

    int i;
    double counter, mean;

    counter = 0;
    cout << NFLIPS << " calls to GARandomBit()\t\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomBit();
    cout << counter << " hits (should be about " << 0.5*NFLIPS << ")\n";
    cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomInt()\t\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomInt();
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 0.5)\n"; cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomInt(0,5)\t\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomInt(0,5);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 2.5)\n"; cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomFloat()\t\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomFloat();
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 0.5)\n"; cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomFloat(0,5)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomFloat(0,5);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 2.5)\n"; cout.flush();
        
    counter = 0;
    cout << NFLIPS << " calls to GARandomFloat(-10,-5)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomFloat(-10,-5);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about -7.5)\n"; cout.flush();
        
    counter = 0;
    cout << NFLIPS << " calls to GARandomFloat(-10,10)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomFloat(-10,10);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 0.0)\n"; cout.flush();

    counter = 0;
    cout << NFLIPS << " calls to GARandomDouble()\t\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomDouble();
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 0.5)\n"; cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomDouble(0,2)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomDouble(0,2);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 1.0)\n"; cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomDouble(0,5)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomDouble(0,5);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 2.5)\n"; cout.flush();
    
    counter = 0;
    cout << NFLIPS << " calls to GARandomDouble(-10,-5)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomDouble(-10,-5);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about -7.5)\n"; cout.flush();
        
    counter = 0;
    cout << NFLIPS << " calls to GARandomDouble(-10,10)\t";
    for(i=0; i<NFLIPS; i++)
      counter += GARandomDouble(-10,10);
    mean = counter/(double)NFLIPS;
    cout << mean << " (should be about 0.0)\n"; cout.flush();

    // check means of repeated coin tosses
    
    cout << "\n";
    float ave;
    cout << "[0, 1] with GARandomFloat()...\t\t";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat();
    cout << ave/50 << " ";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat();
    cout << ave/50 << " ";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat();
    cout << ave/50 << "\n";
    cout.flush();
    
    cout << "[0, 1] with GARandomFloat(0,1)...\t";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat(0,1);
    cout << ave/50 << " ";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat(0,1);
    cout << ave/50 << " ";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat(0,1);
    cout << ave/50 << "\n";
    cout.flush();
    
    cout << "[0, 100] with GARandomFloat(0,100)...\t";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat(0,100);
    cout << ave/50 << " ";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat(0,100);
    cout << ave/50 << " ";
    ave = 0;
    for(i=0; i<50; i++)
      ave += GARandomFloat(0,100);
    cout << ave/50 << "\n";
    cout.flush();
    
    double aved;
    cout << "[0, 1] with GARandomDouble()...\t\t";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble();
    cout << aved/50 << " ";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble();
    cout << aved/50 << " ";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble();
    cout << aved/50 << "\n";
    cout.flush();
    
    cout << "[0, 1] with GARandomDouble(0,1)...\t";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble(0,1);
    cout << aved/50 << " ";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble(0,1);
    cout << aved/50 << " ";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble(0,1);
    cout << aved/50 << "\n";
    cout.flush();
    
    cout << "[0, 100] with GARandomDouble(0,100)...\t";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble(0,100);
    cout << aved/50 << " ";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble(0,100);
    cout << aved/50 << " ";
    aved = 0;
    for(i=0; i<50; i++)
      aved += GARandomDouble(0,100);
    cout << aved/50 << "\n";
    cout.flush();
    
    cout << "\n";
    
    int count;
    cout << "1% coin flip...\t\t";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.01);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.01);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.01);
    counter = (double)count / (double)i;
    cout << counter << "\n";
    cout.flush();
    
    cout << "25% coin flip...\t";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.25);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.25);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.25);
    counter = (double)count / (double)i;
    cout << counter << "\n";
    cout.flush();

    cout << "50% coin flip...\t";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.5);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.5);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.5);
    counter = (double)count / (double)i;
    cout << counter << "\n";
    cout.flush();
    
    cout << "75% coin flip...\t";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.75);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.75);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.75);
    counter = (double)count / (double)i;
    cout << counter << "\n";
    cout.flush();
    
    cout << "99% coin flip...\t";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.99);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.99);
    counter = (double)count / (double)i;
    cout << counter << " ";
    count = 0;
    for(i=0; i<NFLIPS; i++)
      count += GAFlipCoin(0.99);
    counter = (double)count / (double)i;
    cout << counter << "\n";
    cout.flush();
    
    long lcount;
    cout << "\ncalls to GARandomBit (these should yield 50%)...\n";
    lcount = 0;
    for(i=0; i<NFLIPS; i++)
      lcount += GARandomBit();
    counter = (double)lcount / (double)i;
    cout << counter << " ";
    lcount = 0;
    for(i=0; i<NFLIPS; i++)
      lcount += GARandomBit();
    counter = (double)lcount / (double)i;
    cout << counter << " ";
    lcount = 0;
    for(i=0; i<NFLIPS; i++)
      lcount += GARandomBit();
    counter = (double)lcount / (double)i;
    cout << counter << "\n";
    cout.flush();
  }

// check for values outside of bounds

  if(dobnds) {
    cout << "\n";
    cerr << "bounds test...\n";
    cout << "check for out of bounds...\n";

    long int ii, err;

    int vali;

    cerr << "  " << NUM_CHECKS << "  GARandomInt(-10,5)...\n";
    cout << "GARandomInt(-10,5) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      vali = GARandomInt(-10,5);
      if(vali < -10 || vali > 5){ err += 1; }
    }
    if(err) cout << " " << err << " values out of bounds.\n";
    else cout << "ok\n";
    cout.flush();
    
    cerr << "  " << NUM_CHECKS << "  GARandomInt(0,3)...\n";
    cout << "GARandomInt(0,3) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      vali = GARandomInt(0,3);
      if(vali < 0 || vali > 3){ err += 1; }
    }
    if(err) cout << " " << err << " values out of bounds.\n";
    else cout << "ok\n";
    cout.flush();
    
    cerr << "  " << NUM_CHECKS << "  GARandomInt(200,255)...\n";
    cout << "GARandomInt(200,255) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      vali = GARandomInt(200,255);
      if(vali < 200 || vali > 255){ err += 1; }
    }
    if(err) cout << " " << err << " values out of bounds.\n";
    else cout << "ok\n";
    cout.flush();

    float valf, lastf=0.0;

    cerr << "  " << NUM_CHECKS << "  GARandomFloat(-10,5)...\n";
    cout << "GARandomFloat(-10,5) ..."; cout.flush();
    for(ii=0; ii<NUM_CHECKS; ii++){
      valf = GARandomFloat(-10,5);
      if(valf < -10 || valf > 5){ err += 1; lastf = valf; }
    }
    if(err) cout << " " << err << " values out of bounds (" << lastf << ").\n";
    else cout << "ok\n";
    cout.flush();
    
    cerr << "  " << NUM_CHECKS << "  GARandomFloat(0,3)...\n";
    cout << "GARandomFloat(0,3) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      valf = GARandomFloat(0,3);
      if(valf < 0 || valf > 3){ err += 1; lastf = valf; }
    }
    if(err) cout << " " << err << " values out of bounds (" << lastf << ").\n";
    else cout << "ok\n";
    cout.flush();
    
    cerr << "  " << NUM_CHECKS << "  GARandomFloat(200,255)...\n";
    cout << "GARandomFloat(200,255) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      valf = GARandomFloat(200,255);
      if(valf < 200 || valf > 255){ err += 1; lastf = valf; }
    }
    if(err) cout << " " << err << " values out of bounds (" << lastf << ").\n";
    else cout << "ok\n";
    cout.flush();

    double vald, lastd=0.0;

    cerr << "  " << NUM_CHECKS << "  GARandomDouble(-10,5)...\n";
    cout << "GARandomDouble(-10,5) ..."; cout.flush();
    for(ii=0; ii<NUM_CHECKS; ii++){
      vald = GARandomDouble(-10,5);
      if(vald < -10 || vald > 5){ err += 1; lastd = vald; }
    }
    if(err) cout << " " << err << " values out of bounds (" << lastd << ").\n";
    else cout << "ok\n";
    cout.flush();
    
    cerr << "  " << NUM_CHECKS << "  GARandomDouble(0,3)...\n";
    cout << "GARandomDouble(0,3) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      vald = GARandomDouble(0,3);
      if(vald < 0 || vald > 3){ err += 1; lastd = vald; }
    }
    if(err) cout << " " << err << " values out of bounds (" << lastd << ").\n";
    else cout << "ok\n";
    cout.flush();
    
    cerr << "  " << NUM_CHECKS << "  GARandomDouble(200,255)...\n";
    cout << "GARandomDouble(200,255) ..."; cout.flush();
    err = 0;
    for(ii=0; ii<NUM_CHECKS; ii++){
      vald = GARandomDouble(200,255);
      if(vald < 200 || vald > 255){ err += 1; lastd = vald; }
    }
    if(err) cout << " " << err << " values out of bounds (" << lastd << ").\n";
    else cout << "ok\n";
    cout.flush();
  }

  return 0;
}
