/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 *
 * 2001-November-29		Jason Rohrer
 * Fixed some typos.
 */
 
#ifndef SIMPLE_INSTRUCTION_SET_INCLUDED
#define SIMPLE_INSTRUCTION_SET_INCLUDED

#include "InstructionSet.h"
#include "CommonInstructionSet.h"

#include <stdio.h>

/**
 * Simple instruction set that uses 2 bit instructions.
 * 
 * Instructions used from CommonInstructionSet:
 * COPY A1 A3 L1		::Copies L1 bits of memory starting at A1 into A3
 * AND A1 A2 A3 L1		::Computes A1 and A2 for L1 bits and puts result at A3
 * NOT A1 A3 L1			::Computes not A1 for L1 bits and puts result at A3 
 *
 * Also includes:
 * JUMP A1				::Jumps to bit address A1
 */
class SimpleInstructionSet : public InstructionSet, 
	public CommonInstructionSet {
	
	
	public:
		
		// implements the InstructionSet interface
		unsigned long stepProgram( BitMemory *inMemory, 
			unsigned long inProgramCounter );
	
	};

#endif
