// Jason Rohrer
// StdRandomSource.h

/**
*
*	Implementation of random number generation that uses stdlib calls
*
*
*	Created 12-7-99
*	Mods:
*   	Jason Rohrer	9-28-2000	Added a getRandomBoundedInt()
*									implementation
*   	Jason Rohrer	12-7-2000	Overloaded constructor to support
*									specifying a seed.
*   	Jason Rohrer	12-16-2000	Added a getRandomDouble() function.
*   	Jason Rohrer	12-17-2000	Fixed bug in initialization of invDMAX
*									in default constructor.
*   	Jason Rohrer	9-13-2001	Fixed a bug in getRandomBoundedInt()
*									as floats were being used, and they
*									don't provide enough resolution.
*   	Jason Rohrer	10-11-2002	Fixed some type casting warnings.
*   	Jason Rohrer	07-09-2006	Added getRandomBoundedDouble.
*   	Jason Rohrer	07-27-2006	Added getRandomBoolean.
*/

#include "minorGems/common.h"



#ifndef STD_RANDOM_SOURCE_INCLUDED
#define STD_RANDOM_SOURCE_INCLUDED

#include <stdlib.h>
#include <limits.h>
#include "RandomSource.h"

class StdRandomSource : public RandomSource {

	public:		
		
		StdRandomSource();		// needed to seed stdlib generator
		
		// specify the seed for the stdlib generator
		StdRandomSource( unsigned int inSeed );
			
			
		// implements these functions
		float getRandomFloat();	// in interval [0,1.0]
		double getRandomDouble(); // in interval [0,1.0]
		unsigned int getRandomInt();		// in interval [0,MAX]
		unsigned int getIntMax();	// returns MAX
        
		int getRandomBoundedInt( int inRangeStart,
			int inRangeEnd );	

        double getRandomBoundedDouble( double inRangeStart,
                                       double inRangeEnd );
        char getRandomBoolean();

        
	private:
		double mInvMAXPlusOne; //  1 / ( MAX + 1 )
	};



inline StdRandomSource::StdRandomSource() {
	MAX = RAND_MAX;
	
    srand( (unsigned int)fmod( Time::timeSec(), UINT_MAX ) );
	invMAX = (float)1.0 / ((float)MAX);
	invDMAX = 1.0 / ((double)MAX);
	mInvMAXPlusOne = 1.0 / ( ( (float)MAX ) + 1.0 );
	}



inline StdRandomSource::StdRandomSource( unsigned int inSeed ) {
	MAX = RAND_MAX;
	srand( inSeed );
	invMAX = (float)1.0 / ((float)MAX);
	invDMAX = 1.0 / ((double)MAX);
	mInvMAXPlusOne = 1.0 / ( ( (double)MAX ) + 1.0 );
	}
	


inline float StdRandomSource::getRandomFloat() {
	
	return (float)(rand()) * invMAX;
	}



inline double StdRandomSource::getRandomDouble() {
	
	return (double)(rand()) * invDMAX;
	}



inline unsigned int StdRandomSource::getRandomInt() {
	
	return rand();
	}

inline unsigned int StdRandomSource::getIntMax() {
	
	return MAX;
	}

inline int StdRandomSource::getRandomBoundedInt( int inRangeStart,
	int inRangeEnd ) {
	
	// float in range [0,1)
	double randFloat = (double)( rand() ) * mInvMAXPlusOne;

	int onePastRange = inRangeEnd + 1;

	int magnitude = (int)( randFloat * ( onePastRange - inRangeStart ) );
	
	return magnitude + inRangeStart;
	}


inline double StdRandomSource::getRandomBoundedDouble( double inRangeStart,
	double inRangeEnd ) {
	
	// double in range [0,1]
	double randDouble = getRandomDouble();

	double magnitude = randDouble * ( inRangeEnd - inRangeStart );
	
	return magnitude + inRangeStart;
	}



inline char StdRandomSource::getRandomBoolean() {

    // float in range [0,1]
	double randFloat = getRandomFloat();

    if( randFloat < 0.5 ) {
        return true;
        }
    else {
        return false;
        }
	}


#endif
