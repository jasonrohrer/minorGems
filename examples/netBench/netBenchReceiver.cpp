/*
 * Modification History
 *
 * 2002-June-3   Jason Rohrer
 * Created.  
 */


#include "minorGems/network/SocketServer.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define BUFFER_SIZE 5000


void usage( char *inAppName );


int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 2 ) {
		usage( inArgs[0] );
		}

	long port;
	int numRead = sscanf( inArgs[1], "%d", &port );

	if( numRead != 1 ) {
		printf( "port number must be a valid integer:  %s\n", inArgs[1] );
		usage( inArgs[0] );
		}

   
	SocketServer *server = new SocketServer( port, 1 );
	
	printf( "listening for a connection on port %d\n", port );
	Socket *sock = server->acceptConnection();

	if( sock == NULL ) {
		printf( "socket connection failed\n" );
		return( 1 );
		}
	printf( "connection received\n" );
	
	
	SocketStream *inStream = new SocketStream( sock );

	
	unsigned long checksum = 0;

	unsigned char *buffer = new unsigned char[ BUFFER_SIZE ];
	

	numRead = BUFFER_SIZE;

    int bytesReceived = 0;
    
	while( numRead == BUFFER_SIZE ) {

		// read a buffer full of data from standard in
		numRead = inStream->read( buffer, BUFFER_SIZE );

        bytesReceived += numRead;
		}

	
	printf( "connection broken.  %d bytes received.\n", bytesReceived );


	delete sock;
	delete server;
	delete inStream;
	
	delete [] buffer;
	
	return 0;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s receiver_port\n", inAppName );

	printf( "Example:\n" );
    printf( "\t%s 5888 \n", inAppName );
	
	exit( 1 );
	}
