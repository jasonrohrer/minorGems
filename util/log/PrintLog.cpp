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



void PrintLog::logString( const char *inString, int inLevel ) {
    logString( (char *)mDefaultLoggerName, inString, inLevel ); 
    }


        
void PrintLog::logString( const char *inLoggerName, const char *inString,
                         int inLevel ) {


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
                                            inString,
                                            inLevel );

        threadPrintF( "%s\n", message );
        
        delete [] message;

        }
    }



void PrintLog::logPrintf( int inLevel, const char* inFormatString, ... ) {
    
    unsigned int bufferSize = 200;

    va_list argList;
    va_start( argList, inFormatString );

    char *buffer = new char[ bufferSize ];
    
    int stringLength =
        vsnprintf( buffer, bufferSize, inFormatString, argList );
    
    va_end( argList );

    if( stringLength == -1 || stringLength >= (int)bufferSize ) {
        // too long!
        delete [] buffer;
        buffer = stringDuplicate( "Message too long" );
        }
    

    logString( (char *)mDefaultLoggerName, buffer, inLevel ); 

    delete [] buffer;
    }



void PrintLog::logNPrintf( const char *inLoggerName,
                           int inLevel, 
                           const char* inFormatString, ... ) {
    
    unsigned int bufferSize = 200;

    va_list argList;
    va_start( argList, inFormatString );

    char *buffer = new char[ bufferSize ];
    
    int stringLength =
        vsnprintf( buffer, bufferSize, inFormatString, argList );
    
    va_end( argList );

    if( stringLength == -1 || stringLength >= (int)bufferSize ) {
        // too long!
        delete [] buffer;
        buffer = stringDuplicate( "Message too long" );
        }
    

    logString( inLoggerName, buffer, inLevel ); 

    delete [] buffer;
    }



char *PrintLog::generateLogMessage( const char *inLoggerName, 
                                    const char *inString,
                                    int inLevel ) {

    unsigned long seconds, milliseconds;
    
    Time::getCurrentTime( &seconds, &milliseconds );

    
    // lock around ctime call, since it returns a static buffer
    mLock->lock();

    char *dateString = stringDuplicate( ctime( (time_t *)( &seconds ) ) );
    
    // done with static buffer, since we made a copy
    mLock->unlock();

    
    // this date string ends with a newline...
	// get rid of it
	dateString[ strlen(dateString) - 1 ] = '\0';
    
    char *messageBuffer = autoSprintf( "L%d | %s (%ld ms) | %s | %s",
                                       inLevel, dateString, milliseconds,
                                       inLoggerName, inString );
    
    delete [] dateString;
    
    
    return messageBuffer;
    }


