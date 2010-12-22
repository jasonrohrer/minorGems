#include "DemoCodeChecker.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/crypto/hashes/sha1.h"
#include "minorGems/util/log/AppLog.h"

#include <time.h>


// secret shared with demo server (so that we can detect that it's a real
// demo server)
static const char *sharedSecret = "control_this";

static const char *demoServerAddress = "http://sleepisdeath.net/demoServer/server.php";



DemoCodeChecker::DemoCodeChecker( char *inCode )
        : mError( false ),
          mErrorString( NULL ) {

    char *challengeString = autoSprintf( "%d%s", time( NULL ), sharedSecret );
    
    mChallenge = computeSHA1Digest( challengeString );
    
    delete [] challengeString;
    

    char *url = autoSprintf( "%s?action=check_permitted&demo_id=%s"
                             "&challenge=%s",
                             demoServerAddress, inCode, mChallenge );
    
    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL, "Checking demo code with URL %s", url );

    mRequest = new WebRequest( "GET", url, NULL );
    delete [] url;

    mPermitted = false;

    mStepCount = 0;
    }


        
DemoCodeChecker::~DemoCodeChecker() {
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }

    if( mRequest != NULL ) {
        delete mRequest;
        }

    delete [] mChallenge;
    }



char DemoCodeChecker::step() {
    
    int result = mRequest->step();

    mStepCount ++;
    
    if( mStepCount < 45 ) {
        // wait a bit to avoid instant, flicker-inducing response
        return true;
        }

    if( mPermitted || isError() ) {
        // don't do any more processing
        return false;
        }
    
    
    

    if( result == -1 ) {
        setError( "err_webRequest" );
        
        // done
        return false;
        }
    if( result == 1 ) {
        // check 
        
        char *resultString = mRequest->getResult();

        AppLog::getLog()->logPrintf( 
            Log::INFO_LEVEL,
            "Result from demo code server:\n%s", resultString );
        
        
        if( strstr( resultString, "permitted" ) != NULL ) {
            
            char *correctResponseString = autoSprintf( "%s%s", mChallenge,
                                                       sharedSecret );
            
            char *correctResponse = computeSHA1Digest( correctResponseString );
            
            delete [] correctResponseString;
            

            char containsResponse = 
                ( strstr( resultString, correctResponse ) != NULL );
            
            delete[] correctResponse;
            delete [] resultString;


            if( containsResponse ) {
                mPermitted = true;
                return false;
                }
            }
        
        delete [] resultString;


        // else permission failed
        setError( "err_codeFailed" );
        return false;
        }
    if( result == 0 ) {
        // request in progress
        return true;
        }
    
    AppLog::getLog()->logPrintf( 
        Log::ERROR_LEVEL,
        "Unexpecte result code from WebRequest: %d", result );
    
    setError( "err_webRequest" );
    return false;
    }



char DemoCodeChecker::codePermitted() {
    return mPermitted;
    }



char DemoCodeChecker::isError() {
    return mError;
    }



void DemoCodeChecker::clearError() {
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }
    mErrorString = NULL;
    mError = false;
    }



char *DemoCodeChecker::getErrorString() {
    return mErrorString;
    }



void DemoCodeChecker::setError( const char *inErrorTranslationKey ) {
    mError = true;
    
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }

    mErrorString = stringDuplicate( TranslationManager::translate( 
                                        (char*)inErrorTranslationKey ) );
    }
