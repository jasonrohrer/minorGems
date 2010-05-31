/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-March-29    Jason Rohrer
 * Fixed include order.
 *
 * 2002-April-5    Jason Rohrer
 * Changed to extend StopSignalThread.
 *
 * 2002-August-2   Jason Rohrer
 * Added use of ConnectionPermissionHandler.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */


 
#ifndef WEB_SERVER_INCLUDED
#define WEB_SERVER_INCLUDED 


#include "PageGenerator.h"
#include "ConnectionPermissionHandler.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"

#include "RequestHandlingThread.h"
#include "ThreadHandlingThread.h"

#include "minorGems/system/StopSignalThread.h"



#include <string.h>
#include <stdio.h>


/**
 * A class that implements a basic web server.
 *
 * @author Jason Rohrer.
 */
class WebServer : public StopSignalThread {



    public:


        
        /**
         * Constructs an starts this server.
         *
         * @param inPort the port to listen on.
         * @param inGenerator the class to use for generating pages.
         *   Will be destroyed when this class is destroyed.
         */
        WebServer( int inPort, PageGenerator *inGenerator );



        /**
         * Stops and destroys this server.
         */
        ~WebServer();


        
        // implements the Thread::run() interface
        void run();

        
        
    private:

        int mPortNumber;
        int mMaxQueuedConnections;

        SocketServer *mServer;
        ThreadHandlingThread *mThreadHandler;

        PageGenerator *mPageGenerator;
        ConnectionPermissionHandler *mConnectionPermissionHandler;
    };




#endif
