/*
 * Modification History
 *
 * 2001-December-14     Jason Rohrer
 * Created.
 * Has not been compiled or tested under windows.
 *
 * 2002-March-25    Jason Rohrer
 * Added function for conversion to numerical addresses.
 *
 * 2002-March-26    Jason Rohrer
 * Changed to use strdup.
 * Fixed problem on some platforms where local address does
 * not include domain name.
 * Fixed so that numerical address is actually retrieved by
 * getNumericalAddress().
 *
 * 2002-April-6    Jason Rohrer
 * Replaced use of strdup.
 *
 * 2002-April-7    Jason Rohrer
 * Changed to return numerical address from getLocalAddress,
 * since getdomainname() call not available on Win32.
 *
 * 2002-April-8   Jason Rohrer
 * Changed to init socket framework before getting local address.
 * Changed to check if address is numerical before
 * performing a lookup on it.
 *
 * 2004-January-4   Jason Rohrer
 * Added use of network function locks.
 *
 * 2008-November-2   Jason Rohrer
 * Added framework init in getNumericalAddress.
 */



#include "minorGems/network/HostAddress.h"
#include "minorGems/network/NetworkFunctionLocks.h"
#include "minorGems/util/stringUtils.h"

#include <winsock.h>
#include <windows.h>

#include <string.h>


#include "minorGems/network/Socket.h"



HostAddress *HostAddress::getLocalAddress() {
	
    // first make sure sockets are initialized
    if( !Socket::isFrameworkInitialized() ) {
		
		// try to init the framework
		
		int error = Socket::initSocketFramework();
		
		if( error == -1 ) {
			
			printf( "initializing network socket framework failed\n" );
			return NULL;
			}
		}
	
	
	
	
	int bufferLength = 200;
	char *buffer = new char[ bufferLength ];

	gethostname( buffer, bufferLength );

    char *name = stringDuplicate( buffer );

	delete [] buffer;


    // this class will destroy name for us
    HostAddress *nameAddress = new HostAddress( name, 0 );

    HostAddress *fullAddress = nameAddress->getNumericalAddress();

    delete nameAddress;

    
    return fullAddress;    
	}



HostAddress *HostAddress::getNumericalAddress() {

    // make sure we're not already numerical
    if( this->isNumerical() ) {
        return this->copy();
        }


    if( !Socket::isFrameworkInitialized() ) {
		
		// try to init the framework
		
		int error = Socket::initSocketFramework();
		
		if( error == -1 ) {
			
			printf( "initializing network socket framework failed\n" );
			return NULL;
			}
		}

    // need to use two function locks here

    // first, lock for gethostbyname
    NetworkFunctionLocks::mGetHostByNameLock.lock();
    struct hostent *host = gethostbyname( mAddressString );

    if (host != NULL) {

        // this line adapted from the
        // Unix Socket FAQ
        struct in_addr *inetAddress = (struct in_addr *) *host->h_addr_list;


        // now lock for inet_ntoa
        NetworkFunctionLocks::mInet_ntoaLock.lock();
        // the returned string is statically allocated, so copy it
        char *inetAddressString = stringDuplicate( inet_ntoa( *inetAddress ) );

        NetworkFunctionLocks::mInet_ntoaLock.unlock();

        
        // done with returned hostent now, so unlock first lock
        NetworkFunctionLocks::mGetHostByNameLock.unlock();
        
        return new HostAddress( inetAddressString, mPort );
        }
    else {

        // unlock first lock before returning
        NetworkFunctionLocks::mGetHostByNameLock.unlock();
        
        return NULL;
        }

    }



char HostAddress::isNumerical() {

    unsigned long returnedValue = inet_addr( mAddressString );


    if( returnedValue == INADDR_NONE ) {
        return false;
        }
    else {
        return true;
        }

    }


