/*
 * Modification History
 *
 * 2001-September-22   Jason Rohrer
 * Created.
 *
 * 2001-October-13   Jason Rohrer
 * Added support for 4-channel images.
 *
 * 2006-November-18   Jason Rohrer
 * Fixed two unused variable warnings.
 *
 * 2011-April-5   Jason Rohrer
 * Fixed MAJOR bug causing double output size for 3-channel images.
 * Fixed float-to-int conversion.  
 */
 
 
#ifndef TGA_IMAGE_CONVERTER_INCLUDED
#define TGA_IMAGE_CONVERTER_INCLUDED 


#include <math.h>


#include "LittleEndianImageConverter.h"

#include "minorGems/graphics/RawRGBAImage.h"


/**
 * TGA (Targa) implementation of the image conversion interface.
 *
 * Note that it only supports 24- and 32-bit TGA files
 * (and thus only 3- and 4-channel Images).
 *
 * TGA format information taken from:
 * http://www.cubic.org/source/archive/fileform/graphic/tga/targa.txt
 *
 * @author Jason Rohrer
 */
class TGAImageConverter : public LittleEndianImageConverter {
	
	public:
		
		// implement the ImageConverter interface
		virtual void formatImage( Image *inImage, 
			OutputStream *inStream );
			
		virtual Image *deformatImage( InputStream *inStream );		

		virtual RawRGBAImage *deformatImageRaw( InputStream *inStream );

	};



inline void TGAImageConverter::formatImage( Image *inImage, 
	OutputStream *inStream ) {

	int numChannels = inImage->getNumChannels();
	
	// make sure the image is in the right format
	if( numChannels != 3 &&
		numChannels != 4 ) {
		printf( "Only 3- and 4-channel images can be converted to " );
		printf( "the TGA format.\n" );
		return;
		}

	long width = inImage->getWidth();
	long height = inImage->getHeight();
	
	long numPixels = width * height;

	
	// a buffer for writing single bytes
	unsigned char *byteBuffer = new unsigned char[1];

	// write the identification field size
	// (an empty identification field)
	byteBuffer[0] = 0;
	inStream->write( byteBuffer, 1 );

	// write the color map type
	// (no color map)
	byteBuffer[0] = 0;
	inStream->write( byteBuffer, 1 );

	// write the image type code
	// (type 2:  unmapped RGB image)
	byteBuffer[0] = 2;
	inStream->write( byteBuffer, 1 );

	// no color map spec
	// (set to 0, though it will be ignored)
	unsigned char *colorMapSpec = new unsigned char[5];
	int i;
	for( i=0; i<5; i++ ) {
		colorMapSpec[i] = 0;
		}
	inStream->write( colorMapSpec, 5 );
	delete [] colorMapSpec;

	// now for the image specification

	// x origin coordinate
	writeLittleEndianShort( 0, inStream );
	// y origin coordinate
	writeLittleEndianShort( 0, inStream );
	
	writeLittleEndianShort( width, inStream );
	writeLittleEndianShort( height, inStream );

	// number of bits in pixels
	if( numChannels == 3 ) {
		byteBuffer[0] = 24;
		}
	else {
		byteBuffer[0] = 32;
		}
	inStream->write( byteBuffer, 1 );

	
	// image descriptor byte
	if( numChannels == 3 ) {
		// setting to 0 specifies:
		// -- no attributes per pixel (for 24-bit)
		// -- screen origin in lower left corner
		// -- non-interleaved data storage
		byteBuffer[0] = 0;
		}
	else {
		// setting to 8 specifies:
		// -- 8 attributes per pixel (for 32-bit) (attributes are alpha bits)
		// -- screen origin in lower left corner
		// -- non-interleaved data storage
		byteBuffer[0] = 8;
		}
	
	// set bit 5 to 1 to specify screen origin in upper left corner
	byteBuffer[0] = byteBuffer[0] | ( 1 << 5 );
	inStream->write( byteBuffer, 1 );

	
	// We skip the image identification field,
	// since we set its length to 0 above.

	// We also skip the color map data,
	// since we have none (as specified above).


	// now we write the pixels, in BGR(A) order
	unsigned char *raster = new unsigned char[ numPixels * numChannels ];
	double *red = inImage->getChannel( 0 );
	double *green = inImage->getChannel( 1 );
	double *blue = inImage->getChannel( 2 );

	long rasterIndex = 0;
	
	if( numChannels == 3 ) {
		for( int i=0; i<numPixels; i++ ) {
			raster[rasterIndex] = 
				(unsigned char)( lrint( 255 * blue[i] ) );
			raster[rasterIndex + 1] = 
				(unsigned char)( lrint( 255 * green[i] ) );
			raster[rasterIndex + 2] = 
				(unsigned char)( lrint( 255 * red[i] ) );
		
			rasterIndex += 3;
			}
		}
	else {  // numChannels == 4
		double *alpha = inImage->getChannel( 3 );
		
		for( int i=0; i<numPixels; i++ ) {
			raster[rasterIndex] = 
				(unsigned char)( lrint( 255 * blue[i] ) );
			raster[rasterIndex + 1] = 
				(unsigned char)( lrint( 255 * green[i] ) );
			raster[rasterIndex + 2] = 
				(unsigned char)( lrint( 255 * red[i] ) );
			raster[rasterIndex + 3] = 
				(unsigned char)( lrint( 255 * alpha[i] ) );
		
			rasterIndex += 4;
			}
		}

	inStream->write( raster, numPixels * numChannels );
	
	delete [] raster;
	delete [] byteBuffer;
	}




inline RawRGBAImage *TGAImageConverter::deformatImageRaw( 
    InputStream *inStream ) {
    
    // a buffer for reading single bytes
	unsigned char *byteBuffer = new unsigned char[1];

	// read the identification field size
	inStream->read( byteBuffer, 1 );

	int identificationFieldSize = byteBuffer[0];
		
	// read the color map type
	// (only 0, or no color map, is supported)
	inStream->read( byteBuffer, 1 );
	if( byteBuffer[0] != 0 ) {
		printf( "Only TGA files without colormaps can be read.\n" );
		delete [] byteBuffer;
		return NULL;
		}
	
	// read the image type code
	// (only type 2, unmapped RGB image, is supported)
	inStream->read( byteBuffer, 1 );
	if( byteBuffer[0] != 2 ) {
		printf(
			"Only TGA files containing unmapped RGB images can be read.\n" );
		delete [] byteBuffer;
		return NULL;
		}
	
	
	// ignore color map spec
	// (skip all 5 bytes of it)
	unsigned char *colorMapSpec = new unsigned char[5];
	inStream->read( colorMapSpec, 5 );
	delete [] colorMapSpec;


	// now for the image specification

    // don't need either of these
    // don't set to a variable for now to avoid unused variable warnings
	// x origin coordinate
	readLittleEndianShort( inStream );
	// y origin coordinate
	readLittleEndianShort( inStream );
	
	long width = readLittleEndianShort( inStream );
	long height = readLittleEndianShort( inStream );

	long numPixels = width * height;

	
	// number of bits in pixels
	// only 24 bits per pixel supported
	
	inStream->read( byteBuffer, 1 );
	if( byteBuffer[0] != 24 && byteBuffer[0] != 32 ) {
		printf( "Only 24- and 32-bit TGA files can be read.\n" );
		delete [] byteBuffer;
		return NULL;
		}

	int numChannels = 0;
	if( byteBuffer[0] == 24 ) {
		numChannels = 3;
		}
	else {
		numChannels = 4;
		}
	
	
	// image descriptor byte
	// setting to 0 specifies:
	// -- no attributes per pixel (for 24-bit)
	// -- screen origin in lower left corner
	// -- non-interleaved data storage
	// set bit 5 to 1 to specify screen origin in upper left corner
	inStream->read( byteBuffer, 1 );
	char originAtTop = byteBuffer[0] & ( 1 << 5 );

	
	if( identificationFieldSize > 0 ) {
		// We skip the image identification field
		unsigned char *identificationField =
			new unsigned char[ identificationFieldSize ];
		inStream->read( identificationField, identificationFieldSize );
		delete [] identificationField;
		}
	
	
	// We also skip the color map data,
	// since we have none (as specified above).


	
	
	// now we read the pixels, in BGR(A) order
	unsigned char *raster = new unsigned char[ numPixels * numChannels ];
	inStream->read( raster, numPixels * numChannels );

    if( !originAtTop ) {

        // flip it
        unsigned char *flippedRaster = 
            new unsigned char[ numPixels * numChannels ];
        

        int lineBytes = width * numChannels;

        for( int y=0; y<height; y++ ) {
            memcpy( &( flippedRaster[lineBytes * y] ),
                    &( raster[ lineBytes * ( height - y - 1 ) ] ),
                    lineBytes );
            }
        
        delete [] raster;
        raster = flippedRaster;
        }
    
    // convert to RGB(A) order
    int numBytes = numPixels * numChannels;
    int r = 2;
    for( int b=0; b<numBytes; b+= numChannels ) {
        unsigned char temp = raster[b];
        
        raster[b] = raster[r];
        raster[r] = temp;
        
        r += numChannels;
        }

    delete [] byteBuffer;
    return new RawRGBAImage( raster, width, height, numChannels );
    }



inline Image *TGAImageConverter::deformatImage( InputStream *inStream ) {
	RawRGBAImage *rawImage = deformatImageRaw( inStream );
	
    if( rawImage == NULL ) {
        return NULL;
        }
    
    unsigned char *raster = rawImage->mRGBABytes;
    int width = rawImage->mWidth;
    int height = rawImage->mHeight;
    
    int numChannels = rawImage->mNumChannels;

    int numPixels = width * height;

    // optimization:  don't init channels to black (found with profiler)
	Image *image = new Image( width, height, numChannels, false );

	double *red = image->getChannel( 0 );
	double *green = image->getChannel( 1 );
	double *blue = image->getChannel( 2 );
	
	long rasterIndex = 0;
	double inv255 = 1.0 / 255.0;

	if( numChannels == 3 ) {
        for( int i=0; i<numPixels; i++ ) {
            red[i] = inv255 * raster[ rasterIndex ];
            green[i] = inv255 * raster[ rasterIndex + 1 ];
            blue[i] = inv255 * raster[ rasterIndex + 2 ];
			
            rasterIndex += 3;
            }
        }
	else {  // numChannels == 4
		double *alpha = image->getChannel( 3 );
        for( int i=0; i<numPixels; i++ ) {
            // optimization:  use postfix increment operators in
            // array index
            // (found with profiler)
            red[i] = inv255 * raster[ rasterIndex ++ ];
            green[i] = inv255 * raster[ rasterIndex ++ ];
            blue[i] = inv255 * raster[ rasterIndex ++ ];
            alpha[i] = inv255 * raster[ rasterIndex ++ ];
            
            // optimization
            // rasterIndex += 4;
            }
		}

	
	delete rawImage;
	

	return image;
	}


		
#endif
