/*
 * Modification History
 *
 * 2000-October-23		Jason Rohrer
 * Created.
 */
 
#include "PopulationBreeder.h"
#include <math.h> 
 
void PopulationBreeder::breedPopulation( Crossbreedable** inPopulation,
	int inPopulationSize,
	float inMutationProb, float inMaxMutationMagnitude, 
	PopulationDeleter *inDeleter ) {

	// crossbreed top portion of the population to replace the bottom
	// portion.
	
	// INCORRECT:
	// Pick if there are n members of population, pick m to breed such that
	// m + m + m-1 + m-2 + m-3 + ... + 1 = n
	// m + (m(m+1))/2 = n
	// 2m + m(m+1) = 2n
	// m^2 + 3m - 2n = 0
	// m = ( -3 +/- ( 9 + 8n )^(1/2) ) / 2
	//int numToBreed = (int)( (-3 + sqrt( 9 + 8 * inPopulationSize ) ) * 0.5 );
	
	// CORRECT:
	// Pick if there are n members of population, pick m to breed such that
	// m + m-1 + m-2 + m-3 + ... + 1 = n
	// (m(m+1))/2 = n
	// m(m+1) = 2n
	// m^2 + m - 2n = 0
	// m = ( -1 +/- ( 1 + 8n )^(1/2) ) / 2
	int numToBreed = (int)( (-1 + sqrt( 1 + 8 * inPopulationSize ) ) * 0.5 );
	int remainder = inPopulationSize - 
			(numToBreed * ( numToBreed + 1 ) ) / 2;
	int startIndReplace = numToBreed + remainder;
	
	//printf( "numToBreed=%d, remainder=%d, startIndReplace=%d\n", 
	//	numToBreed, remainder, startIndReplace );
	
	int indNextReplace = startIndReplace;

	printf( "Population size = %d, numToBreed = %d\n", inPopulationSize,
		numToBreed );

	for( int a=0; a<numToBreed; a++ ) {
		for( int b=a+1; b<numToBreed; b++ ) {
			//printf( "breeding %d with %d\n", a, b );
			//printf( "deleting %d\n", indNextReplace );
			// delete member in bottom segment of population
			//inDeleter->deleteCrossbreedable( inPopulation[indNextReplace] );
			inDeleter->deleteCrossbreedable( indNextReplace );
			
			void *offspring = 
				inPopulation[a]->crossbreed( inPopulation[b], 
					0.5f, inMutationProb,
					inMaxMutationMagnitude );	
			
			//int generation = inPopulation[a]->getGeneration();
			//printf( "generation = %d\n", generation );
			// insert offspring into population
			//inPopulation[indNextReplace] = (Crossbreedable *)offspring;
			inDeleter->addCrossbreedable( indNextReplace, offspring );
			
			indNextReplace++;
			}
		}
	}
