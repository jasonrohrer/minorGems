/*
 * Modification History
 *
 * 2001-February-19		Jason Rohrer
 * Created.
 */

#include <stdio.h>

#include "PipedStream.h"

#include "minorGems/util/random/StdRandomSource.h"

// test function for piped streams
int main() {
	
	PipedStream *stream = new PipedStream;
	InputStream *inStream = stream;
	OutputStream *outStream = stream;
	
	StdRandomSource *randSource = new StdRandomSource();
	
	unsigned char *bufferA = new unsigned char[10];
	unsigned char *bufferB = new unsigned char[15];
	
	int i;
	
	printf( "bufferA = \n" );
	for( i=0; i<10; i++ ) {
		bufferA[i] = (unsigned char)( 
			randSource->getRandomBoundedInt(0, 255) );
		printf( "%d\n", bufferA[i] );
		}
	printf( "bufferB = \n" );
	for( i=0; i<15; i++ ) {
		bufferB[i] = (unsigned char)( 
			randSource->getRandomBoundedInt(0, 255) );
		printf( "%d\n", bufferB[i] );
		}
	
	unsigned char *bufferC = new unsigned char[ 10 + 15];
	
	
	outStream->write( bufferA, 10 );
	outStream->write( bufferB, 15 );

	inStream->read( bufferC, 10 + 10 );

	char *error = inStream->getLastError();
	if( error != NULL ) {
		printf( "Stream error %s\n", error );
		delete [] error;
		}
	printf( "bufferc = \n" );
	for( i=0; i<10 + 15; i++ ) {
		printf( "%d\n", bufferC[i] );
		}
	
	inStream->read( bufferC, 10 );

	error = inStream->getLastError();
	if( error != NULL ) {
		printf( "Stream error %s\n", error );
		delete [] error;
		}
	printf( "bufferc = \n" );
	for( i=0; i<10 + 15; i++ ) {
		printf( "%d\n", bufferC[i] );
		}
	
	
	delete [] bufferA;
	delete [] bufferB;
	delete [] bufferC;
	
	delete randSource;
	delete stream;
	} 
