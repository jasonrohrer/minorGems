/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 *
 * 2001-November-30		Jason Rohrer
 * Changed names of functions to avoid keyword collisions.
 */
 
#include "CommonInstructionSet.h" 

#include <stdio.h>

void CommonInstructionSet::copyi( BitMemory *inMemory, unsigned long inSrc, 
	unsigned long inDest, unsigned long inLength ) {
	
	//printf( "copying %d bits from %d to %d\n", inLength, inSrc, inDest );
	
	if( inLength == 0 ) {
		return;
		}
	
	unsigned long numChars;
	unsigned char *srcValues = inMemory->getMemoryRange( inSrc, 
		inSrc + inLength - 1, &numChars );
	
	inMemory->setMemoryRange( inDest, inDest + inLength - 1, srcValues );
	delete [] srcValues;	
	
	}



void CommonInstructionSet::andi( BitMemory *inMemory, unsigned long inSrc1, 
	unsigned long inSrc2, unsigned long inDest, 
	unsigned long inLength ) {
	
	//printf( "and-ing %d bits from %d and %d and storing in %d\n", 
	//	inLength, inSrc1, inSrc2, inDest );
	
	if( inLength == 0 ) {
		return;
		}
	
	unsigned long numChars;
	unsigned char *src1Values = inMemory->getMemoryRange( inSrc1, 
		inSrc1 + inLength - 1, &numChars );
	unsigned char *src2Values = inMemory->getMemoryRange( inSrc2, 
		inSrc2 + inLength - 1, &numChars );
	
	// store result directly in src2Values
	for( int i=0; i<numChars; i++ ) {
		src2Values[i] = src2Values[i] ^ src1Values[i];
		}
	
	inMemory->setMemoryRange( inDest, inDest + inLength - 1, src2Values );
	delete [] src1Values;
	delete [] src2Values;
	
	}



void CommonInstructionSet::noti( BitMemory *inMemory, unsigned long inSrc, 
	unsigned long inDest, unsigned long inLength ) {
	
	//printf( "not-ing %d bits from %d and storing in %d\n", 
	//	inLength, inSrc, inDest );
	
	if( inLength == 0 ) {
		return;
		}
	
	unsigned long numChars;
	unsigned char *srcValues = inMemory->getMemoryRange( inSrc, 
		inSrc + inLength - 1, &numChars );
	unsigned char *destValues = new unsigned char[numChars];
	
	// store result directly in src2Values
	for( int i=0; i<numChars; i++ ) {
		destValues[i] = ~( srcValues[i] );
		}
	
	inMemory->setMemoryRange( inDest, inDest + inLength - 1, destValues );
	delete [] srcValues;
	delete [] destValues;
	}
