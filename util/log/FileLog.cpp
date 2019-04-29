/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2002-March-13    Jason Rohrer
 * Added a flush after every write.  
 *
 * 2002-April-8    Jason Rohrer
 * Changed to be thread-safe.  
 *
 * 2002-November-5    Jason Rohrer
 * Added support for backing up logs and deleting old log data.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2011-February-16    Jason Rohrer
 * Flag to print next log message to std out.
 *
 * 2019-April-29    Jason Rohrer
 * Backup logs moved instead of copied, which was slow for large files.
 */


#include "FileLog.h"

#include "PrintLog.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/io/file/File.h"


#include <stdio.h>



const char *FileLog::mDefaultLogFileName = "default.log";



FileLog::FileLog( const char *inFileName, unsigned long inSecondsBetweenBackups )
    : mLogFile( NULL ),
      mLogFileName( stringDuplicate( inFileName ) ),
      mSecondsBetweenBackups( inSecondsBetweenBackups ),
      mTimeOfLastBackup( Time::timeSec() ) {

    
    mLogFile = fopen( mLogFileName, "a" );

    if( mLogFile == NULL ) {
        printf( "Log file %s failed to open.\n", mLogFileName );
        printf( "Writing log to default file:  %s\n",
                mDefaultLogFileName );

        // switch to default log file name
        
        delete [] mLogFileName;
        mLogFileName = stringDuplicate( mDefaultLogFileName );

        
        mLogFile = fopen( mLogFileName, "a" );
        
        if( mLogFile == NULL ) {
            printf( "Default log file %s failed to open.\n",
                    mLogFileName );
            }
        }
    
    }



FileLog::~FileLog() {
    if( mLogFile != NULL ) {
        fclose( mLogFile );
        }
    delete [] mLogFileName;
    }


        
void FileLog::logStringV( const char *inLoggerName,
                          int inLevel,
                          const char *inFormatString,
                          va_list inArgList ) {

    if( mLogFile != NULL ) {
        if( inLevel <= mLoggingLevel ) {


            char *message = PrintLog::generateLogMessage( inLoggerName,
                                                          inLevel,
                                                          inFormatString,
                                                          inArgList );

            mLock->lock();
            fprintf( mLogFile, "%s\n", message );
            
            fflush( mLogFile );

            if( mPrintOutNextMessage ) {
                printf( "%s\n", message );
                mPrintOutNextMessage = false;
                }
            else if( mPrintAllMessages ) {
                char *plainMessage = 
                    PrintLog::generatePlainMessage( inFormatString,
                                                    inArgList );
                printf( "%s\n", plainMessage );
                delete [] plainMessage;
                }
            
            
            if( Time::timeSec() - mTimeOfLastBackup > mSecondsBetweenBackups ) {
                makeBackup();
                }
            
            mLock->unlock();

            delete [] message;
            }
        }
    }



void FileLog::makeBackup() {
    fclose( mLogFile );

    char *backupFileName = new char[ strlen( mLogFileName ) + 10 ];
    sprintf( backupFileName, "%s.backup", mLogFileName );

    File *backupLogFile = new File( NULL, backupFileName );
    
    // don't copy
    // this can be a big file, and that will be slow

    // move instead

    // remove old one first 
    // (to avoid implementation-dependent behavior if destination exists)
    backupLogFile->remove();
    
    rename( mLogFileName, backupFileName );
    

    delete [] backupFileName;
    
    delete backupLogFile;


    // clear main log file and start writing to it again
    mLogFile = fopen( mLogFileName, "w" );

    if( mLogFile == NULL ) {
        printf( "Log file %s failed to open.\n", mLogFileName );
        }

    mTimeOfLastBackup = Time::timeSec();
    }









