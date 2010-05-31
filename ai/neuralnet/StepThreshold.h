/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 */

#ifndef STEP_THRESHOLD_INCLUDED
#define STEP_THRESHOLD_INCLUDED

#include "Threshold.h"

// can't have pre-initialized non-const static members,
// so make this a global variable... yuck.
static char gStepThresholdSingletonAllocated = false;

// static members don't seem to work with gcc,
// so make this a global variable too... yuck!
static Threshold *gStepThresholdSingleton;


/**
 * Step function implementation of the threshold interface.
 * Uses the singleton pattern.
 *
 * @author Jason Rohrer 
 */
class StepThreshold : public Threshold {

	public:
		/**
		 * Gets the singleton instance of this class.
		 *
		 * @return the singleton instance of this class.
		 */
		static Threshold *getInstance();
		
		/**
		 * Implements the apply interface with a step function
		 * that maps values below inThreshold to 0 and all other
		 * values to 1.
		 */
		double apply( double inValue, double inThreshold );	
	
	private:
		static Threshold *mSingleton;
		static int mInt;
	}; 



inline Threshold *StepThreshold::getInstance() {
	if( !gStepThresholdSingletonAllocated ) {
		gStepThresholdSingleton = new StepThreshold();
		gStepThresholdSingletonAllocated = true;
		}
	return gStepThresholdSingleton;
	}


	
inline double StepThreshold::apply( double inValue, double inThreshold ) {
	if( inValue < inThreshold ) {
		return 0.0;
		}
	else {
		return 1.0;
		}
	}


#endif
