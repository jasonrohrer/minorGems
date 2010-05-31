/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2001-January-9		Jason Rohrer
 * Changed the "number of bytes written" parameter and return value
 * to longs.
 *
 * 2001-February-3		Jason Rohrer
 * Added a writeDouble function to fix platform-specific double problems. 
 * Also added a writeLong function for completeness.  Implemented
 * these functions, making use of the new TypeIO interface.  
 *
 * 2001-February-12		Jason Rohrer
 * Changed to subclass Stream.
 *
 * 2002-February-25		Jason Rohrer
 * Added a function for writing a string.
 *
 * 2002-March-31    Jason Rohrer
 * Made destructor virtual so it works with subclasses.
 *
 * 2004-May-9   Jason Rohrer
 * Added support for shorts.
 *
 * 2010-May-18    Jason Rohrer
 * String parameters as const to fix warnings.
 */

#include "minorGems/common.h"



#ifndef OUTPUT_STREAM_CLASS_INCLUDED
#define OUTPUT_STREAM_CLASS_INCLUDED

#include "Stream.h"

#include "TypeIO.h"

#include <string.h>



/**
 * Interface for a byte output stream.
 *
 * @author Jason Rohrer
 */ 
class OutputStream : public Stream {

	public:
		
		OutputStream();
		virtual ~OutputStream();
		
		/**
		 * Writes bytes to this stream.
		 *
		 * @param inBuffer the buffer of bytes to send.
		 * @param inNumBytes the number of bytes to send.
		 *
		 * @return the number of bytes written successfully,
		 *   or -1 for a stream error.
		 */
		virtual long write( unsigned char *inBuffer, long inNumBytes ) = 0;
		


        /**
         * Writes a string to this stream.
         *
         * @param inString a \0-terminated string.
         *   Must be destroyed by caller.
         * @return the number of bytes written successfully, or -1 for a 
		 *   stream error.
         */
        long writeString( const char *inString );


        
		/**
		 * Writes a double to the stream in a platform-independent way.
		 *
		 * @param inDouble the double to write
		 *
		 * @return the number of bytes written successfully, or -1 for a 
		 *   stream error.
		 */
		long writeDouble( double inDouble );
		
		
		/**
		 * Writes a long to the stream in a platform-independent way.
		 *
		 * @param inLong the long to write
		 *
		 * @return the number of bytes written successfully, or -1 for a 
		 *   stream error.
		 */
		long writeLong( long inLong );



        /**
		 * Writes a short to the stream in a platform-independent way.
		 *
		 * @param inShort the long to write
		 *
		 * @return the number of bytes written successfully, or -1 for a 
		 *   stream error.
		 */
		long writeShort( short inShort );

        
		
	private:
		unsigned char *mDoubleBuffer;
		unsigned char *mLongBuffer;
        unsigned char *mShortBuffer;
		
	};		



inline OutputStream::OutputStream() 
	: mDoubleBuffer( new unsigned char[8] ),
	mLongBuffer( new unsigned char[4] ),
    mShortBuffer( new unsigned char[2] ) {

	}



inline OutputStream::~OutputStream() {

	delete [] mDoubleBuffer;
	delete [] mLongBuffer;
    delete [] mShortBuffer;
	}



inline long OutputStream::writeString( const char *inString ) {
		
	int numBytes = write( (unsigned char *)inString, strlen( inString ) );

	return numBytes;
	}



inline long OutputStream::writeDouble( double inDouble ) {
	TypeIO::doubleToBytes( inDouble, mDoubleBuffer );
	
	int numBytes = write( mDoubleBuffer, 8 );

	return numBytes;
	}
	
	
		
inline long OutputStream::writeLong( long inLong ) {
	TypeIO::longToBytes( inLong, mLongBuffer );
	
	int numBytes = write( mLongBuffer, 4 );

	return numBytes;
	}



inline long OutputStream::writeShort( short inShort ) {
	TypeIO::shortToBytes( inShort, mShortBuffer );
	
	int numBytes = write( mShortBuffer, 2 );

	return numBytes;
	}


	
#endif
