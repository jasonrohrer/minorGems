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


#include <time.h>
#include <stdio.h>
#include <math.h>



#ifndef TIME_INCLUDED
#define TIME_INCLUDED 


typedef double timeSec_t;



/**
 * Interface for platform-independent, high-resolution time access.
 *
 * The C standard doesn't specify how time_t values work, what units are used,
 * or what they are relative to.  They might not even be in seconds.
 *
 * This class specifies a time() function that returns seconds since
 * a well-defined GMT epoch time.
 * 
 * Note that some epochs might not be available on all platforms.
 * The epoch's are tried in this order (all times GMT) until one works
 * with mktime:
 *
 * 1.   1970-01-01T00:00:00Z     (standard Unix epoch time)
 * 2.   1980-01-01T00:00:00Z
 * 3.   1990-01-01T00:00:00Z
 * 4.   2000-01-01T00:00:00Z
 *
 * If these epochs fail to be usable, the epoch is set to the current system
 * time at the first invocation of one of this class's time functions.
 *
 *
 * Note that on POSIX systems, the output of this class should be identical
 * to a double-caste of the standard time.h time() value on 32-bit time
 * platforms, and also identical on 64-bit time platforms up to where
 * the double mantissa loses precision (with IEEE 754, this is quadrillions
 * of years into the future).
 *
 * The choice of double was made because conversion between time_t and double
 * is easily accessible through the standard difftime function.
 *
 * No other portable conversions for absolute time are available, so even if 
 * we represent absolute time as a sint64, we must pass through double first 
 * on our way out of time_t.
 *
 * @author Jason Rohrer
 */
class Time {
	public:

        // Note that setting our initial epoch is NOT thread-safe
        // (thanks to gmtime).
        // If init isn't called, the epoch is set during the first
        // call to our time functions.
        //
        // To control when this non-thread-safe action happens, call
        // init before using any time functions.
        //
        // But in situations where thread-safety isn't a concern, calling
        // init first is optional.
        static void init();
        


        // gets non-fractional seconds since epoch
        // this is the double equivalent of the time.h function time( NULL ),
        // but with a well-defined epoch and well-defined units.
        //
        // Note that even though this is a double type, the fractional
        // component of it can be ignored (can be printed with %.f format
        // string, for example)
        //
        // On rare platforms where time_t has greater than second resolution
        // this value is rounded to exclude fractional seconds.
        // 
        // fractional time can be obtained with higher resoultion
        // (ms or greater, sometimes microsec) by calling getCurrentTime()
        static timeSec_t timeSec();
        

        // normalize a time_t value into seconds since the epoch.
        static timeSec_t normalize( time_t inTime );

		
		/**
		 * Gets the current time in seconds and milliseconds since the epoch.
         *
		 * @param outSeconds pointer to where the time in seconds
		 *   will be returned.
		 * @param outMilliseconds pointer to where the extra
		 *   milliseconds will be returned.  Value returned is in [0,999].
		 */
		static void getCurrentTime( timeSec_t *outSeconds,
									unsigned long *outMilliseconds );

        

        /**
         * Gets the current time in fractional (double) seconds.
         *
         * This time has exactly millisecond resolution, even if the
         * platform supports greater resolution.
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
			timeSec_t inSeconds, unsigned long inMilliseconds );

        
    private:
        
        static char sEpochTimeSet;
        static time_t sEpochTime;
        
        static void setEpochTime();

        
        // Scans ISO 8601 format UTC string and fills a tm struct
        // returns true on success
        static char stringToTimeStruct( const char *inString, 
                                        struct tm *inStruct );
        

		
	};




inline void Time::init() {
    setEpochTime();
    }



inline timeSec_t Time::timeSec() {
    return normalize( ::time( NULL ) );
    }

        


inline timeSec_t Time::normalize( time_t inTime ) {
    
    if( ! sEpochTimeSet ) {
        setEpochTime();
        }
    
    return floor( difftime( inTime, sEpochTime ) );
    }




inline char Time::stringToTimeStruct( const char *inString, 
                                      struct tm *inStruct ) {
    // strptime not available everywhere

    int year, month, day, hours, minutes, seconds;

    int numScanned = sscanf( inString, "%d-%d-%dT%d:%d:%dZ",
                             &year, &month, &day, &hours, &minutes, &seconds );
    
    if( numScanned != 6 ) {
        return false;
        }
    
    inStruct->tm_year = year - 1900;
    inStruct->tm_mon = month - 1;
    inStruct->tm_mday = day;

    inStruct->tm_hour = hours;
    inStruct->tm_min = minutes;
    inStruct->tm_sec = seconds;
    inStruct->tm_isdst = -1;
    

    return true;
    }


inline void Time::setEpochTime() {

    const char *epochStrings[4] = { "1970-01-01T00:00:00Z",
                                    "1980-01-01T00:00:00Z",
                                    "1990-01-01T00:00:00Z",
                                    "2000-01-01T00:00:00Z" };
    
    for( int e=0; e<4; e++ ) {
        tm localTimeStruct;
        
        char gotStruct = stringToTimeStruct( epochStrings[e],
                                             &localTimeStruct );
        
        if( gotStruct ) {
            
            time_t localEpochTime = mktime( &localTimeStruct );
            
            // -1 returned by mktime if localTimeStruct out of range
            // according to C standard
            if( localEpochTime != (time_t)( -1 ) ) {
                
                // got one!
                printf( "Local epoch time  = %ld\n", localEpochTime );
                
                // localEpochTime represents the GMT time when
                // our locale hit the given epoch time

                
                tm gmtStruct = *( gmtime( &localEpochTime ) );


                // found code for comparing time structs here:
                // https://stackoverflow.com/questions/32424125/
                //   c-code-to-get-local-time-offset-in-minutes-relative-to-utc
                
                // diff is seconds we have to add to local time
                // to get gmt time
                int diff = 
                    ( ( localTimeStruct.tm_hour - gmtStruct.tm_hour ) * 60 + 
                      ( localTimeStruct.tm_min - gmtStruct.tm_min ) ) * 60 + 
                    ( localTimeStruct.tm_sec - gmtStruct.tm_sec );
                int delta_day = localTimeStruct.tm_mday - gmtStruct.tm_mday;
                if( ( delta_day == 1 ) || ( delta_day < -1 ) ) {
                    diff += 24 * 60 * 60;
                    } 
                else if( ( delta_day == -1 ) || ( delta_day > 1 ) ) {
                    diff -= 24 * 60 * 60;
                    }
                
                // add it to get gmt time of our epoch
                localTimeStruct.tm_sec += diff;
                
                // now we have the time_t of when GMT was at our desired epoch
                sEpochTime = mktime( &localTimeStruct );
                
                printf( "GMT epoch time  = %ld\n", sEpochTime );

                sEpochTimeSet = true;
                return;
                }
            }
        }
    
    // none of our epochs worked with mktime...

    // set epoch to current time
    sEpochTime = time( NULL );
    sEpochTimeSet = true;
    }




inline double Time::getCurrentTime() {
    timeSec_t currentTimeS;
	unsigned long currentTimeMS;
	getCurrentTime( &currentTimeS, &currentTimeMS );

    return currentTimeS + currentTimeMS / 1000.0;
    }



inline unsigned long Time::getMillisecondsSince(
	timeSec_t inSeconds, unsigned long inMilliseconds ) {

	timeSec_t currentTimeS;
	unsigned long currentTimeMS;
	getCurrentTime( &currentTimeS, &currentTimeMS );

	
	unsigned long deltaS = (unsigned long)( currentTimeS - inSeconds );
	long deltaMS = ( (long)currentTimeMS - (long)inMilliseconds );

	// carry, if needed
	if( deltaMS < 0 ) {
		deltaS--;
		deltaMS += 1000;
		}

	return 1000 * deltaS + deltaMS;
	}



#endif
 

