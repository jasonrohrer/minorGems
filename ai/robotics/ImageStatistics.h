/*
 * Modification History
 *
 * 2001-February-27							Jason Rohrer
 * Created.
 * Fixed some bugs.
 *
 * 2001-May-12   Jason Rohrer
 * Added functions for dealing with pixel color.
 * Fixed a bug with float to double conversion.
 *
 * 2001-May-13   Jason Rohrer
 * Added a function for finding the average color in a region.
 *
 * 2001-May-14   Jason Rohrer
 * Added an HSB conversion function.
 * Added functions for dealing with single channels that
 * parallel those dealing with 3 channel RGB.
 * Fixed a bug in the image sizes.
 *
 * 2001-May-??   Jeremy Tavan
 * Added difference map function.
 * 
 * 2001-May-15   Jeremy Tavan
 * Added threshold map function and modified 
 * centerOfValue and averageValue to use it.
 * Also modified fractionNearValue similarly.
 */
 
 
#ifndef IMAGE_STATISTICS_INCLUDED
#define IMAGE_STATISTICS_INCLUDED 

#include "ImageRectangle.h"
#include "ImageUtilities.h"
#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/Color.h"

/**
 * Container class for static image statistics functions.
 *
 * @author Jason Rohrer
 */ 
class ImageStatistics {
				
 public:
								
  /**
   * Computes the fraction of pixels in an image region that have
   * values over a specified threshold.
   *
   * All parameters must be destroyed by caller.
   *
   * @param inImage the image to analyze.
   * @param inChannel the channel of inImage to analyze.
   * @param inRectangle the rectangle in inImage to analyze.
   * @param inThreshold the threshold value, in [0,1].
   *
   * @return the fraction of pixels in the region that have values
   *   larger than inThreshold.  In [0,1].
   */
  static double fractionOverThreshold(
				      Image *inImage, int inChannel,
				      ImageRectangle *inRectangle, double inThreshold );
								
								
								
  /**
   * Computes the fraction of pixels in an image region that have
   * values under a specified threshold.
   *
   * All parameters must be destroyed by caller.
   *
   * @param inImage the image to analyze.
   * @param inChannel the channel of inImage to analyze.
   * @param inRectangle the rectangle in inImage to analyze.
   * @param inThreshold the threshold value, in [0,1].
   *
   * @return the fraction of pixels in the region that have values
   *   smaller than inThreshold.  In [0,1].
   */
  static double fractionUnderThreshold(
				       Image *inImage, int inChannel,
				       ImageRectangle *inRectangle, double inThreshold );
								
  /**
   * Computes a difference map between two images of the same
   * size.  Stores the absolute difference, pixel by pixel, 
   * in a new image. 
   *
   * All parameters must be destroyed by caller.
   *
   * @param inImageOne  one of the two images to compare
   * @param inImageTwo  the other image to compare
   * @param outImage    the image to write with the difference map
   * 
   * @return the average distance over the entire image, or -1 if the two
   *         input images are different sizes.
   */
  static double differenceMap(
			      Image *inImageOne, Image *inImageTwo,
			      Image *outImage );

				
  /**
   * Computes a threshold map of an image.  The map contains
   * 1.0 when the corresponding pixels in the image are above
   * the threshold value, and 0.0 otherwise.
   *
   * All parameters must be restroyed by caller.
   *
   * @param inImage    the image to map
   * @param threshold  the mapping threshold
   * @param outImage   the image to write with the threshold map
   *
   * @return the number of pixels above the threshold
   */
  static long thresholdMap(
			   Image *inImage, double threshold,
			   int channelNumber, Image *outImage );			

  /**
   * Computes the fraction of image pixels that
   * are "near" a target color.
   *
   * @param inThreeChannelImage the image to analyze.
   *   Must have 3 channels.  Must be destroyed by caller.
   * @param inColor the color to count in the image.
   *   Must be destroyed by caller.
   * @param inTolerance the amount a pixel can vary
   *   in each color component and still be counted
   *   as being "near" the target color.  In [0,1].
   *   For example, if tolerance = 0.05, and 
   *   inColor = (0.5, 0.0, 1.0 ), then a pixel of
   *   color (0.55, 0.05, 0.95) would count as part
   *   "near" inColor.
   *
   * @return the fraction of image pixels that are
   *   "near" the target color, in [0,1].
   */
  static double fractionNearColor(
				  Image *inThreeChannelImage,
				  Color *inColor,
				  double inTolerance );


  /**
   * Computes the weighted center coordinate
   * of pixels that are "near" a target color.
   *
   * @param inThreeChannelImage the image to analyze.
   *   Must have 3 channels.  Must be destroyed by caller.
   * @param inColor the color to count in the image.
   *   Must be destroyed by caller.
   * @param inTolerance the amount a pixel can vary
   *   in each color component and still be counted
   *   as being "near" the target color.  In [0,1].
   *   For example, if tolerance = 0.05, and 
   *   inColor = (0.5, 0.0, 1.0 ), then a pixel of
   *   color (0.55, 0.05, 0.95) would count as part
   *   "near" inColor.
   * @param outCenterX a pointer to a double
   *   where the center's x coordinate will be returned.
   *   The returned value is in [0,1].
   * @param outCenterY a pointer to a double
   *   where the center's y coordinate will be returned.
   *   The returned value is in [0,1].
   */
  static void centerOfColor(
			    Image *inThreeChannelImage,
			    Color *inColor,
			    double inTolerance,
			    double *outCenterX, double *outCenterY );


  /**
   * Computes the fraction of image pixels that
   * are "near" a target value.
   *
   * @param inImage the image to analyze.
   *   Must be destroyed by caller.
   * @param inChannel the channel to analyze.
   * @param inValue the value to count in the image.
   * @param inTolerance the amount a pixel can vary
   *   in each color component and still be counted
   *   as being "near" the target color.  In [0,1].
   *   For example, if tolerance = 0.05, and 
   *   inColor = (0.5, 0.0, 1.0 ), then a pixel of
   *   color (0.55, 0.05, 0.95) would count as part
   *   "near" inColor.
   * @param ignoreMask Mask of pixels to ignore in 
   *   calculations.
   *
   * @return the fraction of image pixels that are
   *   "near" the target value, in [0,1].
   */
  static double fractionNearValue(
				  Image *inImage,
				  int inChannel,
				  double inValue,
				  double inTolerance,
				  Image *ignoreMask);


  /**
   * Computes the weighted center coordinate
   * of pixels that are "near" a target value.
   *
   * @param inImage the image to analyze.
   *   Must be destroyed by caller.
   * @param inChannel the channel to analyze.
   * @param inValue the value to count in the image.
   * @param inTolerance the amount a pixel can vary
   *   in each color component and still be counted
   *   as being "near" the target color.  In [0,1].
   *   For example, if tolerance = 0.05, and 
   *   inColor = (0.5, 0.0, 1.0 ), then a pixel of
   *   color (0.55, 0.05, 0.95) would count as part
   *   "near" inColor.
   * @param outCenterX a pointer to a double
   *   where the center's x coordinate will be returned.
   *   The returned value is in [0,1].
   * @param outCenterY a pointer to a double
   *   where the center's y coordinate will be returned.
   *   The returned value is in [0,1].
   * @param ignoreMask Mask of pixels to include in 
   *   calculations.
   */
  static void centerOfValue(
			    Image *inImage,
			    int inChannel,
			    double inValue,
			    double inTolerance,
			    double *outCenterX, double *outCenterY,
			    Image *ignoreMask);


								
  /**
   * Computes the average color in an image region.
   *
   * @param inThreeChannelImage the image to analyze.
   *   Must have 3 channels.  Must be destroyed by caller.
   * @param inRectangle the region to analyze.
   *   Must be destroyed by caller.
   *
   * @return the average color, or NULL on an error.
   *   Must be destroyed by caller.
   */
  static Color *averageColor( Image *inThreeChannelImage,
			      ImageRectangle *inRectangle );

								
  /**
   * Computes the average value in an image region.
   *
   * @param inImage the image to analyze.
   *   Must be destroyed by caller.
   * @param inChannel the channel to analyze.
   * @param inRectangle the region to analyze.
   *   Must be destroyed by caller.
   * @param ignoreMask mask of points to include
   *   in calculations
   *
   * @return the average value.
   */
  static double averageValue( Image *inImage,
			      int inChannel,
			      ImageRectangle *inRectangle,
			      Image *ignoreMask);
								

  /**
   * Converts an RGB image to HSB.
   *
   * @param inRGBImage the rgb image to convert.
   *   Must be destroyed by caller.
   *
   * @return a new image that is the HSB conversion of the
   *   RGB image.  Must be destroyed by caller.
   */
  static Image *ImageStatistics::RGBtoHSB( Image *inRGBImage );
								
};
				
				
				
inline double ImageStatistics::fractionOverThreshold(
						     Image *inImage, int inChannel,
						     ImageRectangle *inRectangle, double inThreshold ) {
				
  int w = inImage->getWidth();
  int h = inImage->getHeight();
				
  int xStart = (int)( w * inRectangle->mXStart );
  int xEnd = (int)( ( w * inRectangle->mXEnd ) - 1 );
				
  int yStart = (int)( h * inRectangle->mYStart );
  int yEnd = (int)( ( h * inRectangle->mYEnd ) - 1 );
				
  long numPixels = ( ( xEnd - xStart ) + 1 ) * ( ( yEnd - yStart ) + 1 );
				
  double *channel = inImage->getChannel( inChannel );
				
  long sum = 0;
  for( int y=yStart; y<=yEnd; y++ ) {
    for( int x=xStart; x<=xEnd; x++ ) {
      int index = y * w + x;
      if( channel[index] > inThreshold ) {
	sum++;
      }
    }
  }
				
  return (double)sum / (double)numPixels;
}



inline double ImageStatistics::differenceMap( 
					     Image *inImageOne, Image *inImageTwo,
					     Image *outImage ) {
  
  int numChannels = inImageOne->getNumChannels();
  int w = inImageOne->getWidth();
  int h = inImageOne->getHeight();
  int sizesMatch = 1;
  double *channelOne, *channelTwo, *channelOut;
  double sum = 0;
  long numPixels = w * h * numChannels;
  printf("differenceMap operating on %d pixels.", numPixels);
  // Perform basic size checking.  The difference map isn't valid if the 
  // input images don't match in size.
  if (!(w == inImageTwo->getWidth()) || !(h == inImageTwo->getHeight()) || 
      !(numChannels == inImageTwo->getNumChannels()))
    sizesMatch = 0;
  
  if (sizesMatch > 0) {
    for (int chan = 0; chan < numChannels; chan++) {
      channelOne = inImageOne->getChannel(chan);
      channelTwo = inImageTwo->getChannel(chan);
      channelOut = outImage->getChannel(chan);
      for ( int y = 0; y < h; y++ )
	for ( int x = 0; x < w; x++) {
	  int index = y * w + x;
	  // 0<= channelOne[index] <= 1?
          double diff = fabs(channelOne[index] - channelTwo[index]);
	  sum += diff;
	  channelOut[index] = diff;
	}
    }
    return sum / (double)numPixels;
  } else {
    printf("Sizes of images don't match!");
    return 0;
  }
}

inline long ImageStatistics::thresholdMap(
					  Image *inImage, double threshold,
					  int channelNumber, Image *outImage ) {
  int numChannels = inImage->getNumChannels();
  int w = inImage->getWidth();
  int h = inImage->getHeight();
  double *channel = inImage->getChannel(channelNumber);
  double *channelOut = outImage->getChannel(0);
  long numOverThreshold = 0;
  int sizesMatch = 1;
  if ( (w != outImage->getWidth()) || (h != inImage->getHeight()) )
    sizesMatch = 0;
	
  if (sizesMatch > 0) {
    for ( int y = 0; y < h; y++ ) {
      for ( int x = 0; x < w; x++ ) {
	int index = y * w + x;
	if ( channel[index] >= threshold ) {
	  channelOut[index] = 1.0;
	  numOverThreshold++;
	}
	else
	  channelOut[index] = 0.0;
      }
    }
    return numOverThreshold;
  } else {
    printf("Size of output image doesn't equal size of input image!\n");
    return 0;
  }
}



inline double ImageStatistics::fractionUnderThreshold(
						      Image *inImage, int inChannel,
						      ImageRectangle *inRectangle, double inThreshold ) {
				
				
				// this is just the opposite of fractionOver
  return 1 - fractionOverThreshold( inImage, inChannel, 
				    inRectangle, inThreshold );
}				



inline double ImageStatistics::fractionNearColor(
						 Image *inThreeChannelImage,
						 Color *inColor,
						 double inTolerance ) {

  Image *inImage = inThreeChannelImage;

  if( inImage->getNumChannels() != 3 ) {
    printf(
	   "fractionNearColor requires a 3-channel image as input.\n" );
    return 0;
  }
				
				
  int w = inImage->getWidth();
  int h = inImage->getHeight();

  int numPixels = w * h;

  int numMatchingPixels = 0;

  double *redChannel = inImage->getChannel(0);
  double *greenChannel = inImage->getChannel(1);
  double *blueChannel = inImage->getChannel(2);

  double red = (double)( inColor->r );
  double green = (double)( inColor->g );
  double blue = (double)( inColor->b );
				
  for( int i=0; i<numPixels; i++ ) {
    // if pixel is in color range
    if( fabs( redChannel[i] - red ) <= inTolerance &&
	fabs( greenChannel[i] - green ) <= inTolerance &&
	fabs( blueChannel[i] - blue ) <= inTolerance ) {

      // count it
      numMatchingPixels++;
    }

  }
				
				// return the fraction of pixels in range
  return (double)numMatchingPixels / (double)numPixels;
}



inline void ImageStatistics::centerOfColor(
					   Image *inThreeChannelImage,
					   Color *inColor,
					   double inTolerance,
					   double *outCenterX, double *outCenterY ) {

  Image *inImage = inThreeChannelImage;

  if( inImage->getNumChannels() != 3 ) {
    printf(
	   "centerOfColor requires a 3-channel image as input.\n" );
    return;
  }
				
				
  int w = inImage->getWidth();
  int h = inImage->getHeight();

  int numPixels = w * h;

  int numMatchingPixels = 0;

  double centerX, centerY;
  centerX = 0;
  centerY = 0;
				
  double *redChannel = inImage->getChannel(0);
  double *greenChannel = inImage->getChannel(1);
  double *blueChannel = inImage->getChannel(2);

  double red = (double)( inColor->r );
  double green = (double)( inColor->g );
  double blue = (double)( inColor->b );


  for( int y=0; y<h; y++ ) {
    for( int x=0; x<w; x++ ) {
      int i = y * w + x;

      // if the pixel is in range
      if( fabs( redChannel[i] - red ) <= inTolerance &&
	  fabs( greenChannel[i] - green ) <= inTolerance &&
	  fabs( blueChannel[i] - blue ) <= inTolerance ) {

	// count it
	numMatchingPixels++;

	// add the coordinates into our running sum
	centerX += x;
	centerY += y;
      }

    }
  }

				// finish taking the average
  centerX = centerX / (double)numMatchingPixels;
  centerY = centerY / (double)numMatchingPixels;  

				// scale into [0,1]
  centerX = centerX / (double)w;
  centerY = centerY / (double)h;

				// our return values
  *outCenterX = centerX;
  *outCenterY = centerY;
}



inline double ImageStatistics::fractionNearValue(
						 Image *inImage,
						 int inChannel,
						 double inValue,
						 double inTolerance,
						 Image *ignoreMask) {
		
  int w = inImage->getWidth();
  int h = inImage->getHeight();

  int numPixels = w * h;
  int numCountedPixels = 0;

  int numMatchingPixels = 0;

  double *channel = inImage->getChannel( inChannel );
  double *ignoreChannel = ignoreMask->getChannel( 0 );


  Image *matchImage = new Image( w, h, 1 );
  double *matchChannel = matchImage->getChannel( 0 );
				
  for( int i=0; i<numPixels; i++ ) {
    if ( ignoreChannel[i] > 0.5 ) {
		
      numCountedPixels++;
      // if pixel is in color range
	  
      if( fabs( channel[i] - inValue ) <= inTolerance ) {
				  
		  // count it
		  numMatchingPixels++;
		  
		  // add it to our match image
		  matchChannel[i] = 1;
		  }
		}
	  }

  char *matchFileName = "nearPixels";
  ImageUtilities::imageToBMPFile( matchImage,
				  matchFileName );

  delete matchImage;

				
				// return the fraction of pixels in range
  return (double)numMatchingPixels / (double)numPixels;
}



inline void ImageStatistics::centerOfValue(
					   Image *inImage,
					   int inChannel,
					   double inValue,
					   double inTolerance,
					   double *outCenterX, double *outCenterY,
					   Image *ignoreMask ) {


  int w = inImage->getWidth();
  int h = inImage->getHeight();

  int numPixels = w * h;

  int numMatchingPixels = 0;

  double centerX, centerY;
  centerX = 0;
  centerY = 0;
				
  double *channel = inImage->getChannel( inChannel );
  double *ignoreChannel = ignoreMask->getChannel( 0 );
				
  for( int y=0; y<h; y++ ) {
    for( int x=0; x<w; x++ ) {
      int i = y * w + x;
      // if this pixel should not be ignored
      if ( ignoreChannel[i] > 0.5 ) {
	// if the pixel is in range
	if( fabs( channel[i] - inValue ) <= inTolerance ) {
	  
	  // count it
	  numMatchingPixels++;
	  
	  // add the coordinates into our running sum
	  centerX += x;
	  centerY += y;
	}
      }
    }
  }

				// finish taking the average
  centerX = centerX / (double)numMatchingPixels;
  centerY = centerY / (double)numMatchingPixels;  

				// scale into [0,1]
  centerX = centerX / (double)w;
  centerY = centerY / (double)h;

				// our return values
  *outCenterX = centerX;
  *outCenterY = centerY;
}



inline Color *ImageStatistics::averageColor(
					    Image *inThreeChannelImage,
					    ImageRectangle *inRectangle ) {

  Image *inImage = inThreeChannelImage;
  
	  
  if( inImage->getNumChannels() != 3 ) {
    printf(
	   "averageColor requires a 3-channel image as input.\n" );
    return NULL;
  }

  Image *allOnesMask = new Image( inImage->getWidth(),
								  inImage->getHeight(),
								  1);
  double *maskChannel = allOnesMask->getChannel(0);
	  
  int numPixels = inImage->getWidth() * inImage->getHeight();
  for (int i = 0; i < numPixels; i++) {
	  maskChannel[i] = 1;
	  }
  
  double redAverage = averageValue( inImage, 0, inRectangle, allOnesMask );
  double greenAverage = averageValue( inImage, 1, inRectangle, allOnesMask );
  double blueAverage = averageValue( inImage, 2, inRectangle, allOnesMask);
				

  Color *returnColor = new Color( (float)redAverage,
				  (float)greenAverage,
				  (float)blueAverage );

  delete allOnesMask;
  
  return returnColor;
}



inline double ImageStatistics::averageValue(
					    Image *inImage,
					    int inChannel,
					    ImageRectangle *inRectangle,
					    Image *ignoreMask) {

				
  int w = inImage->getWidth();
  int h = inImage->getHeight();
				
  int xStart = (int)( w * inRectangle->mXStart );
  int xEnd = (int)( ( w * inRectangle->mXEnd ) - 1 );
				
  int yStart = (int)( h * inRectangle->mYStart );
  int yEnd = (int)( ( h * inRectangle->mYEnd ) - 1 );
				
  long numPixels = 0;
				
  double *channel = inImage->getChannel( inChannel );
  double *ignoreChannel = ignoreMask->getChannel( 0 );
  double sum = 0;
				
  for( int y=yStart; y<=yEnd; y++ ) {
    for( int x=xStart; x<=xEnd; x++ ) {
      int index = y * w + x;
      if ( ignoreChannel[index] > 0.5 ) { 
		  numPixels++;
		  sum += channel[index];
		  }
		}
	  }

				// finish taking the average
  double returnValue = sum / (double)numPixels;

  return returnValue;
	}



inline Image *ImageStatistics::RGBtoHSB( Image *inRGBImage ) {
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
    int r = (int)( 255 * redChannel[i] );
    int g = (int)( 255 * greenChannel[i] );
    int b = (int)( 255 * blueChannel[i] );

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

								
#endif
