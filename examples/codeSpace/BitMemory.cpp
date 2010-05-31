/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 */
 
#include "BitMemory.h"


unsigned char *BitMemory::getMemoryRange( unsigned long inRangeStart,
	unsigned long inRangeEnd, unsigned long *outNumCharsReturned ) {
	
	// check memory range
	if( inRangeStart < 0 || inRangeStart > inRangeEnd ) {
		
		printf( "Bad memory range: [%d, %d]\n", inRangeStart, inRangeEnd );
		return NULL;
		}
	// wrapping is handled automatically by getBit()
	
	
	unsigned long numBlocks = ( ( inRangeEnd - inRangeStart + 1 ) >> 3 );
	if( ( inRangeEnd - inRangeStart + 1 ) - ( numBlocks >> 3 ) > 0 ) {
		// extra bits
		numBlocks++;
		}
	
	unsigned char *output = new unsigned char[ numBlocks ];
	
	for( int b=0; b<numBlocks; b++ ) {
		output[b] = 0;
		}
	
	// bit index in output
	int i = 0;
	
	for( int m=inRangeStart; m<=inRangeEnd; m++ ) {
		unsigned char memBit = getBit( m );
		
		unsigned long block = i >> 3;	// index of block containing bit
		int bit = i - ( block << 3 );	// index of bit in block
		int shiftAmount = 7 - bit;
		
		output[block] = output[block] | ( memBit << shiftAmount );
		
		i++;
		}
		
	*outNumCharsReturned = numBlocks;
	return output;
	}
	
	
	
void BitMemory::setMemoryRange( unsigned long inRangeStart,
	unsigned long inRangeEnd, unsigned char *inRangeContents ) {
	
	// check memory range
	if( inRangeStart < 0 || inRangeStart > inRangeEnd ) {
		
		printf( "Bad memory range: [%d, %d]\n", inRangeStart, inRangeEnd );
		return;
		}
	// wrapping is handled automatically by setBit()
	
	// bit index in input
	int i = 0;
	
	for( int m=inRangeStart; m<=inRangeEnd; m++ ) {
		char inputBit;
		
		unsigned long block = i >> 3;	// index of block containing bit
		int bit = i - ( block << 3 );	// index of bit in block
		int shiftAmount = 7 - bit;
		
		// shift and clear to extract bit from block
		inputBit = 
			(char)( ( inRangeContents[block] >> shiftAmount ) & 0x01 );
		
		setBit( m, inputBit );
		i++;
		}
		
	}	
	
	
	
void BitMemory::clearMemoryRange( unsigned long inRangeStart,
	unsigned long inRangeEnd ) {
	
	// check memory range
	if( inRangeStart < 0 || inRangeEnd > mNumBits ||
		inRangeStart > inRangeEnd ) {
		
		printf( "Bad memory range: [%d, %d]\n", inRangeStart, inRangeEnd );
		return;
		}
	
	for( int m=inRangeStart; m<=inRangeEnd; m++ ) {	
		setBit( m, 0 );
		}
	}
	
	
	
void BitMemory::clearMemory() {
	clearMemoryRange( 0, mNumBits - 1 );
	}
