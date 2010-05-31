/*
 * Modification History
 *
 * 2004-November-3   Jason Rohrer
 * Created.
 *
 * 2004-November-4   Jason Rohrer
 * Added code for win32 compatibility.
 * Fixed a memcpy bug.
 *
 * 2004-December-7   Jason Rohrer
 * Fixed a bug in the evaluation of wait return codes.
 */


/*
 * The following changes were needed for win32 compatibility:
 * -- Winsock.h included
 * -- socklen_t defined
 * -- closesocket instead of close
 * -- inet_aton does not exist on win32 (switched to the linux-obsolete
 *    inet_addr)
 */


#include "minorGems/network/SocketUDP.h"
#include "minorGems/network/NetworkFunctionLocks.h"

#include "minorGems/util/stringUtils.h"

#ifndef WIN_32
    // standard unix includes
    #include <sys/types.h>
    #include <sys/socket.h>

    #include <netinet/in.h>

    #include <arpa/inet.h>

    #include <string.h>
    #include <unistd.h>
    #include <sys/time.h>
#else
    // special includes for win32
    #include <winsock.h>

    // windows does not define socklen_t
    typedef int socklen_t;

    // windows does not have a close function for sockets
    #define close( inSocketID )     closesocket( inSocketID )
#endif


#ifdef BSD
    // BSD does not define socklen_t
    typedef int socklen_t;
#endif




// prototypes

/**
 * Waits on a socket for incoming data.
 *
 * @param inSocketID the socket to watch.
 * @param inMilliseconds the timeout.
 *
 * @return 1 if data is ready to be read with recvfrom, -2 if we
 *   timeout, and -1 on an error.
 */
int waitForIncomingData( int inSocketID, 
                         long inMilliseconds );




SocketUDP::SocketUDP( unsigned short inReceivePort ) {

    int socketID;
    socketID = socket( AF_INET, SOCK_DGRAM, 0 );


    // bind to receivePort    
    struct sockaddr_in bindAddress;
    
    bindAddress.sin_family = AF_INET;
    
    bindAddress.sin_port = htons( inReceivePort );
    
    bindAddress.sin_addr.s_addr = INADDR_ANY;
    
    bind( socketID, (struct sockaddr *)&bindAddress, sizeof(bindAddress) );

    
    int *socketIDArray = new int[1];
	socketIDArray[0] = socketID;

    mNativeObjectPointer = (void *)socketIDArray;
    }

        

SocketUDP::~SocketUDP() {
    // unwrap our native object
    int *socketIDArray = (int *)( mNativeObjectPointer );
	int socketID = socketIDArray[0];
	
    close( socketID );
    
	delete [] socketIDArray;
    }



struct UDPAddress *SocketUDP::makeAddress( const char *inAddress,
                                           unsigned short inPort ) {

    // try converting it from aaa.bbb.ccc.ddd
    int convertedAddress = inet_addr( inAddress );
    
    if( convertedAddress != -1 ) {
        struct UDPAddress *address = new struct UDPAddress;

        address->mIPAddress = convertedAddress;
        address->mPort = htons( inPort );

        return address;
        }
    else {
        return NULL;
        }
    }



char *SocketUDP::extractAddress( struct UDPAddress *inAddress,
                                 unsigned short *outPort ) {

    struct in_addr addressStructure;

    addressStructure.s_addr = inAddress->mIPAddress;
    
    
    NetworkFunctionLocks::mInet_ntoaLock.lock();
    // returned string is statically allocated, copy it
    char *addressString = stringDuplicate( inet_ntoa( addressStructure) );
    NetworkFunctionLocks::mInet_ntoaLock.unlock();

    *outPort = ntohs( inAddress->mPort );

    return addressString;
    }



int SocketUDP::send( struct UDPAddress *inAddress,
                     unsigned char *inData, unsigned long inNumBytes ) {

    // unwrap our native object
    int *socketIDArray = (int *)( mNativeObjectPointer );
	int socketID = socketIDArray[0];


    // pack our address into the required structure
    struct sockaddr_in toAddress;
        
    toAddress.sin_family = AF_INET;
        
    toAddress.sin_port = inAddress->mPort; 
        
    toAddress.sin_addr.s_addr = inAddress->mIPAddress;
         

        
    return sendto( socketID, (char *)inData, inNumBytes, 0,
                   (struct sockaddr *)( &toAddress ),
                   sizeof( toAddress ) );
    }



int SocketUDP::receive( struct UDPAddress **outAddress,
                        unsigned char **outData,                     
                        long inTimeout ) {

    // unwrap our native object
    int *socketIDArray = (int *)( mNativeObjectPointer );
	int socketID = socketIDArray[0];



    if( inTimeout != -1 ) {
        int waitValue = waitForIncomingData( socketID, inTimeout );

        // timed out or saw an error while waiting
        if( waitValue == -1 || waitValue == -2 ) {
            *outAddress = NULL;
            *outData = NULL;
            
            return waitValue;
            }
        
        // else we have data waiting
        }


    struct sockaddr_in fromAddress;

    int bufferSize = 10000;
    unsigned char *receiveBuffer = new unsigned char[ bufferSize ];

    socklen_t fromAddressLength = sizeof( fromAddress );

    
    int numReceived;

    
    numReceived = recvfrom( socketID, (char *)receiveBuffer,
                            bufferSize, 0,
                            (struct sockaddr *)( &fromAddress ),
                            &fromAddressLength );

    // if no error and no overflow
    if( numReceived >=0 && numReceived <= bufferSize ) {
        *outAddress = new struct UDPAddress;
    
        (*outAddress)->mPort = fromAddress.sin_port; 
        (*outAddress)->mIPAddress = fromAddress.sin_addr.s_addr;

        *outData = new unsigned char[ numReceived ];

        memcpy( (void *)( *outData ), (void *)receiveBuffer, numReceived );
        }
    else {
        *outAddress = NULL;
        *outData = NULL;
        }
    
    delete [] receiveBuffer;

    return numReceived;
    }


/* socket timing code adapted from gnut, by Josh Pieper */
/* Josh Pieper, (c) 2000 */
/* This file is distributed under the GPL, see file COPYING for details */
int waitForIncomingData( int inSocketID, 
                         long inMilliseconds ) {

    fd_set fsr;
	struct timeval tv;
	int returnValue;
	
	FD_ZERO( &fsr );
	FD_SET( inSocketID, &fsr );
 
	tv.tv_sec = inMilliseconds / 1000;
	int remainder = inMilliseconds % 1000;
	tv.tv_usec = remainder * 1000; 
	returnValue = select( inSocketID + 1, &fsr, NULL, NULL, &tv );
	
	if( returnValue == 0 ) {
		return -2;
		}
	else if( returnValue == -1 ) {
		printf( "Selecting socket during receive failed.\n" );
		return returnValue;
		}
    else {
        return returnValue;
        }    
    }
