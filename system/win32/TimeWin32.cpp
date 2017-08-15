/*
 * Modification History
 *
 * 2001-November-7		Jason Rohrer
 * Created.
 *
 * 2002-April-11		Jason Rohrer
 * Added missing include, and fixed a bug.
 *
 * 2004-January-29		Jason Rohrer
 * Fixed so that 0-point of time is the same as on other platforms.
 *
 * 2004-October-14		Jason Rohrer
 * Fixed bug in second/millisecond callibration.
 * Fixed bug in win32 time to ANSI time translation.
 * Fixed daylight savings time bug.
 */


#include "minorGems/system/Time.h"

#include <windows.h>
#include <winbase.h>
#include <time.h>
#include <stdio.h>


char Time::sEpochTimeSet = false;

// C standard says that -1 is a valid time_t value
time_t Time::sEpochTime = (time_t)( -1 );



/**
 * Windows implementation of Time.h.
 *
 * The 0-point should match the ANSI standard.
 */



void Time::getCurrentTime( timeSec_t *outSeconds,
						   unsigned long *outMilliseconds ) {
    // convert from win32 broken-down time (which has msec resolution)
    // to an ANSI time struct and then convert to an absolute time in
    // seconds
    // This procedure ensures that the 0-point matches the ANSI standard.

    // note:
    // we cannot simply call ANSI time() to get the seconds and then rely
    // on GetLocalTime to get the milliseconds, since the seconds value
    // used by GetLocalTime is (strangely enough) not calibrated to the seconds
    // value of time().
    // In other words, it is possible for the time() seconds to advance
    // at a different clock cycle than the GetLocalTime seconds.

    // get time using a win32 call
    SYSTEMTIME win32TimeStruct;
    GetLocalTime( &win32TimeStruct );

    // convert this win32 structure to the ANSI standard structure
    struct tm ansiTimeStruct;

    ansiTimeStruct.tm_sec = win32TimeStruct.wSecond;
    ansiTimeStruct.tm_min = win32TimeStruct.wMinute;
    ansiTimeStruct.tm_hour = win32TimeStruct.wHour;
    ansiTimeStruct.tm_mday = win32TimeStruct.wDay;
    // ANSI time struct has month in range [0..11]
    ansiTimeStruct.tm_mon = win32TimeStruct.wMonth - 1;
    // ANSI time struct has year that is an offset from 1900
    ansiTimeStruct.tm_year = win32TimeStruct.wYear - 1900;
    // unknown daylight savings time (dst) status
    // if we fail to init this value, we can get inconsistent results
    ansiTimeStruct.tm_isdst = -1;
    
    time_t secondsSinceEpoch = mktime( &ansiTimeStruct );

    *outSeconds = Time::normalize( secondsSinceEpoch );
	*outMilliseconds = (unsigned long)( win32TimeStruct.wMilliseconds );	
	}


