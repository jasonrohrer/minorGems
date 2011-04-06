/*
 * Modification History
 *
 * 2001-February-26		Jason Rohrer
 * Created. 
 * Added functions for converting to and from gray byte arrays.
 *
 * 2001-September-19		Jason Rohrer
 * Added an RGB->HSB conversion function, which was copied
 * from minorGems/ai/robotics/ImageStatistics.
 * Added RGB<->YIQ functions.
 *
 * 2001-September-20		Jason Rohrer
 * Fixed a bug in the YIQ conversion.
 * Got rid of this bug fix, as it distorts the YIQ space,
 * and there is no way to prevent colors from going out of the
 * [0,1] range all of the time anyway.
 *
 * 2001-September-24		Jason Rohrer
 * Added RGB<->YCbCr functions.
 * Abstracted out a common coefficient multiplying function.
 *
 * 2011-April-5     Jason Rohrer
 * Fixed float-to-int conversion.  
 */

#ifndef IMAGE_COLOR_CONVERTER_INCLUDED
#define IMAGE_COLOR_CONVERTER_INCLUDED

#include <math.h>


#include "Image.h"


/**
 * A container class for static functions that convert
 * images between various color spaces.
 *
 * @author Jason Rohrer
 */ 
class ImageColorConverter {
	
	public:
		
		/**
		 * Converts a 3-channel RGB image to a 1-channel grayscale
		 * image using an NTSC luminosity standard.
		 *
		 * @param inImage the RGB image to convert.  Must be destroyed
		 *   by caller.
		 *
		 * @return a new, grayscale version of inImage.  Must be
		 *   destroyed by the caller.  Returns NULL if inImage
		 *   is not a 3-channel image.
		 */
		static Image *RGBToGrayscale( Image *inImage );
		
		/**
		 * Converts a 1-channel grayscae image to a 3-channel RGB
		 * image.
		 *
		 * @param inImage the grayscale image to convert.  Must be destroyed
		 *   by caller.
		 *
		 * @return a new, RGB version of inImage.  Must be
		 *   destroyed by the caller.  Returns NULL if inImage
		 *   is not a 1-channel image.
		 */
		static Image *grayscaleToRGB( Image *inImage );
		

		/**
		 * Converts a 1-channel grayscae image to a 1-channel byte array.
		 *
		 * @param inImage the grayscale image to convert.  Must be destroyed
		 *   by caller.
		 * @param inChannelNumber the channel number to use as the 
		 *   gray channel.  Defaults to 0;
		 *
		 * @return a new byte array with one byte per image pixel,
		 *   and image [0,1] values mapped to [0,255].
		 */
		static unsigned char *grayscaleToByteArray( Image *inImage,
			int inChannelNumber = 0 );
		
		/**
		 * Converts a byte array to a 1-channel grayscale
		 * image.
		 *
		 * @param inBytes the byte array to convert.  Must be destroyed
		 *   by caller.
		 * @param inWidth the width of the image contained in the byte array.
		 * @param inHeight the height of the image contained in the byte array.
		 *
		 * @return a new, grayscale image version of the byte array.  Must be
		 *   destroyed by the caller.
		 */
		static Image *byteArrayToGrayscale( unsigned char *inBytes,
			int inWidth, int inHeight );


		
		/**
		 * Converts an RGB image to HSB.
		 *
		 * @param inRGBImage the rgb image to convert.
		 *   Must be destroyed by caller.
		 *
		 * @return a new image that is the HSB conversion of the
		 *   RGB image.  Must be destroyed by caller.
		 */
		static Image *RGBToHSB( Image *inRGBImage );



		/**
		 * Converts an RGB image to YIQ.
		 *
		 * Note that color values in the resulting YIQ
		 * image may lie outside of the range [0,1].
		 *
		 * @param inRGBImage the rgb image to convert.
		 *   Must be destroyed by caller.
		 *
		 * @return a new image that is the YIQ conversion of the
		 *   RGB image.  Must be destroyed by caller.
		 */
		static Image *RGBToYIQ( Image *inRGBImage );



		/**
		 * Converts a YIQ image to RGB.
		 *
		 *
		 * Note that color values in the resulting RGB
		 * image may lie outside of the range [0,1].
		 *
		 * @param inYIQImage the rgb image to convert.
		 *   Must be destroyed by caller.
		 *
		 * @return a new image that is the RGB conversion of the
		 *   YIQ image.  Must be destroyed by caller.
		 */
		static Image *YIQToRGB( Image *inYIQImage );



		/**
		 * Converts an RGB image to YCbCr.
		 *
		 * Note that in the YCbCr standard, Y is in the range
		 * [0,1], while Cb and Cr are both in the range [-0.5, 0.5].
		 * This function returns Cb and Cr components shifted
		 * into the range [0,1].
		 *
		 * @param inRGBImage the rgb image to convert.
		 *   Must be destroyed by caller.
		 *
		 * @return a new image that is the YCbCr conversion of the
		 *   RGB image.  Must be destroyed by caller.
		 */
		static Image *RGBToYCbCr( Image *inRGBImage );



		/**
		 * Converts a YCbCr image to RGB.
		 *
		 *
		 * Note that in the YCbCr standard, Y is in the range
		 * [0,1], while Cb and Cr are both in the range [-0.5, 0.5].
		 * This function expects input Cb and Cr components to be shifted
		 * into the range [0,1].
		 *
		 * @param inYCbCrImage the rgb image to convert.
		 *   Must be destroyed by caller.
		 *
		 * @return a new image that is the RGB conversion of the
		 *   YCbCr image.  Must be destroyed by caller.
		 */
		static Image *YCbCrToRGB( Image *inYCbCrImage );


		
	protected:

		/**
		 * Converts an 3-channel image to another 3-channel
		 * image using a matrix of conversion coefficients.
		 *
		 * The following formulae are used;
		 * outChan0 = inC00 * inChan0 + inC01 * inChan1 + inC02 * inChan2
		 * outChan1 = inC10 * inChan0 + inC11 * inChan1 + inC12 * inChan2
		 * outChan2 = inC20 * inChan0 + inC21 * inChan1 + inC22 * inChan2
		 *
		 * @param inImage the image to convert.
		 *   Must be destroyed by caller.
		 *
		 * @return a new image that is inImage converted, or NULL if
		 *   conversion failed (usually because inImage does not
		 *   contain 3 channels).
		 *   Must be destroyed by caller.
		 */
		static Image *coefficientConvert( Image *inImage,
										  double inC00,
										  double inC01,
										  double inC02,
										  double inC10,
										  double inC11,
										  double inC12,
										  double inC20,
										  double inC21,
										  double inC22 );
											 
	};		



inline Image *ImageColorConverter::RGBToGrayscale( Image *inImage ) {
	int w = inImage->getWidth();
	int h = inImage->getHeight();
	int numPixels = w * h;
	
	Image *grayImage = new Image( w, h, 1 );
	
	double *red = inImage->getChannel( 0 );
	double *green = inImage->getChannel( 1 );
	double *blue = inImage->getChannel( 2 );
	
	double *gray = grayImage->getChannel( 0 );
	
	for( int i=0; i<numPixels; i++ ) {
		// NTSC luminosity formula
		gray[i] = .299 * red[i] + .587 * green[i] + .114 * blue[i];
		}
		
	return grayImage;
	}
		
		

inline Image *ImageColorConverter::grayscaleToRGB( Image *inImage ) {
	int w = inImage->getWidth();
	int h = inImage->getHeight();
	int numPixels = w * h;
	
	Image *rgbImage = new Image( w, h, 3 );
	
	double *red = rgbImage->getChannel( 0 );
	double *green = rgbImage->getChannel( 1 );
	double *blue = rgbImage->getChannel( 2 );
	
	double *gray = inImage->getChannel( 0 );
	
	for( int i=0; i<numPixels; i++ ) {
		red[i] = gray[i];
		green[i] = gray[i];
		blue[i] = gray[i];
		}
		
	return rgbImage;
	}


inline unsigned char *ImageColorConverter::
	grayscaleToByteArray( Image *inImage, int inChannelNumber ) {
	
	int w = inImage->getWidth();
	int h = inImage->getHeight();
	int numPixels = w * h;
	
	unsigned char *bytes = new unsigned char[ numPixels ];
	
	double *gray = inImage->getChannel( inChannelNumber );
	
	for( int i=0; i<numPixels; i++ ) {
		bytes[i] = (unsigned char)( lrint( 255 * gray[i] ) );
		}
		
	return bytes;
	}



inline Image *ImageColorConverter::
	byteArrayToGrayscale( unsigned char *inBytes,
	int inWidth, int inHeight ) {
	
	int w = inWidth;
	int h = inHeight;
	int numPixels = w * h;
	
	Image *grayImage = new Image( w, h, 1 );
	
	
	double *gray = grayImage->getChannel( 0 );
	
	for( int i=0; i<numPixels; i++ ) {
		gray[i] = (double)( inBytes[i] ) / 255.0;
		}
		
	return grayImage;
	}



inline Image *ImageColorConverter::RGBToHSB( Image *inRGBImage ) {
	// idea modeled after Java Color class.
	
	if( inRGBImage->getNumChannels() != 3 ) {
		printf(
			"RGBtoHSB requires a 3-channel image as input.\n" );
		return NULL;
		}

	int w = inRGBImage->getWidth();
	int h = inRGBImage->getHeight();

	int numPixels = w * h;
				
	Image *hsbImage = new Image( w, h, 3 );

	double *redChannel = inRGBImage->getChannel( 0 );
	double *greenChannel = inRGBImage->getChannel( 1 );
	double *blueChannel = inRGBImage->getChannel( 2 );

	double *hueChannel = hsbImage->getChannel( 0 );
	double *satChannel = hsbImage->getChannel( 1 );
	double *brightChannel = hsbImage->getChannel( 2 );

				
	for( int i=0; i<numPixels; i++ ) {
		int r = (int)( lrint( 255 * redChannel[i] ) );
		int g = (int)( lrint( 255 * greenChannel[i] ) );
		int b = (int)( lrint( 255 * blueChannel[i] ) );

		double hue, sat, bright;
				
		int cmax = (r > g) ? r : g;
		if (b > cmax) {
			cmax = b;
			}

		int cmin = (r < g) ? r : g;
		if (b < cmin) {
			cmin = b;
			}
								
		bright = ( (double)cmax ) / 255.0;
		if( cmax != 0 ) {
			sat = ( (double)( cmax - cmin ) ) / ( (double) cmax );
			}
		else {
			sat = 0;
			}
		if( sat == 0 ) {
			hue = 0;
			}
		else {
			double redc =
				( (double)( cmax - r ) ) / ( (double)( cmax - cmin ) );
			double greenc =
				( (double) ( cmax - g ) ) / ( (double)( cmax - cmin ) );
			double bluec =
				( (double)( cmax - b ) ) / ( (double)( cmax - cmin ) );

			if( r == cmax ) {
				hue = bluec - greenc;
				}
			else if( g == cmax ) {
				hue = 2.0 + redc - bluec;
				}
			else {
				hue = 4.0 + greenc - redc;
				}
			hue = hue / 6.0;
												
			if( hue < 0 ) {
				hue = hue + 1.0;
				}
			}

		hueChannel[i] = hue;
		satChannel[i] = sat;
		brightChannel[i] = bright;
		}

	return hsbImage;
	}



inline Image *ImageColorConverter::RGBToYIQ( Image *inRGBImage ) {
	if( inRGBImage->getNumChannels() != 3 ) {
		printf(
			"RGBtoYIQ requires a 3-channel image as input.\n" );
		return NULL;
		}

	Image *yiqImage = coefficientConvert( inRGBImage,
										  0.299,  0.587,  0.114,
										  0.596, -0.274, -0.322,
										  0.212, -0.523,  0.311 );

	return yiqImage;
	}



inline Image *ImageColorConverter::YIQToRGB( Image *inYIQImage ) {
	if( inYIQImage->getNumChannels() != 3 ) {
		printf(
			"YIQtoRGB requires a 3-channel image as input.\n" );
		return NULL;
		}

	Image *rgbImage = coefficientConvert( inYIQImage,
										  1.0,  0.956,  0.621,
										  1.0, -0.272, -0.647,
										  1.0, -1.105,  1.702 );
	return rgbImage;
	}



inline Image *ImageColorConverter::RGBToYCbCr( Image *inRGBImage ) {
	if( inRGBImage->getNumChannels() != 3 ) {
		printf(
			"RGBtoYCbCr requires a 3-channel image as input.\n" );
		return NULL;
		}

	// coefficients taken from the color space faq
	/*
	  RGB -> YCbCr (with Rec 601-1 specs)
	  Y  =  0.2989 * Red + 0.5866 * Green + 0.1145 * Blue
	  Cb = -0.1687 * Red - 0.3312 * Green + 0.5000 * Blue
	  Cr =  0.5000 * Red - 0.4183 * Green - 0.0816 * Blue

	  YCbCr (with Rec 601-1 specs) -> RGB
	  Red   = Y + 0.0000 * Cb + 1.4022 * Cr
	  Green = Y - 0.3456 * Cb - 0.7145 * Cr
	  Blue  = Y + 1.7710 * Cb + 0.0000 * Cr
	*/
	
	Image *ycbcrImage = coefficientConvert( inRGBImage,
											 0.2989,  0.5866,  0.1145,
											-0.1687, -0.3312,  0.5000,
											 0.5000, -0.4183, -0.0816 );

	// adjust the Cb and Cr channels so they are in the range [0,1]
	int numPixels = ycbcrImage->getWidth() * ycbcrImage->getHeight();
	double *cbChannel = ycbcrImage->getChannel( 1 );
	double *crChannel = ycbcrImage->getChannel( 2 );
	for( int i=0; i<numPixels; i++ ) {
		cbChannel[i] += 0.5;
		crChannel[i] += 0.5;
		}

	// no need to clip pixels to the range [0,1], since
	// all possible rgb pixel values are represented by in-range
	// yCbCr pixel values

	
	return ycbcrImage;
	}



inline Image *ImageColorConverter::YCbCrToRGB( Image *inYCbCrImage ) {
	if( inYCbCrImage->getNumChannels() != 3 ) {
		printf(
			"YCbCrtoRGB requires a 3-channel image as input.\n" );
		return NULL;
		}

	// adjust the normalized Cb and Cr channels
	// so they are in the range [-0.5,0.5]
	
	int numPixels = inYCbCrImage->getWidth() * inYCbCrImage->getHeight();
	double *cbChannel = inYCbCrImage->getChannel( 1 );
	double *crChannel = inYCbCrImage->getChannel( 2 );
	for( int i=0; i<numPixels; i++ ) {
		cbChannel[i] -= 0.5;
		crChannel[i] -= 0.5;
		}

	// coefficients taken from the color space faq
	/*
	  RGB -> YCbCr (with Rec 601-1 specs)
	  Y  =  0.2989 * Red + 0.5866 * Green + 0.1145 * Blue
	  Cb = -0.1687 * Red - 0.3312 * Green + 0.5000 * Blue
	  Cr =  0.5000 * Red - 0.4183 * Green - 0.0816 * Blue

	  YCbCr (with Rec 601-1 specs) -> RGB
	  Red   = Y + 0.0000 * Cb + 1.4022 * Cr
	  Green = Y - 0.3456 * Cb - 0.7145 * Cr
	  Blue  = Y + 1.7710 * Cb + 0.0000 * Cr
	*/
	
	Image *rgbImage = coefficientConvert( inYCbCrImage,
										  1.0,  0.0000,  1.4022,
										  1.0, -0.3456, -0.7145,
										  1.0,  1.7710,  0.0000 );

	// clip r, g, and b channels to the range [0,1], since
	// some YCbCr pixel values might map out of this range
	// (in other words, some YCbCr values map outside of rgb space)
	for( int c=0; c<3; c++ ) {
		double *channel = rgbImage->getChannel( c );
		
		for( int p=0; p<numPixels; p++ ) {
			if( channel[p] < 0 ) {
				channel[p] = 0;
				}
			else if( channel[p] > 1 ) {
				channel[p] = 1.0;
				}
			}
		}
	
	return rgbImage;
	}



inline Image *ImageColorConverter::coefficientConvert( Image *inImage,
													   double inC00,
													   double inC01,
													   double inC02,
													   double inC10,
													   double inC11,
													   double inC12,
													   double inC20,
													   double inC21,
													   double inC22 ) {
	if( inImage->getNumChannels() != 3 ) {
		return NULL;
		}

	
	int w = inImage->getWidth();
	int h = inImage->getHeight();

	int numPixels = w * h;
				
	Image *outImage = new Image( w, h, 3 );

	double *outChannel0 = outImage->getChannel( 0 );
	double *outChannel1 = outImage->getChannel( 1 );
	double *outChannel2 = outImage->getChannel( 2 );

	double *inChannel0 = inImage->getChannel( 0 );
	double *inChannel1 = inImage->getChannel( 1 );
	double *inChannel2 = inImage->getChannel( 2 );
	
	for( int i=0; i<numPixels; i++ ) {
		outChannel0[i] =
			inC00 * inChannel0[i] +
			inC01 * inChannel1[i] +
			inC02 * inChannel2[i];
		outChannel1[i] =
			inC10 * inChannel0[i] +
			inC11 * inChannel1[i] +
			inC12 * inChannel2[i];
		outChannel2[i] =
			inC20 * inChannel0[i] +
			inC21 * inChannel1[i] +
			inC22 * inChannel2[i];
		}

	return outImage;
	}



#endif
