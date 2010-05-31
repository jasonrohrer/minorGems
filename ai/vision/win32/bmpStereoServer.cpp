/*
 * Modification History
 *
 * 2001-February-28		Jason Rohrer
 * Created.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly). 
 */


#include "minorGems/graphics/Image.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"

#include <time.h>
#include <string.h>

// prototypes:
void usage( char *inAppName );

// test function for stereo
int main( int inNumArgs, char **inArgs ) {
	
	if( inNumArgs < 4 ) {
		usage( inArgs[0] );
		} 
	
	int imagePort;
	int dummyPort;
    int windowSize;

    if( sscanf( inArgs[1], "%d", &imagePort ) != 1 ) {
        printf( "image_server_port must be a number.  Bad argument:  %s\n", 
                inArgs[1] );
        usage( inArgs[0] );
        }
	if( sscanf( inArgs[3], "%d", &dummyPort ) != 1 ) {
        printf( "dummy_port must be a number.  Bad argument:  %s\n", 
                inArgs[3] );
        usage( inArgs[0] );
        }	     
	
	
	SocketServer *imageServer = new SocketServer( imagePort, 50 );
	SocketServer *dummyServer = new SocketServer( dummyPort, 100 );
	
	printf( "Waiting for image client connection.\n" );
	Socket *imageSocket = imageServer->acceptConnection();
	printf( "   Image client connection recieved.\n" );
	SocketStream *imageStream = new SocketStream( imageSocket );
	
	BMPImageConverter *converter = new BMPImageConverter();
	
	while( imageStream->OutputStream::getLastError() == NULL ) {
	
		// wait for a new connection on dummyServer
		printf( "Waiting for dummy connection to signal image capture\n" );
		
		Socket *dummySocket = dummyServer->acceptConnection();
		printf( "   Dummy connection received.\n" );
		
		printf( "Reading image from file %s\n", inArgs[2] );
		
		int nameLength = strlen( inArgs[2] );
	
		File *leftOutFile = new File( NULL, inArgs[2], nameLength );
		FileInputStream *fileStream = new FileInputStream( leftOutFile );
		
		Image *image = converter->deformatImage( fileStream );
		
		printf( "Sending the image\n" );
		image->serialize( imageStream );
		
		printf( "   Done.\n" );
		
		printf( "Sending dummy a control byte to tell it to quit\n" );
		
		unsigned char *controlByte = new unsigned char[1];
		controlByte[0] = 1;
		
		dummySocket->send( controlByte, 1 );
		
		delete [] controlByte;
		
		delete fileStream;
		// delete file explicitly
		delete leftOutFile;
		
		delete image;
		
		// this should close the socket and free up a connection slot
		delete dummySocket;
		}
	
	delete imageStream;
	
	delete imageServer;
	delete dummyServer;
	
	delete converter;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s image_server_port bmp_file dummy_port\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s 3000 c:\\capture\\SnapCap.bmp 5003\n", inAppName );
	
	std::exit( 1 );
	}
