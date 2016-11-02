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
 *
 * 2011-February-16    Jason Rohrer
 * Flag to print next log message to std out.
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
        Log::CRITICAL_ERROR_LEVEL, inString );
    }



void AppLog::criticalErrorF( const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV( Log::CRITICAL_ERROR_LEVEL, 
                                         inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::criticalError( const char *inLoggerName, 
                            const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV(
        inLoggerName, Log::CRITICAL_ERROR_LEVEL, inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::error( const char *inString ) {
    mLogPointerWrapper.mLog->logString( Log::ERROR_LEVEL, inString );
    }



void AppLog::errorF( const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV( Log::ERROR_LEVEL, inFormatString, 
                                         argList );
    
    va_end( argList );
    }



void AppLog::error( const char *inLoggerName, 
                    const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV(
        inLoggerName, Log::ERROR_LEVEL, inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::warning( const char *inString ) {
    mLogPointerWrapper.mLog->logString( Log::WARNING_LEVEL, inString );
    }



void AppLog::warningF( const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV( Log::WARNING_LEVEL, 
                                         inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::warning( const char *inLoggerName, 
                      const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV(
        inLoggerName, Log::WARNING_LEVEL, inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::info( const char *inString ) {
    mLogPointerWrapper.mLog->logString( Log::INFO_LEVEL, inString );
    }



void AppLog::infoF( const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV( Log::INFO_LEVEL, 
                                         inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::info( const char *inLoggerName, 
                   const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV(
        inLoggerName, Log::INFO_LEVEL, inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::detail( const char *inString ) {
    mLogPointerWrapper.mLog->logString( Log::DETAIL_LEVEL, inString );
    }



void AppLog::detailF( const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV( Log::DETAIL_LEVEL, 
                                         inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::detail( const char *inLoggerName, 
                     const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV(
        inLoggerName, Log::DETAIL_LEVEL, inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::trace( const char *inString ) {
    mLogPointerWrapper.mLog->logString( Log::TRACE_LEVEL, inString );
    }



void AppLog::traceF( const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV( Log::TRACE_LEVEL, 
                                         inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::trace( const char *inLoggerName, 
                    const char *inFormatString, ... ) {
    va_list argList;
    va_start( argList, inFormatString );

    mLogPointerWrapper.mLog->logStringV(
        inLoggerName, Log::TRACE_LEVEL, inFormatString, argList );
    
    va_end( argList );
    }



void AppLog::printOutNextMessage() {
    mLogPointerWrapper.mLog->printOutNextMessage();
    }



void AppLog::printAllMessages( char inPrintAlso ) {
    mLogPointerWrapper.mLog->printAllMessages( inPrintAlso );
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






