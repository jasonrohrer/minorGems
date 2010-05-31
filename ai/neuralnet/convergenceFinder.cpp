/*
 * Modification History
 *
 * 2000-December-8		Jason Rohrer
 * Created.
 *
 * 2000-December-13		Jason Rohrer  
 * Moved into minorGems. 
 */

#include "BreedingDoubleExampleTrainer.h"
#include "BreedableFeedForwardNeuralNet.h"

#include "DoubleTrainingExample.h"

#include "minorGems/util/random/StdRandomSource.h"

#include "IdentityThreshold.h"

#include <stdio.h>
#include <string.h>

// prototypes:
void loadFromFile( double **inInputs, double *inOutputs, 
	int inNumExamples, int inNumInputs, FILE *inFile );

// how many runs are average together to produce the data
int averagingSetSize = 10;

int numExamples = 100;
int numInputs = 10;

int populationSize = 100;

int numHiddenLayers = 1;

int numRounds = 100;
float mutationProb = 0.0;
float probRangeStart = 0.0;
float probRangeEnd = 1.0;
float mutationProbStepSize = 0.1;

float mutationMagnitude = 0.0;
float magRangeStart = 0.0;
float magRangeEnd = 1.0;
float mutationMagnitudeStepSize = 0.1;

// simple test function
// takes a training set file argument
// runs specified crossbreeding algorithm on set using each mutation
// prob and determines convergence rate.  Runs with each prob for 
// averagingSetSize runs of numRounds each.  Starts with same seed each time.
// determines how many rounds it takes to converge within 10% of goal, or
// stops trying after 100 rounds.
int main( char inNumArgs, char **inArgs ) {
	int i;

	/*
	 * Usage:
	 * convergenceFinder training_set_file crossbreeding_type random_seed 
	 *		stats_file mag_range_start mag_range_end mag_step_size prob_range_start
	 *		prob_range_end prob_step_size
	 */
	 
	unsigned long randSeed = 0;
	if( inNumArgs > 3 ) { 
		sscanf( inArgs[3], "%ld", &randSeed );
		}
	char *crossbreedingType = new char[99];	
	if( inNumArgs > 2 ) { 
		sscanf( inArgs[2], "%s", crossbreedingType );
		}
	else {
		strcpy( crossbreedingType, "forward" );
		}
	printf( "Using %s for crossbreeding.\n", crossbreedingType );

	if( inNumArgs > 5 ) { 
		sscanf( inArgs[5], "%f", &magRangeStart );
		}
	if( inNumArgs > 6 ) { 
		sscanf( inArgs[6], "%f", &magRangeEnd );
		}
	if( inNumArgs > 7 ) { 
		sscanf( inArgs[7], "%f", &mutationMagnitudeStepSize );
		}	
	if( inNumArgs > 8 ) { 
		sscanf( inArgs[8], "%f", &probRangeStart );
		}
	if( inNumArgs > 9 ) { 
		sscanf( inArgs[9], "%f", &probRangeEnd );
		}
	if( inNumArgs > 10 ) { 
		sscanf( inArgs[10], "%f", &mutationProbStepSize );
		}


	double **inputs = new double*[numExamples];
	double *outputs = new double[numExamples];
	
	DoubleTrainingExample **examples = new DoubleTrainingExample*[numExamples];
	
	StdRandomSource *randSource = new StdRandomSource( randSeed );

	FILE *exampleFile = fopen( inArgs[1], "r" );
	
	if( exampleFile == NULL ) {
		printf( "Training example file failed to open for reading\n" );
		return -1;
		}
	
	for( i=0; i<numExamples; i++ ) {
		inputs[i] = new double[numInputs];
		}

	// load training examples from file	
	loadFromFile( inputs, outputs, 
		numExamples, numInputs, exampleFile );
	
	for( i=0; i<numExamples; i++ ) {
		examples[i] = new DoubleTrainingExample( inputs[i], &(outputs[i]), 
			numInputs, 1 );
		}
	
	
	int numMutationProbSteps = (int)( 
		( probRangeEnd - probRangeStart ) / mutationProbStepSize );
	int numMutationMagnitudeSteps = 
		(int)( ( magRangeEnd - magRangeStart ) / mutationMagnitudeStepSize );
	
	// average convergence rate for each mutation setting
	float **averageConvergenceRate = new float*[numMutationMagnitudeSteps];
	for( i=0; i<numMutationMagnitudeSteps; i++ ) {
		averageConvergenceRate[i] = new float[numMutationProbSteps];
		for( int j=0; j<numMutationProbSteps; j++ ) {
			averageConvergenceRate[i][j] = 0;
			}
		}
	
	float minConvergenceRate = 200.0;
	int mMin = -1;
	int pMin = -1;
	
	
	// setup stats file
	FILE *statsFile;
	if( inNumArgs > 4 ) { 
		statsFile = fopen( inArgs[4], "w" );
		}
	else {
		statsFile = fopen( "netStats.txt", "w" );
		}
	
	
	// for each mutation setting
	for( int m=1; m<=numMutationMagnitudeSteps; m++ ) {
		float currentMutationMagnitude = 
			magRangeStart + m * mutationMagnitudeStepSize;
		for( int p=1; p<=numMutationProbSteps; p++ ) {
			float currentMutationProb = probRangeStart + 
				p * mutationProbStepSize;
			
			printf( "mutation magnitude = %f, mutation prob = %f\n", 
				currentMutationMagnitude, currentMutationProb );
			
			// reseed rand source so that it starts the same
			// for each mutation setting
			// ( note that the random generator is in a different state
			//  for each run in the averaging set, however )
			delete randSource;
			randSource = new StdRandomSource( randSeed );
			
			// for each run
			for( int s=0; s<averagingSetSize; s++ ) {
				printf( "run %d\n", s );
				
				// construct population
				int *neuronsPerLayer = new int[numHiddenLayers];
				for( i=0; i<numHiddenLayers; i++ ) {
					neuronsPerLayer[i] = numInputs;
					}

				BreedableFeedForwardNeuralNet **population = 
					new BreedableFeedForwardNeuralNet*[populationSize];

				for( i=0; i<populationSize; i++ ) {
					population[i] = new	BreedableFeedForwardNeuralNet( numInputs, 
						numHiddenLayers, neuronsPerLayer, 1, randSource );

					population[i]->mutate( 1.0, 1.0 );

					population[i]->setCrossbreedingMethod( crossbreedingType );
					}
				
				float *roundErrors = new float[numRounds];
				for( int r=0; r<numRounds; r++ ) {
					roundErrors[r] = 0;
					}
				
				// now create a trainer
				BreedingDoubleExampleTrainer *trainer = 
					new BreedingDoubleExampleTrainer();
				trainer->setPopulation( (NeuralNet**)population, populationSize );
				trainer->setExamples( (TrainingExample**)examples, numExamples );
				trainer->setTrainingRigor( 10.0 );
				
				int result = 1;
				result = trainer->train( numRounds, 
					currentMutationProb, currentMutationMagnitude,
					roundErrors );
				
				
				if( result > 0 ) {
					printf( "Reached training goal before max number of rounds hit.\n" );
					averageConvergenceRate[m-1][p-1] += result;
					}
				else if( result == -1 ) {
					printf( "Error prevented training from completing.\n" );
					}
				else if( result == 0 ) {
					printf( "Hit rounds limit before reaching training goal.\n" );
					averageConvergenceRate[m-1][p-1] += numRounds;
					}
				
				delete trainer;
				
				delete [] roundErrors;
				
				delete [] neuronsPerLayer;
				for( i=0; i<populationSize; i++ ) {
					delete population[i];
					}
				delete [] population;
				}	// end loop over averaging sets s
			
			// finish taking average	
			averageConvergenceRate[m-1][p-1] = averageConvergenceRate[m-1][p-1] / 
				averagingSetSize;
			
			if( averageConvergenceRate[m-1][p-1] < minConvergenceRate ) {
				mMin = m;
				pMin = p;
				minConvergenceRate = averageConvergenceRate[m-1][p-1];
				}
			
			
			// write to stats file
			fprintf( statsFile, "%f ", averageConvergenceRate[m-1][p-1] );		
			}	// end loop over p
		// move to next row of matrix
		fprintf( statsFile, ";\n" );	
		}	// end loop over m
	
	fprintf( statsFile, "Min convergence rate of %f for magnitude %f and prob %f\n",
		minConvergenceRate, magRangeStart + mMin * mutationMagnitudeStepSize, 
		probRangeStart + pMin * mutationProbStepSize );
	
	fclose( statsFile ); 
	 
	
	for( i=0; i<numMutationMagnitudeSteps; i++ ) {
		delete [] averageConvergenceRate[i];
		}
	delete averageConvergenceRate;
	for( i=0; i<numExamples; i++ ) {
		delete examples[i];
		delete [] inputs[i];
		} 
	delete inputs;
	delete outputs;
	delete examples;
	delete randSource;
	}
	 
/**
 * Loads training example vectors from a test file.
 */
void loadFromFile( double **inInputs, double *inOutputs, 
	int inNumExamples, int inNumInputs, FILE *inFile ) {

	int i,j;
	
	printf( "Loading inputs\n" );
	// first, scan in all input vectors
	for( i=0; i<inNumExamples; i++ ) {
		for( j=0; j<inNumInputs; j++ ) {
			 fscanf( inFile, "%lf", &(inInputs[i][j]) );	
			//printf( "%lf ", inInputs[i][j] );
			}
		//printf( ";\n" );	
		// scan the ";" following each example's input vector
		char *tempString = new char[99];
		fscanf( inFile, "%s", tempString );
		delete [] tempString;
		}
	
	printf( "Loading outputs\n" );
	// then, scan in all output values
	for( i=0; i<inNumExamples; i++ ) {
		fscanf( inFile, "%lf", &(inOutputs[i]) );
		
		// scan the ";" following each example's output
		char *tempString = new char[99];
		fscanf( inFile, "%s", tempString );
		delete [] tempString;
		
		//printf( "%lf ", inOutputs[i] );
		}
	
	}	 
