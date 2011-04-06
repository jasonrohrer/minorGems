/*
 * Modification History
 *
 * 2001-February-19		Jason Rohrer
 * Created.
 *
 * 2001-February-19		Jason Rohrer
 * Fixed some bugs in the raster formatting code.
 *
 * 2001-May-19   Jason Rohrer
 * Fixed a bug in zero padding for each line.
 *
 * 2001-May-21   Jason Rohrer
 * Fixed another bug in zero padding for each line.
 *
 * 2001-September-22   Jason Rohrer
 * Changed to subclass LittleEndianImageConverter.
 *
 * 2011-April-5     Jason Rohrer
 * Fixed float-to-int conversion.  
 */
 
 
#ifndef BMP_IMAGE_CONVERTER_INCLUDED
#define BMP_IMAGE_CONVERTER_INCLUDED 


#include <math.h>


#include "LittleEndianImageConverter.h"


/**
 * Windows BMP implementation of the image conversion interface.
 *
 * Note that it only supports 24-bit BMP files (and thus only 3-channel
 * Images).
 *
 * BMP format information taken from:
 * http://www.daubnet.com/formats/BMP.html
 * Some information from this site is incorrect:
 *   24-bit pixels are _not_ stored in 32-bit blocks.
 *
 * @author Jason Rohrer
 */
class BMPImageConverter : public LittleEndianImageConverter {
	
	public:
		
		// implement the ImageConverter interface
		virtual void formatImage( Image *inImage, 
			OutputStream *inStream );
			
		virtual Image *deformatImage( InputStream *inStream );		


	};



inline void BMPImageConverter::formatImage( Image *inImage, 
	OutputStream *inStream ) {
	
	// make sure the image is in the right format
	if( inImage->getNumChannels() != 3 ) {
		printf(
			"Only 3-channel images can be converted to the BMP format.\n" );
		return;
		}

	long width = inImage->getWidth();
	long height = inImage->getHeight();
	
	long numPixels = width * height;

	// each line should be padded with zeros to
	// end on a 4-byte boundary
	int numZeroPaddingBytes = ( 4 - ( width * 3 ) % 4 ) % 4;
	
	short bitCount = 24;
	long rasterSize = numPixels * 3;
	// zero padding bytes for each row
	rasterSize += numZeroPaddingBytes * height;
	
	// offset past two headers
	long offsetToRaster = 14 + 40;
	
	long fileSize = offsetToRaster + rasterSize;
	
	long compressionType = 0;
	
	
	long pixelsPerMeter = 2834;
	
	// both are 0 since we have no color map
	long colorsUsed = 0;
	long colorsImportant = 0;
	
	// write the header
	unsigned char *signature = new unsigned char[2];
	signature[0] = 'B';
	signature[1] = 'M';
	inStream->write( signature, 2 );
	delete [] signature;
	writeLittleEndianLong( fileSize, inStream );
	writeLittleEndianLong( 0, inStream );
	writeLittleEndianLong( offsetToRaster, inStream );
	
	// write the info header
	// header size
	writeLittleEndianLong( 40, inStream );
	writeLittleEndianLong( width, inStream );
	writeLittleEndianLong( height, inStream );
	// numPlanes
	writeLittleEndianShort( 1, inStream );
	writeLittleEndianShort( bitCount, inStream );
	writeLittleEndianLong( compressionType, inStream );
	writeLittleEndianLong( rasterSize, inStream );
	writeLittleEndianLong( pixelsPerMeter, inStream );
	writeLittleEndianLong( pixelsPerMeter, inStream );
	writeLittleEndianLong( colorsUsed, inStream );
	writeLittleEndianLong( colorsImportant, inStream );
	
	// no color table...
	
	// now write the raster
	
	unsigned char *raster = new unsigned char[ rasterSize ];
	double *red = inImage->getChannel( 0 );
	double *green = inImage->getChannel( 1 );
	double *blue = inImage->getChannel( 2 );
	
	// pixels are stored bottom-up, left to right
	// (row major order)
	
	long rasterIndex = 0;
	for( int y=height-1; y>=0; y-- ) {
		for( int x=0; x<width; x++ ) {
			int imageIndex = y * width + x;
			
			raster[rasterIndex] = 
				(unsigned char)( lrint( 255 * blue[imageIndex] ) );
			raster[rasterIndex + 1] = 
				(unsigned char)( lrint( 255 * green[imageIndex] ) );
			raster[rasterIndex + 2] = 
				(unsigned char)( lrint( 255 * red[imageIndex] ) );
			
			rasterIndex += 3;
			}

		for( int p=0; p<numZeroPaddingBytes; p++ ) {
			// insert at the end of this line
			raster[ rasterIndex ] = 0;
			rasterIndex++;
			}
		}
	
	inStream->write( raster, rasterSize );
	
	delete [] raster;
	}



inline Image *BMPImageConverter::deformatImage( InputStream *inStream ) {
	// temp buffer used to skip data in the stream
	unsigned char *temp = new unsigned char[ 100 ];
	
	// skip signature
	inStream->read( temp, 2 );
	
	long fileSize = readLittleEndianLong( inStream );
	
	// skip unused
	inStream->read( temp, 4 );
	
	long rasterOffset = readLittleEndianLong( inStream );
	long rasterSize = fileSize - rasterOffset;
	
	// skip size of header
	inStream->read( temp, 4 );
	
	long width = readLittleEndianLong( inStream );
	long height = readLittleEndianLong( inStream );
	
	// skip planes
	inStream->read( temp, 2 );
	
	short bitCount = readLittleEndianShort( inStream );
	
	char failing = false;
	if( bitCount != 24 ) {
		printf( "Only 24-bit BMP file formats supported.\n" );
		failing = true;
		}
	long compression = readLittleEndianLong( inStream );
	if( compression != 0 ) {
		printf( "Only uncompressed BMP file formats supported.\n" );
		failing = true;
		}
	
	// skip imageSize, resolution, and color usage information
	inStream->read( temp, 20 );
	
	// now we're at the raster.

	// each line should be padded with zeros to
	// end on a 4-byte boundary
	int numZeroPaddingBytes = ( 4 - ( width * 3 ) % 4 ) % 4;
	
	unsigned char *raster = new unsigned char[ rasterSize ];
	inStream->read( raster, rasterSize );
	Image *returnImage;
	if( failing ) {
		return NULL;
		}
	else {
		
		returnImage = new Image( width, height, 3 );
		
		double *red = returnImage->getChannel( 0 );
		double *green = returnImage->getChannel( 1 );
		double *blue = returnImage->getChannel( 2 );

		// pixels are stored bottom-up, left to right
		// (row major order)

		long rasterIndex = 0;
		for( int y=height-1; y>=0; y-- ) {
			for( int x=0; x<width; x++ ) {
				int imageIndex = y * width + x;

				blue[imageIndex] = 
					(double)( raster[rasterIndex] ) / 255.0;
				green[imageIndex] = 
					(double)( raster[rasterIndex + 1] ) / 255.0;
				red[imageIndex] = 
					(double)( raster[rasterIndex + 2] ) / 255.0;

				rasterIndex += 3;
				}
			
			// skip the zero padding bytes at the end of this line
			rasterIndex += numZeroPaddingBytes;
			}
		}
	
	delete [] raster;	
	delete [] temp;
	
	return returnImage;
	}


		
#endif
