/*
 * Modification History
 *
 * 2001-September-20		Jason Rohrer
 * Created.
 * Changed so that images are normalized to [0,1] before
 * being output.
 */

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"

#include "minorGems/graphics/converters/BMPImageConverter.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/io/file/FileOutputStream.h"

#include <stdio.h>
#include <stdlib.h>


void usage( char *inAppName );
Image *loadBMPImage( char *inFileName );
void writeBMPImage( char *inFileName, Image *inImage );
// normalizes an image into the pixel range [0,1] if some
// pixels are out of range
void normalizeImage( Image *inImage );


// a test program that converts an rgb BMP to a yiq BMP
int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 3 ) {
		usage( inArgs[0] );
		}

	Image *yiqImage = loadBMPImage( inArgs[1] );

	if( yiqImage == NULL ) {
		printf( "Reading image from file %s failed\n", inArgs[1] );
		usage( inArgs[0] );
	}
	
	Image *rgbImage = ImageColorConverter::YIQToRGB( yiqImage );

	normalizeImage( rgbImage );
	
	writeBMPImage( inArgs[2], rgbImage );

	delete rgbImage;
	delete yiqImage;
	
	return 0;
	}


void usage( char *inAppName ) {
	printf( "Usage:\n" );
	printf( "\t%s yiq_image_bmp rgb_image_bmp\n", inAppName );
	printf( "Example:\n" );
	printf( "\t%s testYIQ.bmp testRGB.bmp\n", inAppName );

	exit( 1 );
}



void normalizeImage( Image *inImage ) {
	// now normalize the image so that pixels are in the range [0,1]
	
	int numPixels = inImage->getWidth() * inImage->getHeight();
	int numChannels = inImage->getNumChannels();
	double minValue = 1000.0;
	double maxValue = -1000.0;
	int c;

	// search for min and max values
	for( c=0; c<numChannels; c++ ) {
		double *channel = inImage->getChannel( c );
		for( int p=0; p<numPixels; p++ ) {
			if( channel[p] < minValue ) {
				minValue = channel[p];
				}
			else if( channel[p] > maxValue ) {
				maxValue = channel[p];
				}
			}
		}

	double diff = maxValue - minValue;

	if( minValue >= 0 && diff + minValue <= 1 ) {
		// no need to normalize, as values are already
		// in range
		return;
		}
	
	double scale = 1.0 / diff;

	// now normalize all pixels
	for( c=0; c<numChannels; c++ ) {
		double *channel = inImage->getChannel( c );
		for( int p=0; p<numPixels; p++ ) {
			channel[p] = ( channel[p] - minValue ) * scale;
			}
		}
	}



Image *loadBMPImage( char *inFileName ) {
	File *imageFile = new File( NULL, inFileName,
									  strlen( inFileName ) );
	FileInputStream *imageStream
		= new FileInputStream( imageFile );

	BMPImageConverter *converter = new BMPImageConverter();

	Image *returnImage = converter->deformatImage( imageStream );

	delete imageFile;
	delete imageStream;
	delete converter;

	return returnImage;
	}



void writeBMPImage( char *inFileName, Image *inImage ) {
	File *imageFile = new File( NULL, inFileName,
									  strlen( inFileName ) );
	FileOutputStream *imageStream
		= new FileOutputStream( imageFile );

	BMPImageConverter *converter = new BMPImageConverter();

	converter->formatImage( inImage, imageStream );

	delete imageFile;
	delete imageStream;
	delete converter;
	}



