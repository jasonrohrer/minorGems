/*
 * Modification History
 *
 * 2001-February-3		Jason Rohrer
 * Created.
 * Fixed parameter names to match convention.
 *
 * 2001-February-4		Jason Rohrer
 * Fixed a byte-order bug.
 */
 
 
#include "minorGems/io/TypeIO.h"


/*
 * Win32-specific type input and output.
 * Note that all types are output in the order that
 * a big-endian linux machine outputs them with no conversion.
 */ 

// windows machines are all little-endian

void TypeIO::doubleToBytes( double inDouble, unsigned char *outBytes ) {
	
	unsigned char *doubleBuffer = (unsigned char*)( &inDouble );
	
	// output second word first
	outBytes[0] = doubleBuffer[7];
	outBytes[1] = doubleBuffer[6];
	outBytes[2] = doubleBuffer[5];
	outBytes[3] = doubleBuffer[4];
	
	outBytes[4] = doubleBuffer[3];
	outBytes[5] = doubleBuffer[2];
	outBytes[6] = doubleBuffer[1];
	outBytes[7] = doubleBuffer[0];
	}



double TypeIO::bytesToDouble( unsigned char *inBytes ) {
	
	double returnValue;
	
	unsigned char *doubleBuffer = (unsigned char*)( &returnValue );
	
	// put first word at the end of this double
	doubleBuffer[7] = inBytes[0];
	doubleBuffer[6] = inBytes[1];
	doubleBuffer[5] = inBytes[2];
	doubleBuffer[4] = inBytes[3];
	
	doubleBuffer[3] = inBytes[4];
	doubleBuffer[2] = inBytes[5];
	doubleBuffer[1] = inBytes[6];
	doubleBuffer[0] = inBytes[7];
	
	return returnValue;
	}
