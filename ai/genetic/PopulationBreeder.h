/*
 * Modification History
 *
 * 2000-October-23		Jason Rohrer
 * Created.
 *
 * 2000-December-13		Jason Rohrer  
 * Moved into minorGems. 
 */
 
#ifndef POPULATION_BREEDER_INCLUDED
#define POPULATION_BREEDER_INCLUDED

#include "Crossbreedable.h"
#include "PopulationDeleter.h"
 
class PopulationBreeder {
 
	public:
		/**
		 * Breeds members of top portion of population to replace
		 * all members from bottom portion of population.
		 * Selects top portion of population automatically so that
		 * each member of top gets to breed once with each other member.
		 *
		 * Uses crossing fraction of 0.5.
		 *
		 * @param inPopulation the population of Crossbreedables to breed.
		 * @param inPopulationSize the population size.
		 * @param inMutationProb the probability of mutations throughout
		 *   each crossbreeding (passing in 1 will create a mutation at 
		 *   each attribute of each offspring).
		 * @param inMaxMutationMagnitude the maximum "severity" of any
		 *   individual mutation, in [0..1].
		 * @param inDeleter object that knows how to delete population members.
		 */
		void breedPopulation( Crossbreedable** inPopulation, 
			int inPopulationSize, float inMutationProb, 
			float inMaxMutationMagnitude, PopulationDeleter *inDeleter );
	};

#endif
