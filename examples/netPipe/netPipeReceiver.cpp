/*
 * Modification History
 *
 * 2001-August-02   Jason Rohrer
 * Created.  
 *
 * 2002-June-1    Jason Rohrer
 * Added a missing include.  
 */

#include "minorGems/network/SocketServer.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define BUFFER_SIZE 5000


void usage( char *inAppName );


int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs < 2 ) {
		usage( inArgs[0] );
		}
	if( inNumArgs > 3 ) {
		usage( inArgs[0] );
		}
	char useFile = true;
	if( inNumArgs != 3 ) {
		useFile = false;
		}

	long port;
	int numRead = sscanf( inArgs[1], "%d", &port );

	if( numRead != 1 ) {
		printf( "port number must be a valid integer:  %s\n", inArgs[2] );
		usage( inArgs[0] );
		}

	File *outFile;
	FileOutputStream *outStream;

	if( useFile ) {
		outFile = new File( NULL, inArgs[2],
							strlen( inArgs[2] ) );
		
		outStream = new FileOutputStream( outFile );
		}
   
	SocketServer *server = new SocketServer( port, 1 );
	
	printf( "listening for a connection on port %d\n", port );
	Socket *sock = server->acceptConnection();

	if( sock == NULL ) {
		printf( "socket connection failed\n" );
		return( 1 );
		}
	printf( "connection received\n" );
	
	
	SocketStream *inStream = new SocketStream( sock );

	
	unsigned long checksum = 0;

	unsigned char *buffer = new unsigned char[ BUFFER_SIZE ];
	

	numRead = BUFFER_SIZE;
	int numWritten = BUFFER_SIZE;
	
	while( numWritten == numRead && numRead == BUFFER_SIZE ) {

		// read a buffer full of data from standard in
		numRead = inStream->read( buffer, BUFFER_SIZE );

		// add the buffer to our checksum
		for( int i=0; i<numRead; i++ ) {
			checksum += buffer[i];
			}


		if( useFile ) {
			// write the buffer out to our file stream
			numWritten = outStream->write( buffer, numRead );
			}
		else {
			// write to std out
			numWritten = fwrite( buffer, 1, numRead, stdout );
			}
		}

	

	if( numRead != numWritten ) {
		printf( "file output failed\n" );
		}
	

	
	printf( "checksum = %d\n", checksum );


	delete sock;
	delete server;
	delete inStream;
	if( useFile ) {
		delete outStream;
		delete outFile;
		}
	
	delete [] buffer;
	
	return 0;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s receiver_port [output_file_name]\n", inAppName );

	printf( "Examples:\n" );
	printf( "\t%s 5888 myarchive.tar\n", inAppName );
	printf( "\t%s 5888 \n", inAppName );
	
	exit( 1 );
	}
