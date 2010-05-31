/*
 * Modification History
 *
 * 2000-September-13		Jason Rohrer
 * Created.
 */

#ifndef EXAMPLE_TRAINER_INCLUDED
#define EXAMPLE_TRAINER_INCLUDED

#include "TrainingExample.h"

/**
 * Abstract superclass for classes that train with TrainingExamples.
 *
 * @author Jason Rohrer 
 */
class ExampleTrainer {
	
	public:
		
		/**
		 * Sets examples to use in training.
		 *
		 * @param inExamples pointer to an array containing the examples.
		 *   Note that this array is *not* copied before being used by this
		 *   class.
		 * @param inExampleSetSize the number of examples in the array.
		 */
		void setExamples( TrainingExample **inExamples, int inExampleSetSize );
		
		
		/**
		 * Gets the size of the example set.
		 *
		 * @return the size of the example set.
		 */
		int getExampleSetSize();
		
		
		/**
		 * Gets a specfied example. 
		 *
		 * @param inIndex the index of the example to get.
		 *
		 * @return the example.  
		 *   Note that this is not a copy of the example.
		 */
		TrainingExample *getExample( int inIndex );
	
	
	protected:
		TrainingExample **mExamples;
		int mExampleSetSize;
	};



inline void ExampleTrainer::setExamples( TrainingExample **inExamples, 
	int inExampleSetSize ) {
	
	mExampleSetSize = inExampleSetSize;
	mExamples = inExamples;
	}


inline int ExampleTrainer::getExampleSetSize() {
	return mExampleSetSize;
	}


inline TrainingExample *ExampleTrainer::getExample( int inIndex ) {
	return mExamples[inIndex];
	}
	
#endif
