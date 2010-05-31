/*
 * Modification History
 *
 * 2001-August-02   Jason Rohrer
 * Created.  
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 *
 * 2002-June-1   Jason Rohrer
 * Added support for timing the transmission.
 */

#include "minorGems/network/SocketClient.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"
#include "minorGems/network/HostAddress.h"
#include "minorGems/system/Time.h"



#include <string.h>
#include <stdio.h>


#define BUFFER_SIZE 5000


void usage( char *inAppName );


int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 3 ) {
		usage( inArgs[0] );
		}

	long port;
	int numRead = sscanf( inArgs[2], "%d", &port );

	if( numRead != 1 ) {
		printf( "port number must be a valid integer:  %s\n", inArgs[2] );
		usage( inArgs[0] );
		}

	int addressLength = strlen( inArgs[1] );
	char *copiedArg = new char[ addressLength + 1 ];
	strcpy( copiedArg, inArgs[1] );
	
	HostAddress *receiverAddress =
		new HostAddress( copiedArg, port ); 

	printf( "connecting to host:  " );
	receiverAddress->print();
	printf( "\n" );
	Socket *sock = SocketClient::connectToServer( receiverAddress );

	
	if( sock == NULL ) {
		printf( "connection to host failed\n" );
		return( 1 );
		}

	printf( "connection successful\n" );
	
	SocketStream *outStream = new SocketStream( sock );

	
	unsigned long checksum = 0;

	unsigned char *buffer = new unsigned char[ BUFFER_SIZE ];
	

	numRead = BUFFER_SIZE;
	int numWritten = BUFFER_SIZE;

    int bytesSent = 0;

    unsigned long seconds;
    unsigned long milliseconds;

    Time::getCurrentTime( &seconds, &milliseconds );
    
    
	while( numWritten == numRead && numRead == BUFFER_SIZE ) {

		// read a buffer full of data from standard in
		numRead = fread( buffer, 1, BUFFER_SIZE, stdin ); 

		// add the buffer to our checksum
		for( int i=0; i<numRead; i++ ) {
			checksum += buffer[i];
			}

		
		// write the buffer out to our network stream
		numWritten = outStream->write( buffer, numRead );

        bytesSent += numWritten;
		}

    unsigned long elapsedMilliseconds =
        Time::getMillisecondsSince(
            seconds,
            milliseconds );

    float elapsedSeconds = ( (float)elapsedMilliseconds ) / 1000;
    
    float bytesPerSec = (float)bytesSent / elapsedSeconds;

    printf( "%.2f second elapsed\n", elapsedSeconds );
    printf( "%.2f bytes per second\n", bytesPerSec );

	if( numRead != numWritten ) {
		printf( "network connection failed during transmission\n" );
		}
	

	
	printf( "checksum = %d\n", checksum );


	delete receiverAddress;
	delete sock;
	delete outStream;
	
	delete [] buffer;
	
	return 0;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s receiver_address receiver_port\n", inAppName );

	printf( "Examples:\n" );
	printf( "\t%s 192.168.1.2 5888\n", inAppName );
	printf( "\t%s myhost.mydomain.com 5888\n", inAppName );
	
	exit( 1 );
	}
