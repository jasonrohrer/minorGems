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



void Time::getCurrentTime( unsigned long *outSeconds,
						   unsigned long *outMilliseconds ) {
	
	struct timeval currentTime;

    gettimeofday( &currentTime, NULL );

    
	*outMilliseconds = currentTime.tv_usec / 1000;
	*outSeconds = currentTime.tv_sec;

	}


