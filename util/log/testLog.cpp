/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 */


#include "AppLog.h"
#include "FileLog.h"

#include "Log.h"


int main() {

    AppLog::warning( "A test warning" );
    AppLog::warning( "Another test warning" );
    AppLog::warning( "mainApp", "And Another test warning" );

    AppLog::setLoggingLevel( Log::ERROR_LEVEL );

    AppLog::warning( "Yet Another test warning" );
    AppLog::error( "mainApp", "A test error" );


    AppLog::setLog( new FileLog( "test.log" ) );

    // this should be skipped
    AppLog::warning( "A second test warning" );
    // this should not be
    AppLog::criticalError( "A critical error" );
    
    
    AppLog::setLog( new FileLog( "test2.log" ) );

    // this should be skipped
    AppLog::warning( "A third test warning" );


    return 0;
    }
