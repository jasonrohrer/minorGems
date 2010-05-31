/*
 * Modification History
 *
 * 2000-September-9		Jason Rohrer
 * Created.
 */

#ifndef IDENTITY_THRESHOLD_INCLUDED
#define IDENTITY_THRESHOLD_INCLUDED

#include "Threshold.h"


// can't have pre-initialized non-const static members,
// so make this a global variable... yuck.
static char gIdentityThresholdSingletonAllocated = false;

// static members don't seem to work with gcc,
// so make this a global variable too... yuck!
static Threshold *gIdentityThresholdSingleton;

/**
 * Identity function implementation of the threshold interface.
 * Uses the singleton pattern.
 *
 * @author Jason Rohrer 
 */
class IdentityThreshold : public Threshold {

	public:
		/**
		 * Gets the singleton instance of this class.
		 *
		 * @return the singleton instance of this class.
		 */
		static Threshold *getInstance();
		
		/**
		 * Implements the apply interface with an identity function
		 * such that f(n) = n.  Note that inThreshold is ignored.
		 */
		double apply( double inValue, double inThreshold );	
	}; 



inline Threshold *IdentityThreshold::getInstance() {
	if( !gIdentityThresholdSingletonAllocated ) {
		gIdentityThresholdSingleton = new IdentityThreshold();
		gIdentityThresholdSingletonAllocated = true;
		}
	return gIdentityThresholdSingleton;
	}


	
inline double IdentityThreshold::apply( double inValue, double inThreshold ) {
	return inValue;
	}


#endif
