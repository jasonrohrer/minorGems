/*
 * Modification History
 *
 * 2001-April-12   Jason Rohrer
 * Created.
 *
 * 2001-April-17   Jason Rohrer
 * Worked on implementation more.
 *
 * 2001-April-23   Jason Rohrer
 * Fixed many compile bugs.
 * Modified to the point of receiving a jpeg image
 * from the camera correctly.
 *
 * 2001-April-29   Jason Rohrer
 * Updated to actually use a JPEGImageConverter.
 * Changed to work with connections that are not kept alive
 * between images.
 *
 * 2001-May-10   Jason Rohrer
 * Removed many debugging print statements.
 */
 
 
#ifndef AXIS_IMAGE_SOURCE_INCLUDED
#define AXIS_IMAGE_SOURCE_INCLUDED 

#include "minorGems/graphics/converters/JPEGImageConverter.h"

#include "ImageSource.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/network/HostAddress.h"

#include <string.h>
#include <stdio.h>

/**
 * Implementaion of ImageSource that retrieves images from a
 * networked Axis webcam.
 *
 * @author Jason Rohrer
 */
class AxisImageSource : public ImageSource {
	
	public:

		/**
		 * Constructs an AxisImageSource.
		 *
		 * @param inServerAddress the address of the axis camera server.
		 *   Is NOT destroyed when this class is destroyed.
		 * @param inImageWidth the width of the image to retrieve
		 *   from the camera server.  Should be a standard image width
		 *   (for example, 320 or 640 ).
		 * @param inImageHeight the height of the image to retrieve
		 *   from the camera server.  Should be a standard image height
		 *   (for example, 240 or 480 ).
		 * @param inCompression the compression setting for the retireved
		 *   image, in [0,100].  Higher values specify more compression.
		 */
		AxisImageSource( HostAddress *inServerAddress,
						 int inImageWidth,
						 int inImageHeight,
						 int inCompression );


		~AxisImageSource();
		
		
		// implements ImageSource interface
		virtual Image *getNextImage();


		
	private:
		Socket *mServerSocket;
		HostAddress *mServerAddress;
		SocketStream *mSocketStream;
		
		JPEGImageConverter *mConverter;

		int mImageWidth;
		int mImageHeight;
		int mCompression;
	};



inline AxisImageSource::AxisImageSource( HostAddress *inServerAddress,
										 int inImageWidth,
										 int inImageHeight,
										 int inCompression )
	: mServerAddress( inServerAddress ),
	  mConverter( new JPEGImageConverter( 50 ) ),
	  mImageWidth( inImageWidth ), mImageHeight( inImageHeight ),
	  mCompression( inCompression ) {

	if( mCompression > 100 || mCompression < 0 ) {
		// do it this way to avoid having the same print statement in
		// two places.
		if( mCompression > 100 ) {
			mCompression = 100;
			}
		else {
			mCompression = 0;
			}
		printf( "Compression value must be in range [0,100]\n" );
		}
	}



inline AxisImageSource::~AxisImageSource() {
	delete mConverter;
	}



inline Image *AxisImageSource::getNextImage() {

	// connect to the camera
	Socket *mServerSocket =
		SocketClient::connectToServer( mServerAddress );

	if( mServerSocket == NULL ) {
		printf( "connection to camera " );
		mServerAddress->print();
		printf( " failed\n" );

		return NULL;
		}

	// else we connected successfully
	
	mSocketStream = new SocketStream( mServerSocket );

	
	// buffer for the request string
	char *requestBuffer = new char[99];

	// compose the request string


	/*
	  FIXME:  specifying any parameters to
	  image.cgi cause libjpeg to segfault.
	  
	sprintf( requestBuffer,
	"GET /axis-cgi/jpg/image.cgi?resolution=%dx%d&compression=%d HTTP/1.0 \r\nConnection: Keep-Alive \r\nAccept: image/jpeg \r\n\r\n",
		 mImageWidth, mImageHeight, mCompression );
	*/

	// for now, just use the camera's default image settings
	sprintf( requestBuffer,
			 "GET /jpg/image.jpg HTTP/1.0 \r\nAccept: image/jpeg \r\n\r\n" );
	
	// printf( "Sending URL:\n" );
	// printf( "%s", requestBuffer );
	
	// send our request buffer through the socket
	mSocketStream->write( (unsigned char *)requestBuffer,
						  strlen( requestBuffer ) );

	
	unsigned char *buffer = new unsigned char[1];
	buffer[0] = 0;

	// printf( "Reading response:\n" );
	// read past 3 newlines
	// our model of camera doesn't send the image size in its response
	for( int i=0; i<3; i++ ) {
		// look for the ascii code for \n
		while( buffer[0] != 10 ) {
			mSocketStream->read( buffer, 1 );
			// printf( "%c", buffer[0] );
			}
		buffer[0] = 0;
		}

	// printf( "read 3 newlines\n" );
	
	// the next thing in the stream is our jpeg data
	Image *returnImage = mConverter->deformatImage( mSocketStream );

	// printf( "read and converted jpeg image\n" );
	
	// finally, read the last two line end characters
	mSocketStream->read( buffer, 1 );
	mSocketStream->read( buffer, 1 );

	delete [] buffer;

	
	delete [] requestBuffer;

	// close the socket, since the camera doesn't seem
	// to keep it alive anyway
	delete mSocketStream;
	delete mServerSocket;


	// return image will be NULL if JPEG decompression failed.
	return returnImage;
	}



#endif







