/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2010-April-5    Jason Rohrer
 * Printf-like functionality.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2011-February-16    Jason Rohrer
 * Flag to print next log message to std out.
 *
 * 2011-March-9    Jason Rohrer
 * Removed Fortify inclusion.
 */

#include "minorGems/common.h"


#include <stdarg.h>




#ifndef LOG_INCLUDED
#define LOG_INCLUDED






/**
 * An interface for a class that can perform logging functions.
 *
 * @author Jason Rohrer
 */
class Log {



    public:
        
        // These levels were copied from the JLog framework
        // by Todd Lauinger
        
        static const int DEACTIVATE_LEVEL;

        static const int CRITICAL_ERROR_LEVEL;

        static const int ERROR_LEVEL;

        static const int WARNING_LEVEL;

        static const int INFO_LEVEL;

        static const int DETAIL_LEVEL;

        static const int TRACE_LEVEL;


        Log();


        // provided so that virtual deletes work properly
        virtual ~Log();


        
        /**
         * Sets the logging level of the current log.
         *
         * Messages with levels above the current level will not be logged.
         *
         * @param inLevel one of the defined logging levels.
         */
        virtual void setLoggingLevel( int inLevel ) = 0;


        
        /**
         * Gets the logging level of the current log.
         *
         * Messages with levels above the current level will not be logged.
         *
         * @return one of the defined logging levels.
         */
        virtual int getLoggingLevel() = 0;
        

        /**
         * Next message will be printed out as well as written to the log.
         */
        void printOutNextMessage();

        
        /**
         * Toggle whether all messages are printed as well as written
         * to the log.
         */
        void printAllMessages( char inPrintAlso );



        /**
         * Logs a string in this log under the default logger name.
         *
         * @param inLevel the level to log inString at.
         * @param inFormatString the string to log as a \0-terminated string,
         *   may contain printf-style formatting information to be filled
         *   by variable arguments that follow.
         *   Must be destroyed by caller.
         */
        virtual void logString( int inLevel, 
                                const char *inFormatString, ... ) = 0; 
        
        // for backwards compatibility
        virtual void logPrintf( int inLevel, 
                                const char *inFormatString, ... ) = 0; 
        


        /**
         * Logs a string in this log, specifying a logger name.
         *
         * @param inLoggerName the name of the logger
         *   as a \0-terminated string.
         *   Must be destroyed by caller.
         * @param inLevel the level to log inString at.
         * @param inFormatString the string to log as a \0-terminated string,
         *   may contain printf-style formatting information to be filled
         *   by variable arguments that follow.
         *   Must be destroyed by caller.
         */
        virtual void logString( const char *inLoggerName, int inLevel, 
                                const char *inFormatString, ... ) = 0;


        // versions that take var-arg-lists explicitly
        // lists must be va_end-ed by caller.

        virtual void logStringV( int inLevel, const char* inFormatString,
                                 va_list inArgList ) = 0;
        
        virtual void logStringV( const char *inLoggerName,
                                 int inLevel, const char* inFormatString,
                                 va_list inArgList ) = 0;


    protected:
        
        char mPrintOutNextMessage;
        char mPrintAllMessages;
    };



#endif
