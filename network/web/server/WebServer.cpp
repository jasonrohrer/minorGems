/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-April-5    Jason Rohrer
 * Changed to extend StopSignalThread.
 * Added timeouts to socket accept to support checking for stop signal.
 *
 * 2002-August-2   Jason Rohrer
 * Added use of ConnectionPermissionHandler.
 *
 * 2002-August-6   Jason Rohrer
 * Changed member init order.
 *
 * 2002-September-17   Jason Rohrer
 * Removed argument to ConnectionPermissionHandler constructor.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */



#include "WebServer.h"


#include "minorGems/util/log/AppLog.h"



WebServer::WebServer( int inPort, PageGenerator *inGenerator )
    : mPortNumber( inPort ), mMaxQueuedConnections( 100 ),
      mThreadHandler( new ThreadHandlingThread() ),
      mPageGenerator( inGenerator ),
      mConnectionPermissionHandler( new ConnectionPermissionHandler() ) {

    
    mServer = new SocketServer( mPortNumber, mMaxQueuedConnections );

    this->start();
    }



WebServer::~WebServer() {
    stop();
    join();

    delete mServer;
    
    delete mThreadHandler;
    
    delete mPageGenerator;

    delete mConnectionPermissionHandler;
    }



void WebServer::run() {

    
    char *logMessage = new char[100];
    
    sprintf( logMessage, "Listening for connections on port %d\n",
            mPortNumber );

    AppLog::info( "WebServer", logMessage );

    delete [] logMessage;


    char acceptFailed = false;
    
    
    // main server loop
    while( !isStopped() && !acceptFailed ) {
        
        char timedOut = true;
        
        // 5 seconds
        long timeout = 5000;
    
        Socket *sock;

        AppLog::info( "WebServer", "Waiting for connection." );
        while( timedOut && !isStopped() ) {
            sock = mServer->acceptConnection( timeout, &timedOut );
            }

        
        if( sock != NULL ) {
        
            AppLog::info( "WebServer", "Connection received." );
        
            RequestHandlingThread *thread =
                new RequestHandlingThread( sock, mPageGenerator,
                                           mConnectionPermissionHandler );
            
            thread->start();
            
            mThreadHandler->addThread( thread );
            }
        else if( isStopped() ) {
            AppLog::info( "WebServer", "Received stop signal." );
            }
        else {
            AppLog::error( "WebServer", "Accepting a connection failed." );
            acceptFailed = true;
            }
        }
    
    }
