/*
 * Modification History
 *
 * 2000-September-15		Jason Rohrer
 * Created.
 *
 * 2000-September-28		Jason Rohrer
 * Changed so that it only handles training of single-output networks,
 * which simplifies much of the internal code.
 * Switched to using ErrorEvaluators to compute error for each network
 * in a more modular way.
 *
 * 2000-October-28		Jason Rohrer
 * Fixed bugs in calculation for numToBreed breeding fraction. 
 *
 * 2000-December-7		Jason Rohrer
 * Commented out code for writing best network to file.
 * Changed so that train function takes an array in which errors througout
 * training are stored.
 *
 * 2000-December-8		Jason Rohrer
 * Changed so that train function returns num rounds required if training
 * successful.
 *
 * 2000-December-13		Jason Rohrer  
 * Moved into minorGems.
 */
 
#include "BreedingDoubleExampleTrainer.h"

#include "minorGems/math/stats/L1ErrorEvaluator.h"
 
#include <math.h>
#include <stdio.h>
 
 
BreedingDoubleExampleTrainer::BreedingDoubleExampleTrainer()
	: mErrorEvaluator( new L1ErrorEvaluator() ), mEvaluatorPassedIn( false ) {
	
	mBestFileName = "test.net";
	}
	
BreedingDoubleExampleTrainer::BreedingDoubleExampleTrainer( 
	ErrorEvaluator *inErrorEvaluator )
	: mErrorEvaluator( inErrorEvaluator ), mEvaluatorPassedIn( true ) {
	
	mBestFileName = "test.net";
	}
	
BreedingDoubleExampleTrainer::~BreedingDoubleExampleTrainer() {
	if( !mEvaluatorPassedIn ) {
		delete mErrorEvaluator;
		}
	}
 
int BreedingDoubleExampleTrainer::train( int inMaxNumRounds, 
 	float inMutationProb, float inMaxMutationMagnitude, 
	float *inOutErrors ) {
 	
	int errorReturn = -1;
	
	int r;
	int n;
	int e;
	
	// for each round
	for( r=0; r<inMaxNumRounds; r++ ) {
		
		// total error over all examples for each network in the population
		double *populationError = new double[ mPopulationSize ];
		
		// correct output values from each training example
		double *correctOutputs = new double[ mExampleSetSize ];
		
		// for each training example
		for( e=0; e<mExampleSetSize; e++ ) { 
			DoubleTrainingExample *example =
				dynamic_cast < DoubleTrainingExample* >( mExamples[e] );
			if( example == 0 ) {
				printf( "Casting a training example to a ");
				printf( "DoubleTrainingExample failed.\n" );
				return errorReturn;
				}
			
			correctOutputs[e] = example->getOutput(0);
			}
		
		// for each network in the population
		for( n=0; n<mPopulationSize; n++ ) {
			
			populationError[n] = 0.0;
			
			// note that this repeated casting is inefficient,
			// but it preserves the abstraction, since we want to always
			// be working with the array provided by PopulationTrainer
			
			// cast the network to a BreedableFeedForwardNeuralNet
			BreedableFeedForwardNeuralNet *network = 
				dynamic_cast
				< BreedableFeedForwardNeuralNet* >( mPopulation[n] );
			if( network == 0 ) {
				// cast failed.
				printf( "Casting a network in population to a ");
				printf( "BreedableFeedForwardNeuralNet failed.\n" );
				return errorReturn;
				}
			
			// output from this network on each example
			// note that this only works for single output networks
			double *individualOutput = new double[ mExampleSetSize ];
			
			// for each training example
			for( e=0; e<mExampleSetSize; e++ ) {
				DoubleTrainingExample *example =
					dynamic_cast < DoubleTrainingExample* >( mExamples[e] );
				if( example == 0 ) {
					printf( "Casting a training example to a ");
					printf( "DoubleTrainingExample failed.\n" );
					return errorReturn;
					}
				
				int numOutputs = example->getNumOutputs();
				
				double *networkOutputs = new double[numOutputs];
				
				network->run( example->getInputs(), networkOutputs );
				
				// only take first output, since we're only supposed
				// to work with single-output networks anyway
				individualOutput[e] = networkOutputs[0];
				
			/*	double sumCorrectOutputs = 0;
				double error = 0;
				for( int o=0; o<numOutputs; o++ ) {
					double thisOutput = example->getOutput(o);
					double thisError = thisOutput - networkOutputs[o];
					/*if( n== mPopulationSize -1 ) {
						printf( "Correct output = %f, good network output ",
							thisOutput );
						printf( "= %f\n", networkOutputs[o] );
						}
					*/
			/*		if( thisOutput < 0 ) {
						thisOutput = -thisOutput;
						}
					sumCorrectOutputs += thisOutput;
					
					if( thisError < 0 ) {
						thisError = -thisError;
						}
					error += thisError;
			
					}
			*/
				//printf( "error=%f   ", error );
				//populationError[n] += error;// / sumCorrectOutputs);
				
				delete [] networkOutputs;
				}
			
			// now use ErrorEvaluator function to compute the error value
			// over all training examples for this network in the population
			populationError[n] = mErrorEvaluator->evaluate( 
									correctOutputs, individualOutput, mExampleSetSize );
			/*
			if( n == mPopulationSize-1 ) {
				printf( "best network score output:\n" );
				for( e=0; e<mExampleSetSize; e++ ) {
					printf( "%f  ", individualOutput[e] );
					}
				printf( "\n" );
				
				printf( "correct score output:\n" );
				for( e=0; e<mExampleSetSize; e++ ) {
					printf( "%f  ", correctOutputs[e] );
					}
				printf( "\n" );
				}
			*/		
			delete [] individualOutput;
				
			// take average error;
			// skip for now... total error may be more useful
			//populationError[n] = populationError[n] / mExampleSetSize;
			}
		
		// now have total error computed for each member of the population
		
		sortPopulation( populationError ); 
		
		Crossbreedable *crossbreedableTopNetwork =
			dynamic_cast<Crossbreedable *>( mPopulation[0] );
		if( crossbreedableTopNetwork == 0 ) {
			// cast failed.
			printf( "Casting a network in population to a ");
			printf( "Crossbreedable failed.\n" );
			return errorReturn;
			}
		
		int generationTop = crossbreedableTopNetwork->getGeneration();
		
		if( r%10 == 0 ) {
			printf( "Round %d, top member (generation %d) error = %f\n", 
				r, generationTop, populationError[0] );
			}
		
		// store error for this round into passed-in array
		inOutErrors[r] = populationError[0];

		/*printf( "Other errors: " );
		for( int p=1; p<mPopulationSize; p++ ) {
			printf( "%f, ", populationError[p] );
			}
		*/	
		BreedableFeedForwardNeuralNet *toFileTopNetwork =
			dynamic_cast<BreedableFeedForwardNeuralNet *>( mPopulation[0] );
		if( toFileTopNetwork == 0 ) {
			// cast failed.
			printf( "Casting a network in population to a ");
			printf( "BreedableFeedForwardNeuralNet failed.\n" );
			return errorReturn;
			}
		/*
		printf( "writing best network out to file..." );
		FILE *outputBest = fopen( mBestFileName, "w" );
		toFileTopNetwork->writeToFile( outputBest );
		fclose( outputBest );
		printf( "done.\n" );
		*/
		/*printf( "Round %d, member errors =\n", r );
		for( n=0; n<mPopulationSize; n++ ) {
			printf( "%f  ", populationError[n] );
			}
		printf( "\n" );
		*/
		//check if requirement has been reached
		if( populationError[0] <= mTrainingRigor ) {
			return r;
			}
		
		// crossbreed top portion of the population to replace the bottom
		// portion.
		
		// INCORRECT:
		// Pick if there are n members of population, pick m to breed such that
		// m + m + m-1 + m-2 + m-3 + ... + 1 = n
		// m + (m(m+1))/2 = n
		// 2m + m(m+1) = 2n
		// m^2 + 3m - 2n = 0
		// m = ( -3 +/- ( 9 + 8n )^(1/2) ) / 2
		//int numToBreed = (int)( (-3 + sqrt( 9 + 8 * mPopulationSize ) ) * 0.5 );
		
		// CORRECT:
		// Pick if there are n members of population, pick m to breed such that
		// m + m-1 + m-2 + m-3 + ... + 1 = n
		// (m(m+1))/2 = n
		// m(m+1) = 2n
		// m^2 + m - 2n = 0
		// m = ( -1 +/- ( 1 + 8n )^(1/2) ) / 2
		int numToBreed = (int)( (-1 + sqrt( 1 + 8 * mPopulationSize ) ) * 0.5 );
		int remainder = mPopulationSize - 
			( numToBreed + (numToBreed * ( numToBreed + 1 ) ) / 2 );
		int startIndReplace = numToBreed + remainder;
		
		int indNextReplace = startIndReplace;
		
		//printf( "Population size = %d, numToBreed = %d\n", mPopulationSize,
		//	numToBreed );
		
		for( int a=0; a<numToBreed; a++ ) {
			for( int b=a+1; b<numToBreed; b++ ) {
				// delete network in bottom segment of population
				BreedableFeedForwardNeuralNet *networkToDelete = 
					dynamic_cast
					< BreedableFeedForwardNeuralNet* >( mPopulation[indNextReplace] );
				if( networkToDelete == 0 ) {
					// cast failed.
					printf( "Casting a network in population to a ");
					printf( "BreedableFeedForwardNeuralNet failed.\n" );
					return errorReturn;
					}
				delete networkToDelete;

				BreedableFeedForwardNeuralNet *network =
					dynamic_cast
					< BreedableFeedForwardNeuralNet* >( mPopulation[a] );
				BreedableFeedForwardNeuralNet *otherNetwork =
					dynamic_cast
					< BreedableFeedForwardNeuralNet* >( mPopulation[b] );
				
				if( network == 0 || otherNetwork == 0 ) {
					// cast failed.
					printf( "Casting a network in population to a ");
					printf( "BreedableFeedForwardNeuralNet failed.\n" );
					return errorReturn;
					}
				
				int neuronsPerLayer = 1;
				void *offspring = 
					//new	BreedableFeedForwardNeuralNet( 10, 
					//	1, &neuronsPerLayer, 1, network->getRandSource() );
					network->crossbreed( otherNetwork, 
						0.5f, inMutationProb,
						inMaxMutationMagnitude );	
				
				// insert offspring into population
				mPopulation[indNextReplace] = (NeuralNet *)offspring;
				
				/*
				//Sanity check:
				// Does error improve if we just mutate completely
				// without crossbreeding?
				
				BreedableFeedForwardNeuralNet *replacedNetwork =
					dynamic_cast
					< BreedableFeedForwardNeuralNet* >( 
						mPopulation[indNextReplace] );
				replacedNetwork->mutate( 1.0, 1.0 );		
				*/
				indNextReplace++;
				}
			}
		
		
		delete [] populationError;
		delete [] correctOutputs;
		
		}
 	return 0;
 	}
