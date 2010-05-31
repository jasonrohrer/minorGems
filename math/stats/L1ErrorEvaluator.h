/*
 * Modification History
 *
 * 2000-September-28		Jason Rohrer
 * Created.
 */
 
#ifndef L1_ERROR_EVALUATOR_INCLUDED
#define L1_ERROR_EVALUATOR_INCLUDED

#include "ErrorEvaluator.h"

/**
 * L1 distance implementation of ErrorEvaluator.
 *
 * @author Jason Rohrer 
 */
class L1ErrorEvaluator : public ErrorEvaluator {
	
	public:
		// implements ErrorEvaluator interface
		double evaluate( double *inVectorA, 
			double *inVectorB, int inLength );
	};

inline double L1ErrorEvaluator::evaluate( double *inVectorA, 
	double *inVectorB, int inLength ) {
	
	double sum = 0;
	for( int i=0; i<inLength; i++ ) {
		double error = inVectorA[i] - inVectorB[i];
		if( error < 0 ) {
			error = -error;
			}
		sum += error;
		}
	return sum;
	}

#endif
