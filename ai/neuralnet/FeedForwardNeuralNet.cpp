/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 *
 * 2000-September-17		Jason Rohrer
 * Fixed small memory leak in constructor that involved a temp weight array.
 * Added functions for writing to file and reading back in from file. 
 */


#include "FeedForwardNeuralNet.h"

#include <string.h>

#include <stdio.h>
	

FeedForwardNeuralNet::FeedForwardNeuralNet( int inNumInputs, 
	int inNumHiddenLayers,int *inNeuronsPerLayer, int inNumOutputs ) 
	: mNumHiddenLayers( inNumHiddenLayers ) {
	
	// These can't seem to be initialized in the member initialization list
	// because they're members of a superclass.
	mNumInputs = inNumInputs;
	mNumOutputs = inNumOutputs;
	
	int totalNumLayers = inNumHiddenLayers + 2;
	
	mNeuronsPerLayer = new int[ totalNumLayers ];
	mNeuronsPerLayer[0] = inNumInputs;
	mNeuronsPerLayer[ inNumHiddenLayers + 1 ] = inNumOutputs;
	
	int i;
	//printf( "FFNN inited with %d inputs, %d hidden layers, %d outputs, and ",
	//	inNumInputs, inNumHiddenLayers, inNumOutputs ); 
	for( i=1; i<inNumHiddenLayers + 1; i++ ) {
		//printf( "%d, ", inNeuronsPerLayer[i-1] );
		mNeuronsPerLayer[i] = inNeuronsPerLayer[i-1];
		}
	//printf( " neurons per layer.\n" );
	
	mNeuronThresholds = new Threshold**[totalNumLayers];
	mNeuronThresholdValues = new double*[totalNumLayers];
	mWeights = new double**[totalNumLayers];
	
	// fill in all arrays
	for( i=0; i<totalNumLayers; i++ ) {
		mNeuronThresholds[i] = new Threshold*[ mNeuronsPerLayer[i] ];
		mNeuronThresholdValues[i] = new double[ mNeuronsPerLayer[i] ];
		mWeights[i] = new double*[ mNeuronsPerLayer[i] ];
		
		//  Note that a hot spot in the weight initialization
		//  was located (setting every weight to 0 in the
		//  inner-most loop).
		//  The following code attempts to allocate these
		//  weights in an efficient manner.
		
		// This optimization reduced the execution time of this
		// constructor from 200ms per call down to 1.43ms per call.
		// Definitely worth it!
		
		int neuronsNextLayer = 0;
		if( i<totalNumLayers - 1 ) {
			neuronsNextLayer = mNeuronsPerLayer[i+1];
			}
		// these init values should be copied into the
		// weight arrays for every neuron in this layer
		double *tempWeights = new double[ neuronsNextLayer ];
		for( int k=0; k<neuronsNextLayer; k++ ) {
			tempWeights[k] = 0.0;
			}
			
		for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {
			mNeuronThresholds[i][j] = StepThreshold::getInstance();
			mNeuronThresholdValues[i][j] = 0.0;
			// don't create weight arrays that leave the output neurons
			if( i<totalNumLayers - 1 ) {
				mWeights[i][j] = new double[ neuronsNextLayer ];
				memcpy( mWeights[i][j], tempWeights, 
					neuronsNextLayer * sizeof( double ) );
				}
			}
		delete [] tempWeights;
		}
	}


FeedForwardNeuralNet::FeedForwardNeuralNet( FILE *inInputFile ) {
	
	FILE *inFile = inInputFile;

	// read number of hidden layers from file
	fscanf( inFile, "%d", &mNumHiddenLayers );
	int totalNumLayers = mNumHiddenLayers + 2;
	
	mNeuronsPerLayer = new int[ totalNumLayers ];
	
	mNeuronThresholdValues = new double*[totalNumLayers];
	mNeuronThresholds = new Threshold**[totalNumLayers];
	mWeights = new double**[totalNumLayers];
	
	int i;
	int j;
	int k;
	// read number of neurons per layer in from file
	for( i=0; i<totalNumLayers; i++ ) {
		fscanf( inFile, "%d", &( mNeuronsPerLayer[i] ) );	
		// setup arrays of thresholds and threshold values
		mNeuronThresholds[i] = new Threshold*[mNeuronsPerLayer[i]];
		mNeuronThresholdValues[i] = new double[mNeuronsPerLayer[i]];
		// setup arrays of weights
		mWeights[i] = new double*[mNeuronsPerLayer[i]];
		}

	mNumInputs = mNeuronsPerLayer[0];
	mNumOutputs = mNeuronsPerLayer[totalNumLayers-1];
	
	// read threshold values at each neuron in from file
	for( i=1; i<totalNumLayers; i++ ) {
		for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
			fscanf( inFile, "%lf", &( mNeuronThresholdValues[i][j] ) );
			// generate default threshold functions
			mNeuronThresholds[i][j] = StepThreshold::getInstance();
			}	
		}
	
	// read weights in from file
	for( i=0; i<totalNumLayers-1; i++ ) {
		for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
			// setup last tier of weights array
			mWeights[i][j] = new double[mNeuronsPerLayer[i+1]];
			for( k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
				fscanf( inFile, "%lf", &( mWeights[i][j][k] ) );
				}
			}	
		} 

	}

	
	
FeedForwardNeuralNet::~FeedForwardNeuralNet() {	
	int totalNumLayers = mNumHiddenLayers + 2;
	for( int i=0; i<totalNumLayers; i++ ) {
		// singleton, no need to delete Thresholds		
		if( i<totalNumLayers - 1 ) {
			for( int j=0; j<mNeuronsPerLayer[i]; j++ ) {			
				delete [] mWeights[i][j];
				}
			}
		delete [] mNeuronThresholds[i];
		delete [] mNeuronThresholdValues[i];
		delete [] mWeights[i];
		}
	
	delete [] mNeuronThresholds;
	delete [] mNeuronThresholdValues;
	delete [] mWeights;
		
	delete [] mNeuronsPerLayer;	
	}
	
	
		
void FeedForwardNeuralNet::run( double *inInputs, double *outOutputs ) {
	// values at our current layer in the network
	double *currentValues = new double[ mNeuronsPerLayer[0] ];
	register int i;
	memcpy( (void *)currentValues, 
		(void *)inInputs, mNeuronsPerLayer[0] * sizeof( double ) );
	
	for( i=1; i<mNumHiddenLayers + 2; i++ ) {
		// values at neurons in next layer
		double *newValues = new double[ mNeuronsPerLayer[i] ];
		
		register int numNeuronsThisLayer = mNeuronsPerLayer[i];
		
		register int j;
		// first, zero values at all neurons in next layer for base of sum
		for( j=0; j<numNeuronsThisLayer; j++ ) {
			newValues[j] = 0.0;
			}
		
		// for each neuron in the previous layer
		for( int k=0; k<mNeuronsPerLayer[i-1]; k++ ) {
			
			register double currentValuePrevious = currentValues[k];
			double *weightsLeavingPrevious = mWeights[i-1][k];
			
			//
			
			// OPTIMIZE:
			// Rolling the loop reduced total running time of this
			// function from 288ms per call to 174ms per call
			
			// for each neuron in next layer
			
			/*
			for( j=0; j!=numNeuronsThisLayer; j++ ) {
				newValues[j] += weightsLeavingPrevious[j] *
					currentValuePrevious;
				}
			*/
			
			// roll loop
			for( j=5; j<numNeuronsThisLayer; j+=6 ) {
				register int backj = j-5;
				newValues[backj] += weightsLeavingPrevious[backj] *
					currentValuePrevious;
				backj++;
				newValues[backj] += weightsLeavingPrevious[backj] *
					currentValuePrevious;
				backj++;
				newValues[backj] += weightsLeavingPrevious[backj] *
					currentValuePrevious;
				backj++;
				newValues[backj] += weightsLeavingPrevious[backj] *
					currentValuePrevious;
				backj++;
				newValues[backj] += weightsLeavingPrevious[backj] *
					currentValuePrevious;
				newValues[j] += weightsLeavingPrevious[j] *
					currentValuePrevious;
				}
			// finish remainder
			int startRemainder = j-6;
			if( startRemainder < 0 ) {
				startRemainder = 0;
				}
			for( j=startRemainder; j<numNeuronsThisLayer; j++ ) {
				newValues[j] += weightsLeavingPrevious[j] *
					currentValuePrevious;
				}
			// end rolled loop
				
			}
		
		// now we have weighted sum at each neuron in this layer...
		// need to threshold the sum
		for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
			newValues[j] = mNeuronThresholds[i][j]->
				apply( newValues[j], mNeuronThresholdValues[i][j] ); 
			}
		
		delete [] currentValues;
		
		currentValues = newValues;	
		}
		
	// now copy last set of current values int output array	
	memcpy( (void *)outOutputs, (void *)currentValues,
		mNumOutputs * sizeof( double ) );
	delete [] currentValues;
	}


char FeedForwardNeuralNet::writeToFile( FILE *inOutputFile ) {
	FILE *outFile = inOutputFile;
	fprintf( outFile, "%d\n", mNumHiddenLayers );
	int i;
	int j;
	int k;
	// write number of neurons per layer out to file
	for( i=0; i<mNumHiddenLayers+2; i++ ) {
		fprintf( outFile, "%d\n", mNeuronsPerLayer[i] );	
		}
	
	fprintf( outFile, "\n" );	// for human readability
	
	// write threshold values at each neuron out to file
	for( i=1; i<mNumHiddenLayers+2; i++ ) {
		for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
			fprintf( outFile, "%lf\n", mNeuronThresholdValues[i][j] );
			}	
		}
		
	fprintf( outFile, "\n" );	// for human readability 
	
	// write weights out to file
	for( i=0; i<mNumHiddenLayers+1; i++ ) {
		for( j=0; j<mNeuronsPerLayer[i]; j++ ) {
			for( k=0; k<mNeuronsPerLayer[i+1]; k++ ) {
				fprintf( outFile, "%lf\n", mWeights[i][j][k] );
				}
			fprintf( outFile, "\n" );	// for human readability	
			}	
		fprintf( outFile, "\n\n" );	// for human readability
		} 
	
	fprintf( outFile, "\n" );	// for human readability
		
	return true;
	}
	
	
// simple test function
/*
int main() {
	int *neuronsPerLayer = new int[3];
	neuronsPerLayer[0] = 3;
	neuronsPerLayer[1] = 4;
	neuronsPerLayer[2] = 5;
	
	FeedForwardNeuralNet *testNet = 
		new FeedForwardNeuralNet( 10, 3, neuronsPerLayer, 5 );
	
	double *input = new double[10];
	input[0] = 0;
	input[1] = 2;
	input[2] = 3;
	input[3] = 4;
	input[4] = 5;
	input[5] = 6;
	input[6] = 7;
	input[7] = 8;
	input[8] = 9;
	input[9] = 10;
	
	double *output = new double[5];
	
	testNet->setWeight( 0, 9, 0, 1.0 );
	testNet->setWeight( 1, 0, 0, 1.0 ); 
	testNet->setWeight( 2, 0, 0, 1.0 );
	testNet->setWeight( 3, 0, 4, 1.0 );
	
	testNet->run( input, output );
	
	for( int i=0; i<5; i++ ) {
		printf( "%f ", output[i] );
		}
	
	printf( "\n" );
	
	delete testNet;
	
	return 0;
	}
*/	
