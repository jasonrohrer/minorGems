/*
 * Modification History
 *
 * 2006-November-21   Jason Rohrer
 * Created.  Copied from LittleEndian version.
 */
 
 
#ifndef BIG_ENDIAN_IMAGE_CONVERTER_INCLUDED
#define BIG_ENDIAN_IMAGE_CONVERTER_INCLUDED 


#include "minorGems/graphics/ImageConverter.h"


/**
 * A base class for converters that have big endian file formats.
 * Basically includes big endian reading and writing functions.
 *
 * @author Jason Rohrer
 */
class BigEndianImageConverter : public ImageConverter {
	
	public:
		
		// does not implement the ImageConverter interface,
		// which makes this class abstract.

		
	protected:

		/**
		 * Writes a long value in big endian format.
		 *
		 * @param inLong the long value to write.
		 * @param inStream the stream to write inLong to.
		 */
		void writeBigEndianLong( long inLong, 
			OutputStream *inStream );



		/**
		 * Writes a short value in big endian format.
		 *
		 * @param inShort the short value to write.
		 * @param inStream the stream to write inShort to.
		 */
		void writeBigEndianShort( short inShort, 
			OutputStream *inStream );



		/**
		 * Reads a long value in big endian format.
		 *
		 * @param inStream the stream to read the long value from.
		 *
		 * @return the long value.
		 */
		long readBigEndianLong( InputStream *inStream );



		/**
		 * Reads a short value in big endian format.
		 *
		 * @param inStream the stream to read the short value from.
		 *
		 * @return the short value.
		 */
		short readBigEndianShort( InputStream *inStream );


		
	};



inline void BigEndianImageConverter::writeBigEndianLong( long inLong, 
	OutputStream *inStream ) {
	
	unsigned char buffer[4];
	
	buffer[0] = (unsigned char)( ( inLong >> 24 ) & 0xFF );
    buffer[1] = (unsigned char)( ( inLong >> 16 ) & 0xFF );
	buffer[2] = (unsigned char)( ( inLong >> 8 ) & 0xFF );
    buffer[3] = (unsigned char)( inLong & 0xFF );
	
	inStream->write( buffer, 4 );
	}



inline long BigEndianImageConverter::readBigEndianLong(
	InputStream *inStream ) {

	unsigned char buffer[4];
	
	inStream->read( buffer, 4 );
	
	long outLong =
        ( buffer[0] << 24 ) |
        ( buffer[1] << 16 ) |
        ( buffer[2] << 8 )  |
        buffer[3];
	
	return outLong;
	}



inline void BigEndianImageConverter::writeBigEndianShort( short inShort, 
	OutputStream *inStream ) {
	
	unsigned char buffer[2];
	
	buffer[0] = (unsigned char)( ( inShort >> 8 ) & 0xFF );
	buffer[1] = (unsigned char)( inShort & 0xFF );
	
	inStream->write( buffer, 2 );
	}



inline short BigEndianImageConverter::readBigEndianShort(
	InputStream *inStream ) {

	unsigned char buffer[2];
	
	inStream->read( buffer, 2 );
	
	long outShort =
        ( buffer[0] << 8 ) |
        buffer[1];
	
	return outShort;
	}


		
#endif
