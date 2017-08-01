/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-March-29    Jason Rohrer
 * Fixed include order.
 *
 * 2002-August-2   Jason Rohrer
 * Added use of ConnectionPermissionHandler.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */


 
#ifndef REQUEST_HANDLING_THREAD_INCLUDED
#define REQUEST_HANDLING_THREAD_INCLUDED 

#include "PageGenerator.h"
#include "ConnectionPermissionHandler.h"


#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"

#include <string.h>
#include <stdio.h>



#define REQUEST_HANDLING_THREAD_BUFFER_SIZE 4096

/**
 * Request handler for WebServer.
 *
 * @author Jason Rohrer.
 */
class RequestHandlingThread : public Thread {


        
    public:


        
        /**
         * Construct a handler.
         *
         * @param inSocket the socket to send the requested
         *   file trough.  Is destroyed before this thread terminates.
         * @param inGenerator the class that will generate the
         *   page content.
         *   Is not destroyed by this class.
         * @param inConnectionPermissionHandler the class that will
         *   grant connection permissions
         *   Is not destroyed by this class.
         */
        RequestHandlingThread(
            Socket *inSocket,
            PageGenerator *inGenerator,
            ConnectionPermissionHandler *inConnectionPermissionHandler);


        
        ~RequestHandlingThread();


        
        /**
         * Returns true if this handler is done and ready to be destroyed.
         *
         * Synchronized, so may block.
         *
         * @return true if this handler is done and ready to be destroyed.
         */
        char isDone();

        
        
        /**
         * Gets a string representation of the current time.
         *
         * @return a timestamp string.  Must be destroyed by caller.
         */
        static char* getTimestamp();

        
        
        // implements the Thread interface
        virtual void run();

        
        
    private:
        Socket *mSocket;
        PageGenerator *mGenerator;
        ConnectionPermissionHandler *mConnectionPermissionHandler;
        
        MutexLock *mDoneLock;
        char mDone;


        
        /**
         * Sends an HTTP "not found" message with a "not found" web page.
         *
         * @param inStream the stream to send the not found page to.
         * @param inFileName the name of the requested file,
         *   or NULL.
         */
        void sendNotFoundPage( SocketStream *inStream,
                               char *inFileName );

        
        
        /**
         * Sends a "bad request" web page.
         *
         * @param inStream the stream to send the page to.
         */
        void sendBadRequest( SocketStream *inStream );

        
        
    };



#endif

