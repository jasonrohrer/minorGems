/*
 * Modification History
 *
 * 2000-September-10		Jason Rohrer
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
 */


#include "BreedableFeedForwardNeuralNet.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// utility functions



/**
 * Mutate a bounded value using a maximum mutation magnitude,
 * where 1.0 will allow mutations as large as the entire range.
 *
 * @param inValue starting value.
 * @param inMaxMutationMagnitude the maximum "severity" of any
 *   individual mutation, in [0..1].
 * @param inRangeMax highest allowable mutated value.
 * @param inRangeMin lowest allowable mutated value.
 * @param inRandSource random number source.
 *
 * @return mutated value
 */
inline double mutateValue( double inValue, double inMaxMutationMagnitude, 
	double inRangeMax, double inRangeMin, RandomSource *inRandSource ) {
	
	// pick a mutated value in the range
	double mutatedValue = inRandSource->getRandomFloat() * 
		( inRangeMax - inRangeMin ) +
		inRangeMin;
				
	// use inMaxMutationMagnitude to take a weighted average
	// from current value and mutatedValue
	return inMaxMutationMagnitude * mutatedValue +
			( 1 - inMaxMutationMagnitude ) * 
			inValue;
	}


/**
 * Pick an unmarked index at random from an array of markers.
 *
 * @param inMarkers array of markers.
 * @param inLength length of the array of markers.
 * @param inRandSource random number source.
 *
 * @return index of a randomly selected unmarked spot,
 *   or -1 if no unmarked spots exist.
 */
inline int pickUnmarkedIndex( char *inMarkers, int inLength,
	RandomSource *inRandSource ) {
	
	// first, pick a random starting index
	int startIndex = (int)( inRandSource->getRandomFloat() * (inLength - 1) );
	
	if( !inMarkers[startIndex] ) {
		return startIndex;
		}
	int nextIndex = ( startIndex + 1 ) % inLength;
	
	// walk forward until an unmarked position found
	// loop if hit end
	while( nextIndex != startIndex ) {
		if( !inMarkers[nextIndex] ) {
			return nextIndex;
			}
		nextIndex = ( nextIndex + 1 ) % inLength;
		}

	// we looped around without finding an unmarked position
	return -1;
	}


/**
 * Pick an unmarked index with corresponding value of maximum magnitude
 * (positive or negative).
 *
 * @param inMarkers array of markers.
 * @param inValues array of values that are marked by markers.
 * @param inLength length of the arrays.
 *
 * @return index of unmarked value with maximum magnitude,
 *   or -1 if no unmarked spots exist.
 */
inline int getIndexMaxMagnitude( char *inMarkers, double *inValues, 
	int inLength ) {
	
	double maxMagnitude = 0;
	int maxIndex = -1;
	for( int i=0; i<inLength; i++ ) {
		if( !inMarkers[i] ) {
			double value = inValues[i];
			if( value < 0 ) {
				value = -value;
				}
			if( value > maxMagnitude ) {
				maxMagnitude = value;
				maxIndex = i;
				}
			}
		}

	return maxIndex;
	}






BreedableFeedForwardNeuralNet::BreedableFeedForwardNeuralNet (
	int inNumInputs, int inNumHiddenLayers,
	int *inNeuronsPerLayer, int inNumOutputs, RandomSource *inRandSource ) 
	: FeedForwardNeuralNet( inNumInputs, inNumHiddenLayers,
	inNeuronsPerLayer, inNumOutputs ), 
	mRandSource( inRandSource ), mMaxWeight(1.0), mMinWeight(-1.0),
	mCrossbreedingMethod( 1 ) {
	
	int totalNumLayers = mNumHiddenLayers + 2;
	
	mMaxLayerThreshold = new double[totalNumLayers];
	mMinLayerThreshold = new double[totalNumLayers];
	
	for( int i=1; i<totalNumLayers; i++ ) {
		mMaxLayerThreshold[i] = mNeuronsPerLayer[i-1];
		mMinLayerThreshold[i] = -mMaxLayerThreshold[i];
		}
	}

BreedableFeedForwardNeuralNet::BreedableFeedForwardNeuralNet( FILE *inInputFile,
	RandomSource *inRandSource ) 
	: FeedForwardNeuralNet( inInputFile ), Crossbreedable( inInputFile ),
	mRandSource( inRandSource ), mCrossbreedingMethod( 1 ) {

	int totalNumLayers = mNumHiddenLayers + 2;
	
	mMaxLayerThreshold = new double[totalNumLayers];
	mMinLayerThreshold = new double[totalNumLayers];

	FILE *inFile = inInputFile;

	// now read max and min weights from file
	fscanf( inFile, "%lf", &mMaxWeight );
	fscanf( inFile, "%lf", &mMinWeight );
	
	// now read max and min thresholds from file
	for( int   i=1; i<totalNumLayers; i++ ) {
		fscanf( inFile, "%lf", &( mMaxLayerThreshold[i] ) );
		fscanf( inFile, "%lf", &( mMinLayerThreshold[i] ) );
		}
	}



BreedableFeedForwardNeuralNet::~BreedableFeedForwardNeuralNet() {
	delete [] mMaxLayerThreshold;
	delete [] mMinLayerThreshold;
	}


char BreedableFeedForwardNeuralNet::writeToFile( FILE *inOutputFile ) {
	char success = FeedForwardNeuralNet::writeToFile( inOutputFile );
	if( !success ) {
		return success;
		}
	success = Crossbreedable::writeToFile( inOutputFile );
	if( !success ) {
		return success;
		}
		
	FILE *outFile = inOutputFile;
	
	// now write max and min weights to file
	fprintf( outFile, "%lf\n", mMaxWeight );
	fprintf( outFile, "%lf\n", mMinWeight );
	
	for( int i=1; i<mNumHiddenLayers+2; i++ ) {
		fprintf( outFile, "%lf\n", mMaxLayerThreshold[i] );
		fprintf( outFile, "%lf\n", mMinLayerThreshold[i] );
		}
	fprintf( outFile, "\n" );	// for human readability
	
	return success;
	}


void BreedableFeedForwardNeuralNet::setCrossbreedingMethod( 
	const char *inCrossbreedingMethod ) {
	
	if( !strcasecmp( "oneparent", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 0;
		}
	else if( !strcasecmp( "forward", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 1;
		}	
	else if( !strcasecmp( "randomatom", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 2;
		}
	else if( !strcasecmp( "half", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 3;
		}
	else if( !strcasecmp( "randomhalf", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 4;
		}
	else if( !strcasecmp( "random", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 5;
		}
	else if( !strcasecmp( "averaging", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 6;
		}
	else if( !strcasecmp( "bitstring", inCrossbreedingMethod ) ) {
		mCrossbreedingMethod = 7;
		}
	else {
		printf( "WARNING:  unknown crossbreeding proceedure set: %s\n", 
			inCrossbreedingMethod );
		}	
	}
	
void BreedableFeedForwardNeuralNet::setCrossbreedingMethod( 
	int inCrossbreedingMethod ) {
	
	mCrossbreedingMethod = inCrossbreedingMethod;
	}


void *BreedableFeedForwardNeuralNet::crossbreed( Crossbreedable *inOther, 
	float inFractionOther, float inMutationProb,
	float inMaxMutationMagnitude ) {
	
	BreedableFeedForwardNeuralNet *otherNet =
			dynamic_cast< BreedableFeedForwardNeuralNet* >( inOther );
	
	// make sure we can crossbreed with this Crossbreedable
	if( otherNet == 0 ) {
		return NULL;
		}
		
	// make sure the other network is the same size as us
	if( mNumHiddenLayers != otherNet->getNumHiddenLayers() ) {
		return NULL;
		}

	int totalNumLayers = mNumHiddenLayers + 2;
	int i;
	for( i=0; i<totalNumLayers; i++ ) {
		if( mNeuronsPerLayer[i] != otherNet->getNumInLayer( i ) ) {
			return NULL;
			}
		}

	// other network is correct size
	

	
	// The crossing proceedure:
	// For each neuron spot in input layer, pick neurons from other
	// network with probability inFractionOther.
	// Then, for layer i+1, select neuron that is connected to 
	// neuron selected in layer i with the weight of greatest 
	// magnitude.  Consider only unmarked neurons when making
	// this selection, an mark all selected neurons.
	// When a neuron is selected, add it to offspring network
	// along with all of its outbound weights.	
	BreedableFeedForwardNeuralNet *offspring =
		new BreedableFeedForwardNeuralNet( mNumInputs, mNumHiddenLayers,
			&( mNeuronsPerLayer[1] ), mNumOutputs, mRandSource );
	
	// offspring uses same crossbreeding proceedure as parents
	offspring->setCrossbreedingMethod( mCrossbreedingMethod );
	
	// set all ranges in offspring to full extent of union
	// of parent ranges
	// This allows parents with different weight ranges to
	// be crossbreed.
	double offspringMaxWeight = otherNet->getMaxWeight();
	if( mMaxWeight > offspringMaxWeight ) {
		offspringMaxWeight = mMaxWeight;
		}
	offspring->setMaxWeight( offspringMaxWeight );
	
	double offspringMinWeight = otherNet->getMinWeight();
	if( mMinWeight < offspringMinWeight ) {
		offspringMinWeight = mMinWeight;
		}
	offspring->setMinWeight( offspringMinWeight );	

	for( i=1; i<totalNumLayers; i++ ) {
		double offspringMaxLayerThreshold = otherNet->getMaxThreshold(i);
		if( mMaxLayerThreshold[i] > offspringMaxLayerThreshold ) {
			offspringMaxLayerThreshold = mMaxLayerThreshold[i];
			}
		offspring->setMaxThreshold( i, offspringMaxLayerThreshold );
		
		double offspringMinLayerThreshold = otherNet->getMinThreshold(i);
		if( mMinLayerThreshold[i] < offspringMinLayerThreshold ) {
			offspringMinLayerThreshold = mMinLayerThreshold[i];
			}
		offspring->setMinThreshold( i, offspringMinLayerThreshold );
		}
	
	// will be turned off for the bit string case only
	char standardMutate = true;
	
	// offspring's ranges have been set up
	
	if( mCrossbreedingMethod == 1 ) {	// forward paths method
		
		// arrays of markers indicating which neuron positions in the offspring
		// network have already been filled
		char **markers = new char*[totalNumLayers];
		for( i=0; i<totalNumLayers; i++ ) {
			markers[i] = new char[ mNeuronsPerLayer[i] ];
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				markers[i][j] = false;
				}
			}
		
		
		// now pick input neuron positions randomly
		int input = pickUnmarkedIndex( markers[0], mNumInputs, mRandSource );
		while( input != -1 ) {
			// we found an unmarked input neuron
			markers[0][input] = true; // mark it

			//copy input neuron into offspring
			// No need to copy threshold attributes, since
			// input neurons have no thresholds
			// Outbound weights are copied inside the loop, see below

			// pick a parent to take this neuron and it's strongest
			// path from
			BreedableFeedForwardNeuralNet *parent;
			if( mRandSource->getRandomFloat() > inFractionOther ) {
				parent = this;
				}
			else {
				parent = otherNet;
				}

			int currentLayerNeuron = input;
			for( int j=1; j<totalNumLayers; j++ ) {
				// for each next layer


				double *nextLayerWeights = new double[ mNeuronsPerLayer[j] ];

				int currentLayer = j-1;

				// copy all weights for current layer neuron
				for( int k=0; k<mNeuronsPerLayer[j]; k++ ) {
					double parentWeight = 
						parent->getWeight( currentLayer, currentLayerNeuron, k );
					offspring->setWeight( currentLayer, currentLayerNeuron, k,
						parentWeight );
					// while we're at it, copy all weights into
					// a temporary array to use when looking for max weight
					nextLayerWeights[k] = parentWeight;
					} 

				// select an unmarked neuron from next layer that currentLayerNeuron
				// is connected to with maximum magnitude weight
				int nextLayerNeuron = getIndexMaxMagnitude( markers[j], 
					nextLayerWeights, mNeuronsPerLayer[j] );

				// make sure an unmarked neuron was found in the next layer
				if( nextLayerNeuron != -1 ) {

					markers[j][nextLayerNeuron] = true;

					//copy next layer neuron into offspring
					offspring->setThreshold( j, nextLayerNeuron, 
						parent->getThreshold( j, nextLayerNeuron ) );
					offspring->setThresholdValue( j, nextLayerNeuron, 
						parent->getThresholdValue( j, nextLayerNeuron ) );

					currentLayerNeuron = nextLayerNeuron;
					}
				else {
					// no unmarked neuron was found in this layer
					// for now, just truncate this path and don't
					// worry about further layers.
					// any unmarked neurons in further layers
					// will be dealt with at the end.  
					j = totalNumLayers;
					}

				delete [] nextLayerWeights;

				}	
			input = pickUnmarkedIndex( markers[0], mNumInputs, mRandSource );
			}

		// now we've dealt with all input neurons.
		// there may be other neurons in further layers that haven't
		// been marked yet, particularly in a case where 
		// |layerA| < |layerA+1|

		// walk through each layer, looking for unmarked neurons
		for( i=1; i<totalNumLayers; i++ ) {

			int unmarked = pickUnmarkedIndex( markers[i], 
				mNeuronsPerLayer[i], mRandSource );
			while( unmarked != -1 ) {
				// found an unmarked neuron in this layer

				// mark it
				markers[i][unmarked] = true;

				// pick a parent to take this neuron from for offspring
				BreedableFeedForwardNeuralNet *parent;
				if( mRandSource->getRandomFloat() > inFractionOther ) {
					parent = this;
					}
				else {
					parent = otherNet;
					}

				//copy neuron into offspring
				offspring->setThreshold( i, unmarked, 
					parent->getThreshold( i, unmarked ) );
				offspring->setThresholdValue( i, unmarked, 
					parent->getThresholdValue( i, unmarked ) );

				// if not output neuron
				if( i!=totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, unmarked, k,
							parent->getWeight( i, unmarked, k ) );
						} 
					}

				// look for another unmarked neuron in this layer
				unmarked = pickUnmarkedIndex( markers[i], 
					mNeuronsPerLayer[i], mRandSource );
				}
			}	

		for( i=0; i<totalNumLayers; i++ ) {
			delete [] markers[i];
			}
		delete [] markers;
		}	// end check for forward path following algorithm
				
/*	else {	
		// use standard random layer selection algorithm instead.
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			// pick a parent to take this *entire layer* from for offspring
			BreedableFeedForwardNeuralNet *parent;
			if( mRandSource->getRandomFloat() > inFractionOther ) {
				parent = this;
				}
			else {
				parent = otherNet;
				}
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				//copy neuron into offspring
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				offspring->setThresholdValue( i, j, 
					parent->getThresholdValue( i, j ) );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, j, k,
							parent->getWeight( i, j, k ) );
						} 
					}
				}
			}
		// end of standard crossing proceedure	
		}	
*/

		
	else if( mCrossbreedingMethod == 0 ) {	
		// use "one parent" crossbreeding procedure
		
		// pick a parent to take this neuron from for offspring
		BreedableFeedForwardNeuralNet *parent;
		if( mRandSource->getRandomFloat() > inFractionOther ) {
			parent = this;
			}
		else {
			parent = otherNet;
			}
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {

				//copy neuron into offspring
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				offspring->setThresholdValue( i, j, 
					parent->getThresholdValue( i, j ) );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, j, k,
							parent->getWeight( i, j, k ) );
						} 
					}
				}
			}
		// end of "oneparent" crossing proceedure	
		}
	else if( mCrossbreedingMethod == 2 ) {	
		// use standard random node selection algorithm instead.
		// (randomatom method)
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				// pick a parent to take this neuron from for offspring
				BreedableFeedForwardNeuralNet *parent;
				if( mRandSource->getRandomFloat() > inFractionOther ) {
					parent = this;
					}
				else {
					parent = otherNet;
					}

				//copy neuron into offspring
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				offspring->setThresholdValue( i, j, 
					parent->getThresholdValue( i, j ) );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, j, k,
							parent->getWeight( i, j, k ) );
						} 
					}
				}
			}
		// end of standard (random atom) crossing proceedure	
		}	
	else if( mCrossbreedingMethod == 3 ) {	
		// use "half" crossbreeding procedure.
		// split network in half and take half from each parent
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				// pick a parent to take this neuron from for offspring
				BreedableFeedForwardNeuralNet *parent;
				// place crossing point deterministicly based on 
				// inFractionOther
				if( j > inFractionOther * mNeuronsPerLayer[i]  ) {
					parent = this;
					}
				else {
					parent = otherNet;
					}

				//copy neuron into offspring
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				offspring->setThresholdValue( i, j, 
					parent->getThresholdValue( i, j ) );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, j, k,
							parent->getWeight( i, j, k ) );
						} 
					}
				}
			}
		// end of "half" crossing proceedure	
		}
	else if( mCrossbreedingMethod == 4 ) {	
		// use standard random half selection algorithm instead.
		
		float parentARangeStart = mRandSource->getRandomFloat() *
			inFractionOther;
		float parentARangeEnd = parentARangeStart + ( 1 - inFractionOther );
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			// pick a parent to take this *entire layer* from for offspring
			BreedableFeedForwardNeuralNet *parent;
			if( mRandSource->getRandomFloat() > inFractionOther ) {
				parent = this;
				}
			else {
				parent = otherNet;
				}
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				if( j < parentARangeStart * mNeuronsPerLayer[i]
					||  j > parentARangeEnd * mNeuronsPerLayer[i] ) {
					parent = otherNet;
					}
				else {
					parent = this;
					}
				
				//copy neuron into offspring
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				offspring->setThresholdValue( i, j, 
					parent->getThresholdValue( i, j ) );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, j, k,
							parent->getWeight( i, j, k ) );
						} 
					}
				}
			}
		// end of standard crossing proceedure	
		}	
	else if( mCrossbreedingMethod == 5 ) {	
		// use compltely random crossbreeding procedure
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				// pick a parent to take this neuron from for offspring
				BreedableFeedForwardNeuralNet *parent;
				if( mRandSource->getRandomFloat() > inFractionOther ) {
					parent = this;
					}
				else {
					parent = otherNet;
					}

				//copy neuron into offspring
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				offspring->setThresholdValue( i, j, 
					parent->getThresholdValue( i, j ) );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						// pick a parent to take this weight from
						if( mRandSource->getRandomFloat() > inFractionOther ) {
							parent = this;
							}
						else {
							parent = otherNet;
							}
						
						offspring->setWeight( i, j, k,
							parent->getWeight( i, j, k ) );
						} 
					}
				}
			}
		// end of completely random crossing proceedure	
		}
	else if( mCrossbreedingMethod == 6 ) {	
		// use "averaging" crossbreeding proceedure
		
		// for each layer
		for( int i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				// pick a parent to take this neuron from for offspring
				BreedableFeedForwardNeuralNet *parent;
				if( mRandSource->getRandomFloat() > inFractionOther ) {
					parent = this;
					}
				else {
					parent = otherNet;
					}

				//copy neuron into offspring
				
				// can't average threshold functions types,
				// so take one randomly
				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				
				// take average
				double averageThresholdValue = 
					( 1 - inFractionOther ) *
						this->getThresholdValue( i, j ) +
					inFractionOther * otherNet->getThresholdValue( i, j );
					
				offspring->setThresholdValue( i, j, averageThresholdValue );

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						// take average
						double averageWeight = 
							( 1 - inFractionOther ) *
								this->getWeight( i, j, k ) +
							inFractionOther * 
								otherNet->getWeight( i, j, k );
						
						offspring->setWeight( i, j, k, averageWeight );
						} 
					}
				}
			}
		// end of "averaging" crossing proceedure	
		}
	else if( mCrossbreedingMethod == 7 ) {	
		// use "bitstring" crossbreeding proceedure
		int i, j;
		
		double *parent1String;
		double *parent2String;
		double *offspringString;
		
		// count the number of doubles that make up a network
		int numDoubles = 0;
		for( int i=0; i<totalNumLayers; i++ ) {
			
			// thresholds in this layer
			numDoubles += mNeuronsPerLayer[i];
			
			if( i != totalNumLayers-1 ) {
				// weights to next layer
				numDoubles += 	mNeuronsPerLayer[i] * mNeuronsPerLayer[i+1];
				}
			}
		parent1String = new double[numDoubles];
		parent2String = new double[numDoubles];
		offspringString = new double[numDoubles];

		int index = 0;
		// for each layer
		for( i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
				parent1String[index] = this->getThresholdValue( i, j );
				parent2String[index] = otherNet->getThresholdValue( i, j );
				index++;

				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						parent1String[index] = this->getWeight( i, j, k );
						parent2String[index] = otherNet->getWeight( i, j, k );
						index++;
						} 
					}
				}
			}
		
		// now we have a bit string representing each parent
		
		// pick a crossing point
		float parent1RangeStart = mRandSource->getRandomFloat() *
			inFractionOther;
		float parent1RangeEnd = parent1RangeStart + ( 1 - inFractionOther );
		
		long lengthInBytes = sizeof( double ) * numDoubles;
		long parent1IndStart = (long)( parent1RangeStart * lengthInBytes );
		long parent1IndEnd = (long)( parent1RangeEnd * lengthInBytes );
		long parent1RangeLength = parent1IndEnd - parent1IndStart;
		
		// first, copy all of parent 2 into offspring
		memcpy( (void *)offspringString, (void *)parent2String,
			lengthInBytes );
		
		// then copy parent 1 range into offspring
		memcpy( (char *)offspringString + parent1IndStart, 
			(char *)parent1String + parent1IndStart,
			parent1RangeLength );		
		
		// now we need to mutate each bit of the offspring
		char *offspringByteString = (char * )offspringString;
		for( i=0; i<lengthInBytes; i++ ) {
			for( int b=0; b<8; b++ ) {
				if( mRandSource->getRandomFloat() <= inMutationProb ) {
					// flip this bit
					// XOR with a string of the form 00001000
					offspringByteString[i] = 
						offspringByteString[i] ^ (1 << b );
					}
				}
			} 
		
		// now need to convert the bit string back into the offspring network
		
		index = 0;
		// for each layer
		for( i=0; i<totalNumLayers; i++ ) {
			
			// for each neuron position in the layer
			for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
				
				// pick a parent to take this neuron 
				// threshold type from for offspring
				BreedableFeedForwardNeuralNet *parent;
				if( mRandSource->getRandomFloat() > inFractionOther ) {
					parent = this;
					}
				else {
					parent = otherNet;
					}

				//copy neuron threshold into offspring

				offspring->setThreshold( i, j, 
					parent->getThreshold( i, j ) );
				
				// copy threshold value from bit string
				offspring->setThresholdValue( i, j, offspringString[index] );
				index++;
				
				// if not output neuron
				if( i != totalNumLayers-1 ) {
					// copy weights leaving this neuron from bit string
					for( int k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
						offspring->setWeight( i, j, k, offspringString[index] );
						index++;
						} 
					}
				}
			}
		// already mutated bit string, so turn standard mutation off
		standardMutate = false;	
		
		delete [] parent1String;
		delete [] parent2String;
		delete [] offspringString;
		
		// end of "bitstring" crossing proceedure	
		}				
	else {
		// error case if none of the above cases catch the crossbreeding type.
		printf( "Error:  unknown crossbreeding type in " );
		printf( "BreedableFeedForwardNeuralNet::crossbreed function.\n" );
		}
	
	if( standardMutate ) {
		offspring->mutate( inMutationProb, inMaxMutationMagnitude );
		offspring->setGeneration( this, otherNet );
		}
		
	//printf( "Offspring generation = %d\n", offspring->getGeneration() );
	return (void *)offspring;
	}
			


void BreedableFeedForwardNeuralNet::mutate( float inMutationProb, 
	float inMaxMutationMagnitude ) {
	
	if( inMutationProb != 0 || inMaxMutationMagnitude != 0 ) {
		int totalNumLayers = mNumHiddenLayers + 2;

		for( int i=0; i<totalNumLayers; i++ ){
			int numInNextLayer = mNeuronsPerLayer[i+1];

			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {

				// decide whether to mutate threshold of
				// this neuron
				if( mRandSource->getRandomFloat() <= inMutationProb ) {

					mNeuronThresholdValues[i][j] = mutateValue( 
						mNeuronThresholdValues[i][j], inMaxMutationMagnitude, 
						mMaxLayerThreshold[i], mMinLayerThreshold[i], 
						mRandSource );
					}

				// brought outside of inner loop as an optimization
				double *weightsThisNeuron = mWeights[i][j];

				if( i<totalNumLayers-1 ) {
					for( int k=0; k<numInNextLayer; k++ ) {

						// decide whether to mutate connection to this other neuron
						if( mRandSource->getRandomFloat() <= inMutationProb ) {
								weightsThisNeuron[k] = mutateValue( 
								weightsThisNeuron[k], inMaxMutationMagnitude, 
								mMaxWeight, mMinWeight, 
								mRandSource );

							}
						}
					}
				}
			}
		}
		// else, don't bother to mutate, since it wouldn't change
		// the network at all anyway
	}			
/*	
#include "StdRandomSource.h"
#include "IdentityThreshold.h"


// simple test function
int main() {
	int *neuronsPerLayer = new int[1];
	
	int numInputs = 1083;
	
	neuronsPerLayer[0] = 1083;
	//neuronsPerLayer[1] = 1083;
	//neuronsPerLayer[2] = 1;
	
	int numOutputs = 1;
	
	RandomSource *randSource = new StdRandomSource();
	
	BreedableFeedForwardNeuralNet *testNet = 
		new BreedableFeedForwardNeuralNet( numInputs, 1, neuronsPerLayer, numOutputs, 
			randSource );
	
	BreedableFeedForwardNeuralNet *testNet2 = 
		new BreedableFeedForwardNeuralNet( 10, 1, neuronsPerLayer, numOutputs, 
			randSource );
	
	double *input = new double[numInputs];
	for( int i=0; i<numInputs; i++ ) {
		input[i] = -1;
		}
	input[0] = 1;
	
	double *output = new double[numOutputs];
	
	testNet->setWeight( 0, 0, 0, 1.0 );
	testNet->setWeight( 1, 0, 0, 35 ); 
	//testNet->setWeight( 2, 0, 0, 1.0 );
	//testNet->setWeight( 3, 0, 4, -100.0 );
	
	testNet2->setWeight( 0, 1, 1, 1.0 );
	testNet2->setWeight( 1, 1, 0, 1.0 ); 
	//testNet2->setWeight( 2, 0, 0, 1.0 );
	//testNet2->setWeight( 3, 0, 0, -80.0 );
	
	for( int o=0; o<numOutputs; o++ ) {
		testNet->setThreshold( 2, o, IdentityThreshold::getInstance() );
		testNet2->setThreshold( 2, o, IdentityThreshold::getInstance() );
		}
	
	
	printf( "test net before breeding\n" );
	testNet->run( input, output );

	for( int i=0; i<numOutputs; i++ ) {
		printf( "%f ", output[i] );
		}
	printf( "\n" );
	
	printf( "test net2 before breeding\n" );
	testNet2->run( input, output );

	for( int i=0; i<numOutputs; i++ ) {
		printf( "%f ", output[i] );
		}
	printf( "\n" );
	
	
/*	long startTime = time( NULL );
	
	int numRounds = 50;
	
	for( int r=0; r<numRounds; r++ ) {
		testNet->run( input, output );
		}
	long endTime = time( NULL );
	printf( "net time for %d running rounds = %d\n", 
		numRounds, endTime - startTime );
*/	
	
	/*
	printf( "test net after breeding\n" );
	testNet->run( input, output );

	for( int i=0; i<numOutputs; i++ ) {
		printf( "%f ", output[i] );
		}
	printf( "\n" );
	
	printf( "test net2 after breeding\n" );
	testNet2->run( input, output );

	for( int i=0; i<numOutputs; i++ ) {
		printf( "%f ", output[i] );
		}
	printf( "\n" );
	
	
	printf( "offspring net after breeding\n" );
	offspring->run( input, output );

	for( int i=0; i<numOutputs; i++ ) {
		printf( "%f ", output[i] );
		}
	printf( "\n" );
	
	/*int numMutations = 10;
	
	for( int m=0; m<numMutations; m++ ) {
		testNet->run( input, output );

		for( int i=0; i<numOutputs; i++ ) {
			printf( "%f ", output[i] );
			}

		printf( "\n" );
		
		testNet->mutate( 0.1, 0.1 );
		}
	
	
	delete testNet;
	delete testNet2;
	
	delete [] input;
	return 0;
	}	
*/
