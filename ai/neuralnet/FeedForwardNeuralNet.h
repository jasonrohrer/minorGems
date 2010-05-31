/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 *
 * 2000-September-17		Jason Rohrer
 * Added functions for writing to file and reading back in from file.
 */

#ifndef FEED_FORWARD_NEURAL_NET_INCLUDED
#define FEED_FORWARD_NEURAL_NET_INCLUDED

#include "DoubleNeuralNet.h"
#include "StepThreshold.h"

#include <stdio.h>

/**
 * Class that implements the DoubleNeuralNet interface with
 * a feed-forward neural network containing a specifiable number
 * of hidden layers. 
 *
 * Defaults to using StepThreshold for all neurons.
 * Note that after construction, if there are n hidden layers,
 * layer 0 is the input layer and layer n+1 is the output layer.
 *
 * Note:  For performance reasons, none of the set or get
 *			functions do any bounds checking.
 *
 * @author Jason Rohrer 
 */
class FeedForwardNeuralNet : public DoubleNeuralNet {

	public:
		
		
		/**
		 * Constructs a FeedForwardNeuralNet with all weights and
		 * thresholds set to 0.
		 * 
		 * @param inNumInputs number of input neurons.
		 * @param inNumHiddenLayers number of hidden layers of neurons.
		 * @param inNeuronsPerLayer pointer to an array describing how
		 *   many neurons are in each hidden layer.
		 * @param inNumOutputs number of ouput neurons.
		 */
		FeedForwardNeuralNet( int inNumInputs, int inNumHiddenLayers,
			int *inNeuronsPerLayer, int inNumOutputs );
		
		/**
		 * Constructs a network by reading it in from a text file.
		 *
		 * @param inInputFile the text file to read the network from.
		 */
		FeedForwardNeuralNet( FILE *inInputFile );
		
		
		virtual ~FeedForwardNeuralNet();	
		
		
		/**
		 * Gets the number of hidden layers in the network.
		 *
		 * @return the number of hidden layers in the network.
		 */
		int getNumHiddenLayers();
		
		/**
		 * Gets the number of neurons in a layer.
		 *
		 * @param inLayer the layer to query, where 0 is the input layer.
		 */
		int getNumInLayer( int inLayer );
	
		
		/**
		 * Sets the threshold function of a neuron.
		 * The default is a StepThreshold.
		 * 
		 * @param inLayer the layer containing the neuron.
		 * @param inNeuron the position of the neuron in Layer.
		 * @param inThreshold the threshold of the specifed neuron.
		 */
		void setThreshold( int inLayer, int inNeuron, Threshold *inThreshold );
		
		
		/**
		 * Gets the threshold function of a neuron.
		 * 
		 * See setThreshold for a parameter description.
		 *
		 * @return the threshold function for the specified neuron. 
		 */
		Threshold *getThreshold( int inLayer, int inNeuron );		
		
		
		/**
		 * Sets the threshold value for the threshold function of a neuron.
		 * 
		 * @param inLayer the layer containing the neuron.
		 * @param inNeuron the position of the neuron in Layer.
		 * @param inThresholdValue the threshold value of the specifed neuron.
		 */
		void setThresholdValue( int inLayer, int inNeuron, 
								double inThresholdValue );
		
		
		/**
		 * Gets the threshold value for the threshold function of a neuron.
		 * 
		 * See setThresholdValue for a parameter description.
		 *
		 * @return the threshold value for the specified neuron. 
		 */
		double getThresholdValue( int inLayer, int inNeuron );
		
		
		/**
		 * Sets the weight between two neurons.
		 *
		 * @param inLayerA the layer containing neuron A.
		 * @param inNeuronA the position of neuron A in LayerA.
		 * @param inNeuronB the position of neuron B in LayerA+1.
		 * @param inWeight the weight between neurons A and B.
		 */
		void setWeight( int inLayerA, int inNeuronA, 
						int inNeuronB,
						double inWeight );
		
		/**
		 * Gets the weight between two neurons in the network.
		 * 
		 * See setWeight for parameter description.
		 *
		 * @return the weight between neurons A and B.
		 */
		double getWeight( int inLayerA, int inNeuronA, int inNeuronB );
						
		
		/**
		 * Implements the DoubleNeuralNet:run interface.
		 */
		void run( double *inInputs, double *outOutputs );
	
		
		/**
		 * Writes this network out to a text file.
		 *
		 * Note that information about what kind of threshold
		 * is being used at each node (i.e., what class was 
		 * potentially passed in for the threshold at each node)
		 * is discareded in this process.
		 *
		 * Reading such a file back in will create a FFNN with
		 * all threshold functions set to the default, StepThreshold.
		 *
		 * @param inOutputFile the text file to write this network out to.
		 *
		 * @return true iff file was written to successfully.
		 */
		virtual char writeToFile( FILE *inOutputFile );
	
	protected:
		
		int mNumHiddenLayers;
		// number of neurons in each layer (including input and output layers)
		int *mNeuronsPerLayer;
		
		// Note that NeuralNet superclass provides access to other
		// size values.
		
		// indexed by [layer][neuron]
		// where input is layer 0 and output is layer n+1
		// (for n hidden layers)
		Threshold ***mNeuronThresholds;
		double **mNeuronThresholdValues;
		
		// indexed by [layerA][neuronA][neuronB]
		double ***mWeights;
		
	}; 


inline int FeedForwardNeuralNet::getNumHiddenLayers() {
	return mNumHiddenLayers;
	}


inline int FeedForwardNeuralNet::getNumInLayer( int inLayer ) {
	return mNeuronsPerLayer[inLayer];
	}


inline void FeedForwardNeuralNet::setThreshold( int inLayer, int inNeuron, 
	Threshold *inThreshold ) {
	
	mNeuronThresholds[inLayer][inNeuron] = inThreshold;
	}
	
		
inline Threshold *FeedForwardNeuralNet::getThreshold( int inLayer, 
	int inNeuron ) {	
	
	return mNeuronThresholds[inLayer][inNeuron];
	}


inline void FeedForwardNeuralNet::setThresholdValue( int inLayer, int inNeuron, 
	double inThresholdValue ) {
	
	mNeuronThresholdValues[inLayer][inNeuron] = inThresholdValue;
	}
	
		
inline double FeedForwardNeuralNet::getThresholdValue( int inLayer, 
	int inNeuron ) {
	
	return mNeuronThresholdValues[inLayer][inNeuron];
	}


inline void FeedForwardNeuralNet::setWeight( int inLayerA, int inNeuronA, 
	int inNeuronB, double inWeight ) {
	
	mWeights[inLayerA][inNeuronA][inNeuronB] = inWeight;
	}
	
		
inline double FeedForwardNeuralNet::getWeight( int inLayerA, 
	int inNeuronA, int inNeuronB ) {
	
	return mWeights[inLayerA][inNeuronA][inNeuronB];
	}


#endif
