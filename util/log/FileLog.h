/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2002-November-5    Jason Rohrer
 * Added support for backing up logs and deleting old log data.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */


#include "minorGems/common.h"
#include "minorGems/system/Time.h"



#ifndef FILE_LOG_INCLUDED
#define FILE_LOG_INCLUDED



#include "PrintLog.h"

#include <stdio.h>


/**
 * A file-based implementation of the Log interface.
 *
 * @author Jason Rohrer
 */
class FileLog : public PrintLog {



    public:


        
        /**
         * Constructs a file log.
         *
         * @param inFileName the name of the file to write log messages to.
         *   Must be destroyed by caller.
         * @param inSecondsBetweenBackups the number of seconds to wait
         *   before making a backup of the current log file (deleting any
         *   old backups), clearing the current log file, and starting
         *   a fresh log in the current log file.  Defaults to 3600
         *   seconds (one hour).  Backup logs are saved to inFileName.backup   
         */
        FileLog( const char *inFileName,
                 unsigned long inSecondsBetweenBackups = 3600 );


        
        virtual ~FileLog();

        

        /**
         * Makes a backup of the current log file, deletes old backups,
         * and clears the current log file.
         */
        void makeBackup();


        
        // overrides PrintLog::logStringV
        virtual void logStringV( const char *inLoggerName,
                                 int inLevel, const char* inFormatString,
                                 va_list inArgList );


        

    protected:

        FILE *mLogFile;

        char *mLogFileName;
                
        unsigned long mSecondsBetweenBackups;
        
        timeSec_t mTimeOfLastBackup;
        
        
        static const char *mDefaultLogFileName;

        
    }; 



#endif
