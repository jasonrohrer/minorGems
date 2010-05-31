// Jason Rohrer
// P_M.cpp

/**
*
*	Functions for P_M object
*
*	Created 2-27-99
*	Mods:
*		Jason Rohrer	3-1-2000	Added support for comments
*
*
*
*	BIG NOTE:	This is the older version of P_M and doesn't work on 
*				little endian machines.
*
*				If you try to use this on NT, IT WILL CRASH
*
*				
*
*/

#include "P_M.h"


// protos
char passComment( FILE *inFile );		// pass up a comment if next in input


P_M::P_M( char *fileName, int *status ) {

	FILE* inFile = fopen( fileName, "r" );		// read text from start of file
	
	if( inFile == NULL ) {		// file open failed
		printf( "Failed to open file for reading: %s\n", fileName );
		*status = -1;
		return;
		}
	
	passComment( inFile );
	
	// read ASCII type
	int magicNum;
	char tempC;
		
	fscanf( inFile, "%c%d", &tempC, &magicNum );
	
	if( magicNum == 4 ) {
		format = PBM;
		}
	else if( magicNum == 5 ) {
		format = PGM;
		}
	else {
		printf( "File is not a supported PBM or PGM format: %s\n", fileName );
		*status = -1;
		return;
		}

	passComment( inFile );

	// now read ASCII width, height
	fscanf( inFile, "%d", &width );
	
	passComment( inFile );
	
	fscanf( inFile, "%d", &height );
	
	if( format == PGM ) {		// need to read max gray value
		fscanf( inFile, "%d", &maxGray );
		}
	
	// should only be one character of white space after this...

	// save file position, and reopen in binary mode
	fpos_t pos;
	fgetpos( inFile, &pos );

	pos ++;		// skip one more character of white space

	fclose( inFile );



	inFile = fopen( fileName, "rb" );		// read binary
	
	if( inFile == NULL ) {		// file open failed
		printf( "Failed to open file for reading: %s\n", fileName );
		*status = -1;
		return;
		}
	
	
	
	// skip to pos to get to binary data
	fsetpos( inFile, &pos );
	
	if( format == PGM ) {	// read one byte per pixel
		buffer = new unsigned char[ width * height ];
		fread( buffer, width * height, 1, inFile );
		}
	else {		// each byte contains 8 pixes or-ed together
		
	
		int numPix = width * height;
		
		buffer = new unsigned char[ numPix ];
		
		int numBytes = numPix / 8;
		
		if( numPix % 8 != 0 ) {	// overflow bits
			numBytes ++;
			}
		
		
		char *tempBytes = new char[ numBytes ];
		
		fread( tempBytes, numBytes, 1, inFile );
		
		int p=0;
		for( int b=0; b<numBytes; b++ ) {
			for( int n=7; n>=0; n-- ) {
				
				if( p<numPix ) {
					buffer[ p ] = (tempBytes[ b ] >> n) & 0x01; 
					p++;
					}
				}
			}
		
		delete [] tempBytes;
		}
	
	// done reading data...
	fclose( inFile );
		
	// calculate row offsets
	rowOffsets = new int[ height ];
	for( int y=0; y<height; y++ ) {
		rowOffsets[y] = y*width;
		}
	
	*status = 0;
	}		// end P_M constructor taking fileName as input
	
		

P_M::P_M( P_M_format f, int w, int h, unsigned char *b, int maxGry )
	:	format( f ), width( w ), height( h ),
		buffer( new unsigned char[ w*h ] ), rowOffsets( new int[ h ] ),
		maxGray( maxGry)
	{
	
	// setup row offsets
	for( int y=0; y<height; y++ ) {
		rowOffsets[y] = y*width;
		}
	
	
	// copy data from buffer
	for( int y=0; y<height; y++ ) {
		int offset = rowOffsets[y];
		
		for( int x=0; x<width; x++ ) {
			buffer[ offset + x ] = b[ offset + x ];	
			}
		
		}
	}	 		// end P_M constructor taking data as input
	
	



int P_M::write( char *fileName ) {
	FILE* outFile = fopen( fileName, "wb" );		// write text at start of file
	
	if( outFile == NULL ) {		// file open failed
		printf( "Failed to open file for writing: %s\n", fileName );
		return -1;
		}
	
	// write ASCII type
	int magicNum;
		
	if( format == PBM ) {
		magicNum = 4;
		}
	else {
		magicNum = 5;
		}

	fprintf( outFile, "P%d\n", magicNum );

	// now write ASCII width, height
	fprintf( outFile, "%d %d\n", width, height );
	
	if( format == PGM ) {		// need to write max gray value
		fprintf( outFile, "%d\n", maxGray );
		}
/*	
	// save file position, and reopen in binary mode
	fpos_t pos;
	fgetpos( outFile, &pos );
	
	fclose( outFile );


	outFile = fopen( fileName, "wb+" );		// write binary
	
	if( outFile == NULL ) {		// file open failed
		printf( "Failed to open file for writing: %s\n", fileName );
		return -1;
		}
	
	
	// skip to pos to get to binary data
	fsetpos( outFile, &pos );
	
	
*/
	if( format == PGM ) {	// write one byte per pixel
		fwrite( buffer, width * height, 1, outFile );
		}
	else {		// each byte contains 8 pixes or-ed together
		
		int numPix = width * height;
		
		int numBytes = numPix / 8;
		
		if( numPix % 8 != 0 ) {	// overflow bits
			numBytes ++;
			}
		
		
		char *tempBytes = new char[ numBytes ];
		
		// clear bytes before or-ing values into them
		for( int t=0; t<numBytes; t++ ) {
			tempBytes[t] = 0;
			}


		int p=0;		
		for( int b=0; b<numBytes; b++ ) {
			for( int n=7; n>=0; n-- ) {
			
				if( p<numPix ) {
					tempBytes[b] = tempBytes[b] | ( buffer[p] << n );
					p++;
					}
				}
			}
		
		fwrite( tempBytes, numBytes, 1, outFile );
		
		delete [] tempBytes;
		}
	
	// done reading data...
	fclose( outFile );
	
	return 0;
	}		// end write P_M
	
	
	
	
char passComment( FILE *inFile ) {		// pass up a comment if next in input

	fpos_t pos;		// save start pos incase no comment found
	fgetpos( inFile, &pos );
	

	char tempC = ' ';
	
	
	// skip any white space before comment
	while( tempC == ' ' || tempC == '\n' || tempC == '\r' || tempC == '\t') {
		fgetpos( inFile, &pos );
		
		// get next char
		tempC = (int) fgetc( inFile );
		}
	
	if( tempC == '#' ) {
		// comment found, skip it
		while( tempC != '\n' && tempC != '\r' ) {			
			// get next char
			tempC = (int) fgetc( inFile );
			}
			
		passComment( inFile );
		return true;
		}
	else {
		// no comment found, move file pos back to normal
		fsetpos( inFile, &pos );
		return false;
		}
	}
	
	
	