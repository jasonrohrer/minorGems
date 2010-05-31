/*
 * Modification History
 *
 * 2001-February-27		Jason Rohrer
 * Created.  
 * Added the use of a RohrerMoveGenerator (move generator option 2).
 *
 * 2001-March-4		Jason Rohrer
 * Changed the parameters passed to RohrerMoveGenerator.
 *
 * 2001-April-29   Jason Rohrer
 * Changed to pass image sources into the StereoClient.
 * Changed to use an AxisMoveProcessor.
 *
 * 2001-May-2   Jason Rohrer
 * Changed to read stereo processing type from INI file.
 *
 * 2001-May-7   Jason Rohrer
 * Added support for turning output image files on and off.
 *
 * 2001-May-12   Jason Rohrer
 * Added support for the WaypointMoveGenerator.
 *
 * 2001-May-14   Jason Rohrer
 * Updated to use new WaypointMoveGenerator constructor.
 *
 * 2001-December-12		Jason Rohrer
 * Changed to use new HostAddress constructor.
 */



#include "StereoClient.h" 
#include "NetworkedStereo.h"
#include "DummyStereo.h"
#include "AxisImageSource.h"

#include "MovementStereoProcessor.h"
#include "minorGems/ai/robotics/DefaultMoveGenerator.h"
#include "minorGems/ai/robotics/RohrerMoveGenerator.h"
#include "minorGems/ai/robotics/TavanMoveGenerator.h"
#include "minorGems/ai/robotics/WaypointMoveGenerator.h"
#include "minorGems/ai/robotics/AxisMoveProcessor.h"

#include "DefaultImagePairSideProcessor.h"

#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/util/random/StdRandomSource.h"



// prototypes:
void usage( char *inAppName );

// test function for stereo client
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
	
	
	
	// read left server info from ini file
	
	
	
	char *leftAddressString = new char[99];
	numRead = fscanf( iniFile, "%s", leftAddressString );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	int leftAddressLength = strlen( leftAddressString );
	int leftPort;
	if( fscanf( iniFile, "%d", &leftPort) != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	HostAddress *leftServerAddress = new HostAddress( leftAddressString,
		leftPort );
	
	
	// read right server info from ini file
	
	char *rightAddressString = new char[99];
	numRead = fscanf( iniFile, "%s", rightAddressString );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	int rightAddressLength = strlen( rightAddressString );
	int rightPort;
	if( fscanf( iniFile, "%d", &rightPort) != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
		
	HostAddress *rightServerAddress = new HostAddress( rightAddressString,
		rightPort );
	
	// read control server info from ini file
		
	char *controlAddressString = new char[99];
	numRead = fscanf( iniFile, "%s", controlAddressString );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	int controlAddressLength = strlen( controlAddressString );
	
	int controlPort;
	if( fscanf( iniFile, "%d", &controlPort) != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	
	HostAddress *controlServerAddress = new HostAddress( controlAddressString,
		controlPort );
	
	/*printf( "Trying to connect to control host " );
	controlServerAddress->print();
	printf( "\n" );
	Socket *controlSock = 
		SocketClient::connectToServer( controlServerAddress );
	SocketStream *controlStream;
		
	if( controlSock == NULL ) {
		printf( "connection failed\n" );
		// return... don't worry about cleanup for now
		return -1;
		}	
	else {
		printf( "connection established\n" );

		// add stream for connection to array of streams
		controlStream = new SocketStream( controlSock );
		// delete these, since they're no longer needed
		delete [] controlAddressString;
		delete controlServerAddress;
		}
	*/
	
	// read whether to output debug files or not
	int outputFiles = 0;
	numRead = fscanf( iniFile, "%d", &outputFiles );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}
	printf( "Output image files are " );
	if( outputFiles ) {
		printf( "on.\n" );
		}
	else {
		printf( "off.\n" );
		}
	
	
	// read the move generator number
	int moveGeneratorNumber = 0;
	numRead = fscanf( iniFile, "%d", &moveGeneratorNumber );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}


	
	// read stereo algorithm number
	int stereoAlgorithmNumber = 0;
	numRead = fscanf( iniFile, "%d", &stereoAlgorithmNumber );
	if( numRead != 1 ) {
		printf( "bad ini file format: %s\n", inArgs[1] );
		usage( inArgs[0] );
		}

	int numConnections;
	SocketStream **streams;
	
	if( stereoAlgorithmNumber == 0 ) {
		// dummy stereo, so we can stop reading INI file

		}
	else if( stereoAlgorithmNumber == 1 ) {
		// networked stereo algorithm
		
		// now read info from ini file about partial stereo servers
	
	// read the number of connections from the file
		numConnections = 0;
		numRead = fscanf( iniFile, "%d", &numConnections );
		if( numRead != 1 ) {
			printf( "bad ini file format: %s\n", inArgs[1] );
			usage( inArgs[0] );
			}
	
		// now read network information for each connection 
		// and construct a stream for it
		streams = new SocketStream*[ numConnections ]; 	
	
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
			int addressLength = strlen( addressString );	
			HostAddress *address = new HostAddress( addressString,
													addressLength, port );	
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
				// delete these, since they're no longer needed
				delete [] addressString;
				delete address;
				}
			}	
		}
	else {
		printf( "bad stereo algorithm number: %d\n",
				stereoAlgorithmNumber );
		usage( inArgs[0] );
		}

	
	
	
	
	Stereo *stereo;
	switch( stereoAlgorithmNumber ) {
		case 0:
			printf( "Using DummyStereo.\n" );
			stereo = new DummyStereo();
			break;
		case 1:
			printf( "Using NetworkedStereo.\n" );
			stereo = new NetworkedStereo( streams, numConnections );
			break;
		default:
			// default is caught above
			break;
			
		}
									   
	
	StereoMoveGenerator *moveGenerator;

	char *waypointConfigFileName;
	FILE *waypointConfigFile;
	
	// select a move generator
	switch( moveGeneratorNumber ) {
		case 0:
			printf( "Using DefaultMoveGenerator.\n" );
			moveGenerator = 
				new DefaultMoveGenerator();
			break;
		case 1:
			printf( "Using TavanMoveGenerator.\n" );
			moveGenerator = 
				new TavanMoveGenerator();
			break;
		case 2:
			printf( "Using RohrerMoveGenerator.\n" );
			moveGenerator = 
				new RohrerMoveGenerator( 0.3, 0.4 );
			break;
		case 3:
			printf( "Using WaypointMoveGenerator.\n" );
			waypointConfigFileName = new char[99];
			numRead = fscanf( iniFile, "%s",
							  waypointConfigFileName );
			
			if( numRead != 1 ) {
				printf( "bad ini file format: %s\n", inArgs[1] );
				printf( "failed to read waypoint config file name.\n" );
				usage( inArgs[0] );
				}
			
			waypointConfigFile =
				fopen( waypointConfigFileName, "r" );

			if( waypointConfigFile == NULL ) {
				printf( "faile to open waypoint config file:  %s\n",
						waypointConfigFileName );
				usage( inArgs[0] );
				}
					   
			moveGenerator = 
				new WaypointMoveGenerator( waypointConfigFile,
										   outputFiles );

			fclose( waypointConfigFile );
			delete [] waypointConfigFileName;
			
			break;
		default:
			printf( "Unknown move generator option: %d.\n", 
				moveGeneratorNumber );
			printf( "Using DefaultMoveGenerator.\n" );
			moveGenerator = 
				new DefaultMoveGenerator();
			break;
		}


	// close the ini file
	fclose( iniFile );
	
	AxisMoveProcessor *moveProcessor =
		new AxisMoveProcessor( controlServerAddress );
	
	MovementStereoProcessor *stereoProcessor = 
		new MovementStereoProcessor( moveGenerator,
									 moveProcessor,
									 outputFiles );
	
		
	DefaultImagePairSideProcessor *imageProcessor =
		new DefaultImagePairSideProcessor();

	AxisImageSource *leftSource =
		new AxisImageSource( leftServerAddress, 320, 240, 50 );
	AxisImageSource *rightSource =
		new AxisImageSource( rightServerAddress, 320, 240, 50 );
	
	StereoClient *client =
		new StereoClient( leftSource, rightSource,
			stereo, stereoProcessor, imageProcessor );	
	
	client->run();
	
	
	client->join();
	
	delete client;

	delete leftServerAddress;
	delete rightServerAddress;

	delete controlServerAddress;
	
	return 0;
	}



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s ini_file_name\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s stereoClient.ini\n", inAppName );
	
	std::exit( 1 );
	}
