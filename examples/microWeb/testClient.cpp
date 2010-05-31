/*
 * Modification History
 *
 * 2001-May-12   Jason Rohrer
 * Created.  
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 */

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/HostAddress.h"


#include <string.h>
#include <stdio.h>
#include <time.h>


#define DEFAULT_CONFIG_FILE "microWeb.config"

void usage( char *inAppName );

// a test client for benchmarking web servers

int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 4 ) {
		usage( inArgs[0] );
		}

	// read the port number argument
	int portNumber;
	int numRead = sscanf( inArgs[2], "%d", &portNumber );

	if( numRead != 1 ) {
		usage( inArgs[0] );
		}

	
	int addressLength = strlen( inArgs[1] );
	char *copiedArg = new char[ addressLength + 1 ];
	strcpy( copiedArg, inArgs[1] );
	
	HostAddress *address = new HostAddress( copiedArg, portNumber );

	
	printf( "trying to connect to host " );
	address->print();
	printf( "\n" );

	Socket *sock = SocketClient::connectToServer( address );

	if( sock == NULL ) {
		printf( "connection failed\n" );
		}
	else {
		// connection established
		printf( "connection established\n" );

		SocketStream *stream = new SocketStream( sock );

		char *requestBuffer = new char[200];

		sprintf( requestBuffer, "GET /%s HTTP/1.0\r\nAccept: */*\r\n\r\n",
				 inArgs[3] );

		printf( "sending HTTP request:\n" );
		printf( "%s", requestBuffer );

		stream->write( (unsigned char*)requestBuffer,
					   strlen( requestBuffer ) );

		delete [] requestBuffer;

		int bufferSize = 4096;
		unsigned char *responseBuffer = new unsigned char[ bufferSize ];
		int bytesRead = bufferSize;
		int totalNumBytesRead = 0;

		printf( "reading response\n" );
		
		unsigned long startTime = time( NULL );
		
		while( bytesRead == bufferSize ) {
			bytesRead = stream->read( responseBuffer, bufferSize ); 

			totalNumBytesRead += bytesRead;
			}

		unsigned long netTime = time( NULL ) - startTime;

		printf( "Read %d bytes in %d seconds (at %f bytes/sec)\n",
				totalNumBytesRead, netTime,
				(double) totalNumBytesRead / (double)netTime );
		
		delete [] responseBuffer;

		delete stream;
		delete sock;
		}
	delete address;


	return 0;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s server_address server_port file_name\n", inAppName );

	printf( "Example:\n" );
	printf( "\t%s 192.168.1.2 80 test.mp3\n", inAppName );
	
	exit( 1 );
	}
