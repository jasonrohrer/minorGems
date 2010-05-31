/*
 * Modification History
 *
 * 2000-September-15		Jason Rohrer
 * Created.
 *
 * 2000-October-12		Jason Rohrer
 * Changed to use PopulationSorter's sort routine.
 */

#include "PopulationTrainer.h"

void PopulationTrainer::sortPopulation( double *inScores ) {
	

	PopulationSorter::sortPopulation( (PopulationMember**)mPopulation, 
		inScores, mPopulationSize );
	/*
	// for now, this uses selection sort, which has
	// an n^2 running time, but it shouldn't matter much...
	
	for( int t=0; t<mPopulationSize; t++ ) {
		double min = inScores[t];
		int minInd = t;
		
		for( int u=t+1; u<mPopulationSize; u++ ) {
			if( inScores[u] < min ) {
				min = inScores[u];
				minInd = u;
				}
			}
		
		if( minInd != t ) {
			// swap t with min
			NeuralNet *temp = mPopulation[minInd];
			mPopulation[minInd] = mPopulation[t];
			inScores[minInd] = inScores[t];
			mPopulation[t] = temp;
			inScores[t] = min;
			}
		}
	*/
	}
