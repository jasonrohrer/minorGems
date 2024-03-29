#ifndef WEB_REQUEST_INCLUDED
#define WEB_REQUEST_INCLUDED


#include "minorGems/network/Socket.h"
#include "minorGems/network/HostAddress.h"
#include "minorGems/network/LookupThread.h"
#include "minorGems/network/web/WebRequestCompletionThread.h"



// a non-blocking web request
class WebRequest {
        

    public:
        
        // inMethod = GET, POST, etc.
        // inURL the url to retrieve
        // inBody the body of the request, can be NULL
        // request body must be in application/x-www-form-urlencoded format
        // inProxy is the address of a proxy in address:port format, 
        //         or NULL to not use a proxy
        //         defaults to NULL
        // inTimeoutSeconds specifies the overall timeout for the request
        //   if this much time passes before the request result is ready
        //   step() will return -1.
        //   Set to -1 for no timeout (will wait forever for request to 
        //   complete).  Defaults to -1
        WebRequest( const char *inMethod, const char *inURL,
                    const char *inBody, const char *inProxy = NULL,
                    double inTimeoutSeconds = -1 );
        

        // if request is not complete, destruction cancels it
        ~WebRequest();


        // take anoter non-blocking step
        // return 1 if request complete
        // return -1 if request hit an error
        // return 0 if request still in-progress
        int step();
        
        // gets the number of bytes downloaded for the request so far
        // note that this is only an estimate of result data size
        int getProgressSize();
        

        // gets the response body as a \0-terminated string
        char *getResult();


        // gets the response body as bytes
        unsigned char *getResult( int *outSize );
        
        

    protected:
        char mError;
        
        char *mURL;

        char *mRequest;
        
        int mRequestPosition;
        
        SimpleVector<char> mResponse;
        
        char mResultReady;
        
        char *mResult;
        
        int mResultSize;

        HostAddress *mSuppliedAddress;
        HostAddress *mNumericalAddress;
        LookupThread *mLookupThread;
        
        Socket *mSock;

        double mRequestStartTime;
        double mRequestTimeoutSeconds;

        WebRequestCompletionThread *mCompletionThread;
    };



#endif
