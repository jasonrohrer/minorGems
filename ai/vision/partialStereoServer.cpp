/*
 * Modification History
 *
 * 2001-February-26		Jason Rohrer
 * Created.
 *
 * 2001-February-28		Jason Rohrer
 * Fixed some memory leaks. 
 */

#include "LocalWindowStereo.h"

#include "minorGems/graphics/Image.h"

#include "minorGems/util/random/StdRandomSource.h"

#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"

#include <time.h>
#include <string.h>

// prototypes:
void usage( char *inAppName );

// test function for stereo
int main( int inNumArgs, char **inArgs ) {
	
	if( inNumArgs < 3 ) {
		usage( inArgs[0] );
		} 
	
	int port;
    int windowSize;

    if( sscanf( inArgs[1], "%d", &port ) != 1 ) {
        printf( "port_number must be a number.  Bad argument:  %s\n", 
                inArgs[1] );
        usage( inArgs[0] );
        }   
	if( sscanf( inArgs[2], "%d", &windowSize ) != 1 ) {
        printf( "window_size must be a number.  Bad argument:  %s\n", 
                inArgs[2] );
        usage( inArgs[0] );
        }  
	
	StdRandomSource *randSource = new StdRandomSource();
	
	LocalWindowStereo *stereo = new LocalWindowStereo( 30,
		windowSize, randSource );
	
	
	SocketServer *server = new SocketServer( port, 50 );
	
	printf( "waiting for a connection\n" );
	Socket *sock = server->acceptConnection();
	printf( "connection received.\n" );
	
	SocketStream *stream = new SocketStream( sock );
	
	// while there are no errors
	while( stream->InputStream::getLastError() == NULL
			&& stream->OutputStream::getLastError() == NULL ) {
		// first, receive ranges, x and then y
		double xStart;
		double xEnd;
		double yStart;
		double yEnd;
		stream->readDouble( &xStart );
		stream->readDouble( &xEnd );
		stream->readDouble( &yStart );
		stream->readDouble( &yEnd );
		
		stereo->setRange( xStart, xEnd, yStart, yEnd );
		
		Image *left = new Image( 1, 1, 1 );
		Image *right = new Image( 1, 1, 1 );
		
		printf( "received image pair\n" );
			
		// receive left, then right images
		left->deserialize( stream );
		right->deserialize( stream );

		// compute the result
		printf( "computing the stereo result...\n" );
		Image *outImage = stereo->computeDepthMap( left, right );
		
		// send the result
		outImage->serialize( stream );
		
		printf( "sent back result\n" );
		
		delete left;
		delete right;
		delete outImage;
		}
	// a socket error occured
	printf( "connection broken\n" );
	
	delete stereo;
	delete randSource;
	delete server;
	delete stream;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s port_number window_size\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s 3003 15\n", inAppName );
	
	std::exit( 1 );
	}
