/*
 * Modification History
 *
 * 2001-January-28		Jason Rohrer
 * Created.  
 *
 * 2001-February-4		Jason Rohrer
 * Fixed receive so that it waits for all requested bytes to arrive.
 *
 * 2001-March-4		Jason Rohrer
 * Replaced include of <winbase.h> and <windef.h> with <windows.h> 
 * to fix compile bugs encountered with newer windows compilers.
 *
 * 2001-May-12   Jason Rohrer
 * Fixed a bug in socket receive error checking. 
 *
 * 2001-November-13		Jason Rohrer
 * Changed timeout parameter to signed, since -1 is a possible argument. 
 *
 * 2002-April-15    Jason Rohrer
 * Removed call to WSAGetLastError, since it seems to pick up errors from
 * non-socket calls.  For example, if sys/stat.h stat() is called on a file
 * that does not exist, WSAGetLastError returns 2, which is not even a
 * winsock error code.  We should probably report this bug, huh?
 *
 * 2002-August-2    Jason Rohrer
 * Added functon for getting remote host address, but no implementation.
 *
 * 2002-August-5    Jason Rohrer
 * Added implementation of getRemoteHostAddress().
 *
 * 2002-September-8    Jason Rohrer
 * Fixed a major looping bug with broken sockets.
 *
 * 2002-November-15   Jason Rohrer
 * Fixed a security hole when getting the remote host address.
 *
 * 2003-February-4   Jason Rohrer
 * Added a function for getting the local host address from a socket.
 * Still need to test the win32 version of this.
 *
 * 2003-February-5   Jason Rohrer
 * Fixed a bug in call to gethostname.  Removed unused variable.
 *
 * 2004-January-4   Jason Rohrer
 * Added use of network function locks.
 *
 * 2004-January-11   Jason Rohrer
 * Fixed a bug in handling of timeout return value.
 *
 * 2004-March-23   Jason Rohrer
 * Removed timeout error message.
 *
 * 2004-December-13   Jason Rohrer
 * Added a breakConnection function.
 *
 * 2004-December-24   Jason Rohrer
 * Fixed bug in close call.
 *
 * 2005-July-5   Jason Rohrer
 * Added port number when getting address of remote host.
 *
 * 2006-May-28   Jason Rohrer
 * Changed timeout behavior slightly to support emulation of non-blocking mode.
 *
 * 2006-June-5   Jason Rohrer
 * Added support for non-blocking sends.
 *
 * 2008-September-30   Jason Rohrer
 * Added support for non-blocking connect.
 * Fixed close-detection in non-blocking read.
 *
 * 2010-January-26  Jason Rohrer
 * Added support for disabling Nagle algorithm.
 *
 * 2013-January-25  Jason Rohrer
 * Fixed signing inconsistencies and unused variable warning.
 *
 * 2013-April-23  Jason Rohrer
 * Fixed interrupt handling during select (not a fatal error).
 *
 * 2013-December-12  Jason Rohrer
 * Fixed bug in handling partial receive with no timeout when socket is
 * gracefully closed.
 */



#include "minorGems/network/Socket.h"
#include "minorGems/network/NetworkFunctionLocks.h"

#include <winsock.h>
#include <windows.h>

#include <stdio.h>
#include <string.h>

// prototypes
int timed_read( int inSock, unsigned char *inBuf, 
	int inLen, long inMilliseconds );
	

/**
 * Windows-specific implementation of the Socket class member functions.
 *
 */



// Win32 does not define socklen_t
typedef int socklen_t;



char Socket::sInitialized = false;



int Socket::initSocketFramework() {
	WORD wVersionRequested;
	WSADATA wsaData;
	
	int err; 
	wVersionRequested = MAKEWORD( 1, 0 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	
	if ( err != 0 ) {
    	// no usable DLL found  
    	printf( "WinSock DLL version 1.0 or higher not found.\n" );  
    	
		return -1;
		}
    
    sInitialized = true;
	return 0;
	}
		



Socket::~Socket() {
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    if( !mIsConnectionBroken ) {
        
        // 2 specifies shutting down both sends and receives
        shutdown( socketID, 2 );
        mIsConnectionBroken = true;
        }
    
	closesocket( socketID );
	
	delete [] socketIDptr;
	}



int Socket::isConnected() {
    
    if( mConnected ) {
        return 1;
        }
    
    unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    int ret;
	fd_set fsr;
	struct timeval tv;
	int val;
    socklen_t len;

	FD_ZERO( &fsr );
	FD_SET( socketID, &fsr );

    // check if connection event waiting right now
    // timeout of 0
    tv.tv_sec = 0;
	tv.tv_usec = 0;    

	ret = select( socketID + 1, NULL, &fsr, NULL, &tv );

	if( ret==0 ) {
		// timeout
		return 0;
        }

    // no timeout
    // error?

	len = 4;
	ret = getsockopt( socketID, SOL_SOCKET, SO_ERROR, (char*)( &val ), &len );
	
	if( ret < 0 ) {
		// error
        return -1;
        }

	if( val != 0 ) {
        // error
		return -1;
        }
	
    // success
    mConnected = true;
    
    return 1;
    }



void Socket::setNoDelay( int inValue ) {
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    int flag = inValue;
    setsockopt( socketID,
                IPPROTO_TCP,
                TCP_NODELAY,
                (char *) &flag,
                sizeof(int) ); 
    }




int Socket::send( unsigned char *inBuffer, int inNumBytes,
                  char inAllowedToBlock,
                  char inAllowDelay ) {
	
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    if( inAllowedToBlock ) {
        if( ! inAllowDelay ) {
            // turn nodelay on
            setNoDelay( 1 );
            }        
        
        int returnVal = ::send( socketID, (char*)inBuffer, inNumBytes, 0 );
        
        if( ! inAllowDelay ) {
            // turn nodelay back off
            setNoDelay( 0 );
            }
        
        return returnVal;
        }
    else {
        // 1 for non-blocking, 0 for blocking
        u_long socketMode = 1;
        ioctlsocket( socketID, FIONBIO, &socketMode );

        if( ! inAllowDelay ) {
            // turn nodelay on
            setNoDelay( 1 );
            }

        int result = ::send( socketID, (char*)inBuffer, inNumBytes, 0 );
        
        if( ! inAllowDelay ) {
            // turn nodelay back off
            setNoDelay( 0 );
            }


        // set back to blocking
        socketMode = 0;
        ioctlsocket( socketID, FIONBIO, &socketMode );
        
        
        if( result == -1 &&
            WSAGetLastError() == WSAEWOULDBLOCK ) {

            return -2;
            }
        else {
            return result;
            }
        }
    }
		
		
		
int Socket::receive( unsigned char *inBuffer, int inNumBytes,
	long inTimeout ) {
	
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];
	
	int numReceived = 0;
	
	char error = false;
	char errorReturnValue = -1;


    char stopLooping = false;
    
    
	// for win32, we can't specify MSG_WAITALL
	// so we have too loop until the entire message is received,
	// as long as there is no error.

    // note that if a timeout is set, we use the stopLooping flag
    // to return only the available data (we do not emulate MSG_WAITALL)
    
	while( numReceived < inNumBytes &&
           !error &&
           !stopLooping ) {
		   
		// the number of bytes left to receive
		int numRemaining = inNumBytes - numReceived;
		
		// pointer to the spot in the buffer where the
		// remaining bytes should be stored
		unsigned char *remainingBuffer = &( inBuffer[ numReceived ] );
		
		int numReceivedIn;
		
		if( inTimeout == -1 ) {		
			numReceivedIn = 
				recv( socketID, (char*)remainingBuffer, numRemaining, 0 );
			}
		else {		
			numReceivedIn = 
				timed_read( socketID, remainingBuffer,
                            numRemaining, inTimeout );

            // stop looping after one timed read
            stopLooping = true;
			}
			
			
		if( numReceivedIn > 0 ) {
			numReceived += numReceivedIn;
			}
        else {
            if( numReceivedIn == 0 ) {
                // the socket was gracefully closed
                // return whatever we have received
                stopLooping = true;
                }
            else if( numReceivedIn == SOCKET_ERROR ) {
                error = true;
                // socket error
                errorReturnValue = -1;
                }
            else if( numReceivedIn == -2 ) {
                error = true;
                // timeout
                errorReturnValue = -2;
                }
            else {
                printf( "Unexpected return value from socket receive: %d.\n",
                        numReceivedIn );
                error = true;
                errorReturnValue = -1;
                }
            
            }
			
		}

    if( error ) {
        return errorReturnValue;
        }
    else {
        return numReceived;
        }
	}



void Socket::breakConnection() {
	unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    if( !mIsConnectionBroken ) {

        shutdown( socketID, 2 );
        mIsConnectionBroken = true;
        }
    
	closesocket( socketID );
    }



HostAddress *Socket::getRemoteHostAddress() {

    unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    // adapted from Unix Socket FAQ
    
    socklen_t len;
    struct sockaddr_in sin;
    
    len = sizeof sin;
    int error = getpeername( socketID, (struct sockaddr *) &sin, &len );

    if( error ) {
        return NULL;
        }

    // this is potentially insecure, since a fake DNS name might be returned
    // we should use the IP address only
    //
    // struct hostent *host = gethostbyaddr( (char *) &sin.sin_addr,
    //                                       sizeof sin.sin_addr,
    //                                       AF_INET );


    NetworkFunctionLocks::mInet_ntoaLock.lock();
    // returned string is statically allocated, copy it
    char *ipAddress = stringDuplicate( inet_ntoa( sin.sin_addr ) );
    NetworkFunctionLocks::mInet_ntoaLock.unlock();

    int port = ntohs( sin.sin_port );
    
    return new HostAddress( ipAddress, port );    
    }



HostAddress *Socket::getLocalHostAddress() {
    unsigned int *socketIDptr = (unsigned int *)( mNativeObjectPointer );
	unsigned int socketID = socketIDptr[0];

    // adapted from GTK-gnutalla code, and elsewhere

    struct sockaddr_in addr;
	int len = sizeof( struct sockaddr_in );

    int result = getsockname( socketID, (struct sockaddr*)( &addr ), &len );

    if( result == -1 ) {
        return NULL;
        }
    else {

        char *stringAddress = inet_ntoa( addr.sin_addr );

        return new HostAddress( stringDuplicate( stringAddress ),
                                0 );
        }
    
    }



/* timed_read adapted from gnut, by Josh Pieper */
/* Josh Pieper, (c) 2000 */
/* This file is distributed under the GPL, see file COPYING for details */

// exactly like the real read, except that it returns -2
// if no data was read before the timeout occurred...
int timed_read( int inSock, unsigned char *inBuf, 
	int inLen, long inMilliseconds ) {
	fd_set fsr;
	struct timeval tv;
	int ret;
	
	FD_ZERO( &fsr );
	FD_SET( inSock, &fsr );
 
	tv.tv_sec = inMilliseconds / 1000;
	int remainder = inMilliseconds % 1000;
	tv.tv_usec = remainder * 1000;
	
	ret = select( inSock + 1, &fsr, NULL, NULL, &tv );
	
	if( ret==0 ) {
		// printf( "Timed out waiting for data on socket receive.\n" );
		return -2;
		}

    while( ret<0 && WSAGetLastError() == WSAEINTR ) {
        // interrupted
        // try again
        ret = select( inSock + 1, &fsr, NULL, NULL, &tv );
        }
    

	if( ret<0 ) {
        perror( "Selecting socket during receive failed" );
        return ret;
        }
	
	
	ret = recv( inSock, (char*)inBuf, inLen, 0 );
	

    if( ret == 0  ) {
        // select came back as 1, but no data there
        // connection closed on remote end
        return -1;
        }

	return ret;
	}
