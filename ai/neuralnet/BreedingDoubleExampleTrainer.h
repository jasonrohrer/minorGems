/*
 * Modification History
 *
 * 2000-September-13		Jason Rohrer
 * Created.
 *
 * 2000-September-28		Jason Rohrer
 * Changed so that it only handles training of single-output networks,
 * which simplifies much of the internal code.
 * Switched to using ErrorEvaluators to compute error for each network
 * in a more modular way.
 *
 * 2000-December-7		Jason Rohrer
 * Changed so that train function takes an array in which errors througout
 * training are stored.
 *
 * 2000-December-8		Jason Rohrer
 * Changed so that train function returns num rounds required if training
 * successful.
 *
 * 2000-December-13		Jason Rohrer  
 * Moved into minorGems.    
 */

#ifndef BREEDING_DOUBLE_EXAMPLE_TRAINER_INCLUDED
#define BREEDING_DOUBLE_EXAMPLE_TRAINER_INCLUDED

#include "BreedableFeedForwardNeuralNet.h"

#include "minorGems/util/random/RandomSource.h"

#include "PopulationTrainer.h"
#include "ExampleTrainer.h"

#include "DoubleTrainingExample.h"

#include "minorGems/math/stats/ErrorEvaluator.h"

/**
 * Class that trains a population of BreedableFeedForwardNeuralNets 
 * on an example set using a crossbreeding evolutionary proceedure.
 *
 * Note:
 * Only works to train single-output networks.
 *
 * @author Jason Rohrer 
 */
class BreedingDoubleExampleTrainer 
	: public PopulationTrainer, public ExampleTrainer {

	public:
		
		/**
		 * Constructs a trainer that uses the default error evaluation function.
		 */
		BreedingDoubleExampleTrainer();
		
		/**
		 * Constructs a trainer specifying an error evaluation function.
		 *
		 * @param inErrorEvaluator evaluation function to use when
		 *   determining the error for a net work in the population.
		 */
		BreedingDoubleExampleTrainer( ErrorEvaluator *inErrorEvaluator ); 
		
		~BreedingDoubleExampleTrainer();
		
		/**
		 * Runs the training proceedure until PopulationTrainer::mTrainingRigor
		 * satisfied, or until inMaxNumRounds completed.
		 *
		 * @param inMaxNumRounds the maximum number of rounds to run before giving
		 *  up.
		 * @param inMutationProb the probability of a mutation at each step
		 *   of the crossbreeding of each pair of networks.
		 * @param inMaxMutationMagnitude the maximum magnitude, in [0..1] of
		 *   a mutation.
		 * @param inOutErrors pre-allocated array of size inMaxNumRounds
		 *   where best population member error will be stored during each
		 *   round.
		 *
		 * @return 0 if round limit reached before rigor reached,
		 *   numRoundsRequired if training
		 *   was successful (reached rigor requirement), or -1 if there was
		 *   an unrecoverable error during the training (e.g., one of the networks
		 *   in the population did not cast to a BreedableFeedForwardNeuralNet ).
		 *	 
		 */
		int train( int inMaxNumRounds,
			float inMutationProb, float inMaxMutationMagnitude,
			float *inOutErrors );
		
		/**
		 * Sets the name of the file in which the best network
		 * from each round will be written.
		 *
		 * Default value is "test.net".
		 *
		 * @param inBestFileName the name or path of the file
		 */
		void setBestFileName( char *inBestFileName );
	
	protected:
		ErrorEvaluator *mErrorEvaluator;	
		char mEvaluatorPassedIn;
		// name of file in which to save best network in population 
		// during each round
		char *mBestFileName;
	};

inline void BreedingDoubleExampleTrainer::setBestFileName( 
	char *inBestFileName ) {
	
	mBestFileName = inBestFileName;
	}
	
#endif
