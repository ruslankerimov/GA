/* ----------------------------------------------------------------------------
  gaview.C
  mbwall 10apr96
  Copyright (c) 1996  Massachusetts Institute of Technology

  Visual genetic algorithm example code.  The program pops up a single window
with a set of buttons that control the evolution.  You can view an entire
population or a single individual.  The program reads standard GAlib settings
files and understands the usual GAlib command-line options.
  Apologies for the mix of C and C++ coding styles here, but you get the idea.
  By the way, this thing looks WAY better when you plot the functions in 3D
(using an OpenGL widget rather than the drawing area widget, for example).
  You can compile this using either motif widgets or the athena widgets, 
depending on which you have on your system.
---------------------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
#include <ga/ga.h>
#include <ga/std_stream.h>

#define cout STD_COUT
#define cerr STD_CERR
#define endl STD_ENDL

#define INSTANTIATE_REAL_GENOME
#include <ga/GARealGenome.h>

// comment this line if you don't have MOTIF on your system
//#define USE_MOTIF

#ifdef USE_MOTIF
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Protocols.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#else
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#endif


#define APP_CLASS     "GAView"
#define SETTINGS_FILE "settings.txt"
#define MAX_POPS      5

typedef struct _AppData {
  Pixel bestcolor;
  Pixel popcolor[MAX_POPS];

  int geninc;		// how many generations in each step
  XtWorkProcId procid;
  XtAppContext appc;
  Widget canvas, counter;
  GC bestgc, dotgc[MAX_POPS];

  GAGenome* genome;
  GAGeneticAlgorithm* ga;
  int whichGA;		// which genetic algorithm to use
  int whichGenome;	// which genome to use
  int whichFunction;	// which function to use
} AppData, *AppDataPtr, **AppDataHdl;

static AppData theAppData;	// global data for the program
int done = 0;		        // is the program finished yet?



#include "bitmaps/gaview.xbm"

#ifdef USE_MOTIF

#include "bitmaps/rew.xbm"
#include "bitmaps/stop.xbm"
#include "bitmaps/fwds.xbm"
#include "bitmaps/ffst.xbm"
#include "bitmaps/ffwd.xbm"

typedef struct _BitmapInfo {
  int width, height;
  unsigned char *bits;
} BitmapInfo;

static BitmapInfo bm[] = {
  {stop_width, stop_height, (unsigned char *)stop_bits},
  {rew_width, rew_height, (unsigned char *)rew_bits},
  {fwds_width, fwds_height, (unsigned char *)fwds_bits},
  {ffst_width, ffst_height, (unsigned char *)ffst_bits},
  {ffwd_width, ffwd_height, (unsigned char *)ffwd_bits}
};

enum {
  bmStop,
  bmRewind,
  bmForwardStop,
  bmFastForwardStop,
  bmFastForward,
  nBitmaps
};
#endif

Widget ConstructWidgets(Widget);
void UpdateCounter(GAGeneticAlgorithm*);
static Boolean Evolve(int);
void QuitCB(Widget, XtPointer, XtPointer);
void InitCB(Widget, XtPointer, XtPointer);
void DrawCB(Widget, XtPointer, XtPointer);
void ResetCB(Widget, XtPointer, XtPointer);
void StopCB(Widget, XtPointer, XtPointer);
void StepCB(Widget, XtPointer, XtPointer);
void EvolveSomeCB(Widget, XtPointer, XtPointer);
void EvolveCB(Widget, XtPointer, XtPointer);
void DumpStatsCB(Widget, XtPointer, XtPointer);
void DumpParamsCB(Widget, XtPointer, XtPointer);
void DumpScoreCB(Widget, XtPointer, XtPointer);
void DrawPopulation(Widget, const GAPopulation&, GC, GC);

static char *fallbacks[] = {
  "*shell.title: gaview",
  "*background:	thistle4",
  "*canvas.background:	black",
  "*leftOffset:	3",
  "*rightOffset:	3",
  "*topOffset:	3",
  "*bottomOffset:	3",
  "*canvas.width:	400",
  "*canvas.height:	300",
  "*bestColor: green",
  "*populationColor1: yellow",
  "*populationColor2: blue",
  "*populationColor3: red",
  "*populationColor4: purple",
  "*populationColor5: cyan",
  "*ga: 2",
  "*genome: 0",
  "*function: 3",
  "*generationsPerStep: 10",

// motif-specific fallbacks

  "*fontList:	-*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
  "*score.labelString:	score",
  "*stats.labelString:	stats",
  "*params.labelString:	params",

// athena-specific fallbacks

  "*font:		-*-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
  "*rewind.label:	reset",
  "*stop.label:		stop",
  "*step.label:		one",
  "*some.label:		some",
  "*evolve.label:	all",
  "*score.label:	score",
  "*stats.label:	stats",
  "*params.label:	params",

  (char *)NULL
};




static XtResource resources[] = {
#define Offset(field) (XtOffset(AppDataPtr, field))
  {"bestColor", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(bestcolor), XtRString, (XtPointer)XtDefaultForeground},
  {"populationColor1", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(popcolor[0]), XtRString, (XtPointer)XtDefaultForeground},
  {"populationColor2", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(popcolor[1]), XtRString, (XtPointer)XtDefaultForeground},
  {"populationColor3", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(popcolor[2]), XtRString, (XtPointer)XtDefaultForeground},
  {"populationColor4", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(popcolor[3]), XtRString, (XtPointer)XtDefaultForeground},
  {"populationColor5", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(popcolor[4]), XtRString, (XtPointer)XtDefaultForeground},
  {"ga", "GA", XtRInt, sizeof(int),
     Offset(whichGA), XtRImmediate, (XtPointer)2},
  {"genome", "Genome", XtRInt, sizeof(int),
     Offset(whichGenome), XtRImmediate, (XtPointer)0},
  {"function", "Function", XtRInt, sizeof(int),
     Offset(whichFunction), XtRImmediate, (XtPointer)3},
  {"generationsPerStep", "GenerationsPerStep", XtRInt, sizeof(int),
     Offset(geninc), XtRImmediate, (XtPointer)10}
#undef Offset
};

static XrmOptionDescRec options[] = {
  {"ga",       "ga",                 XrmoptionSepArg, 0},
  {"genome",   "genome",             XrmoptionSepArg, 0},
  {"function", "function",           XrmoptionSepArg, 0},
  {"geninc",   "generationsPerStep", XrmoptionSepArg, 0}
};





float RealObjective(GAGenome&);
float Bin2DecObjective(GAGenome&);

typedef float (*Function)(float, float);
float Function1(float x, float y);
float Function2(float x, float y);
float Function3(float x, float y);
float Function4(float x, float y);
Function obj[] = { Function1, Function2, Function3, Function4 };
float minx[] = {-6, -60, -500, -10 };
float maxx[] = { 6,  60,  500, 10 };
float ai[25],bi[25];








int
main(int argc, char** argv) {
  cout << "Graphic genetic algorithm demonstration program.\n\n";
  cout << "This program understands the standard GAlib and Xt\n";
  cout << "arguments plus the following:\n\n";
  cout << "  function  which function to solve\n";
  cout << "        0   loaf of bread with 4 smooth humps\n";
  cout << "        1   Shekel's foxholes from DeJong\n";
  cout << "        2   Schwefel's nasty function\n";
  cout << "        3   concentric rings (ripple in a pond) (default)\n";
  cout << "  ga        specify which genetic algorithm to use\n";
  cout << "        0   incremental genetic algorithm\n";
  cout << "        1   simple genetic algorithm\n";
  cout << "        2   steady-state genetic algorithm (default)\n";
  cout << "        3   deme genetic algorithm\n";
  cout << "  genome    specify which genome to use\n";
  cout << "        0   real number genome (default)\n";
  cout << "        1   binary-to-decimal genome\n";
  cout << "\n";
  cout << endl;

// make the application widget, grab resource, and parse command line

  Widget toplevel = XtAppInitialize(&theAppData.appc, APP_CLASS, 
				    options, XtNumber(options),
				    &argc, argv, fallbacks, (ArgList)NULL, 0);
  XtGetApplicationResources(toplevel, (XtPointer) &theAppData,
                            resources, XtNumber(resources), NULL, 0);

// do some setup for one of the functions

  for (int j=0; j<25; j++) {
    ai[j] = 16 * ((j % 5) -2);
    bi[j] = 16 * ((j / 5) -2);
  }

// Create the appropriate genome and genetic algorithm

  if(theAppData.whichGenome == 1) {
    GABin2DecPhenotype map;
    map.add(31, minx[theAppData.whichFunction],
	    maxx[theAppData.whichFunction]);
    map.add(31, minx[theAppData.whichFunction],
	    maxx[theAppData.whichFunction]);
    theAppData.genome = new GABin2DecGenome(map, Bin2DecObjective);
  }
  else {
    GARealAlleleSet alleleset(minx[theAppData.whichFunction], 
			      maxx[theAppData.whichFunction]);
    theAppData.genome = new GARealGenome(2, alleleset, RealObjective);
  }

  if(theAppData.whichGA == 0)
    theAppData.ga = new GAIncrementalGA(*theAppData.genome);
  else if(theAppData.whichGA == 1)
    theAppData.ga = new GASimpleGA(*theAppData.genome);
  else if(theAppData.whichGA == 3)
    theAppData.ga = new GADemeGA(*theAppData.genome);
  else
    theAppData.ga = new GASteadyStateGA(*theAppData.genome);

// Now set up the genetic algorithm parameters

  theAppData.ga->parameters(SETTINGS_FILE);
  theAppData.ga->parameters(argc, argv);
  theAppData.ga->initialize();

// we don't allow too many populations (due to our color limit)

  if(theAppData.whichGA == 3) {
    int val;
    theAppData.ga->get(gaNnPopulations, &val);
    if(val > MAX_POPS) {
      val = MAX_POPS;
      theAppData.ga->set(gaNnPopulations, val);
      cerr << "this demo limits the number of populations to "<<MAX_POPS<<"\n";
    }
  }

// Create and manage all of the widgets

  Widget shell = ConstructWidgets(toplevel);

// report status

  cout << "The evolution will use the following:\n";
  cout << "   function: ";
  switch(theAppData.whichFunction) {
  case 0:
    cout << "loaf of bread";
    break;
  case 1: 
    cout << "foxholes";
    break;
  case 2:
    cout << "nasty function";
    break;
  case 3:
    cout << "concentric rings";
    break;
  default:
    cout << "unrecognized function selected";
    break;
  }
  cout << "\n";
  cout << "     genome: ";
  switch(theAppData.whichGenome) {
  case 1:
    cout << "binary-to-decimal";
    break;
  default:
    cout << "real number";
    break;
  }
  cout << "\n";
  cout << "  algorithm: ";
  switch(theAppData.whichGA) {
  case 0:
    cout << "incremental (immediate replacement)";
    break;
  case 1:
    cout << "simple (non-overlapping population)";
    break;
  case 3:
    cout << "deme (multiple populations)";
    break;
  default:
    cout << "steady-state (overlapping population)";
    break;
  }
  cout << "\n" << endl;


// now realize all the widgets and pop 'em up

  XtRealizeWidget(shell);
  XtMapWidget(shell);

  static Atom wmDeleteWindow;
  wmDeleteWindow = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW", False);
#ifdef USE_MOTIF
  XmAddWMProtocolCallback(shell, wmDeleteWindow, QuitCB, (XtPointer)0);
#else
  XSetWMProtocols(XtDisplay(toplevel), XtWindow(toplevel), &wmDeleteWindow, 1);
#endif

  while(!done){
    XEvent event;
    XtAppNextEvent(theAppData.appc, &event);
    XtDispatchEvent(&event);
  }

  XFreeGC(XtDisplay(toplevel), theAppData.bestgc);
  for(int qq=0; qq<MAX_POPS; qq++)
    XFreeGC(XtDisplay(toplevel), theAppData.dotgc[qq]);
  delete theAppData.genome;
  delete theAppData.ga;

  return 0;
}






















void
UpdateCounter(GAGeneticAlgorithm* ga) {
#ifdef USE_MOTIF
  XmString str;
  static char txt[62];
  sprintf(txt, "%d", ga->generation());
  str = XmStringLtoRCreate(txt, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(theAppData.counter, XmNlabelString, str, NULL);
  XmStringFree(str);
#else
  static char txt[62];
  sprintf(txt, "%d", ga->generation());
  XtVaSetValues(theAppData.counter, XtNlabel, txt, NULL);
#endif
}

Boolean
Evolve(int n){
  if((n < 0 && theAppData.ga->done() == gaFalse) || 
     theAppData.ga->generation() < n){
    theAppData.ga->step();
    DrawCB(theAppData.canvas, (XtPointer)&theAppData, 0);
    UpdateCounter(theAppData.ga);
    return False;
  }
  return True;
}

void
ResetCB(Widget, XtPointer cd, XtPointer){
  AppDataPtr data = (AppDataPtr)cd;
  if(data->procid){
    XtRemoveWorkProc(data->procid);
    data->procid = 0;
  }
  data->ga->initialize();
  DrawCB(data->canvas, data, 0);
  UpdateCounter(data->ga);
}

void
StopCB(Widget, XtPointer cd, XtPointer){
  AppDataPtr data = (AppDataPtr)cd;
  if(data->procid){
    XtRemoveWorkProc(data->procid);
    data->procid = 0;
  }
}

void
StepCB(Widget, XtPointer cd, XtPointer){
  AppDataPtr data = (AppDataPtr)cd;
  Evolve(data->ga->generation() + 1);
}

void
EvolveSomeCB(Widget, XtPointer cd, XtPointer){
  AppDataPtr data = (AppDataPtr)cd;
  data->procid = XtAppAddWorkProc(data->appc, (XtWorkProc)Evolve,
    (XtPointer)(data->ga->generation() + data->geninc));
}

void
EvolveCB(Widget, XtPointer cd, XtPointer){
  AppDataPtr data = (AppDataPtr)cd;
  data->procid = 
    XtAppAddWorkProc(data->appc, (XtWorkProc)Evolve,(XtPointer)(-1));
}

void
QuitCB(Widget, XtPointer, XtPointer){
  done = 1;
}

void
DumpStatsCB(Widget, XtPointer cd, XtPointer){
  cerr << "\nstatistics are:\n" << 
    ((GAGeneticAlgorithm*)cd)->statistics() << "\n";
}

void
DumpParamsCB(Widget, XtPointer cd, XtPointer){
  cerr << "\nparameters are:\n" << 
    ((GAGeneticAlgorithm*)cd)->parameters() << "\n";
}

void
DumpScoreCB(Widget, XtPointer cd, XtPointer){
  cerr << "\nbest individual score is: " <<
    ((GAGeneticAlgorithm*)cd)->population().best().score() << "\n";
}





// This routine draws the entire population or a single individual depending 
// on the value of the single flag.  It needs to know how much of a
// buffer to use for spacing between individuals.  We assume that each 
// individual draws from its centroid.

// This is much more nicely done when you derive your own genome that includes
// a draw routine and all the graphics info in it, but for the purpose of this
// example we'll just make a separate function that draws the genome and others
// that return the graphics info about the genomes.

#define BUF 10

void
DrawCB(Widget w, XtPointer cd, XtPointer){
  AppDataPtr data = (AppDataPtr)cd;
  XClearWindow(XtDisplay(w), XtWindow(w));
  if(data->whichGA == 3) {
    GADemeGA* ga = (GADemeGA*)data->ga;
    for(int i=0; i<ga->nPopulations(); i++)
      DrawPopulation(w, ga->population(i), data->dotgc[i], data->dotgc[i]);
  }
  else {
    DrawPopulation(w, data->ga->population(), data->dotgc[0], data->bestgc);
  }
}


void
DrawPopulation(Widget widget, const GAPopulation& pop, GC dotgc, GC bestgc) {
  static int npts = 0;
  static XPoint* pts = 0;

  if(npts != pop.size()) {
    npts = pop.size();
    delete [] pts;
    pts = new XPoint [npts];
  }

  Dimension width = 0, height = 0;
  XtVaGetValues(widget, XtNwidth, &width, XtNheight, &height, NULL);
  Dimension w = width - 2 * BUF;
  Dimension h = height - 2 * BUF;
  Dimension d = (w < h ? w : h);
  w -= d;
  h -= d;
  Dimension originx = BUF + w/2;
  Dimension originy = BUF + h/2;
  float factor = (float)d;
  factor /= (maxx[theAppData.whichFunction] - minx[theAppData.whichFunction]);
  int xbest = 0, ybest = 0;
  if(theAppData.whichGenome == 1) {
    for(int i=0; i<pop.size(); i++) {
      pts[i].x = originx + d/2 + factor *
	((GABin2DecGenome&)(pop.individual(i))).phenotype(0);
      pts[i].y = originy + d/2 - factor * 
	((GABin2DecGenome&)(pop.individual(i))).phenotype(1);
    }
    xbest = originx + d/2 + factor *
      ((GABin2DecGenome&)(pop.best())).phenotype(0);
    ybest = originy + d/2 - factor *
      ((GABin2DecGenome&)(pop.best())).phenotype(1);
  }
  else {
    for(int i=0; i<pop.size(); i++) {
      pts[i].x = originx + d/2 + factor *
	((GARealGenome&)(pop.individual(i))).gene(0);
      pts[i].y = originy + d/2 - factor * 
	((GARealGenome&)(pop.individual(i))).gene(1);
    }
    xbest = originx + d/2 + factor *
      ((GARealGenome&)(pop.best())).gene(0);
    ybest = originy + d/2 - factor *
      ((GARealGenome&)(pop.best())).gene(1);
  }

  XDrawPoints(XtDisplay(widget), XtWindow(widget),
	      dotgc, pts, npts, CoordModeOrigin);

  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest,   ybest);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest-1, ybest);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest,   ybest-1);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest+1, ybest);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest,   ybest+1);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest+1, ybest+1);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest+1, ybest-1);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest-1, ybest+1);
  XDrawPoint(XtDisplay(widget), XtWindow(widget), bestgc, xbest-1, ybest-1);
}

#undef BUF





















// These are the objective functions for the genomes.  They simply call the
// appropriate function.
float
Bin2DecObjective(GAGenome& g) {
  GABin2DecGenome& genome = (GABin2DecGenome&)g;
  return (obj[theAppData.whichFunction])(genome.phenotype(0),
					 genome.phenotype(1));
}

float
RealObjective(GAGenome& g) {
  GARealGenome& genome = (GARealGenome&)g;
  return (obj[theAppData.whichFunction])(genome.gene(0), genome.gene(1));
}














/*****************************************************************************/
/* Type:        2D FUNCTION                                                  */
/* Name:        Objective2D_1                                                */
/* Description: 2D tooth                                                     */
/* Boundaries:  -6 < x < 6                                                   */
/*              -6 < y < 6                                                   */
/* Source:      modified Himmelblau's function from Deb, K.                  */
/*              'GA in multimodal function optimazation' Masters thesis      */
/*		TCGA Rep. 89002 / U. of Alabama                              */
/*****************************************************************************/
float
Function1(float x, float y) 
{
  float z = -((x*x+y-11)*(x*x+y-11)+(x+y*y-7)*(x+y*y-7))/200 + 10;
  return z;
}


/*****************************************************************************/
/* Type:        2D FUNCTION                                                  */
/* Name:        Objective2D_2                                                */
/* Description: Foxholes (25)                                                */
/* Boundaries:  -60 < x < 60                                                 */
/*              -60 < y < 60                                                 */
/* Source:      Shekel's Foxholes problem from De Jong's Diss.(1975)         */
/*              'GA in multimodal function optimazation' Masters thesis      */
/*		TCGA Rep. 89002 / U. of Alabama                              */
/*****************************************************************************/
float
Function2(float x, float y) 
{
  int i;
  float sum = 0;

  for (i=0; i<25; i++) {
    sum += (1 / (1 + i + pow((x-ai[i]),6) + pow((y-bi[i]),6)));
  }
  float z = 500.0 - (1 / (0.002 + sum));
  return z;
}


/*****************************************************************************/
/* Type:        2D FUNCTION                                                  */
/* Name:        Objective2D_3                                                */
/* Description: Schwefel's nasty (4 glob. Max bei (+-420.96/+-420.96)        */
/* Boundaries:  -500 < x < 500                                               */
/*              -500 < y < 500                                               */
/* Source:      Schwefel's function in Schoeneburg                           */
/*****************************************************************************/
float
Function3(float x, float y) 
{
  float z = fabs(x) * sin(sqrt(fabs(x))) + fabs(y) * sin(sqrt(fabs(y)));
  return 500 + z;
}


/*****************************************************************************/
/* Type:        2D FUNCTION                                                  */
/* Name:        Objective2D_4                                                */
/* Description: Mexican Hat                                                  */
/* Boundaries:  -10 < x < 10                                                 */
/*              -10 < y < 10                                                 */
/* Source:                                                                   */
/*****************************************************************************/
float
Function4(float x, float y) 
{
  float z = sin(sqrt(x*x + y*y))*sin(sqrt(x*x + y*y)) - 0.5;
  z /= ((1.0 + 0.001*(x*x + y*y))*(1.0 + 0.001*(x*x + y*y)));
  z = (0.5 - z);
  return (z);
}

















// Here are two versions of the graphic interface.  One version for those of
// you with MOTIF on your systems, and one version for those of you with only
// the athena widget set.  Sorry, no Windoze version yet...

#ifdef USE_MOTIF

Widget
ConstructWidgets(Widget toplevel) {
  Pixmap icon =
    XCreateBitmapFromData(XtDisplay(toplevel),
                          RootWindowOfScreen(XtScreen(toplevel)),
                          (char *)gaview_bits, gaview_width, gaview_height);
  Pixmap mask =
    XCreateBitmapFromData(XtDisplay(toplevel),
                          RootWindowOfScreen(XtScreen(toplevel)),
                          (char *)gaview_bits, gaview_width, gaview_height);
  Widget shell =
    XtVaCreatePopupShell("shell", topLevelShellWidgetClass, toplevel, 
			 XmNiconPixmap, icon,
                         XmNiconMask, mask,
			 NULL);
  Widget form = 
    XtVaCreateManagedWidget("form", xmFormWidgetClass, shell, NULL);

  Pixmap pix;
  Pixel fg, bg;
  unsigned int depth;
  XtVaGetValues(form, XtNforeground, &fg, XtNbackground, &bg, 
		XtNdepth, &depth, NULL);

  pix = 
    XCreatePixmapFromBitmapData(XtDisplay(form),
				RootWindowOfScreen(XtScreen(form)),
				(char *)bm[bmRewind].bits,
				bm[bmRewind].width, bm[bmRewind].height,
				fg, bg, depth);
  Widget rewind = 
    XtVaCreateManagedWidget("rewind", xmPushButtonWidgetClass, form,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNlabelType, XmPIXMAP,
			    XmNlabelPixmap, pix,
			    NULL);
  pix = 
    XCreatePixmapFromBitmapData(XtDisplay(form),
				RootWindowOfScreen(XtScreen(form)),
				(char *)bm[bmStop].bits,
				bm[bmStop].width, bm[bmStop].height,
				fg, bg, depth);
  Widget stop = 
    XtVaCreateManagedWidget("stop", xmPushButtonWidgetClass, form,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, rewind,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNlabelType, XmPIXMAP,
			    XmNlabelPixmap, pix,
			    NULL);
  pix = 
    XCreatePixmapFromBitmapData(XtDisplay(form),
				RootWindowOfScreen(XtScreen(form)),
				(char *)bm[bmForwardStop].bits,
				bm[bmForwardStop].width,
				bm[bmForwardStop].height,
				fg, bg, depth);
  Widget step = 
    XtVaCreateManagedWidget("step", xmPushButtonWidgetClass, form,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, stop,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNlabelType, XmPIXMAP,
			    XmNlabelPixmap, pix,
			    NULL);
  pix = 
    XCreatePixmapFromBitmapData(XtDisplay(form),
				RootWindowOfScreen(XtScreen(form)),
				(char *)bm[bmFastForwardStop].bits,
				bm[bmFastForwardStop].width,
				bm[bmFastForwardStop].height,
				fg, bg, depth);
  Widget some = 
    XtVaCreateManagedWidget("some", xmPushButtonWidgetClass, form,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, step,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNlabelType, XmPIXMAP,
			    XmNlabelPixmap, pix,
			    NULL);
  pix = 
    XCreatePixmapFromBitmapData(XtDisplay(form),
				RootWindowOfScreen(XtScreen(form)),
				(char *)bm[bmFastForward].bits,
				bm[bmFastForward].width,
				bm[bmFastForward].height,
				fg, bg, depth);
  Widget evolve = 
    XtVaCreateManagedWidget("evolve", xmPushButtonWidgetClass, form,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, some,
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNlabelType, XmPIXMAP,
			    XmNlabelPixmap, pix,
			    NULL);

  XtAddCallback(rewind, XmNactivateCallback, ResetCB,      
		(XtPointer)&theAppData);
  XtAddCallback(stop,   XmNactivateCallback, StopCB,       
		(XtPointer)&theAppData);
  XtAddCallback(step,   XmNactivateCallback, StepCB,       
		(XtPointer)&theAppData);
  XtAddCallback(some,   XmNactivateCallback, EvolveSomeCB, 
		(XtPointer)&theAppData);
  XtAddCallback(evolve, XmNactivateCallback, EvolveCB,     
		(XtPointer)&theAppData);

  Widget params = 
    XtVaCreateManagedWidget("params", xmPushButtonWidgetClass, form,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNbottomAttachment, XmATTACH_FORM,
			    NULL);
  Widget stats = 
    XtVaCreateManagedWidget("stats", xmPushButtonWidgetClass, form,
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, params,
			    XmNbottomAttachment, XmATTACH_FORM,
			    NULL);
  Widget score =
    XtVaCreateManagedWidget("score", xmPushButtonWidgetClass, form,
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, stats,
			    XmNbottomAttachment, XmATTACH_FORM,
			    NULL);

  XtAddCallback(params,  XmNactivateCallback, DumpParamsCB, 
		(XtPointer)theAppData.ga);
  XtAddCallback(stats,   XmNactivateCallback, DumpStatsCB,  
		(XtPointer)theAppData.ga);
  XtAddCallback(score,   XmNactivateCallback, DumpScoreCB,  
		(XtPointer)theAppData.ga);

  theAppData.counter = 
    XtVaCreateManagedWidget("counter", xmLabelWidgetClass, form,
			    XmNtopAttachment, XmATTACH_FORM,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNalignment, XmALIGNMENT_CENTER,
			    NULL);
  Widget frame =
    XtVaCreateManagedWidget("frame", xmFrameWidgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, theAppData.counter,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNbottomAttachment, XmATTACH_WIDGET,
			    XmNbottomWidget, rewind,
			    NULL);
  theAppData.canvas =
    XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, frame,
			    NULL);

  XtAddCallback(theAppData.canvas, XmNexposeCallback, DrawCB, 
		(XtPointer)&theAppData);

  XtGCMask gcvalmask = GCFunction | GCForeground;
  XGCValues gcval;
  gcval.function = GXcopy;
  gcval.foreground = theAppData.bestcolor;
  theAppData.bestgc = 
    XCreateGC(XtDisplay(toplevel), RootWindowOfScreen(XtScreen(toplevel)),
	      gcvalmask, &gcval );
  for(int kk=0; kk<MAX_POPS; kk++) {
    gcval.foreground = theAppData.popcolor[kk];
    theAppData.dotgc[kk] = 
      XCreateGC(XtDisplay(toplevel), RootWindowOfScreen(XtScreen(toplevel)),
		gcvalmask, &gcval );
  }

  return shell;
}


#else

void
ExposureEH(Widget w, XtPointer cd, XEvent*, Boolean*) {
  DrawCB(w,cd,0);
}

Widget
ConstructWidgets(Widget toplevel) {
  Pixmap icon =
    XCreateBitmapFromData(XtDisplay(toplevel),
                          RootWindowOfScreen(XtScreen(toplevel)),
                          (char *)gaview_bits, gaview_width, gaview_height);
  Pixmap mask =
    XCreateBitmapFromData(XtDisplay(toplevel),
                          RootWindowOfScreen(XtScreen(toplevel)),
                          (char *)gaview_bits, gaview_width, gaview_height);
  XtVaSetValues(toplevel, XtNiconPixmap, icon, XtNiconMask, mask, NULL);

  Widget form = 
    XtVaCreateManagedWidget("form", formWidgetClass, toplevel, NULL);

  theAppData.counter = 
    XtVaCreateManagedWidget("counter", labelWidgetClass, form,
			    NULL);

  theAppData.canvas =
    XtVaCreateManagedWidget("canvas", widgetClass, form,
			    XtNfromVert, theAppData.counter,
			    NULL);
  XtAddEventHandler(theAppData.canvas, ExposureMask, False,
		    ExposureEH, (XtPointer)&theAppData);

  Widget ctrlbox = 
    XtVaCreateManagedWidget("controls", boxWidgetClass, form, 
			    XtNfromVert, theAppData.canvas,
			    XtNorientation, "vertical",
			    NULL);
  Widget rewind = 
    XtVaCreateManagedWidget("rewind", commandWidgetClass, ctrlbox, NULL);
  Widget stop = 
    XtVaCreateManagedWidget("stop", commandWidgetClass, ctrlbox, NULL);
  Widget step = 
    XtVaCreateManagedWidget("step", commandWidgetClass, ctrlbox, NULL);
  Widget some = 
    XtVaCreateManagedWidget("some", commandWidgetClass, ctrlbox, NULL);
  Widget evolve = 
    XtVaCreateManagedWidget("evolve", commandWidgetClass, ctrlbox, NULL);

  XtAddCallback(rewind, XtNcallback, ResetCB,      (XtPointer)&theAppData);
  XtAddCallback(stop,   XtNcallback, StopCB,       (XtPointer)&theAppData);
  XtAddCallback(step,   XtNcallback, StepCB,       (XtPointer)&theAppData);
  XtAddCallback(some,   XtNcallback, EvolveSomeCB, (XtPointer)&theAppData);
  XtAddCallback(evolve, XtNcallback, EvolveCB,     (XtPointer)&theAppData);

  Widget params = 
    XtVaCreateManagedWidget("params", commandWidgetClass, ctrlbox, NULL);
  Widget stats = 
    XtVaCreateManagedWidget("stats", commandWidgetClass, ctrlbox, NULL);
  Widget score = 
    XtVaCreateManagedWidget("score", commandWidgetClass, ctrlbox, NULL);

  XtAddCallback(params,  XtNcallback, DumpParamsCB, (XtPointer)theAppData.ga);
  XtAddCallback(stats,   XtNcallback, DumpStatsCB,  (XtPointer)theAppData.ga);
  XtAddCallback(score,   XtNcallback, DumpScoreCB,  (XtPointer)theAppData.ga);

  Widget quit = 
    XtVaCreateManagedWidget("quit", commandWidgetClass, ctrlbox, NULL);
  XtAddCallback(quit,   XtNcallback, QuitCB,  (XtPointer)0);

  XtGCMask gcvalmask = GCFunction | GCForeground;
  XGCValues gcval;
  gcval.function = GXcopy;
  gcval.foreground = theAppData.bestcolor;
  theAppData.bestgc = 
    XCreateGC(XtDisplay(toplevel), RootWindowOfScreen(XtScreen(toplevel)),
	      gcvalmask, &gcval );
  for(int kk=0; kk<MAX_POPS; kk++) {
    gcval.foreground = theAppData.popcolor[kk];
    theAppData.dotgc[kk] = 
      XCreateGC(XtDisplay(toplevel), RootWindowOfScreen(XtScreen(toplevel)),
		gcvalmask, &gcval );
  }

  return toplevel;
}

#endif
