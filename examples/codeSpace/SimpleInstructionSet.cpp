/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 *
 * 2001-November-30		Jason Rohrer
 * Changed names of functions to avoid keyword collisions.
 */
 
#include "SimpleInstructionSet.h" 

// prototypes:
unsigned long charArrayToLong( unsigned char *inArray, int inLength,
	int numBits );



unsigned long SimpleInstructionSet::stepProgram( BitMemory *inMemory, 
	unsigned long inProgramCounter ) {
	
	
	unsigned long numChars;
	// fetch 2-bit instruction
	unsigned char *instruction = 
		inMemory->getMemoryRange( inProgramCounter, inProgramCounter + 1, 
			&numChars );
	
	unsigned char instructionValue = instruction[0];
	delete [] instruction;
	
	inProgramCounter += 2;
	
	instructionValue = instructionValue >> 6;
	
	unsigned char *address;
	unsigned long jumpAddress;
	
	unsigned char *src1;
	unsigned char *src2;
	unsigned char *dest;
	unsigned char *length;
	
	switch( instructionValue ) {
		case 0:
			// jump
			// simply return new program counter
			
			address =  
				inMemory->getMemoryRange( inProgramCounter, 
					inProgramCounter + mBitsInAddress - 1, 
					&numChars );
			jumpAddress = charArrayToLong( address, numChars, mBitsInAddress );
			delete [] address;
			
			//printf( "jumping to %d\n", jumpAddress );
			return jumpAddress;
			break;
		case 1:
			// copy
			src1 = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			dest = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			length = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			
			copyi( inMemory,
				   charArrayToLong( src1, numChars, mBitsInAddress ), 
				   charArrayToLong( dest, numChars, mBitsInAddress ), 
				   charArrayToLong( length, numChars, mBitsInAddress ) );
			
			delete [] src1;
			delete [] dest;
			delete [] length;
			
			return inProgramCounter;
			break;
		case 2:
			// and
			src1 = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;	
			src2 = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );	
			inProgramCounter += mBitsInAddress;
			dest = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			length = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			
			andi( inMemory,
				  charArrayToLong( src1, numChars, mBitsInAddress ), 
				  charArrayToLong( src2, numChars, mBitsInAddress ),
				  charArrayToLong( dest, numChars, mBitsInAddress ), 
				  charArrayToLong( length, numChars, mBitsInAddress ) );
			
			delete [] src1;
			delete [] src2;
			delete [] dest;
			delete [] length;
			
			return inProgramCounter;
			break;
		case 3:
			// not
			src1 = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			dest = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			length = inMemory->getMemoryRange( inProgramCounter, 
				inProgramCounter + mBitsInAddress - 1, 
				&numChars );
			inProgramCounter += mBitsInAddress;
			
			noti( inMemory,
				  charArrayToLong( src1, numChars, mBitsInAddress ), 
				  charArrayToLong( dest, numChars, mBitsInAddress ), 
				  charArrayToLong( length, numChars, mBitsInAddress ) );
			
			delete [] src1;
			delete [] dest;
			delete [] length;
			
			return inProgramCounter;
			break;
		default:
			break;	
		}	
	
	}

unsigned long charArrayToLong( unsigned char *inArray, int inLength,
	int inNumBits ) {
	
	unsigned long output = 0;
	
	int outputShiftAmount = inNumBits - 1;
	for( int i=0; i<inNumBits; i++ ) {
		// get i'th bit
		int block = i >> 3;		// index of block containing bit
		int bit = i - ( block << 3 );	// index of bit in block
		int shiftAmount = 7 - bit;
		
		// shift and clear to extract bit from block
		unsigned long bitValue = 
			(inArray[block] >> shiftAmount ) & 0x01; 
		
		output = output | ( bitValue << outputShiftAmount ); 
		outputShiftAmount--;
		}
	return output;
	/*
	unsigned long output = 0;
	int shiftAmount = 0;
	for( int i=inLength-1; i>=0; i-- ) {
		output = output | ( (long)( inArray[i] ) << shiftAmount );
		shiftAmount += 8;
		}
	return output;
	*/
	}
