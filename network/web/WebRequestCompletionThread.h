#ifndef WEB_REQUEST_COMPLETION_THREAD_CLASS_INCLUDED
#define WEB_REQUEST_COMPLETION_THREAD_CLASS_INCLUDED

#include "minorGems/system/FinishedSignalThread.h"
#include "minorGems/system/MutexLock.h"

#include "minorGems/network/Socket.h"

#include "minorGems/util/SimpleVector.h"



/**
 * Thread that finishes receive portion of a web request.
 *
 * Handles part of web request after request has been sent to server.
 *
 * @author Jason Rohrer
 */
class WebRequestCompletionThread : public FinishedSignalThread {
	
	public:
		/**
         * Constructs and starts a thread.
         *
		 * @param inSocket the socket on which the web request has already.
         *   been sent.  Destroyed by caller after this class is destroyed.
		 */
		WebRequestCompletionThread( Socket *inSocket );
        
        
        /**
         * Joins and destroys this thread
         *
         * Will force thread to stop waiting for more data, if response
         * not fully received yet.
         */
        ~WebRequestCompletionThread();
        
		

        /**
         * Returns true if the web response is done (socket closed by remote
         * host).
         */
        char isWebRequestDone();
        


        /**
         * Returns number of bytes received on socket so far.
         */
        int getBytesReceivedSoFar();


        /**
         * Gets the response as bytes.
         * Includes all headers sent by server.
         *
         * Can only be called safely after isWebRequestDone returns true;
         *
         * Note that the return array will be \0 terminated, beyond
         * the end of outSize characters.
         *
         * Result destroyed by caller.
         */
        unsigned char *getResponse( int *outSize );
        

		// override the run method from Thread
		void run();
	
	private:
        MutexLock mLock;
        
        char mDone;
        
        char mForceEnd;
        

        Socket *mSocket;

        SimpleVector<unsigned char> mReceivedBytes;
        
        int mBytesSoFar;

        
	};

	
#endif
