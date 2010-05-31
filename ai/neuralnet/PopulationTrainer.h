/*
 * Modification History
 *
 * 2000-September-13		Jason Rohrer
 * Created.
 *
 * 2000-October-12		Jason Rohrer
 * Changed to subclass PopulationSorter to abstract away sorting routine.
 */

#ifndef POPULATION_TRAINER_INCLUDED
#define POPULATION_TRAINER_INCLUDED

#include "NeuralNet.h"

#include "minorGems/ai/genetic/PopulationSorter.h"

/**
 * Abstract superclass for classes that train a population of NeuralNets.
 *
 * @author Jason Rohrer 
 */
class PopulationTrainer : public PopulationSorter {
	
	public:
		
		/**
		 * Sets the population to be trained.
		 *
		 * @param inPopulation pointer to an array containing the population.
		 *  Note that this array, and each network it contains, is 
		 *  copied before being modified by this class.
		 * @param inPopulationSize the number of networks in the popuation.
		 */
		void setPopulation( NeuralNet **inPopluation, int inPopulationSize );
		
		
		/**
		 * Gets the size of the population being trained.
		 *
		 * @return the size of the population being trained.
		 */
		int getPopulationSize();
		
		
		/**
		 * Gets the population being trained.
		 *
		 * @param outPopulation pointer pre-allocated space where popluation
		 *   will be put.  Note that population will be sorted best->worst,
		 *   with the best member at index 0.
		 */
		void getPopulation( NeuralNet **outPopulation );
		
		
		/**
		 * Sets the unnormalized maximum error that must be reached
		 * by top population member before training stops.
		 *
		 * @param inTrainingRigor the unnormalized maximum error 
		 *   that must be reached by top population member before training 
		 *   stops.
		 */
		void setTrainingRigor( double inTrainingRigor );
	
		/**
		 * Gets the unnormalized maximum error that must be reached
		 * by top population member before training stops.
		 *
		 * @return the unnormalized maximum error 
		 *   that must be reached by top population member before training 
		 *   stops.
		 */
		float getTrainingRigor();
	
	protected:
		NeuralNet **mPopulation;
		int mPopulationSize;
		double mTrainingRigor;
		
		/**
		 * Sorts the population based on an array of scores for each member,
		 * where lower scores are better.  Puts population in best->worst
		 * order with the best member at index 0.  After return, inScores
		 * is sorted too.
		 *
		 * @param inScores an array of scores, one for each member of
		 *   population.
		 */
		void sortPopulation( double *inScores );
	};




inline void PopulationTrainer::setPopulation( NeuralNet **inPopulation, 
	int inPopulationSize ) {
	mPopulationSize = inPopulationSize;
	mPopulation = inPopulation;
	}


inline int PopulationTrainer::getPopulationSize() {
	return mPopulationSize;
	}


inline void PopulationTrainer::getPopulation( NeuralNet **outPopulation ) {
	memcpy( (void *)outPopulation, (void *)mPopulation, 
		mPopulationSize * sizeof( void * ) );
	}


inline void PopulationTrainer::setTrainingRigor( double inTrainingRigor ) {
	mTrainingRigor = inTrainingRigor;
	}


inline float PopulationTrainer::getTrainingRigor() {
	return mTrainingRigor;
	}
	
#endif
