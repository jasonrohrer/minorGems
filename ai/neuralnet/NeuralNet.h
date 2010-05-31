/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 *
 * 2000-September-17	Jason Rohrer
 * Changed mebmber functions to virtual so dynamic_cast operator
 * would work.
 *
 * 2000-October-12		Jason Rohrer
 * Changed to subclass PopluationMember to allow for abstraction of population
 * sorting.
 *
 * 2000-December-13		Jason Rohrer  
 * Moved into minorGems. 
 */

#ifndef NEURAL_NET_INCLUDED
#define NEURAL_NET_INCLUDED

#include "minorGems/ai/genetic/PopulationMember.h"

/**
 * Superclass for a generic neural network. 
 *
 * @author Jason Rohrer 
 */
class NeuralNet : public PopulationMember {

	public:
		
		// note that these functions are declared virtual
		// primarily so dynamic casts would work
		
		/**
		 * Gets the number of input nodes in this network.
		 *
		 * @return the number of input nodes in this network.
		 */
		virtual int getNumInputs();
		
		/**
		 * Gets the number of output nodes in this network.
		 *
		 * @return the number of output nodes in this network.
		 */
		virtual int getNumOutputs();
		
		
	protected:
		int mNumInputs;
		int mNumOutputs;
	}; 


inline int NeuralNet::getNumInputs() {
	return mNumInputs;
	}
	
inline int NeuralNet::getNumOutputs() {
	return mNumOutputs;
	}

#endif
