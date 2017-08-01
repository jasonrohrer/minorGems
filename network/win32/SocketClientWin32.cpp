/*
 * Modification History
 *
 * 2001-January-28		Jason Rohrer
 * Created. 
 *
 * 2001-January-29		Jason Rohrer
 * Fixed an endian bug that was messing up the port numbers.  
 *
 * 2002-October-13   Jason Rohrer
 * Added support for timeout on connect.
 * Fixed several windows compile bugs.
 *
 * 2002-December-2   Jason Rohrer
 * Fixed resource leak when connection fails.
 * Changed close calls to win32 closesocket calls.
 *
 * 2004-January-2   Jason Rohrer
 * Fixed a thread safety issue with gethostbyname.
 *
 * 2004-January-4   Jason Rohrer
 * Added use of network function locks.
 *
 * 2006-April-25   Jason Rohrer
 * Added missing check for a NULL outTimedOut parameter.
 *
 * 2008-September-30   Jason Rohrer
 * Added support for non-blocking connect.
 *
 * 2009-October-5   Jason Rohrer
 * Fixed bug when connect timeout not used.
 */



#include "minorGems/network/SocketClient.h"
#include "minorGems/network/NetworkFunctionLocks.h"
#include "minorGems/system/MutexLock.h"

#include <Winsock.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>




// prototypes
struct in_addr *nameToAddress( char *inAddress ); 

int timed_connect( int inSocketID,
                   struct sockaddr *inSocketAddress,
                   int inAddressLength,
                   int inTimeoutInMilliseconds );



Socket *SocketClient::connectToServer( HostAddress *inAddress,
                                       long inTimeoutInMilliseconds,
                                       char *outTimedOut ) {

	if( !Socket::isFrameworkInitialized() ) {
		
		// try to init the framework
		
		int error = Socket::initSocketFramework();
		
		if( error == -1 ) {
			
			printf( "initializing network socket framework failed\n" );
			return NULL;
			}
		}

	unsigned long socketID = socket( AF_INET, SOCK_STREAM, 0 );	

	if( socketID == INVALID_SOCKET ) {
		printf( "Creating socket failed.\n" );
		return NULL;
		}

	union sock {
		struct  sockaddr s;
		struct  sockaddr_in i;
		} sock;
	//struct in_addr internet_address;
	//struct hostent *hp;

	struct in_addr *internet_address = 
		nameToAddress( inAddress->mAddressString );

	if( internet_address == NULL ) {
		printf( "Host name lookup failed:  " );
		inAddress->print();
		printf( "\n" );

        closesocket( socketID );
        return NULL;
		}

	//hp = gethostbyname( inAddress->mAddressString );
	//memcpy( &internet_address, *( hp->h_addr_list ), sizeof( struct in_addr ) );
	
	sock.i.sin_family = AF_INET;
	sock.i.sin_port = htons( inAddress->mPort );
	sock.i.sin_addr = *internet_address;


    int error;

    if( inTimeoutInMilliseconds != -1 ) {
        // use timeout
        
        error = timed_connect( socketID, &sock.s, sizeof( struct sockaddr ),
                               inTimeoutInMilliseconds );

        if( outTimedOut != NULL ) {
            if( error == -2 ) {
                *outTimedOut = true;
                
                if( inTimeoutInMilliseconds == 0 ) {
                    // caller didn't want to wait at all
                    error = 0;
                    }
                else {
                    // treat timeout as error
                    error = -1;
                    }
                }
            else {
                *outTimedOut = false;
                }
            }
        }
    else {
        // don't use timeout
        
        error = connect( socketID, &sock.s, sizeof( struct sockaddr ) );
        }


    delete internet_address;
    

    if( error == -1 ) {
		//printf( "Connecting to host failed:  " );
		//inAddress->print();
		//printf( "\n" );

        closesocket( socketID );
		return NULL;
		}
	
	// package into a Socket and return it
	Socket *returnSocket = new Socket();
	
	unsigned long *idSpace = new unsigned long[1];
	idSpace[0] = socketID;
	returnSocket->mNativeObjectPointer = (void *)idSpace;
	
    if( outTimedOut != NULL &&
        *outTimedOut ) {
        // not connected yet
        returnSocket->setConnected( false );
        }

	
	return returnSocket;
	}



/* Converts ascii text to in_addr struct.  NULL is returned if the
          address can not be found.
          Result must be destroyed by caller.
Adapted from the Unix Socket FAQ		  */
struct in_addr *nameToAddress( char *inAddress ) {
	struct hostent *host;

    static struct in_addr saddr;
    struct in_addr *copiedSaddr = new struct in_addr;
    
    
	/* First try it as aaa.bbb.ccc.ddd. */
	saddr.s_addr = inet_addr( inAddress );
	if( saddr.s_addr != INADDR_NONE ) {

        // copy to avoid returning pointer to stack
        memcpy( copiedSaddr, &saddr, sizeof( struct in_addr ) );
        return copiedSaddr;
		}

    // must keep this locked until we are done copying the in_addr out
    // of the returned hostent
    NetworkFunctionLocks::mGetHostByNameLock.lock();

    char hostFound = false;
    host = gethostbyname( inAddress );
    if( host != NULL ) {
        
        memcpy( copiedSaddr,
                *host->h_addr_list,
                sizeof( struct in_addr ) );
        
        hostFound = true;
        }

    NetworkFunctionLocks::mGetHostByNameLock.unlock();


    
    if( hostFound ) {
        return copiedSaddr;
        }
    else {
        delete copiedSaddr;
        }
    
	return NULL;
	}



/* timed_read adapted from gnut, by Josh Pieper */
/* Josh Pieper, (c) 2000 */
/* This file is distributed under the GPL, see file COPYING for details */

// just like connect except that it times out after time secs
// returns -2 on timeout, otherwise same as connect
int timed_connect( int inSocketID,
                   struct sockaddr *inSocketAddress,
                   int inAddressLength,
                   int inTimeoutInMilliseconds ) {
	int ret;
	fd_set fsr;
	struct timeval tv;
	int val;
    int len;
    
	//g_debug(1,"entering sock=%i secs=%i\n",inSocketID,inTimeoutInSeconds);
	
	//ret = fcntl( inSocketID, F_SETFL, O_NONBLOCK );
    // use ioctlsocket on win32 instead of fcntl
    // set to non-blocking mode
    unsigned long onFlag = 1;
    ret = ioctlsocket( inSocketID, FIONBIO, &onFlag );

    
    //g_debug(5,"fcntl returned %i\n",ret);
	
	if( ret < 0 ) {
        return ret;
        }
    
	ret = connect( inSocketID, inSocketAddress, inAddressLength );
	//g_debug(5,"connect returned %i\n",ret);
	
	if( ret == 0 ) {
		//g_debug(0,"immediate connection!\n");
		// wooah!  immediate connection
		// return -2;

        // changed from what Josh originally returned (-2)
        // immediate connection *can* happen sometimes, right?
        // for example, when connecting to localhost...
        return 0;
        }
	
//	if (errno != EINPROGRESS) {
//		perror("timed_connect, connect");
//		return ret;
//	}
	

	FD_ZERO( &fsr );
	FD_SET( inSocketID, &fsr );

    tv.tv_sec = inTimeoutInMilliseconds / 1000;
	int remainder = inTimeoutInMilliseconds % 1000;
	tv.tv_usec = remainder * 1000;
    

	ret = select( inSocketID+1, NULL, &fsr, NULL, &tv );
	//g_debug(5,"select returned %i\n",ret);

	if( ret==0 ) {
		// timeout
		//g_debug(1,"timeout\n");
		//fcntl( inSocketID, F_SETFL, 0 );
		unsigned long offFlag = 0;
        ret = ioctlsocket( inSocketID, FIONBIO, &offFlag );
		
		return -2;
        }

	len = 4;
	ret = getsockopt( inSocketID, SOL_SOCKET, SO_ERROR, (char*)( &val ), &len );
	//g_debug(5,"getsockopt returned %i val=%i\n",ret,val);
	
	if( ret < 0 ) {
		//g_debug(1,"getsockopt returned: %i\n",ret);
		return ret;
        }

	if (val!=0) {
		//g_debug(3,"returning failure!\n");
		return -1;
        }
	
	//ret = fcntl( inSocketID, F_SETFL, 0 );
    // use ioctlsocket on win32 instead of fcntl
    // set back to blocking mode
    unsigned long offFlag = 0;
    ret = ioctlsocket( inSocketID, FIONBIO, &offFlag );
    
    
	//g_debug(1,"fcntl: %i\n",ret);
	
	//g_debug(3,"returning success val=%i\n",val);
	return 0;
    }
