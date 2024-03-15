#include "WebRequestCompletionThread.h"


WebRequestCompletionThread::WebRequestCompletionThread( Socket *inSocket )
        : mDone( false ), mForceEnd( false ), mSocket( inSocket ),
          mBytesSoFar( 0 ) {
    
    start();
    }

        
        
WebRequestCompletionThread::~WebRequestCompletionThread() {
    mLock.lock();
    mForceEnd = true;
    mLock.unlock();
    
    join();
    }

        
		

char WebRequestCompletionThread::isWebRequestDone() {
    mLock.lock();
    char done = mDone;
    mLock.unlock();
    
    return done;
    }

        


int WebRequestCompletionThread::getBytesReceivedSoFar() {
    mLock.lock();
    int byteCount = mBytesSoFar;
    mLock.unlock();
    
    return byteCount;
    }



unsigned char *WebRequestCompletionThread::getResponse( int *outSize ) {
    // only called after thread done, does not need to be thread safe
    
    *outSize = mReceivedBytes.size();

    // temporarliy add \0 termination
    mReceivedBytes.push_back( '\0' );
    
    unsigned char *bytes = mReceivedBytes.getElementArray();
    
    // remove temporary \0 termination
    mReceivedBytes.deleteLastElement();
    

    return bytes;
    }

    

void WebRequestCompletionThread::run() {
    long bufferLength = 5000;
    unsigned char *buffer = new unsigned char[ bufferLength ];

    char endForced = false;
    
    while( ! mDone && ! endForced ) {

        // non-blocking
        
        // keep reading as long as we get non-empty buffers
        int numRead = bufferLength;
        
        while( numRead > 0 && ! endForced ) {
            
            numRead = mSocket->receive( buffer, bufferLength, 0 );
            
            if( numRead > 0 ) {
                
                // this is fast for unsigned char vectors, using
                // memcpy internally
                mReceivedBytes.push_back( buffer, numRead );                

                // protect mBytesSoFar with lock, not add-to-vector code
                mLock.lock();
                mBytesSoFar += numRead;
                endForced = mForceEnd;
                mLock.unlock();
                }
            }
        
        
        // if we got here, then either our socket was closed OR we completely
        // emptied the receive buffer.  In either case, we can lock safely
        // here and check if we've been force-ended.
        mLock.lock();
        endForced = mForceEnd;
        mLock.unlock();
                

        if( numRead == -1 ) {
            // connection closed, done done receiving result

            mLock.lock();
            mDone = true;
            mLock.unlock();
            }
        }
    


    delete [] buffer;
            
        

    }
    
