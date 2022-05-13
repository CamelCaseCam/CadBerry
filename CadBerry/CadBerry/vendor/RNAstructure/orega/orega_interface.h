/*
 * A program that designs a nucleic acid sequence, with low ensemble defect, 
 * that is expected to fold into the structure specified by the user.
 * The designed strand can be composed of either DNA or RNA.
 *
 * (c) 2015 Mathews Lab, University of Rochester Medical Center.
 * Written by Richard M. Watson
 */

#ifndef OREGA_INTERFACE_H
#define OREGA_INTERFACE_H

#include <vector>

#include "../RNA_class/RNA.h"
#include "GeneticAlgorithm.h"

class Orega_interface {
 public:
	// Public constructor and methods.

	/*
	 * Name:        Constructor.
	 * Description: Initializes all private variables.
	 */
	Orega_interface();

	/*
	 * Name:        parse
	 * Description: Parses command line arguments to determine what options are required for a particular calculation.
	 * Arguments:
	 *     1.   The number of command line arguments.
	 *     2.   The command line arguments themselves.
	 * Returns:
	 *     True if parsing completed without errors, false if not.
	 */
	bool parse( int argc, char** argv );

	/*
	 * Name:        run
	 * Description: Run calculations.
	 */
	int run();

	/**
	 * Shows interface parameters (for debugging)
	 */
	void showParams();

 private:
	// Private variables.

	// Description of the calculation type.
	string calcType;

	// The input ct file which describes the structure to design. The sequence in the ct file will be ignored.
	string sequenceFile;

	// Random seed
	long randSeed;

	// The program displays the random seed, unless it was specified by the user. This keeps track of whether or not they did.
	bool setRandomSeed;

	// Name of the alphabet (e.g. "rna" or "dna" etc).
	const char* alphabet;

	// Number of individuals in the population 
	int populationSize;

	// Define the target segment in the sequence where mutation/optimization should be performed.
	int rangeStart, rangeLength;

	// Number of mutation loops
	int iterations;

	// probability that a nucleotide in the target segment should be mutated.
	double mutationRate;

	// Number of iterations before a recombination/crossover step should occur.
	// (i.e. recombination will occur every # of iterations)
	int recombinationFrequency;

	// Probability that a nucleotide will be selected as a recombination marker
	// in the recombination/crossover steps.
	double recombinationRate;

	// Name of file where the optimized sequence is saved.
	std::string outputSequenceFile;

	// Name of file where the most recently calculated state is saved.
	// (can be used to restart the program from a prior state)
	std::string outputStateFile;

	// State file that can be input to restart the program from a prior saved state.
	std::string restartFile;

	// Name of the objective function to use.
	int objectiveFunction; // OregaObjectiveFunction is an enum defined in GeneticAlgorithm.h
};

#endif /* OREGA_INTERFACE_H */
