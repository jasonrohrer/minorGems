/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2002-April-8    Jason Rohrer
 * Changed to be thread-safe.  
 *
 * 2010-April-5    Jason Rohrer
 * Printf-like functionality.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */

#include "minorGems/common.h"



#ifndef PRINT_LOG_INCLUDED
#define PRINT_LOG_INCLUDED



#include "Log.h"

#include "minorGems/system/MutexLock.h"



/**
 * A console-based implementation of the Log interface.
 *
 * @author Jason Rohrer
 */
class PrintLog : public Log {



    public:


        
        /**
         * Constructs a print log.
         */
        PrintLog();


        
        virtual ~PrintLog();

        

        // implement the Log interface
        virtual void setLoggingLevel( int inLevel );

        virtual int getLoggingLevel();

        virtual void logString( int inLevel, 
                                const char *inFormatString, ... ); 

        virtual void logPrintf( int inLevel, 
                                const char *inFormatString, ... ); 
        
        virtual void logString( const char *inLoggerName, int inLevel, 
                                const char *inFormatString, ... );
        
        virtual void logStringV( int inLevel, const char* inFormatString,
                                 va_list inArgList );
        
        virtual void logStringV( const char *inLoggerName,
                                 int inLevel, const char* inFormatString,
                                 va_list inArgList );


    protected:

        int mLoggingLevel;

        static const char *mDefaultLoggerName;


        MutexLock *mLock;


        // format just the message itself with no meta information
        char *generatePlainMessage( const char *inFormatString,
                                    va_list inArgList );

        
        
        /**
         * Generates a string representation of a log message.
         *
         * @param inLoggerName the name of the logger
         *   as a \0-terminated string.
         *   Must be destroyed by caller.
         * @param inLevel the level to log inString at.
         * @param inFormatString the string to log as a \0-terminated string,
         *   may contain printf-style formatting information to be filled
         *   by variable arguments that follow.
         *   Must be destroyed by caller.
         * @param inArgList var arg list, must be va_end-ed by caller.
         *
         * @return the log message as a \0-terminated string.
         *   Must be destroyed by caller.
         */
        char *generateLogMessage( const char *inLoggerName, 
                                  int inLevel, const char *inFormatString,
                                  va_list inArgList );


        
    };



#endif
