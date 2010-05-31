/*
 * Modification History
 *
 * 2000-October-12		Jason Rohrer
 * Created.
 *
 * 2000-November-5		Jason Rohrer
 * Added support for sorting in two different orders (increasing
 * and decreasing order).
 */

#include "PopulationSorter.h"

void PopulationSorter::sortPopulation( PopulationMember** inPopulation, 
	double* inScores, int inPopulationSize, char inDecreasingOrder ) {

	// for now, this uses selection sort, which has
	// an n^2 running time, but it shouldn't matter much...
	
	for( int t=0; t<inPopulationSize; t++ ) {
		// best is max in case of decreasing order and
		// min in case of increasing order.
		
		double best = inScores[t];
		int bestInd = t;
		
		for( int u=t+1; u<inPopulationSize; u++ ) {
			if( !inDecreasingOrder && inScores[u] < best
				|| inDecreasingOrder && inScores[u] > best ) {
				best = inScores[u];
				bestInd = u;
				}
			}
		
		if( bestInd != t ) {
			// swap t with min
			PopulationMember *temp = inPopulation[bestInd];
			inPopulation[bestInd] = inPopulation[t];
			inScores[bestInd] = inScores[t];
			inPopulation[t] = temp;
			inScores[t] = best;
			}
		}
	}
