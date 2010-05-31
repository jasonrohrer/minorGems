/*
 * Modification History
 *
 * 2000-December-9		Jason Rohrer
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
// runs specified crossbreeding algorithm on set using specified
// mutation settings, and computes the error during the convergence process
int main( char inNumArgs, char **inArgs ) {
	int i;

	/*
	 * Usage:
	 * errorFinder training_set_file crossbreeding_type random_seed 
	 *		stats_file mutation_mag mutation_prob
	 *		averaging_set_size
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
		sscanf( inArgs[5], "%f", &mutationMagnitude );
		}
	if( inNumArgs > 6 ) { 
		sscanf( inArgs[6], "%f", &mutationProb );
		}
	if( inNumArgs > 7 ) { 
		sscanf( inArgs[7], "%d", &averagingSetSize );
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
	
	

	
	// setup stats file
	FILE *statsFile;
	if( inNumArgs > 4 ) { 
		statsFile = fopen( inArgs[4], "w" );
		}
	else {
		statsFile = fopen( "netStats.txt", "w" );
		}
	
	
	float *avgRoundErrors = new float[numRounds];
	for( int r=0; r<numRounds; r++ ) {
		avgRoundErrors[r] = 0;
		}
	
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
		trainer->setTrainingRigor( 0.0 );

		int result = 1;
		result = trainer->train( numRounds, 
			mutationProb, mutationMagnitude,
			roundErrors );


		if( result > 0 ) {
			printf( "Reached training goal before max number of rounds hit.\n" );
			}
		else if( result == -1 ) {
			printf( "Error prevented training from completing.\n" );
			}
		else if( result == 0 ) {
			printf( "Hit rounds limit before reaching training goal.\n" );
			}

		// add errors into averag
		for( int r=0; r<numRounds; r++ ) {
			avgRoundErrors[r] += roundErrors[r];
			}

		delete trainer;

		delete [] roundErrors;

		delete [] neuronsPerLayer;
		for( i=0; i<populationSize; i++ ) {
			delete population[i];
			}
		delete [] population;
		}	// end loop over averaging sets s
	
	// print matlab array code
	fprintf( statsFile, "%sError = [ ", crossbreedingType );
	
	// finish taking average
	for( int r=0; r<numRounds; r++ ) {
		avgRoundErrors[r] = avgRoundErrors[r] / averagingSetSize;
		fprintf( statsFile, "%f; ", avgRoundErrors[r] );
		if( (r+1) % 10 == 0 ) {
			// for human readability
			fprintf( statsFile, "\n" );
			}
		}
	// finish matlab array code
	fprintf( statsFile, "]\n", crossbreedingType );

	fclose( statsFile ); 
	 
	
	delete avgRoundErrors;
	
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
