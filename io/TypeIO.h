/*
 * Modification History
 *
 * 2001-February-3		Jason Rohrer
 * Created.
 * Fixed parameter names to match convention.
 *
 * 2003-January-11		Jason Rohrer
 * Added missing casts.
 *
 * 2004-May-9   Jason Rohrer
 * Added support for shorts.
 */

#include "minorGems/common.h"


#ifndef TYPE_IO_INCLUDED
#define TYPE_IO_INCLUDED

/**
 * Interfaces for platform-independent type input and output.
 *
 * The specification for the input/output format for types is as follows:
 *
 * Types should be output in the order and format that a big-endian Linux
 * outputs them by default.
 *
 * Note that minorGems/numtest.cpp can be used to test how doubles are
 * stored on a specific platform. 
 *
 * @author Jason Rohrer
 */ 
class TypeIO {

	public:
		
		/**
		 * Converts an 32-bit integer to a byte array in a 
		 * platform-independent fashion.
		 *
		 * @param inInt the integer to convert to a byte array.
		 * @param outBytes preallocated array where bytes will be returned.
		 */
		static void longToBytes( long inInt, unsigned char *outBytes ); 
		
		
		/**
		 * Converts a 4-byte array to a 32-bit integer  
		 * platform-independent fashion.
		 *
		 * @param inBytes array of bytes to convert.
		 *
		 * @return the integer represented by the bytes.
		 */
		static long bytesToLong( unsigned char *inBytes );

		
		/**
		 * Converts an 16-bit integer to a byte array in a 
		 * platform-independent fashion.
		 *
		 * @param inInt the integer to convert to a byte array.
		 * @param outBytes preallocated array where bytes will be returned.
		 */
		static void shortToBytes( short inInt, unsigned char *outBytes ); 
		
		
		/**
		 * Converts a 2-byte array to a 16-bit integer  
		 * platform-independent fashion.
		 *
		 * @param inBytes array of bytes to convert.
		 *
		 * @return the integer represented by the bytes.
		 */
		static short bytesToShort( unsigned char *inBytes );
		
		
		/**
		 * Converts an 64-bit float to a byte array in a 
		 * platform-independent fashion.
		 *
		 * @param inDouble the double to convert to a byte array.
		 * @param outBytes preallocated array where bytes will be returned.
		 */
		static void doubleToBytes( 
			double inDouble, unsigned char *outBytes ); 
		
		
		/**
		 * Converts a 8-byte array to a 64-bit float  
		 * platform-independent fashion.
		 *
		 * @param inBytes array of bytes to convert.
		 *
		 * @return the double represented by the bytes.
		 */
		static double bytesToDouble( unsigned char *inBytes );
		
	};		



// for now, these long IO functions can be implemented in the same way 
// on every platform.

inline void TypeIO::longToBytes( long inInt, 
	unsigned char *outBytes ) {
	// use a big-endian conversion
	outBytes[0] = (unsigned char)( inInt >> 24 & 0xFF );
	outBytes[1] = (unsigned char)( inInt >> 16 & 0xFF );
	outBytes[2] = (unsigned char)( inInt >> 8 & 0xFF );
	outBytes[3] = (unsigned char)( inInt & 0xFF );
	} 



inline long TypeIO::bytesToLong( unsigned char *inBytes ) {
	return (long)( inBytes[0] << 24 | 
		inBytes[1] << 16 | inBytes[2] << 8 | inBytes[3] );
	}



inline void TypeIO::shortToBytes( short inInt, 
	unsigned char *outBytes ) {
	// use a big-endian conversion
	outBytes[0] = (unsigned char)( inInt >> 8 & 0xFF );
	outBytes[1] = (unsigned char)( inInt & 0xFF );
	} 



inline short TypeIO::bytesToShort( unsigned char *inBytes ) {
	return (short)( inBytes[0] << 8 | inBytes[1] );
	}


	
#endif
