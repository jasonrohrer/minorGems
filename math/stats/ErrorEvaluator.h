/*
 * Modification History
 *
 * 2000-September-28		Jason Rohrer
 * Created.
 */
 
#ifndef ERROR_EVALUATOR_INCLUDED
#define ERROR_EVALUATOR_INCLUDED

/**
 * Interface for a class that evaluates a vector against another
 * to produce a scalar error value.
 *
 * @author Jason Rohrer 
 */
class ErrorEvaluator {
	
	public:
		/**
		 * Evaluates two input vectors to produce an output error value.
		 *
		 * @param inVectorA the first vector (the actual, or "correct" value)
		 * @param inVectorB the second vector ( the predicted, or "test" value)
		 * 
		 * @return the error value between the two vectors
		 */
		virtual double evaluate( double *inVectorA, 
			double *inVectorB, int inLength ) = 0;

	};
	
#endif
