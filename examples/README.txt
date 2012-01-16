GAlib examples
mbw 5jan96

How to compile the examples
-------------------------------------------------------------------------------
Copy the contents of this directory into your own filespace.  In the new
directory, edit the makefile as needed to specify the location of the GAlib
headers and library.  If you system supports makedepend, do 'make depend' to 
update the makefile dependencies.

To build all of the examples, type 'make all' or simply 'make'.

To build a specific example, type 'make exN' where N is the number of the
example you want to build.  For example, to build example 5 you would type
'make ex5'.

To remove old core, executable, object, and data files, do 'make clean'.

You only need to do 'make depend' when the dependencies change.  For example,
if you modify one of the examples so that it uses routines in another file,
you will have to modify the makefile then do 'make depend' so that make will
know about the new dependency.



What the examples do
-------------------------------------------------------------------------------

  ex1	Fill a 2DBinaryStringChromosome with alternating 0s and 1s using a
	SimpleGA.

  ex2	Generate a sequence of random numbers, then use a Bin2DecChromosome 
	and SimpleGA to try and match the sequence.  This example shows how
	to use the user-data member of genomes in objective functions.

  ex3	Read a 2D pattern from a data file then try to match the pattern using
	a 2DBinaryStringChromosome and a SimpleGA.  This example also shows
	how to use the GAParametes object for setting genetic algorithm
 	parameters and reading command-line arguments.

  ex4	Fill a 3DBinaryStringChromosome with alternating 0s and 1s using a
	SteadyStateGA.  This example uses many member functions of the genetic
	algorithm to control which statistics are recorded and dumped to file.

  ex5	This example shows how to build a composite genome (a cell?) using
	a 2DBinaryStringGenome and a Bin2DecGenome.  The composite genome uses
        behaviors that are defined in each of the genomes that it contains.
        The objective is to match a pattern and sequence of numbers.

  ex6	Grow a GATreeGenome<int> using a SteadyStateGA.  This example
	illustrates the use of specialized methods to override the default
	initialization method and to specialize the output from a tree.  It 
  	also shows how to use templatized genome classes.  Finally, it shows
        the use of the parameters object to set default values then allow these
 	to be modified from the command line.  The objective function in this 
	example tries to grow the tree as large as possible.

  ex7	Identical in function to example 3, this example shows how to use the
	increment operator (++), completion measure, and other member functions
	of the GA.  It uses a GA with overlapping populations rather than the
	non-overlapping GA in example 3 and illustrates the use of many of the
	GA member functions.  It also illustrates the use of the parameter list
 	for reading settings from a file, and shows how to stuff a genome with
	data from an input stream.

  ex8	Grow a GAListGenome<int> using a GA with overlapping populations.
	This shows how to randomly initialize a list of integers, how to use
	the sigma truncation scaling object to handle objective scores that 
	may be positive or negative, and the 'set' member of the genetic 
	algorithm for controlling statistics and other genetic algorithm
	parameters.

  ex9	Find the maximum value of a continuous function in two variables.  This
	example uses a GABin2DecGenome and simple GA.  It also illustrates
	how to use the GASigmaTruncationScaling object (rather than the default
	linear scaling).  Sigma truncation is particularly useful for 
	objective functions that return negative values.

  ex10	Find the maximum value of a continuous, periodic function.  This 
	example illustrates the use of sharing to do speciation.  It defines
	a sample distance function (one that does the distance measure based
	on the genotype, the other based on phenotype).  It uses a binary-
	to-decimal genome to represent the function values.

  ex11	Generate a sequence of descending numbers using an order-based list.
	This example illustrates the use of a GAListGenome as an 
	order-based chromosome.  It contains a custom initializer and shows
        how to use this custom initializer in the List genome.

  ex12	Alphabetize a sequence of characters.  Similar to example 11, this
	example illustrates the use of the GAStringGenome (rather than a
        list) as an order-based chromosome.

  ex13  This program runs a GA-within-GA.  The outer level GA tries to
	match the pattern read in from a file.  The inner GA tries to match
        a sequence of randomly generated numbers (the sequence is generated
        at the beginning of the program's execution).  The inner level GA is
	run only when the outer GA reaches a threshhold objective score.

  ex14	Another illustration of how to use composite chromosomes.  In this
	example, the composite chromosome contains a user-specifiable number 
	of lists.  Each list behaves differently and is not affected by 
	mutations, crossovers, or initializations of the other lists.

  ex15	The completion function of a GA determines when it is "done".  This
	example uses the convergence to tell when the GA has reached the
	optimum (the default completion measure is number-of-generations).
        It uses a binary-to-decimal genome and tries to match a sequence of
        randomly generated numbers.

  ex16  Tree chromosomes can contain any kind of object in the nodes.  This
	example shows how to put a point object into the nodes of a tree to
	represent a 3D plant.  The objective function tries to maximize the
        size of the plant.

  ex17  Array chromsomes can be used when you need tri-valued alleles.  This
	example uses a 2D array with trinary alleles.

  ex18	This example compares the performance of three different genetic
        algorithms.  The genome and objective function are those used in
        example 3, but this example lets you specify which type of GA you
        want to use to solve the problem.  You can use steady state, simple,
        or incremental just by specifying one of them on the command line.
        The example saves the generational data to file so that you can then 
	plot the convergence data to see how the performance of each genetic
	algorithm compares to the others.

  ex19	The 5 DeJong test problems.

  ex20  Holland's royal road function.  This example computes Holland's 1993
        ICGA version of the Royal Road problem.  Holland posed this problem as
        a challenge to test the performance of genetic algorithms and 
        challenged other GA users to match or beat his performance.

  ex21  This example illustrates various uses of the allele set in array
	genomes.  The allele set may be an enumerated list of items or a 
	bounded range of continuous values, or a bounded set of discrete 
	values.  This example shows how each of these may be used in
	combination with a real number genome.

  ex22  This example shows how to derive a new genetic algorithm class in 
	order to customize the replacement method.  Here we derive a new type
	of steady-state genetic algorithm in which speciation is done more
	effectively by not only scaling fitness values but also by controlling
	the way new individuals are inserted into the population.

  ex23  The genetic algorithm object can either maximize or minimize your
	objective function.  This example shows how to use the minimize
	abilities of the genetic algorithm.  It uses a real number genome with
	one element to find the maximum or minimum of a sinusoid.

  ex24  This example shows how to restricted mating using a custom genetic 
  	algorithm and custom selection scheme.  The restricted mating in the 
	genetic algorithm tries to pick individuals that are similar (based 
	upon their comparator).  The selector chooses only the upper half of 
	the population (so it cannot choose very bad individuals, unlike the
	roulette wheel selector, for example).

  ex25  Multiple populations on a single CPU.  This example uses its own
	genetic algorithm class called a 'DemeGA'.  The genetic algorithm
        controls the migration behavior for moving individuals between
        populations.

  ex26  Travelling Salesperson Problem.  Although genetic algorithms are not
        the best way to solve the TSP, we include an example of how it can
        be done.  This example uses an order-based list as the genome to
        figure out the shortest path that connects a bunch of towns such that
        each town is visited exactly once.  It uses the edge recombination
        crossover operator (you can try it with the partial match crossover 
        as well to see how poorly PMX does on this particular problem).

  ex27  Deterministic crowding.  Although the algorithms built-in to GAlib 
        allow you to do quite a bit of customization, sometimes you'll want to
        derive your own class so that you can really tweak the way the
	algorithm works.  This example shows one way of implementing the
	deterministic crowding method by deriving an entirely new genetic
	algorithm class.

randtest
	Use this program to verify that the random number generator is 
	generating suitably random numbers on your machine.  This is by no
	means a comprehensive random number test suite, but it will give you 
	some idea of how well GAlib's random number generator is working.

graphic     (available only in the UNIX distribution)
	You can learn a great deal by watching the genetic algorithm evolve.
	This directory contains two examples that show populations of solutions
	evolving in real time.  Both programs use X resources as well as
        command-line arguments to control their behavior.  You can also use a
        standard GAlib settings file.  The programs will compile with either
	the Motif or athena widget set.

        The first example has a simple X windows interface that lets you start,
        stop, restart, and incrementally evolve a population of indivdiuals.
	The objective function is a continuous function in two variables with
	concentric rings and a maximal value located in the center.
        You can see the evolution in action, so it becomes very obvious if
        your operators are not working correctly or if the algorithm is
        converging prematurely.  You can choose between 3 different genetic
        algorithms, 2 different genomes (real or binary-to-decimal), and 4
        different functions.

        The second example shows solutions to the travelling salesman problem 
        evolving in real time.  You can compare three different algorithms:
    	simple, steady-state, and deterministic crowding.

   gnu      (available only in the UNIX distribution)
	This directory contains the code for an example that uses the BitString
        object from the GNU class library.  The example illustrates how to 
        incorporate an existing object (in this case the BitString) into a
        GAlib Genome type.  The gnu directory contains the source code needed
        for the BitString object (taken from the GNU library) plus the two
        files (bitstr.h and bitstr.C) needed to define the new genome type and
        the example file that runs the GA (gnuex.C).

pvmind      (available only in the UNIX distribution)
        This directory contains code that illustrates how to use GAlib with
	PVM in a master-slave configuration wherein the master process is the
	genetic algorithm with a single population and each slave process is
	a genome evaluator.  The master sends individual genomes to the slave
	processes to be evaluated then the slaves return the evaluations.

pvmpop      (available only in the UNIX distribution)
        This directory contains code that illustrates a PVM implementation of
	parallel populations.  The master process initiates a cluster of slaves
	each of which contains a single population.  The master process 
	harvests individuals from all of the distributed populations.  With a
        few modifications you can also use this example with the deme GA from
        example 25 (it uses migration to distribute diversity between pops).
