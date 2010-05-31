/*
 * Modification History
 *
 * 2001-May-15     Jeremy Tavan
 * Created.
 * 
 * 2001-May-16     Jeremy Tavan
 * Modified to use the much faster quick_select 
 * median-finding algorithm.
 *
 * 2001-May-17   Jaosn Rohrer
 * Tried to optimize by moving stuff out of the inner-inner loop.
 * It helped a bit, but not much.
 *
 */
 
 
#ifndef MEDIAN_FILTER_INCLUDED
#define MEDIAN_FILTER_INCLUDED
 
#include "minorGems/graphics/ChannelFilter.h" 
#include "quickselect.h"


int medianFilterCompareInt( const void *x, const void *y );

/**
 * Median convolution filter.
 *
 * @author Jeremy Tavan 
 */
class MedianFilter : public ChannelFilter { 

 public:
								
  /**
   * Constructs a median filter.
   *
   * @param inRadius the radius of the box in pixels.
   */
  MedianFilter( int inRadius );
								
								
  /**
   * Sets the box radius.
   *
   * @param inRadius the radius of the box in pixels.
   */
  void setRadius( int inRadius );
								
								
  /**
   * Gets the box radius.
   *
   * @return the radius of the box in pixels.
   */
  int getRadius();
								
								
  // implements the ChannelFilter interface
  void apply( double *inChannel, int inWidth, int inHeight );

 private:
  int mRadius;
};
				
				
				
inline MedianFilter::MedianFilter( int inRadius ) 
  : mRadius( inRadius ) {				
				
}



inline void MedianFilter::setRadius( int inRadius ) {
  mRadius = inRadius;
}



inline int MedianFilter::getRadius() {
  return mRadius;
}				
				
				
				
inline void MedianFilter::apply( double *inChannel, 
																 int inWidth, int inHeight ) {

  // pre-compute an integer version of the channel for the
  // median alg to use
  int numPixels = inWidth * inHeight;
  int *intChannel = new int[ numPixels ];
  for( int p=0; p<numPixels; p++ ) {
	  intChannel[p] = (int)( 1000 * inChannel[p] );
	  }


  
  double *medianChannel = new double[ inWidth * inHeight ];
//  printf("inWidth = %d, inHeight = %d\n", inWidth, inHeight);	
//  printf("mRadius = %d\n",mRadius);
  for( int y=0; y<inHeight; y++ ) {
    int yIndexContrib = y * inWidth;
								
    int startBoxY = y - mRadius;
    int endBoxY = y + mRadius;
								
    if( startBoxY < 0 ) {
      startBoxY = 0;
    }
    if( endBoxY >= inHeight ) {
      endBoxY = inHeight - 1;
    }
								
    int boxSizeY = endBoxY - startBoxY + 1;
								

    for( int x=0; x<inWidth; x++ ) {
      int startBoxX = x - mRadius;
      int endBoxX = x + mRadius;
									
      if( startBoxX < 0 ) {
				startBoxX = 0;
      }
      if( endBoxX >= inWidth ) {
				endBoxX = inWidth - 1;
      }
												
      int boxSizeX = endBoxX - startBoxX + 1;
      int *buffer = new int[boxSizeX * boxSizeY];
      											
      // sum all pixels in the box around this pixel
      for( int boxY = startBoxY; boxY<=endBoxY; boxY++ ) {
				int yBoxIndexContrib = boxY * inWidth;
				int yBoxContrib = boxSizeX * ( boxY-startBoxY );

				for( int boxX = startBoxX; boxX<=endBoxX; boxX++ ) {		
				  //buffer[boxSizeX*(boxY-startBoxY)+(boxX-startBoxX)] = (int)(1000.0 * inChannel[yBoxIndexContrib + boxX]);
				  buffer[ yBoxContrib + ( boxX-startBoxX ) ] = intChannel[ yBoxIndexContrib + boxX ];	
				}
      }
      
      medianChannel[ yIndexContrib + x ] = (double)quick_select( buffer, boxSizeX*boxSizeY ) / 1000.0;

      delete [] buffer;
    }
  }
  

  // copy blurred image back into passed-in image
  memcpy( inChannel, medianChannel, sizeof(double) * inWidth * inHeight );
  
  delete [] medianChannel;
  delete [] intChannel;
}

#endif
