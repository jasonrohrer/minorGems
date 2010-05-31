/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 */

#ifndef BOOLEAN_NEURAL_NET_INCLUDED
#define BOOLEAN_NEURAL_NET_INCLUDED

#include "NeuralNet.h"

/**
 * Abstract superclass for a boolean neural network.
 * Note that chars are used to represent booleans to support
 * platforms that don't support booleans as a type. 
 *
 * False is represented by a 0, and true is represented
 * by any non-zero char.
 *
 * @author Jason Rohrer 
 */
class BooleanNeuralNet : public NeuralNet {

	public:
		
		/**
		 * Runs the neural network on input values to produce
		 * output values.
		 *
		 * @param inInputs pointer to array of input values.
		 * @param outOutputs pointer to array where output values 
		 *   will be stored.
		 */
		virtual void run( char *inInputs, char *outOutputs ) = 0;
	}; 

#endif
