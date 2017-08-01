/*
 * Modification History
 *
 * 2001-October-29		Jason Rohrer
 * Created.
 *
 * 2002-March-13		Jason Rohrer
 * Added include of time.h so that FreeBSD compile will work.
 * Changed to use newer gettimeofday that should work on all unix platforms.
 * Fixed a conversion bug.
 */


#include "minorGems/system/Time.h"


#include <time.h>

#include <sys/time.h>

#include <unistd.h>

#include <stdio.h>


char Time::sEpochTimeSet = false;

// C standard says that -1 is a valid time_t value
time_t Time::sEpochTime = (time_t)( -1 );


void Time::getCurrentTime( timeSec_t *outSeconds,
						   unsigned long *outMilliseconds ) {
	
	struct timeval currentTime;

    gettimeofday( &currentTime, NULL );

    
	*outMilliseconds = currentTime.tv_usec / 1000;
	
    
    *outSeconds = normalize( currentTime.tv_sec );
	}


