/*
 * Modification History
 *
 * 2001-February-25		Jason Rohrer
 * Created. 
 *
 * 2001-February-26		Jason Rohrer
 * Changed to convert images to grayscale and then use a single-threaded
 * stereo computer. 
 * Changed to run multi threaded stereo computation.
 * Changed to use NetworkedStereo.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to comply with new FileInput/OutputStream interface
 * (underlying File must be destroyed explicitly). 
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 */

#include "ThreadedPartialStereo.h"
#include "NetworkedStereo.h"
#include "LocalWindowStereo.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"
#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/io/file/FileOutputStream.h"

#include "minorGems/util/random/StdRandomSource.h"

#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketStream.h"

#include <time.h>
#include <string.h>

// prototypes:
void usage( char *inAppName );

// test function for stereo
int main( int inNumArgs, char **inArgs ) {
	
	if( inNumArgs < 2 ) {
		usage( inArgs[0] );
		} 
	
	// open ini file in text mode
	FILE *iniFile = fopen( inArgs[1], "r" );
	
	if( iniFile == NULL ) {
		printf( "file open failed: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	
	int numRead = 0;
	
	// read the number of connections from the file
	int numConnections = 0;
	numRead = fscanf( iniFile, "%d", &numConnections );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	
	// now read network information for each connection 
	// and construct a stream for it
	SocketStream **streams = new SocketStream*[ numConnections ]; 	
	
	for( int c=0; c<numConnections; c++ ) {
		char *addressString = new char[99];
		numRead = fscanf( iniFile, "%s", addressString );
		if( numRead != 1 ) {
			printf( "bad ini file format: %s\n", inArgs[1] );
			usage( inArgs[0] );
			}
		int port;
		numRead = fscanf( iniFile, "%d", &port );
		if( numRead != 1 ) {
			printf( "bad ini file format: %s\n", inArgs[1] );
			usage( inArgs[0] );
			}
		HostAddress *address = new HostAddress( addressString,
			port );	
		printf( "Trying to connect to host " );
		address->print();
		printf( "\n" );
		Socket *sock = SocketClient::connectToServer( address );
		if( sock == NULL ) {
			printf( "connection failed\n" );
			// return... don't worry about cleanup for now
			return -1;
			}	
		else {
			printf( "connection established\n" );
			
			// add stream for connection to array of streams
			streams[c] = new SocketStream( sock );
			// delete this, since it's no longer needed
			delete address;
			}
		}	
	
	// close the ini file
	fclose( iniFile );
	
		
	File *leftFile = new File( NULL, "receivedLeft.bmp", 16 );
	FileInputStream *leftInStream = new FileInputStream( leftFile );
	
	File *rightFile = new File( NULL, "receivedRight.bmp", 17 );
	FileInputStream *rightInStream = new FileInputStream( rightFile );
	
	BMPImageConverter *converter = new BMPImageConverter();

	Image *leftImage = converter->deformatImage( leftInStream );
	Image *rightImage = converter->deformatImage( rightInStream );
	
	StdRandomSource *randSource = new StdRandomSource();
	
	
	
	//LocalWindowStereo *localStereo = new LocalWindowStereo( 30,
	//	windowSize, randSource );
	
	//ThreadedPartialStereo *stereo = 
	//	new ThreadedPartialStereo( localStereo, numThreads );
	
	NetworkedStereo *stereo = 
		new NetworkedStereo( streams, numConnections );
	
	
	//ThreadedMultiChannelStereo *stereo = 
	//	new ThreadedMultiChannelStereo( localStereo );
	
	// convert to grayscale
	Image *leftGrayImage = 
		ImageColorConverter::RGBToGrayscale( leftImage );
	Image *rightGrayImage = 
		ImageColorConverter::RGBToGrayscale( rightImage );
	
	long startTime = time( NULL );	
	Image *stereoImage = 
		stereo->computeDepthMap( leftGrayImage, rightGrayImage );
	
	long endTime = time( NULL );	
	
	
	printf( "net time = %d seconds\n", endTime - startTime );
	
	delete leftImage;
	delete rightImage;
	delete leftGrayImage;
	delete rightGrayImage;
	delete stereo;
	delete leftInStream;
	delete rightInStream;
	delete leftFile;
	delete rightFile;

	
	File *outFile = new File( NULL, "computedStereo.bmp", 18 );
	FileOutputStream *fileOutStream = new FileOutputStream( outFile );
	
	Image *threeChannelImage = 
		ImageColorConverter::grayscaleToRGB( stereoImage );
		
	converter->formatImage( threeChannelImage, fileOutStream );
	
	
	delete stereoImage;
	delete threeChannelImage;
	delete converter;
	delete fileOutStream;
	delete outFile;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s ini_file_name\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s networkedStereo.ini\n", inAppName );
	
	std::exit( 1 );
	}
