/* ----------------------------------------------------------------------------
  seed.C
  mbwall 16nov98
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#include <ga/ga.h>
#include <ga/std_stream.h>

#define cout STD_COUT
#define cerr STD_CERR
#define ifstream STD_IFSTREAM

float objective(GAGenome &);

int
main(int argc, char *argv[]) {
  cout << "Random Seed Test\n\n";
  cout << "This program does three runs of a genetic algorithm, with the \n";
  cout << "random seed resetting between each run.  Each of the three runs \n";
  cout << "should be identical\n\n";
  cout.flush();

  GAParameterList params;
  GASteadyStateGA::registerDefaultParameters(params);
  params.set(gaNnGenerations, 100);
  params.set(gaNflushFrequency, 5);
  params.set(gaNpMutation, 0.001);
  params.set(gaNpCrossover, 0.8);
  params.parse(argc, argv, gaFalse);

  int i,j;
  char filename[128] = "smiley.txt";
  unsigned int seed=0;

  for(i=1; i<argc; i++){
    if(strcmp("file", argv[i]) == 0 || strcmp("f", argv[i]) == 0){
      if(++i >= argc){
        cerr << argv[0] << ": the file option needs a filename.\n";
        exit(1);
      }
      else{
        sprintf(filename, argv[i]);
        continue;
      }
    }
    else if(strcmp("seed", argv[i]) == 0){
      if(++i >= argc){
        cerr << argv[0] << ": the seed option needs a filename.\n";
        exit(1);
      }
      else {
	seed = atoi(argv[i]);
	continue;
      }
    }
    else {
      cerr << argv[0] << ":  unrecognized arguement: " << argv[i] << "\n\n";
      cerr << "valid arguments include standard GAlib arguments plus:\n";
      cerr << "  f\tfilename from which to read (" << filename << ")\n";
      cerr << "\n";
      exit(1);
    }
  }

  const int n=5;

  cout << n << " random numbers\n";
  GAResetRNG(seed);
  for(i=0; i<n; i++)
    cout << " " << GARandomFloat();
  cout << "\n";

  cout << n << " random numbers\n";
  GAResetRNG(seed);
  for(i=0; i<n; i++)
    cout << " " << GARandomFloat();
  cout << "\n";

  cout << n << " random numbers\n";
  GAResetRNG(seed);
  for(i=0; i<n; i++)
    cout << " " << GARandomFloat();
  cout << "\n";
  cout.flush();

  ifstream inStream(filename);
  if(!inStream){
    cerr << "Cannot open " << filename << " for input.\n";
    exit(1);
  }

  int height, width;
  inStream >> height >> width;

  short **target = new short*[width];
  for(i=0; i<width; i++)
    target[i] = new short[height];

  for(j=0; j<height; j++)
    for(i=0; i<width; i++)
      inStream >> target[i][j];

  inStream.close();

  GA2DBinaryStringGenome genome(width, height, objective, (void *)target);
  GASimpleGA ga(genome);
  ga.parameters(params);

  // first run

  GAResetRNG(seed);
  genome.initialize();
  cout << genome << "\n";
  ga.set(gaNscoreFilename, "bog1.dat");
  ga.evolve();

  genome = ga.statistics().bestIndividual();
  cout << "run 1:  the random seed is: " << GAGetRandomSeed() << "\n";
  for(j=0; j<height; j++){
    for(i=0; i<width; i++)
      cout << (genome.gene(i,j) == 1 ? '*' : ' ') << " ";
    cout << "\n";
  }
  cout << "\n"; cout.flush();

  // second run

  GAResetRNG(seed);
  genome.initialize();
  cout << genome << "\n";
  ga.set(gaNscoreFilename, "bog2.dat");
  ga.evolve();

  genome = ga.statistics().bestIndividual();
  cout << "run 2:  the random seed is: " << GAGetRandomSeed() << "\n";
  for(j=0; j<height; j++){
    for(i=0; i<width; i++)
      cout << (genome.gene(i,j) == 1 ? '*' : ' ') << " ";
    cout << "\n";
  }
  cout << "\n"; cout.flush();

  // third run

  GAResetRNG(seed);
  genome.initialize();
  cout << genome << "\n";
  ga.set(gaNscoreFilename, "bog3.dat");
  ga.evolve();

  genome = ga.statistics().bestIndividual();
  cout << "run 3:  the random seed is: " << GAGetRandomSeed() << "\n";
  for(j=0; j<height; j++){
    for(i=0; i<width; i++)
      cout << (genome.gene(i,j) == 1 ? '*' : ' ') << " ";
    cout << "\n";
  }
  cout << "\n"; cout.flush();

  for(i=0; i<width; i++)
    delete target[i];
  delete [] target;

  return 0;
}
 

float
objective(GAGenome & c) {
  GA2DBinaryStringGenome & genome = (GA2DBinaryStringGenome &)c;
  short **pattern = (short **)c.userData();

  float value=0.0;
  for(int i=0; i<genome.width(); i++)
    for(int j=0; j<genome.height(); j++)
      value += (float)(genome.gene(i,j) == pattern[i][j]);
  return(value);
}
