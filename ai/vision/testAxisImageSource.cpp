/*
 * Modification History
 *
 * 2001-April-23   Jason Rohrer
 * Created.
 *
 * 2001-April-29   Jason Rohrer
 * Fixed a bug that caused a socket to be deleted twice.
 * Changed so that the image source creates the socket.
 * Completed testing successfully.
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 */
 

#include "AxisImageSource.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/HostAddress.h"

#include "stdio.h"

// test program for an axis image source.

int main() {
	char *address = new char[ 99 ];
	sprintf( address, "128.84.220.234" );

	HostAddress *address = new HostAddress( address, 80 );

	AxisImageSource *source =
		new AxisImageSource( address, 320, 240, 100 );
	
	Image *image = source->getNextImage();
	if( image == NULL ) {
		printf( "failed to retrieve image from image source.\n" );
		}
	delete image;

	printf( "trying to get a second image from the same socket\n" );
	image = source->getNextImage();
	if( image == NULL ) {
		printf( "failed to retrieve image from image source.\n" );
		}
	
		
	delete source;
	delete image;

	delete address;

	return 0;
	}







