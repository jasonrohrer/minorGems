/*
 * Modification History
 *
 * 2000-September-13		Jason Rohrer
 * Created.
 *
 * 2000-September-28		Jason Rohrer
 * Added a protected default constructor to faciliate subclassing.
 */

#ifndef DOUBLE_TRAINING_EXAMPLE_INCLUDED
#define DOUBLE_TRAINING_EXAMPLE_INCLUDED

#include "TrainingExample.h"

/**
 * TrainingExample implementation that uses double values
 * for inputs and outputs
 *
 * @author Jason Rohrer 
 */
class DoubleTrainingExample : public TrainingExample {
	public:
		/**
		 * Constructs a training example with inputs and outputs.
		 *
		 * Note that these arrays are not copied by this constructor,
		 * nor are they destroyed by the destructor.
		 */
		DoubleTrainingExample( double *inInputs, double *inOutputs, 
			int inNumInputs, int inNumOutputs );
		
		/**
		 * Gets the number of inputs.
		 */
		int getNumInputs();
		
		/**
		 * Gets the number of outputs.
		 */
		int getNumOutputs();
		
		/**
		 * Gets a specific input value.
		 */
		double getInput( int inIndex );
		
		/**
		 * Gets a specific output value.
		 */
		double getOutput( int inIndex );
	
	
		/**
		 * Gets all input values.
		 */
		double *getInputs();
		
		/**
		 * Gets all output values.
		 */
		double *getOutputs();
	
	protected :
		// default constructor to be called by subclasses
		DoubleTrainingExample();
		
		double *mInputs;
		double *mOutputs;
		int mNumInputs;
		int mNumOutputs;
		
	};

inline DoubleTrainingExample::DoubleTrainingExample( double *inInputs, 
	double *inOutputs, int inNumInputs, int inNumOutputs ) 
	: mInputs( inInputs ), mOutputs( inOutputs ), mNumInputs( inNumInputs ),
	mNumOutputs( inNumOutputs ) {
	}

inline DoubleTrainingExample::DoubleTrainingExample() {
	}

inline int DoubleTrainingExample::getNumInputs() {
	return mNumInputs;
	}

inline int DoubleTrainingExample::getNumOutputs() {
	return mNumOutputs;
	}

inline double DoubleTrainingExample::getInput( int inIndex ) {
	return mInputs[inIndex];
	}
	
inline double DoubleTrainingExample::getOutput( int inIndex ) {
	return mOutputs[inIndex];
	}
	
inline double *DoubleTrainingExample::getInputs() {
	return mInputs;
	}
	
inline double *DoubleTrainingExample::getOutputs() {
	return mOutputs;
	}
	
#endif
