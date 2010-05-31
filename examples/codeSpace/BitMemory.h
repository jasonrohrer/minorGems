/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 *
 * 2001-November-30		Jason Rohrer
 * Added a missing include.
 */
 
#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

#include <stdio.h>
#include <string.h>


/**
 * A codeSpace bit-addressable memory object.  Memory access is circular.
 */
class BitMemory {

	public:
		
		
		BitMemory( unsigned long inNumBits );
		~BitMemory();
		
		
		/**
		 * Gets the size of this memory.
		 *
		 * @return the number of bits in this memory.
		 */
		unsigned long getSize();
		
		
		/** 
		 * Gets the contents of a memory range.  Memory bounds check is
		 * performed before accessing memory.  Memory wraps upward, but not
		 * downward (i.e., positive addresses past end bound wrap to lower
		 * addresses, but negative addresses are illegal). 
		 *
		 * @param inRangeStart start of range in bits, inclusive.
		 * @param inRangeEnd end of range in bits, inclusive.
		 * @param outNumCharsReturned the number of chars in the
		 *   returned array.
		 *
		 * @return an array of chars in big endian order representing
		 *   the requested memory range.  The first bit of the 0-index
		 *   char corresponds to the inRangeStart memory location.  
		 *   Caller is responsible for destroying the returned array.
		 *   Returns NULL if out of range.
		 */
		unsigned char *getMemoryRange( unsigned long inRangeStart,
			unsigned long inRangeEnd, unsigned long *outNumCharsReturned );
	
		
		/**
		 * Gets complete memory contents.
		 *
		 * @param outNumCharsReturned the number of chars in the
		 *   returned array.
		 *
		 * @return an array of chars in big endian order representing
		 *   the entire memory range.  Caller is responsible for destroying 
		 *   the returned array.
		 */
		unsigned char *getAllMemory( unsigned long *outNumCharsReturned );
		
		
		/** 
		 * Sets the contents of a memory range.  Memory bounds check is
		 * performed before accessing memory.  Memory wraps upward, but not
		 * downward (i.e., positive addresses past end bound wrap to lower
		 * addresses, but negative addresses are illegal). 
		 *
		 * @param inRangeStart start of range in bits, inclusive.
		 * @param inRangeEnd end of range in bits, inclusive.
		 * @param inRangeContents the contents to put in memory
		 *   packed into a char array.  The first bit of the 0-index
		 *   char corresponds to the inRangeStart memory location.
		 */
		void setMemoryRange( unsigned long inRangeStart,
			unsigned long inRangeEnd, unsigned char *inRangeContents );
		
		
		
		
		/**
		 * Clears contents of a memory range (sets bits to 0).
		 *
		 * @param inRangeStart start of range in bits, inclusive.
		 * @param inRangeEnd end of range in bits, inclusive.
		 */
		void clearMemoryRange( unsigned long inRangeStart,
			unsigned long inRangeEnd );
		
		
		/**
		 * Clears contents of entire memory object (sets bits to 0).
		 */
		void clearMemory();
		
		
		/**
		 * Prints the contents of memory to standar out.
		 */
		void printMemory();
		
		
	private:
		// memory packed into a char array
		unsigned char *mMemoryArray;
		unsigned long mNumBits;
		
		
		/**
		 * Gets a bit.  Note that no out-of-range test is performed, and
		 * large positive indices wrap around.
		 *
		 * @param inIndex the index of the bit to get.
		 */
		char getBit( unsigned long inIndex );
		
		
		/**
		 * Sets a bit.  Note that no out-of-range test is performed, and
		 * large positive indices wrap around.
		 *
		 * @param inValue the value to set the bit to, treated as a boolean.
		 */
		void setBit( unsigned long inIndex, char inValue );
		
	};



inline BitMemory::BitMemory( unsigned long inNumBits )
	: mNumBits( inNumBits ),
	mMemoryArray( new unsigned char[ inNumBits >> 3 ] ) {	// divide by 8
	
	}



inline BitMemory::~BitMemory() {
	delete [] mMemoryArray;
	}



inline char BitMemory::getBit( unsigned long inIndex ) {
	inIndex = inIndex % mNumBits;	// wrap around
	// >> 3 = divide by 8
	unsigned long block = inIndex >> 3;		// index of block containing bit
	int bit = inIndex - ( block << 3 );	// index of bit in block
	int shiftAmount = 7 - bit;
	// shift and clear to extract bit from block
	return (char)( ( mMemoryArray[block] >> shiftAmount ) & 0x01 );
	}



inline void BitMemory::setBit( unsigned long inIndex, char inValue ) {
	inIndex = inIndex % mNumBits;	// wrap around
	// >> 3 = divide by 8
	unsigned long block = inIndex >> 3;		// index of block containing bit
	int bit = inIndex - ( block << 3 );	// index of bit in block
	int shiftAmount = 7 - bit;
	
	unsigned char invMask = 0x01 << shiftAmount;
	unsigned char mask = ~( invMask );
	
	// clear the bit we're trying to set
	mMemoryArray[block] = mMemoryArray[block] & mask;
	
	if( inValue ) {
		mMemoryArray[block] = mMemoryArray[block] | invMask;
		}
	// else do nothing, since bit value should be 0 and 
	// we've already cleared it
	}



inline unsigned char *BitMemory::getAllMemory( unsigned long *outNumCharsReturned ) {
	*outNumCharsReturned = mNumBits >> 3;
	
	unsigned char *outArray = new unsigned char[ mNumBits >> 3 ];
	memcpy( (void*)outArray, (void *)mMemoryArray, mNumBits >> 3 );
	
	return outArray;
	}



inline void BitMemory::printMemory() {
	for( int i=0; i<mNumBits; i++ ) {
		if( i%8 == 0 ) {
			printf( " " );
			}
		printf( "%d", ( mMemoryArray[ i / 8 ] >> ( 7 - ( i % 8 ) ) ) & 0x01 );
		
		}
	printf( "\n" );
	}
	
	
#endif
