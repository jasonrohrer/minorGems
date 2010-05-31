/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 */

#ifndef DOUBLE_NEURAL_NET_INCLUDED
#define DOUBLE_NEURAL_NET_INCLUDED

#include "NeuralNet.h"

/**
 * Abstract superclass for a double-precision floating point neural network. 
 *
 * @author Jason Rohrer 
 */
class DoubleNeuralNet : public NeuralNet {

	public:
		
		/**
		 * Runs the neural network on input values to produce
		 * output values.
		 *
		 * @param inInputs pointer to array of input values.
		 * @param outOutputs pointer to array where output values 
		 *   will be stored.
		 */
		virtual void run( double *inInputs, double *outOutputs ) = 0;
	}; 

#endif
