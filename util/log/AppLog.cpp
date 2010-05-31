/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2002-March-30    Jason Rohrer
 * Wrapped our dynamically allocated static member in a statically
 * allocated class to avoid memory leaks at program termination.  
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */



#include "AppLog.h"
#include "Log.h"

#include <stdlib.h>



// wrap our static member in a statically allocated class
LogPointerWrapper AppLog::mLogPointerWrapper( new PrintLog );



LogPointerWrapper::LogPointerWrapper( Log *inLog )
    : mLog( inLog ) {

    }



LogPointerWrapper::~LogPointerWrapper() {
    if( mLog != NULL ) {
        delete mLog;
        }
    }



void AppLog::criticalError( const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inString, Log::CRITICAL_ERROR_LEVEL );
    }



void AppLog::criticalError( const char *inLoggerName, const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inLoggerName, inString, Log::CRITICAL_ERROR_LEVEL );
    }



void AppLog::error( const char *inString ) {
    mLogPointerWrapper.mLog->logString( inString, Log::ERROR_LEVEL );
    }



void AppLog::error( const char *inLoggerName, const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inLoggerName, inString, Log::ERROR_LEVEL );
    }



void AppLog::warning( const char *inString ) {
    mLogPointerWrapper.mLog->logString( inString, Log::WARNING_LEVEL );
    }



void AppLog::warning( const char *inLoggerName, const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inLoggerName, inString, Log::WARNING_LEVEL );
    }



void AppLog::info( const char *inString ) {
    mLogPointerWrapper.mLog->logString( inString, Log::INFO_LEVEL );
    }



void AppLog::info( const char *inLoggerName, const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inLoggerName, inString, Log::INFO_LEVEL );
    }



void AppLog::detail( const char *inString ) {
    mLogPointerWrapper.mLog->logString( inString, Log::DETAIL_LEVEL );
    }



void AppLog::detail( const char *inLoggerName, const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inLoggerName, inString, Log::DETAIL_LEVEL );
    }



void AppLog::trace( const char *inString ) {
    mLogPointerWrapper.mLog->logString( inString, Log::TRACE_LEVEL );
    }



void AppLog::trace( const char *inLoggerName, const char *inString ) {
    mLogPointerWrapper.mLog->logString(
        inLoggerName, inString, Log::TRACE_LEVEL );
    }



void AppLog::setLog( Log *inLog ) {
    int currentLoggingLevel = getLoggingLevel();
    
    if( inLog != mLogPointerWrapper.mLog ) {
        delete mLogPointerWrapper.mLog;
        }
    mLogPointerWrapper.mLog = inLog;
    
    setLoggingLevel( currentLoggingLevel );
    }



Log *AppLog::getLog() {
    return mLogPointerWrapper.mLog;
    }



void AppLog::setLoggingLevel( int inLevel ) {
    mLogPointerWrapper.mLog->setLoggingLevel( inLevel );
    }



int AppLog::getLoggingLevel() {
    return mLogPointerWrapper.mLog->getLoggingLevel();
    }






