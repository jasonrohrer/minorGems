#include "minorGems/network/web/WebRequest.h"



class DemoCodeChecker {
    public:
        
        // start a checker connection
        DemoCodeChecker( char *inCode,
                         const char *inSharedSecret,
                         const char *inServerURL );    
        
        ~DemoCodeChecker();
        
        
        // perform another step of any pending network operations
        // returns true if work remains to be done
        // returns false if client is done with all pending work
        char step();
        
        // after steps done, check result
        char codePermitted();
        

        char isError();
        
        void clearError();
        
        // destroyed internally
        char *getErrorString();

  protected:
        const char *mSharedSecret;
        const char *mServerURL;

        char mError;
        char *mErrorString;
        
        void setError( const char *inErrorTranslationKey );


        WebRequest *mRequest;
        
        char *mChallenge;
        char mPermitted;
        

        int mStepCount;
    };
