/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 */

#ifndef THRESHOLD_INCLUDED
#define THRESHOLD_INCLUDED


/**
 * Abstract interface for double-precision threshold functions.
 *
 * @author Jason Rohrer 
 */
class Threshold {

	public:
	
		/**
		 * Applies this threshold function to a value.
		 *
		 * @param inValue the value to apply the threshold function to.
		 * @param inThreshold the threshold value to use when
		 *   applying the threshold function.
		 */
		virtual double apply( double inValue, double inThreshold ) = 0;	
	}; 

#endif
