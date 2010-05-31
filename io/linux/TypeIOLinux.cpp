/*
 * Modification History
 *
 * 2001-February-3		Jason Rohrer
 * Created.
 * Fixed a bug in the big-endian code.
 * Fixed parameter names to match convention.
 *
 * 2001-February-26		Jason Rohrer
 * Fixed a bug in the little-endian implementation.
 *
 * 2001-August-28		Jason Rohrer
 * Changed to be FreeBSD compatible. 
 *
 * 2002-March-13   Jason Rohrer
 * Started to change to work with solaris.
 * Finished changing to work with solaris.
 *
 * 2002-April-11   Jason Rohrer
 * Added a default BSD case to work with OSX.
 *
 * 2002-May-25   Jason Rohrer
 * Changed to use minorGems endian.h
 */
 
 
#include "minorGems/io/TypeIO.h"
#include "minorGems/system/endian.h"




/*
 * Linux-specific type input and output.
 * Note that all types are output in the order that
 * a big-endian linux machine outputs them with no conversion.
 */ 



#if __BYTE_ORDER == __LITTLE_ENDIAN


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



#endif



#if __BYTE_ORDER == __BIG_ENDIAN



void TypeIO::doubleToBytes( double inDouble, unsigned char *outBytes ) {
	
	unsigned char *doubleBuffer = (unsigned char*)( &inDouble );
	
	// output in stored order
	outBytes[0] = doubleBuffer[0];
	outBytes[1] = doubleBuffer[1];
	outBytes[2] = doubleBuffer[2];
	outBytes[3] = doubleBuffer[3];
	
	outBytes[4] = doubleBuffer[4];
	outBytes[5] = doubleBuffer[5];
	outBytes[6] = doubleBuffer[6];
	outBytes[7] = doubleBuffer[7];
	}



double TypeIO::bytesToDouble( unsigned char *inBytes ) {
	
	double returnValue;
	
	unsigned char *doubleBuffer = (unsigned char*)( &returnValue );
	
	// store in input order
	doubleBuffer[0] = inBytes[0];
	doubleBuffer[1] = inBytes[1];
	doubleBuffer[2] = inBytes[2];
	doubleBuffer[3] = inBytes[3];
	
	doubleBuffer[4] = inBytes[4];
	doubleBuffer[5] = inBytes[5];
	doubleBuffer[6] = inBytes[6];
	doubleBuffer[7] = inBytes[7];
	
	return returnValue;
	}
	
	
	
#endif
