/*
 * Modification History
 *
 * 2001-October-29		Jason Rohrer
 * Created.
 *
 * 2004-October-14		Jason Rohrer
 * Fixed sign bug.
 *
 * 2005-February-10		Jason Rohrer
 * Added function to get time in floating point format.
 */

#include "minorGems/common.h"



#ifndef TIME_INCLUDED
#define TIME_INCLUDED 



/**
 * Interface for platform-independent, high-resolution time access.
 *
 * @author Jason Rohrer
 */
class Time {
	public:


		
		/**
		 * Gets the current time in seconds and milliseconds.
		 *
		 * No guarentee about when absolute 0 of this time
		 * scale is for particular systems.
		 *
		 * @param outSeconds pointer to where the time in seconds
		 *   will be returned.
		 * @param outMilliseconds pointer to where the extra
		 *   milliseconds will be returned.  Value returned is in [0,999].
		 */
		static void getCurrentTime( unsigned long *outSeconds,
									unsigned long *outMilliseconds );

        

        /**
         * Gets the current time in fractional (double) seconds.
         *
         * @return the current time in seconds.
         */
        static double getCurrentTime();

        

		/**
		 * Gets the number of milliseconds that have passed
		 * since a time in seconds and milliseconds.
		 *
		 * @param inSeconds the start time, in seconds.
		 * @param inMilliseconds the start time's additional milliseconds.
		 *
		 * @return the number of milliseconds that have passed
		 *   since inSeconds:inMilliseconds.  May overflow if
		 *   more than 49 days have passed (assuming 32-bit longs).
		 */
		static unsigned long getMillisecondsSince(
			unsigned long inSeconds, unsigned long inMilliseconds );


		
	};



inline double Time::getCurrentTime() {
    unsigned long currentTimeS;
	unsigned long currentTimeMS;
	getCurrentTime( &currentTimeS, &currentTimeMS );

    return currentTimeS + currentTimeMS / 1000.0;
    }



inline unsigned long Time::getMillisecondsSince(
	unsigned long inSeconds, unsigned long inMilliseconds ) {

	unsigned long currentTimeS;
	unsigned long currentTimeMS;
	getCurrentTime( &currentTimeS, &currentTimeMS );

	
	unsigned long deltaS = ( currentTimeS - inSeconds );
	long deltaMS = ( (long)currentTimeMS - (long)inMilliseconds );

	// carry, if needed
	if( deltaMS < 0 ) {
		deltaS--;
		deltaMS += 1000;
		}

	return 1000 * deltaS + deltaMS;
	}



#endif
 

