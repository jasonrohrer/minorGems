/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 *
 * 2000-December-7		Jason Rohrer
 * Added support for turning "forward path following" crossbreeding
 * algorithm on and off.  When off, a standard random node selection
 * proceedure is used. 
 *
 * 2000-December-8		Jason Rohrer
 * Changed so that multiple crossbreeding methods can be specified.
 *
 * 2000-December-11		Jason Rohrer
 * Added a bit string crossbreeding type.
 *
 * 2000-December-11		Jason Rohrer  
 * Fixed a major bug that prevented crossbreeding type from being passed
 * down to offspring.  
 *
 * 2000-December-13		Jason Rohrer  
 * Moved into minorGems.   
 */

#ifndef BREEDABLE_FEED_FORWARD_NEURAL_NET_INCLUDED
#define BREEDABLE_FEED_FORWARD_NEURAL_NET_INCLUDED

#include "FeedForwardNeuralNet.h"
#include "minorGems/ai/genetic/Crossbreedable.h"

#include "minorGems/util/random/RandomSource.h"

/**
 * Subclass of FeedForwardNeuralNet that is crossbreedable 
 *
 * Defaults to using StepThreshold for all neurons.
 * Note that after construction, if there are n hidden layers,
 * layer 0 is the input layer and layer n+1 is the output layer.
 *
 * @author Jason Rohrer 
 */
class BreedableFeedForwardNeuralNet 
	: public FeedForwardNeuralNet, public Crossbreedable {

	public:
		
		
		/**
		 * Constructs a BreedableFeedForwardNeuralNet with all weights and
		 * thresholds set to 0.
		 * Weight range set to [-1,+1].
		 * If layer A has n neurons, then threshold in layer A+1 
		 * has range set to [-n, +n].
		 * 
		 * @param inNumInputs number of input neurons.
		 * @param inNumHiddenLayers number of hidden layers of neurons.
		 * @param inNeuronsPerLayer pointer to an array describing how
		 *   many neurons are in each hidden layer.
		 * @param inNumOutputs number of ouput neurons.
		 * @param inRandSource random number generator to use
		 *   during all crossbreeding and mutation.
		 */
		BreedableFeedForwardNeuralNet( int inNumInputs, int inNumHiddenLayers,
			int *inNeuronsPerLayer, int inNumOutputs, 
			RandomSource *inRandSource );
		
		
		/**
		 * Constructs a network by reading it in from a text file.
		 *
		 * @param inInputFile the text file to read the network from.
		 * @param inRandSource random number generator to use
		 *   during all crossbreeding and mutation.
		 */
		BreedableFeedForwardNeuralNet( FILE *inInputFile,
			RandomSource *inRandSource );
		
		virtual ~BreedableFeedForwardNeuralNet();
		
		/**
		 * Inline access to private members.
		 */
		double getMaxWeight() {
			return mMaxWeight;
			}
		void setMaxWeight( double inMax ) {
			mMaxWeight = inMax;
			}
		
		double getMinWeight() {
			return mMinWeight;
			}
			
		void setMinWeight( double inMin ) {
			mMinWeight = inMin;
			}
		
		double getMaxThreshold( int inLayer ) {
			return mMaxLayerThreshold[inLayer];
			}
		void setMaxThreshold( int inLayer, double inMax ) {
			mMaxLayerThreshold[inLayer] = inMax;
			}
			
		double getMinThreshold( int inLayer ) {
			return mMinLayerThreshold[inLayer];
			}
		void setMinThreshold( int inLayer, double inMin ) {
			mMinLayerThreshold[inLayer] = inMin;
			}
			
			
		/**
		 * Implements the Crossbredable::crossbreed interface.
		 */	
		void *crossbreed( Crossbreedable *inOther, 
			float inFractionOther, float inMutationProb,
			float inMaxMutationMagnitude );
			
		/**
		 * Implements the Crossbredable::mutate interface.
		 */	
		void mutate( float inMutationProb, float maxMutationMagnitude );
	
		
		/**
		 * Gets the random source used by this network.
		 *
		 * @return the random source used by this network.
		 */
		RandomSource *getRandSource() {
			return mRandSource;
			}
		
		/**
		 * Writes this network out to a text file.
		 *
		 * Note that information about what kind of threshold
		 * is being used at each node (i.e., what class was 
		 * potentially passed in for the threshold at each node)
		 * is discareded in this process.
		 *
		 * Reading such a file back in will create a BFFNN with
		 * all threshold functions set to the default, StepThreshold.
		 *
		 * @param inOutputFile the text file to write this network out to.
		 *
		 * @return true iff file was written to successfully.
		 */
		virtual char writeToFile( FILE *inOutputFile );
		
		
		/**
		 * Turns "forward path following" algorithm on and off.
		 *
		 * @param inUseForwardPaths set to true to turn forward path
		 *  following on, set to false to enable standard random
		 *  node selection algorithm.
		 */
		//void setForwardPaths( char inUseForwardPaths ) {
		//	mForwardPathFollowing = inUseForwardPaths;
		//	}
		
		
		/**
		 * Specifies a crossbreeding method.
		 *
		 * @param inCrossBreedingMethod string representing crossbreeding
		 *   method to be used.  One of
		 *   "oneparent"-	take 100% of this parent.
		 *   "forward"-		use forward paths crossbreeding method.
		 *   "randomatom"-	take nodes and their outbound connections
		 *						atomicly randomly from each parent.
		 *   "half"-		take exactly half of network from each parent
		 *						and use the same crossing point every time.
		 *   "randomhalf"-	take a randomly placed half of the network
		 *						(that is, use a different crossing point
		 *						each time, but take contiguous sections
		 *						from each parent)
		 *	"random"-		take a completely random selection of thresholds
		 *						and weights from the two parents.
		 *	"averaging"-	take the average of each threshold and weight
		 *						from each parent
		 *  "bitstring"-	convert each parent to a bit string and cross/mutate
		 *						the strings.
		 */
		void setCrossbreedingMethod( const char *inCrossbreedingMethod );
		
		
		/**
		 * Specify the crossbreeding method using its integer code.
		 */
		void setCrossbreedingMethod( int inCrossbreedingMethod );
		
	private:
		RandomSource *mRandSource;
		
		// range of possible threshold for each layer of the network
		// (note that all neurons in the same layer have thresholds
		// in the same range)
		double *mMaxLayerThreshold;
		double *mMinLayerThreshold;
		
		// range of possible edge weights in network
		double mMaxWeight;
		double mMinWeight;
		
		// true if "forward path following" crossbreeding algorithm
		// should be used
		char mForwardPathFollowing;
		
		int mCrossbreedingMethod;
		/*
		 * 0 = "oneparent"
		 * 1 = "forward"
		 * 2 = "randomatom"
		 * 3 = "half"
		 * 4 = "randomhalf"
		 * 5 = "random"
		 * 6 = "averaging"
		 * 7 = "bitstring"
		 */		
		
		
	}; 




#endif
