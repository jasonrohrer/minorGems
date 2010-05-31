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

#include "minorGems/common.h"



#ifndef APP_LOG_INCLUDED
#define APP_LOG_INCLUDED


#include "Log.h"
#include "PrintLog.h"



// internally used class
class LogPointerWrapper;



/**
 * A singleton wrapper for implementations of the Log interface.
 *
 * This class should be the interface for log-access for applications.
 *
 * @author Jason Rohrer
 */
class AppLog {
    
    
    
    public:
    
    
        /**
         * These log errors at various levels.
         *
         * All char* parameters must be \0-terminated and destroyed by caller.
         */
        
        static void criticalError( const char *inString );
        static void criticalError( const char *inLoggerName, 
                                   const char *inString );

        static void error( const char *inString );
        static void error( const char *inLoggerName, const char *inString );

        static void warning( const char *inString );
        static void warning( const char *inLoggerName, const char *inString );

        static void info( const char *inString );
        static void info( const char *inLoggerName, const char *inString );

        static void detail( const char *inString );
        static void detail( const char *inLoggerName, const char *inString );

        static void trace( const char *inString );
        static void trace( const char *inLoggerName, const char *inString );
    


        /**
         * Sets the log to use.
         * Note that this call destroys the current log.
         *
         * @param inLog the log to use.
         *   Will be destroyed by this class.
         */
        static void setLog( Log *inLog );


        
        /**
         * Gets the log being used.
         *
         * @return the log being used.
         *   Will be destroyed by this class.
         */
        static Log *getLog();

        
        
        /**
         * Sets the logging level of the current log.
         *
         * Messages with levels above the current level will not be logged.
         *
         * @param inLevel one of the defined logging levels.
         */
        static void setLoggingLevel( int inLevel );


        
        /**
         * Gets the logging level of the current log.
         *
         * Messages with levels above the current level will not be logged.
         *
         * @return one of the defined logging levels.
         */
        static int  getLoggingLevel();



    protected:

        // note that all static objects
        // are destroyed at program termination
        //static Log *mLog;
        static LogPointerWrapper mLogPointerWrapper;
        
    };



/**
 * Wrapper for static member pointer to ensure
 * deletion of static member object at program termination.
 */
class LogPointerWrapper {


        
    public:



        /**
         * Constructor allows specification of the object
         * to wrap and destroy at program termination.
         *
         * @param inLog the log object to wrap.
         *   Will be destroyed at program termination if non-NULL.
         */
        LogPointerWrapper( Log *inLog );



        /**
         * Destructor will get called at program termination
         * if the object is statically allocated.
         */
        ~LogPointerWrapper();


        
        Log *mLog;


        
    };



#endif
