/*
 * Modification History
 *
 * 2001-February-21		Jason Rohrer
 * Created. 
 *
 * 2001-February-22		Jason Rohrer
 * Fixed a bug in random channel selection. 
 *
 * 2001-February-24		Jason Rohrer
 * Updated to open and send a file called "test.bmp" to test
 * pipes and BMP conversion. 
 *
 * 2001-February-25		Jason Rohrer
 * Now opens two files, testRight.bmp and testLeft.bmp.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly). 
 */

#include <unistd.h>

#include "StereoClient.h" 
#include "EdgeBoundedStereo.h"
#include "SusanEdgeDetector.h"

#include "DefaultStereoProcessor.h"
#include "ScreenImagePairSideProcessor.h"

#include "minorGems/network/HostAddress.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/io/PipedStream.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"

#include "minorGems/util/random/StdRandomSource.h"

// prototypes:
void usage( char *inAppName );

// test stereo server for testing stereo client
int main( int inNumArgs, char **inArgs ) {
	
	
	if( inNumArgs != 3 ) {
		usage( inArgs[0] );
		}
	
	int port;
	if( sscanf( inArgs[1], "%d", &port) != 1 ) {
		printf( "Port must be a number.  Bad argument:  %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	
	int fileNameLength = 0;
	while( inArgs[2][fileNameLength] != '\0' ) {
		fileNameLength++;
		}
	
	StdRandomSource *randSource = new StdRandomSource();
	
	SocketServer *server = new SocketServer( port, 50 );
	
	Image *image = new Image( 320, 240, 3 );
	
	int chan = randSource->getRandomBoundedInt( 0, 2 );
	double *channelA = image->getChannel( chan );
	
	while( true ) {
		printf( "Waiting for a connection.\n" );
		Socket *socket = server->acceptConnection();
		SocketStream *stream = new SocketStream( socket );
		
		printf( "Connection received.\n" );
		
		while( stream->OutputStream::getLastError() == NULL ) {
			// socket still operational
			
			printf( "Sending image.\n" );
			
			// test pipe and BMP formater
			File *bmpFile = new File( NULL, inArgs[2], fileNameLength );
			//File *bmpFile = new File( NULL, "test.bmp", 8 );
			FileInputStream *fileStream = new FileInputStream( bmpFile );
			
			int fileSize = bmpFile->getLength();
			printf( "File Size = %d\n", fileSize );
			
			unsigned char *fileBuffer = new unsigned char[fileSize];
			fileStream->read( fileBuffer, fileSize );
			
			PipedStream *pipe = new PipedStream();
			BMPImageConverter *converter = new BMPImageConverter();
			
			delete image;
			
			pipe->write( fileBuffer, fileSize );
			
			delete [] fileBuffer;
			
			image = converter->deformatImage( pipe);
			
			delete pipe;
			delete converter;
			delete fileStream;
			delete bmpFile;
			
			image->serialize( stream );
			
			usleep( 2000000 );
			
			int x = randSource->getRandomBoundedInt( 0, 319 );
			int y = randSource->getRandomBoundedInt( 0, 239 );

			// fill with random noise over time
			//channelA[ x + y * 320 ] = 1.0;
			}
		
		printf( "Connection broken.\n" );
			
		delete stream;
		delete socket;
		}
	
	
	delete server;
	delete image;
	
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s port_number bmp_file_name\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s 6877 testLeft.bmp\n", inAppName );
	
	std::exit( 1 );
	}
