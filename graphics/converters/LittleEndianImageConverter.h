/*
 * Modification History
 *
 * 2001-September-22   Jason Rohrer
 * Created.
 */
 
 
#ifndef LITTLE_ENDIAN_IMAGE_CONVERTER_INCLUDED
#define LITTLE_ENDIAN_IMAGE_CONVERTER_INCLUDED 


#include "minorGems/graphics/ImageConverter.h"


/**
 * A base class for converters that have little endian file formats.
 * Basically includes little endian reading and writing functions.
 *
 * @author Jason Rohrer
 */
class LittleEndianImageConverter : public ImageConverter {
	
	public:
		
		// does not implement the ImageConverter interface,
		// which makes this class abstract.

		
	protected:

		/**
		 * Writes a long value in little endian format.
		 *
		 * @param inLong the long value to write.
		 * @param inStream the stream to write inLong to.
		 */
		void writeLittleEndianLong( long inLong, 
			OutputStream *inStream );



		/**
		 * Writes a short value in little endian format.
		 *
		 * @param inShort the short value to write.
		 * @param inStream the stream to write inShort to.
		 */
		void writeLittleEndianShort( short inShort, 
			OutputStream *inStream );



		/**
		 * Reads a long value in little endian format.
		 *
		 * @param inStream the stream to read the long value from.
		 *
		 * @return the long value.
		 */
		long readLittleEndianLong( InputStream *inStream );



		/**
		 * Reads a short value in little endian format.
		 *
		 * @param inStream the stream to read the short value from.
		 *
		 * @return the short value.
		 */
		short readLittleEndianShort( InputStream *inStream );


		
	};



inline void LittleEndianImageConverter::writeLittleEndianLong( long inLong, 
	OutputStream *inStream ) {
	
	unsigned char buffer[4];
	
	buffer[0] = (unsigned char)( inLong & 0xFF );
	buffer[1] = (unsigned char)( ( inLong >> 8 ) & 0xFF );
	buffer[2] = (unsigned char)( ( inLong >> 16 ) & 0xFF );
	buffer[3] = (unsigned char)( ( inLong >> 24 ) & 0xFF );
	
	inStream->write( buffer, 4 );
	}



inline long LittleEndianImageConverter::readLittleEndianLong(
	InputStream *inStream ) {

	unsigned char buffer[4];
	
	inStream->read( buffer, 4 );
	
	long outLong = buffer[0] | ( buffer[1] << 8 ) | ( buffer[2] << 16 )
		| ( buffer[3] << 24 );
	
	return outLong;
	}



inline void LittleEndianImageConverter::writeLittleEndianShort( short inShort, 
	OutputStream *inStream ) {
	
	unsigned char buffer[2];
	
	buffer[0] = (unsigned char)( inShort & 0xFF );
	buffer[1] = (unsigned char)( ( inShort >> 8 ) & 0xFF );
	
	inStream->write( buffer, 2 );
	}



inline short LittleEndianImageConverter::readLittleEndianShort(
	InputStream *inStream ) {

	unsigned char buffer[2];
	
	inStream->read( buffer, 2 );
	
	long outShort = buffer[0] | ( buffer[1] << 8 );
	
	return outShort;
	}


		
#endif
