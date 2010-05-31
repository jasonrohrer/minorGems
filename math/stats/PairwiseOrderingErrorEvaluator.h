/*
 * Modification History
 *
 * 2000-September-28		Jason Rohrer
 * Created.
 */
 
#ifndef PAIRWISE_ORDERING_ERROR_EVALUATOR_INCLUDED
#define PAIRWISE_ORDERING_ERROR_EVALUATOR_INCLUDED

#include "ErrorEvaluator.h"

/**
 * Implementation of ErrorEvaluator that generates an error
 * value based on the number of pairs of elements in the
 * the test vector that are not given the same order by their values
 * as are the corresponding pair of values in the correct vector.
 *
 * I.e., if A[i] > A[j], then B[i] < B[j] adds 1 to the error term,
 * while B[i] > B[j] adds 0 to the error term.
 *
 * @author Jason Rohrer 
 */
class PairwiseOrderingErrorEvaluator : public ErrorEvaluator {
	
	// implements ErrorEvaluator interface
	double evaluate( double *inVectorA, 
		double *inVectorB, int inLength );

	};

inline double PairwiseOrderingErrorEvaluator::evaluate( double *inVectorA, 
	double *inVectorB, int inLength ) {
	
	double sum = 0;
	// examine all pairs of components in the vectors
	for( int i=0; i<inLength; i++ ) {
		for( int j=i+1; j<inLength; j++ ) {
			if( ( inVectorA[i] > inVectorA[j] &&
				inVectorB[i] <= inVectorB[j] ) ||
				( inVectorA[i] < inVectorA[j] &&
				inVectorB[i] >= inVectorB[j] ) ) {
				
				sum++;
				}
			}
		}
	return sum;
	}

#endif
