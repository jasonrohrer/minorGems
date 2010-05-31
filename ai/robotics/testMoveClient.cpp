/*
 * Modification History
 *
 * 2001-February-25		Jason Rohrer
 * Created.
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor. 
 */


#include "DirectControl.h"

#include "minorGems/network/HostAddress.h"
#include "minorGems/network/HostAddress.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketStream.h"

#include <string.h>


// prototypes:
void usage( char *inAppName );


int main( int inNumArgs, char **inArgs ) {
	printf( "Test networked robot control client.\n\n" );
	
	if( inNumArgs != 3 ) {
		usage( inArgs[0] );
		}
	
	int port;
	if( sscanf( inArgs[2], "%d", &port) != 1 ) {
		printf( "Port must be a number.  Bad argument:  %s\n", inArgs[2] );
		usage( inArgs[0] );
		}
	
	int addressLength = strlen( inArgs[1] );
	char *copiedArg = new char[ addressLength + 1 ];
	strcpy( copiedArg, inArgs[1] );

	HostAddress *address = new HostAddress( copiedArg, port );
	
	printf( "Trying to connect to server " );
	address->print();
	printf( "\n" );
	
	Socket *socket = SocketClient::connectToServer( address );
	if( socket != NULL ) {
		printf( "connection established\n" );
		}
	else {
		printf( "connection failed\n" );
		delete address;
		return -1;
		} 
	
	SocketStream *stream = new SocketStream( socket );
	
	
	
	
	printf( "Enter -1 to for command to quit at any time.\n\n" );

	int lastInt = 0;	
	
	while( lastInt != -1 ) {
		printf( "Enter command:\n" );
			
		
		unsigned char command;
		double time;

		scanf( "%d", &lastInt );
		command = lastInt;

		if( lastInt != -1 ) {
			
			int numDoublesRead = 0;
			while( numDoublesRead == 0 ) {
				printf( "Enter time in seconds:\n" );
				numDoublesRead = scanf( "%lf", &time );
			
			
				DirectControl *control = new DirectControl( command, time );
				control->serialize( stream );
				delete control;
				}
			}
		}
	
	delete stream;
	delete address;
	
	return 0;
	}


void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s server_address port_number\n", inAppName );

	printf( "examples:\n\t" );
	printf( "%s localhost 6877\n\t", inAppName );
	printf( "%s 192.168.1.57 6877\n", inAppName );
	
	std::exit( 1 );
	}
