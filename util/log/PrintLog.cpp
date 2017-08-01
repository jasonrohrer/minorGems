/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2002-March-11    Jason Rohrer
 * Added a missing include.  
 *
 * 2002-April-8    Jason Rohrer
 * Fixed a casting, dereferencing Win32 compile bug.  
 * Changed to be thread-safe.
 * Changed to use thread-safe printing function. 
 *
 * 2002-April-8    Jason Rohrer
 * Fixed a signed-unsigned mismatch.
 *
 * 2004-January-11    Jason Rohrer
 * Added lock around asctime call.
 * Increased scope of lock.
 *
 * 2004-January-29    Jason Rohrer
 * Changed to use ctime instead of localtime and asctime.
 * Improved locking scope.
 * Changed to use autoSprintf.
 *
 * 2010-April-5    Jason Rohrer
 * Printf-like functionality.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2013-December-12    Jason Rohrer
 * Added dynamically increasing buffer size for longer log messages.
 *
 * 2013-December-16    Jason Rohrer
 * Removed assumptions about 32-bit int time_t (time_t is a structure for MS
 * compilers.
 */


#include "PrintLog.h"

#include "minorGems/system/Time.h"

#include "minorGems/util/printUtils.h"
#include "minorGems/util/stringUtils.h"


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


const char *PrintLog::mDefaultLoggerName = "general";



PrintLog::PrintLog()
    : mLoggingLevel( Log::TRACE_LEVEL ),
      mLock( new MutexLock() ) {
    
    }



PrintLog::~PrintLog() {
    delete mLock;
    }



void PrintLog::setLoggingLevel( int inLevel ) {
    mLock->lock();
    mLoggingLevel = inLevel;
    mLock->unlock();
    }



int PrintLog::getLoggingLevel() {
    mLock->lock();
    int level = mLoggingLevel;
    mLock->unlock();
    
    return level;
    }



void PrintLog::logString( int inLevel, const char* inFormatString, ... ) {

    va_list argList;
    va_start( argList, inFormatString );

    logStringV( inLevel, inFormatString, argList );
    
    va_end( argList );
    }



void PrintLog::logPrintf( int inLevel, const char* inFormatString, ... ) {

    va_list argList;
    va_start( argList, inFormatString );

    logStringV( inLevel, inFormatString, argList );
    
    va_end( argList );
    }



void PrintLog::logString( const char *inLoggerName,
                          int inLevel, 
                          const char* inFormatString, ... ) {

    va_list argList;
    va_start( argList, inFormatString );

    logStringV( inLoggerName, inLevel, inFormatString, argList );
    
    va_end( argList );
    }



void PrintLog::logStringV( int inLevel, const char* inFormatString,
                           va_list inArgList ) {
    
    logStringV( mDefaultLoggerName, inLevel, inFormatString, inArgList );
    }


        
void PrintLog::logStringV( const char *inLoggerName,
                           int inLevel, const char* inFormatString,
                           va_list inArgList ) {
    
    // not thread-safe to read mLoggingLevel here
    // without synchronization.
    // However, we want logging calls that are above
    // our level to execute with nearly no overhead.
    // mutex might be too much overhead....
    // Besides, not being thread-safe in this case might
    // (worst case) result in a missed log entry or
    // an extra log entry... but setting the logging level should be rare.
    if( inLevel <= mLoggingLevel ) {

        
        char *message = generateLogMessage( inLoggerName,
                                            inLevel,
                                            inFormatString, inArgList );

        threadPrintF( "%s\n", message );
        
        delete [] message;
        }
    }




char *PrintLog::generatePlainMessage( const char *inFormatString,
                                      va_list inArgList ) {
    
    // use copy to preserve inArgList
    // this allows multiple calls of generatePlainMessage with the same list
    va_list listCopy;
    va_copy( listCopy, inArgList );
    

    unsigned int bufferSize = 200;

    
    char *buffer = new char[ bufferSize ];
    
    int stringLength =
        vsnprintf( buffer, bufferSize, inFormatString, listCopy );
    
    va_end( listCopy );

    while( stringLength == -1 || stringLength >= (int)bufferSize ) {
        // too long!
        delete [] buffer;

        // double buffer size and try again
        bufferSize *= 2;
        buffer = new char[ bufferSize ];
    
        // make another working copy
        va_copy( listCopy, inArgList );

        stringLength = 
            vsnprintf( buffer, bufferSize, inFormatString, listCopy );
        
        va_end( listCopy );
        }

    return buffer;
    }



char *PrintLog::generateLogMessage( const char *inLoggerName, 
                                    int inLevel, 
                                    const char *inFormatString,
                                    va_list inArgList ) {
    
    char *buffer = generatePlainMessage( inFormatString, inArgList );
    


    timeSec_t seconds;
    unsigned long milliseconds;
    
    Time::getCurrentTime( &seconds, &milliseconds );

    time_t timeT = time( NULL );
    
    
    // lock around ctime call, since it returns a static buffer
    mLock->lock();

    char *dateString = stringDuplicate( ctime( &timeT ) );
    
    // done with static buffer, since we made a copy
    mLock->unlock();

    
    // this date string ends with a newline...
	// get rid of it
	dateString[ strlen(dateString) - 1 ] = '\0';
    
    char *messageBuffer = autoSprintf( "L%d | %s (%ld ms) | %s | %s",
                                       inLevel, dateString, milliseconds,
                                       inLoggerName, buffer );
    
    delete [] dateString;
    
    delete [] buffer;

    
    return messageBuffer;
    }


