/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2002-March-27		Jason Rohrer
 * Added a timeout to accept.
 *
 * 2014-January-3		Jason Rohrer
 * Made native object pointer public for SocketPoll implementations.
 */

#include "minorGems/common.h"


#ifndef SOCKET_SERVER_CLASS_INCLUDED
#define SOCKET_SERVER_CLASS_INCLUDED

#include <stdio.h>
#include "Socket.h"


/**
 * Network socket server that listens for connections on a certain port.
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the mac/ linux/ and win32/ 
 *   subdirectories).
 *
 * @author Jason Rohrer
 */ 
class SocketServer {

	public:


        
		/**
		 * Constructs a SocketServer and begins listening for connections.
		 *
		 * @param inPort the port to listen on.
		 * @param inMaxQueuedConnections the number of connection requests 
		 *   that will be queued before further requests are refused.
		 */
		SocketServer( int inPort, int inMaxQueuedConnections );
		

        
		~SocketServer();


        
		/**
		 * Accepts a an incoming connection on our port.
		 *
         * @param inTimeoutInMilliseconds the timeout in milliseconds,
         *   or -1 for no timeout.  Defaults to -1.
         * @param outTimedOut pre-allocated char where timeout
         *   flag will be returned.  If non-NULL, true will
         *   be inserted upon timeout, and false will be inserted
         *   upon other error or no error/timeout.
         *   Must be destroyed by caller if non-NULL.
         *   Defaults to NULL.
         *   
		 * @return a socket for the accepted connection,
		 *   or NULL if a socket error/timeout occurred.
		 */
		Socket *acceptConnection( long inTimeoutInMilliseconds = -1,
                                  char *outTimedOut = NULL );
		

		
		/**
		 * Used by platform-specific implementations.
		 */		
		void *mNativeObjectPointer;
		
	};		
	
#endif
