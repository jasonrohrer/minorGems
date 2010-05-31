/*
 * Modification History
 *
 * 2000-October-12		Jason Rohrer
 * Created.
 *
 * 2000-November-5		Jason Rohrer
 * Added support for sorting in two different orders (increasing
 * and decreasing order).
 */
 
#ifndef POPULATION_SORTER_INCLUDED
#define POPULATION_SORTER_INCLUDED

#include "PopulationMember.h"
 
class PopulationSorter {
 
	public:
		/**
		 * Sorts a population according to scores (scores are sorted too).
		 *
		 * @param inPopulation array of population members.
		 * @param inScores array of scores associated with each population
		 *   member.
		 * @param inPopulationSize size of population.
		 * @param inIncreasingOrder set to true if population should
		 *   be sorted into decreasing score order (default is false).
		 */ 
		void sortPopulation( PopulationMember** inPopulation, 
			double* inScores, int inPopulationSize, 
			char inDecreasingOrder=false );
	};

#endif
