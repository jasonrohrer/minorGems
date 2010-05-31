/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2001-January-28		Jason Rohrer
 * Added more output.
 * Changed so that it doesn't sleep in the middle of a block
 * since the requirements of the socket interface have changed
 * and implementations no longer have to support this behavior.
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 *
 * 2002-July-30		Jason Rohrer
 * Fixed a bug in localhost address.  Added a test of streams.
 *
 * 2002-July-31   Jason Rohrer
 * Changed to test multiple blocks.
 *
 * 2002-December-2   Jason Rohrer
 * Changed to test socket creation failure.
 * Added counter to output.
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

    char *name = new char[99];
	sprintf( name, "63.249.65.249" );
	
    
	//char *name = new char[99];
	//sprintf( name, "192.168.1.2" );
	
	//char *name = "192.168.1.1";
	//int addressLength = 11;
	
	//char *name = "monolith.2y.net";
	//int addressLength = 15;
	
	int port = 5158;

	HostAddress *address = new HostAddress( name, port );

	printf( "Trying to connect to server: " );
	address->print();
	printf( "\n" );

    Socket *sock;
    int numConnections = 0;
    while( true ) {
        sock = SocketClient::connectToServer( address );
	
        if( sock == NULL ) {
            printf( "%d Connecting to server failed\n", numConnections );
            //return 1;
            }
        else {
            printf( "%d Connection established\n", numConnections );
            HostAddress *localAddress = sock->getLocalHostAddress();

            if( localAddress != NULL ) {
                printf( "Our local address (fetched from socket) is  " );
                localAddress->print();
                printf( "\n" );
                delete localAddress;
                }

            delete sock;
            }
        //usleep( 1000 );
        numConnections++;
        }
    
	int numBytes = 4000;

	unsigned char *buffer = new unsigned char[numBytes];
	for( int i=0; i<numBytes; i++ ) {
		buffer[i] = i;
		}

    SocketStream *stream = new SocketStream( sock );
    
	//printf( "sleeping\n" );
	//sleep( 10 );
    
    int count = 0;
    while( true ) {
        printf( "sending %d bytes\n", numBytes );
        int numSent = stream->write( buffer, numBytes );
	
        printf( "Sent %d successfully,\tcount = %d\n", numSent, count );
        count++;
        }
    
    int checksum = 0;
	for( int i=0; i<numBytes; i++ ) {
		checksum += buffer[ i ];
		}
	printf( "Checksum: %d\n", checksum );

    
    printf( "Deleting stream\n" );
	delete stream;
    printf( "Deleting socket\n" );
	delete sock;
    printf( "Deleting address\n" );
	delete address;

    printf( "Returning\n" );
	return 0;
	} 
