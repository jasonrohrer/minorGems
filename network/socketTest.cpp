/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2001-January-28		Jason Rohrer
 * Added more output.
 * Added better error checking.
 *
 * 2001-December-14		Jason Rohrer
 * Added a test of HostAddress::getLocalAddress().
 *
 * 2002-July-30		Jason Rohrer
 * Added a test of streams.
 *
 * 2002-July-31   Jason Rohrer
 * Changed to test multiple blocks.
 *
 * 2002-February-4   Jason Rohrer
 * Added test of getting local address from socket.
 */


#include "Socket.h"
#include "SocketStream.h"
#include "SocketServer.h"
#include "SocketClient.h" 
#include "HostAddress.h"

#include <unistd.h>


int main() {

	HostAddress *address = HostAddress::getLocalAddress();

	printf( "Local address: " );
	address->print();
	printf( "\n" );

	int port = 5158;
    
	SocketServer *server = new SocketServer( port, 100 );
	
	printf( "Waiting for a connection on port %d\n", port );
	Socket *receiveSocket = server->acceptConnection();
	
	if( receiveSocket == NULL ) {
		return 1;
		}
	
	printf( "Connection received\n" );

    HostAddress *localAddress = receiveSocket->getLocalHostAddress();

    if( localAddress != NULL ) {
        printf( "Our local address (fetched from socket) is  " );
        localAddress->print();
        printf( "\n" );
        delete localAddress;
        }
    
    
    SocketStream *receiveStream = new SocketStream( receiveSocket );
    receiveStream->setReadTimeout( 10000 );
    
	int numBytes = 4000;

	int checksum = 0;
    
	unsigned char *buffer = new unsigned char[numBytes];
    /*    
    for( int i=0; i<numBytes; i++ ) {
        int numRec = receiveStream->read( buffer, 1 );
        checksum += buffer[ 0 ];

        if( numRec != 1) {
            printf( "Faiedl to read.\n" );
            return 1;
            }
        //sleep( 1 );
        }
    */
    int count = 0;
    while( true ) {
        int numRec = receiveStream->read( buffer, numBytes );
        printf( "Received %d successfully,\tcount = %d\n", numBytes, count );
        count++;
        }
    /*
	for( int i=0; i<numBytes; i++ ) {
		checksum += buffer[ i ];
		}
    */
	printf( "Checksum: %d\n", checksum );

    delete receiveStream;
	delete receiveSocket;
	delete server;
	
	return 0;
	} 
