/*
 * Modification History
 *
 * 2001-February-20		Jason Rohrer
 * Created.  
 *
 * 2001-February-24		Jason Rohrer
 * Changed to use a screen stereo processor. 
 *
 * 2001-February-25		Jason Rohrer
 * Changed to use a local window stereo implementation. 
 * Changed to use a FileStereoProcessor.   
 *
 * 2001-February-26		Jason Rohrer
 * Changed back to the single-threaded version of the stereo computer,
 * since we're only going to be computing on single-channel images anyway.   
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 */

#include "StereoClient.h" 
#include "ThreadedMultiChannelStereo.h"
#include "LocalWindowStereo.h"


#include "DefaultStereoProcessor.h"
//#include "ScreenStereoProcessor.h"
#include "FileStereoProcessor.h"

#include "ScreenImagePairSideProcessor.h"
#include "DefaultImagePairSideProcessor.h"

#include "minorGems/network/HostAddress.h"


#include "minorGems/util/random/StdRandomSource.h"



// prototypes:
void usage( char *inAppName );

// test function for stereo client
int main( int inNumArgs, char **inArgs ) {
	
	
	if( inNumArgs != 5 ) {
		usage( inArgs[0] );
		}
	
	int leftPort;
	if( sscanf( inArgs[2], "%d", &leftPort) != 1 ) {
		printf( "Port must be a number.  Bad argument:  %s\n", inArgs[2] );
		usage( inArgs[0] );
		}
	
	int leftAddressLength = -1;
	char lastChar = 'a';
	while( lastChar != '\0' ) {
		lastChar = inArgs[1][leftAddressLength+1];
		leftAddressLength++;
		}
	
	if( leftAddressLength == -1 ) {
		printf( "Address entered is not a valid string.\n" );
		usage( inArgs[0] );
		}
	
	
	int rightPort;
	if( sscanf( inArgs[4], "%d", &rightPort) != 1 ) {
		printf( "Port must be a number.  Bad argument:  %s\n", inArgs[4] );
		usage( inArgs[0] );
		}
	
	int rightAddressLength = -1;
	lastChar = 'a';
	while( lastChar != '\0' ) {
		lastChar = inArgs[3][rightAddressLength+1];
		rightAddressLength++;
		}
	
	if( rightAddressLength == -1 ) {
		printf( "Address entered is not a valid string.\n" );
		usage( inArgs[0] );
		}
	


	int addressLength = strlen( inArgs[1] );
	char *copiedArg = new char[ addressLength + 1 ];
	strcpy( copiedArg, inArgs[1] );
	
	HostAddress *leftServerAddress = new HostAddress( copiedArg, leftPort );

	addressLength = strlen( inArgs[3] );
	copiedArg = new char[ addressLength + 1 ];
	strcpy( copiedArg, inArgs[3] );
	
	HostAddress *rightServerAddress = new HostAddress( copiedArg, rightPort );
	
	StdRandomSource *randSource = new StdRandomSource();
	
	//EdgeBoundedStereo *stereo = new EdgeBoundedStereo( 7,
	//	new SusanEdgeDetector( 15 ), randSource );
	
	LocalWindowStereo *stereo = new LocalWindowStereo( 7,
		5, randSource );
	
	//ThreadedMultiChannelStereo *stereo = 
	//	new ThreadedMultiChannelStereo( localStereo );
	
	/*DefaultStereoProcessor *stereoProcessor = 
		new DefaultStereoProcessor();
	ScreenImagePairSideProcessor *imageProcessor =
		new ScreenImagePairSideProcessor( 320, 480 );
	*/
	
	//ScreenStereoProcessor *stereoProcessor = 
	//	new ScreenStereoProcessor( 320, 480 );
	FileStereoProcessor *stereoProcessor = new FileStereoProcessor();
	
		
	DefaultImagePairSideProcessor *imageProcessor =
		new DefaultImagePairSideProcessor();
	
	StereoClient *client =
		new StereoClient( leftServerAddress, rightServerAddress,
			stereo, stereoProcessor, imageProcessor );	
	
	client->run();
	
	
	client->join();
	
	delete client;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s left_server_address left_port_number", inAppName );
	printf( " right_server_address right_port_number\n" );

	printf( "examples:\n\t" );
	printf( "%s localhost 6877 localhost 6878\n\t", inAppName );
	printf( "%s 192.168.1.57 6877 192.168.1.58 6877\n", inAppName );
	
	std::exit( 1 );
	}
