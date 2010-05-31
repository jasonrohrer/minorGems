/*
 * Modification History
 *
 * 2001-September-24		Jason Rohrer
 * Created.
 */

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ImageColorConverter.h"

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/io/file/FileOutputStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void usage( char *inAppName );
Image *loadTGAImage( char *inFileName );
void writeTGAImage( char *inFileName, Image *inImage );

// normalizes an image into the pixel range [0,1] if some
// pixels are out of range
void normalizeImage( Image *inImage );



// a test program that converts a tga image between
// parameterizable color formats
int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs != 5 ) {
		usage( inArgs[0] );
		}

	Image *inImage = loadTGAImage( inArgs[1] );

	Image *outImage;

	// convert inImage to outImage based on the color space strings
	if( !strcmp( inArgs[3], "rgb" ) ) {
		if( !strcmp( inArgs[4], "yiq" ) ) {
			outImage = ImageColorConverter::RGBToYIQ( inImage );
			}
		if( !strcmp( inArgs[4], "ycbcr" ) ) {
			outImage = ImageColorConverter::RGBToYCbCr( inImage );
			}
		}
	if( !strcmp( inArgs[3], "yiq" ) ) {
		if( !strcmp( inArgs[4], "rgb" ) ) {
			outImage = ImageColorConverter::YIQToRGB( inImage );
			}
		if( !strcmp( inArgs[4], "ycbcr" ) ) {
			Image *tempImage = ImageColorConverter::YIQToRGB( inImage );
			
			outImage = ImageColorConverter::RGBToYCbCr( tempImage );

			delete tempImage;
			}
		}
	if( !strcmp( inArgs[3], "ycbcr" ) ) {
		if( !strcmp( inArgs[4], "rgb" ) ) {
			outImage = ImageColorConverter::YCbCrToRGB( inImage );
			}
		if( !strcmp( inArgs[4], "yiq" ) ) {
			Image *tempImage = ImageColorConverter::YCbCrToRGB( inImage );
			
			outImage = ImageColorConverter::RGBToYIQ( tempImage );

			delete tempImage;
			}
		}

	writeTGAImage( inArgs[2], outImage );

	delete inImage;
	delete outImage;
	
	return 0;
	}


void usage( char *inAppName ) {
	printf( "Usage:\n" );
	printf( "\t%s in_image_tga out_image_tga in_format out_format\n",
			inAppName );
	printf( "Example:\n" );
	printf( "\t%s testRGB.tga testYIQ.bmp rgb yiq\n", inAppName );

	printf( "The following color formats are supported:\n" );
	printf( "\trgb, yiq, ycbcr\n" );
	
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



Image *loadTGAImage( char *inFileName ) {
	File *imageFile = new File( NULL, inFileName,
									  strlen( inFileName ) );
	FileInputStream *imageStream
		= new FileInputStream( imageFile );

	TGAImageConverter *converter = new TGAImageConverter();

	Image *returnImage = converter->deformatImage( imageStream );

	delete imageFile;
	delete imageStream;
	delete converter;

	return returnImage;
	}



void writeTGAImage( char *inFileName, Image *inImage ) {
	File *imageFile = new File( NULL, inFileName,
									  strlen( inFileName ) );
	FileOutputStream *imageStream
		= new FileOutputStream( imageFile );

	TGAImageConverter *converter = new TGAImageConverter();

	converter->formatImage( inImage, imageStream );

	delete imageFile;
	delete imageStream;
	delete converter;
	}



