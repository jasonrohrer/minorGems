/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2001-January-15		Jason Rohrer
 * Commented out redundant status messages that should be handled at a 
 * higher level.
 *
 * 2001-January-28		Jason Rohrer
 * Changed to comply with new initSocketFramework Socket interface. 
 *
 * 2001-January-29		Jason Rohrer
 * Fixed an endian bug with the port number.
 *
 * 2001-May-12   Jason Rohrer
 * Added a port number to the "bad socket bind" error message.
 *
 * 2001-November-13		Jason Rohrer
 * Added a missing include.
 *
 * 2001-December-12		Jason Rohrer
 * Changed some type usage and the include order to make BSD compatible.  
 * Added a BSD definition test to make socklen_t type compatible
 * on both BSD and Linux.
 *
 * 2002-March-27		Jason Rohrer
 * Added a timeout to accept.
 *
 * 2002-March-29		Jason Rohrer
 * Fixed a bug in return value when accept fails.
 *
 * 2002-April-11		Jason Rohrer
 * Changed to use select instead of poll to make it more portable.
 * Fixed a bug in the timeout implementation.
 *
 * 2002-April-12   Jason Rohrer
 * Changed to use poll on Linux and select on BSD,
 * since select does not seem to work on LinuxX86.
 * Fixed X86 accept/select bug by getting rid of union
 * and some unused parameters (it seems that stack space was a problem).
 *
 * 2004-February-23   Jason Rohrer
 * Added socket option to clear up lingering bound ports after shutdown.
 * Changed to close server socket instead of using shutdown.
 *
 * 2010-January-26  Jason Rohrer
 * Fixed socklen_t on later versions of MacOSX.
 */



#include "minorGems/network/SocketServer.h"

#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>



// BSD does not define socklen_t
#ifdef BSD
#ifndef IPHONE
#ifndef __OpenBSD__
#ifndef _SOCKLEN_T     // later versions of MacOS define it and mark it
typedef int socklen_t;
#endif
#endif
#endif
#endif

#ifndef BSD
// poll not available on BSD
#include <sys/poll.h>
#endif


/**
 * BSD Compilation note:
 *
 * Use g++ option -DBSD to define the BSD preprocessor variable.
 */



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
	int sockID = socket( AF_INET, SOCK_STREAM, 0 );
	
	if( sockID == -1 ) {
		printf( "Failed to construct a socket\n" );
		exit( 1 );
    	}
	
	// store socket id in native object pointer
	int *idStorage = new int[1];
	idStorage[0] = sockID;
	mNativeObjectPointer = (void *)idStorage;


    // this setsockopt code partially copied from gnut
    
    // set socket option to enable reusing addresses so that the socket is
    // unbound immediately when the server is shut down
    // (otherwise, rebinding to the same port will fail for a while
    //  after the server is shut down)
    int reuseAddressValue = 1;
	error = setsockopt( sockID,
                        SOL_SOCKET,      // socket-level option
                        SO_REUSEADDR,    // reuse address option
                        &reuseAddressValue,  // value to set for this option
                        sizeof( reuseAddressValue) );  // size of the value

    
    if( error == -1  ) {
		printf( "Failed to set socket options\n" );
		exit( 1 );
		}
	
    	
	// bind socket to the port
    struct sockaddr_in address;
	
	address.sin_family = AF_INET;
	address.sin_port = htons( inPort );
	address.sin_addr.s_addr = INADDR_ANY;
	
	error = bind( sockID, (struct sockaddr *) &address, sizeof( address ) );
	
	if( error == -1  ) {
		printf( "Bad socket bind, port %d\n", inPort );
		exit( 1 );
		}
	
	
	// start listening for connections
	error = listen( sockID, inMaxQueuedConnections );
	if( error == -1 ) {
		printf( "Bad socket listen\n" );
		exit(1);
		}
	
	}



SocketServer::~SocketServer() {
	
	int *socketIDptr = (int *)( mNativeObjectPointer );
	int socketID = socketIDptr[0];
	
    close( socketID );
    
	delete [] socketIDptr;
	}
	
	
	
Socket *SocketServer::acceptConnection( long inTimeoutInMilliseconds,
                                        char *outTimedOut ) {
    
	// printf( "Waiting for a connection.\n" );
	// extract socket id from native object pointer
	int *socketIDptr = (int *)( mNativeObjectPointer );
	int socketID = socketIDptr[0];

    if( outTimedOut != NULL ) {
        *outTimedOut = false;
        }

    
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
    
    
    int acceptedID = accept( socketID, NULL, NULL );

    if( acceptedID == -1 ) {
		printf( "Failed to accept a network connection.\n" );
		return NULL;
		}
    else {

        Socket *acceptedSocket = new Socket();
        int *idStorage = new int[1];
        idStorage[0] = acceptedID;
        acceptedSocket->mNativeObjectPointer = (void *)idStorage;
	
        //printf( "Connection received.\n" );
        
        return acceptedSocket;
        }
	}	













