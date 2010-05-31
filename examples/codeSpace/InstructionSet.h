/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 */
 
#ifndef INSTRUCTION_SET_INCLUDED
#define INSTRUCTION_SET_INCLUDED

#include "BitMemory.h"

#include <stdio.h>

/**
 * Abstract base class for a codeSpace instruction set.
 */
class InstructionSet {
	public:
		
	
		/**
		 * Sets the desired memory size in bits.  The memory size
		 * is constrained by the length of a memory address.  If a
		 * non-power-of-2 is passed in, it is converted to the next smallest
		 * power of 2 and passed back.
		 *
		 * @param inNumBits desired memory size in bits.  Must be >= 2.
		 * 
		 * @return actually memory size in bits.  Returned value <= inNumBits.
		 */
		unsigned long setMemorySize( unsigned long inNumBits );
	
	
		/**
		 * Gets the memory size in bits.
		 *
		 * @return the memory size in bits.
		 */
		unsigned long getMemorySize();
	
	
		/**
		 * Runs one step of the program.
		 *
		 * @param inMemory memory object containing program.
		 * @param inProgramCounter current index (in bits) into
		 *   memory.  Instruction for step will be fetched starting
		 *   at this index.
		 *
		 * @return program counter index (in bits) after step.
		 */
		virtual unsigned long stepProgram( BitMemory *inMemory, 
			unsigned long inProgramCounter ) = 0;
		
		
	protected:
		int mBitsInAddress;
		
		
		/**
		 * Constructs an instruction set, setting default number of bits
		 * in an address to 8.
		 */
		InstructionSet();
	
	};



inline InstructionSet::InstructionSet() :
	mBitsInAddress( 8 ) {
	
	}



inline unsigned long InstructionSet::setMemorySize( 
	unsigned long inNumBits ) {
	
	int shiftCounter = 0;
	while( inNumBits != 0 ) {
		inNumBits = inNumBits >> 1;
		shiftCounter++;
		}
		
	mBitsInAddress = shiftCounter - 1;
	
	return( getMemorySize() );
	}



inline unsigned long InstructionSet::getMemorySize() {
	unsigned long output = 0x01;
	return( output << ( mBitsInAddress ) );
	}



#endif
