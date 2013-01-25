/*
 * Modification History
 *
 * 2001-January-28		Jason Rohrer
 * Created.
 *
 * 2001-January-29		Jason Rohrer
 * Fixed an endian bug that was messing up the port numbers.   
 *
 * 2002-April-6   Jason Rohrer
 * Changed to implement the new timeout-on-accept interface,
 * though did no actually implement timeouts.
 *
 * 2002-September-8   Jason Rohrer
 * Added a check for a potentially NULL argument.
 *
 * 2004-February-23   Jason Rohrer
 * Added timeout code for accepting connections.
 * Removed use of shutdown on listening socket.
 *
 * 2013-January-25  Jason Rohrer
 * Fixed signing inconsistencies.
 */


#include "minorGems/network/SocketServer.h"

#include <Winsock.h>
#include <stdio.h>


union sock {
	struct sockaddr s;
	struct sockaddr_in i;
	} sock;





SocketServer::SocketServer( int inPort, int inMaxQueuedConnections ) {
	int error = 0;
	
	if( !Socket::isFrameworkInitialized() ) {
		
		// try to init the framework
		
		int error = Socket::initSocketFramework();
		
		if( error == -1 ) {
			
			printf( "initializing network socket framework failed\n" );
			exit( 1 );
			}
		}
	
	// create the socket
	unsigned int sockID = socket( AF_INET, SOCK_STREAM, 0 );
	
	if( sockID == INVALID_SOCKET ) {
		printf( "Creating socket failed.\n" );
		exit( 1 );
    	}
	
	// store socket id in native object pointer
	unsigned int *idStorage = new unsigned int[1];
	idStorage[0] = sockID;
	mNativeObjectPointer = (void *)idStorage;
	
	
	// bind socket to the port
	union sock sockAddress;
	
	sockAddress.i.sin_family = AF_INET;
	sockAddress.i.sin_port = htons( inPort );
	sockAddress.i.sin_addr.s_addr = INADDR_ANY;
	
	error = bind( sockID, &(sockAddress.s), sizeof( struct sockaddr ) );
	
	if( error == -1  ) {
		printf( "Network socket bind to port %d failed\n", inPort );
		exit( 1 );
		}
	
	
	// start listening for connections
	error = listen( sockID, inMaxQueuedConnections );
	if( error == -1 ) {
		printf( "Listening for network socket connections failed.\n" );
		exit(1);
		}
	
	}



SocketServer::~SocketServer() {
	
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];
	
    closesocket( socketID );
	
	delete [] socketIDptr;
	}
	
	
	
Socket *SocketServer::acceptConnection( long inTimeoutInMilliseconds,
                                        char *outTimedOut ) {
    if( outTimedOut != NULL ) {
        *outTimedOut = false;
        }
    

    // printf( "Waiting for a connection.\n" );
	// extract socket id from native object pointer
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];


    // same timeout code as in linux version
    if( inTimeoutInMilliseconds != -1 ) {
        // if we have a timeout specified, select before accepting

        // this found in the Linux man page for select,
        // but idea (which originally used poll) was found
        // in the Unix Socket FAQ
        fd_set rfds;
        struct timeval tv;
        int retval;
        
        // insert our socket descriptor into this set
        FD_ZERO( &rfds );
        FD_SET( socketID, &rfds );

        // convert our timeout into the structure's format
        tv.tv_sec = inTimeoutInMilliseconds / 1000;
        tv.tv_usec = ( inTimeoutInMilliseconds % 1000 ) * 1000 ;
        
        retval = select( socketID + 1, &rfds, NULL, NULL, &tv );
        if( retval == 0 ) {
            // timeout
            if( outTimedOut != NULL ) {
                *outTimedOut = true;
                }

            return NULL;
            }

        
        }
    
    
	//int addressLength;
	//union sock acceptedAddress;
	
	struct sockaddr acceptedAddress;
	//int addressLength = sizeof( struct sockaddr );
	
	unsigned int acceptedID = accept( socketID, 
		&( acceptedAddress ), NULL );
	
	if( acceptedID == INVALID_SOCKET ) {
		printf( "Failed to accept a network connection.\n" );
		return NULL;
		}
	
	Socket *acceptedSocket = new Socket();
	unsigned int *idStorage = new unsigned int[1];
	idStorage[0] = acceptedID;
	acceptedSocket->mNativeObjectPointer = (void *)idStorage;
	
	//printf( "Connection received.\n" );
	
	return acceptedSocket;
	}	
