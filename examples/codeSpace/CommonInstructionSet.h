/*
 * Modification History
 *
 * 2000-November-19		Jason Rohrer
 * Created.
 *
 * 2001-November-29		Jason Rohrer
 * Fixed some typos.
 *
 * 2001-November-30		Jason Rohrer
 * Changed names of functions to avoid keyword collisions.
 */
 
#ifndef COMMON_INSTRUCTION_SET_INCLUDED
#define COMMON_INSTRUCTION_SET_INCLUDED

#include "BitMemory.h"

#include <stdio.h>

/**
 * Abstract class that implements some commonly used instructions.
 * 
 * Instructions include:
 * COPY A1 A3 L1		::Copies L1 bits of memory starting at A1 into A3
 * AND A1 A2 A3 L1		::Computes A1 and A2 for L1 bits and puts result at A3
 * NOT A1 A3 L1			::Computes not A1 for L1 bits and puts result at A3 
 */
class CommonInstructionSet {

	protected:
		
		// force this class to be abstract by protecting
		// it's constructor
		CommonInstructionSet();
		
		void copyi( BitMemory *inMemory, unsigned long inSrc, 
			unsigned long inDest, unsigned long inLength );
		
		
		void andi( BitMemory *inMemory, unsigned long inSrc1, 
			unsigned long inSrc2, unsigned long inDest, 
			unsigned long inLength );	
		
		
		void noti( BitMemory *inMemory, unsigned long inSrc, 
			unsigned long inDest, unsigned long inLength );	
	
	};

inline CommonInstructionSet::CommonInstructionSet() {
	}

#endif
