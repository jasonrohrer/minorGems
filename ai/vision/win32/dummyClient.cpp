/*
 * Modification History
 *
 * 2001-February-28		Jason Rohrer
 * Created.
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 */



#include "minorGems/network/SocketClient.h"

#include <time.h>
#include <string.h>

// prototypes:
void usage( char *inAppName );

// test function for stereo
int main( int inNumArgs, char **inArgs ) {
	
	int port = 5003;
	
	char *address = "localhost";

	int addressLength = strlen( address );
	char *copiedArg = new char[ addressLength + 1 ];
	strcpy( copiedArg, address );
	
	
	HostAddress *serverAddress = new HostAddress( copiedArg, port );
	
	printf( "Dummy client trying to connect to \n" );
	
	serverAddress->print();
	printf( "\n" );
	
	Socket *sock = 
		SocketClient::connectToServer( serverAddress );
	if( sock == NULL ) {	
		printf( "   connection failed\n" );
		return -1;
		}
	printf( "   connected.\n" );
	
	
	printf( "Waiting for control byte\n" );
	
	unsigned char *controlByte = new unsigned char[1];
	
	sock->receive( controlByte, 1, -1 );
	
	printf( "Byte received.  Exiting\n" );
	
	delete sock;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s image_server_port bmp_file dummy_port\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s 3000 c:\\capture\\SnapCap.bmp 5003\n", inAppName );
	
	std::exit( 1 );
	}
