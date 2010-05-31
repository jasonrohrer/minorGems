/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2001-January-9		Jason Rohrer
 * Changed the "number of bytes read" parameter and return value
 * to longs.  
 *
 * 2001-February-3		Jason Rohrer
 * Added a readDouble function to fix platform-specific double problems. 
 * Also added a readLong function for completeness.  Implemented
 * these functions, making use of the new TypeIO interface.
 *
 * 2001-February-12		Jason Rohrer
 * Changed to subclass Stream.  
 *
 * 2002-March-9    Jason Rohrer
 * Added a readByte() function.  
 *
 * 2002-March-31    Jason Rohrer
 * Made destructor virtual so it works with subclasses.
 *
 * 2004-May-9   Jason Rohrer
 * Added support for shorts.
 */

#include "minorGems/common.h"


#ifndef INPUT_STREAM_CLASS_INCLUDED
#define INPUT_STREAM_CLASS_INCLUDED

#include "Stream.h"

#include "TypeIO.h"



/**
 * Interface for a byte input stream.
 *
 * @author Jason Rohrer
 */ 
class InputStream : public Stream {

	public:
		
		InputStream();
		virtual ~InputStream();
		
		/**
		 * Reads bytes from this stream.
		 *
		 * @param inBuffer the buffer where read bytes will be put.
		 *   Must be pre-allocated memory space.
		 * @param inNumBytes the number of bytes to read from the stream.
		 *
		 * @return the number of bytes read successfully,
		 *   or -1 for a stream error.
		 */
		virtual long read( unsigned char *inBuffer, long inNumBytes ) = 0;



        /**
         * Reads a byte from this stream.
         *
         * @param outByte pointer to where the byte should be stored.
         *
         * @return the number of bytes read successfully, or -1 for a 
         *   stream error.
         */
        long readByte( unsigned char *outByte );


        
		/**
		 * Reads a double from the stream in a platform-independent way.
		 *
		 * @param outDouble pointer to where the double should be stored.
		 *
		 * @return the number of bytes read successfully, or -1 for a 
		 *   stream error.
		 */
		long readDouble( double *outDouble );
		
		
		/**
		 * Reads a long from the stream in a platform-independent way.
		 *
		 * @param outLong pointer to where the long should be stored.
		 *
		 * @return the number of bytes read successfully, or -1 for a 
		 *   stream error.
		 */
		long readLong( long *outLong );
		
		
		/**
		 * Reads a short from the stream in a platform-independent way.
		 *
		 * @param outShort pointer to where the short should be stored.
		 *
		 * @return the number of bytes read successfully, or -1 for a 
		 *   stream error.
		 */
		long readShort( short *outShort );
	
	
	private:
		unsigned char *mDoubleBuffer;
		unsigned char *mLongBuffer;
        unsigned char *mShortBuffer;
        unsigned char *mByteBuffer;
        
	};		



inline InputStream::InputStream() 
	: mDoubleBuffer( new unsigned char[8] ),
      mLongBuffer( new unsigned char[4] ),
      mShortBuffer( new unsigned char[2] ),
      mByteBuffer( new unsigned char[1] ) {

	}



inline InputStream::~InputStream() {

	delete [] mDoubleBuffer;
	delete [] mShortBuffer;
	delete [] mLongBuffer;
    delete [] mByteBuffer;
	}



inline long InputStream::readByte( unsigned char *outByte ) {
    int numBytes = read( mByteBuffer, 1 );

    *outByte = mByteBuffer[0];

    return numBytes;
    }



inline long InputStream::readDouble( double *outDouble ) {
	int numBytes = read( mDoubleBuffer, 8 );
	*outDouble = TypeIO::bytesToDouble( mDoubleBuffer );
	
	return numBytes;
	}
	
	
		
inline long InputStream::readLong( long *outLong ) {
	int numBytes = read( mLongBuffer, 4 );
	*outLong = TypeIO::bytesToLong( mLongBuffer );
	
	return numBytes;
	}



inline long InputStream::readShort( short *outShort ) {
	int numBytes = read( mShortBuffer, 4 );
	*outShort = TypeIO::bytesToShort( mShortBuffer );
	
	return numBytes;
	}

	
	
#endif
