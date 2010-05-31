#include <stdio.h>
#include <stdlib.h>
#include "loadFile.h"

// loads any file into the dest ptr.


void loadFile( const char* fileName, long sizeInBytes, unsigned char* byteDestPtr) {


	
	FILE *f;
	
	int numBytesRead;
	

	f = fopen( fileName, "rb");	// open the file
	
	// don't load if file failed to open
	if( f == NULL ) {
		return;
		}
	
	numBytesRead = fread( (void*)byteDestPtr, sizeof(char), sizeInBytes, f);
	
	fclose(f);
	
	}
	
	
	
	


// loads a photoshop RAW image file, 32-bit

// NOTE:
	// This function exists because photoshop swaps the red and blue channels when
	// it saves a file as raw.  Thus, the file stream doesn't match the way a video
	// card deals with 32-bit color.
	
	// This function loads the file and then swaps the appropriate bytes
	
void loadRawFile( const char* fileName, long sizeInBytes, unsigned char* byteDestPtr) {

	//unsigned char tempChannel;

	
	long byteCount;

	FILE *f;
	
	int numBytesRead;

	
	f = fopen( fileName, "rb");	// open the file
	
	// don't load if file failed to open
	if( f == NULL ) {
		return;
		}
	
	numBytesRead = fread( (void*)byteDestPtr, sizeof(char), sizeInBytes, f);
	
	
	// now that file read, swap the red and blue channels:

	for( byteCount = 0; byteCount< sizeInBytes; byteCount=byteCount+4) {

		unsigned char alpha = byteDestPtr[byteCount+3];
		
		byteDestPtr[byteCount+3] = byteDestPtr[byteCount+2];
		byteDestPtr[byteCount+2] = byteDestPtr[byteCount+1];
		byteDestPtr[byteCount+1] = byteDestPtr[byteCount];
		
		byteDestPtr[byteCount] = alpha;

/*		tempChannel = byteDestPtr[byteCount];				// currently in red position
		byteDestPtr[byteCount] = byteDestPtr[byteCount+3];	// currently set to what's in alpha position
		byteDestPtr[byteCount+2] = tempChannel;
*/		
		
		}
	
	fclose(f);
	}