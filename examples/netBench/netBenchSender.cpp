/*
 * Modification History
 *
 * 2002-June-3   Jason Rohrer
 * Created.  
 */

#include "minorGems/network/SocketClient.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"
#include "minorGems/network/HostAddress.h"
#include "minorGems/system/Time.h"

#include "minorGems/util/stringUtils.h"



#include <string.h>
#include <stdio.h>


#define BUFFER_SIZE 5000


void usage( char *inAppName );


int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 4 ) {
		usage( inArgs[0] );
		}

	long port;
	int numRead = sscanf( inArgs[2], "%d", &port );

	if( numRead != 1 ) {
		printf( "port number must be a valid integer:  %s\n", inArgs[2] );
		usage( inArgs[0] );
		}

    long kiBToSend;

    numRead = sscanf( inArgs[3], "%d", &kiBToSend );

	if( numRead != 1 ) {
		printf( "kiB to send must be a valid integer:  %s\n", inArgs[3] );
		usage( inArgs[0] );
		}
    
	char *copiedArg = stringDuplicate( inArgs[1] );
	
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
	

	int numWritten = BUFFER_SIZE;
    
    int bytesSent = 0;

    unsigned long seconds;
    unsigned long milliseconds;

    Time::getCurrentTime( &seconds, &milliseconds );

    long blocksToSend = ( kiBToSend * 1024 ) / BUFFER_SIZE;
    
    for( int i=0; i<blocksToSend && numWritten == BUFFER_SIZE; i++ ) {
        numWritten = outStream->write( buffer, BUFFER_SIZE );

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

	if( BUFFER_SIZE != numWritten ) {
		printf( "network connection failed during transmission\n" );
		}	

	delete receiverAddress;
	delete sock;
	delete outStream;
	
	delete [] buffer;
	
	return 0;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s receiver_address receiver_port kiB_to_send\n", inAppName );

	printf( "Examples:\n" );
	printf( "\t%s 192.168.1.2 5888 5000\n", inAppName );
	printf( "\t%s myhost.mydomain.com 5888 10\n", inAppName );
	
	exit( 1 );
	}
