/*
 * Modification History
 *
 * 2000-December-21		Jason Rohrer
 * Created.
 *
 * 2001-January-6		Jason Rohrer
 * Added a getRadius function for completeness.  
 *
 * 2006-August-22    Jason Rohrer
 * Fixed major bug:  sum for box must start at zero.
 *
 * 2006-September-7    Jason Rohrer
 * Optimized inner loop.
 * Optimized more, avoiding summing over entire box for each pixel.
 *
 * 2011-January-17   Jason Rohrer
 * Optimized with accumulation buffer pre-processing step, found on Gamasutra.
 */
 
 
#ifndef BOX_BLUR_FILTER_INCLUDED
#define BOX_BLUR_FILTER_INCLUDED
 
#include "minorGems/graphics/ChannelFilter.h" 
 
/**
 * Blur convolution filter that uses a box for averaging.
 *
 * @author Jason Rohrer 
 */
class BoxBlurFilter : public ChannelFilter { 
	
	public:
		
		/**
		 * Constructs a box filter.
		 *
		 * @param inRadius the radius of the box in pixels.
		 */
		BoxBlurFilter( int inRadius );
		
		
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
	
	
	
inline BoxBlurFilter::BoxBlurFilter( int inRadius ) 
	: mRadius( inRadius ) {	
	
	}



inline void BoxBlurFilter::setRadius( int inRadius ) {
	mRadius = inRadius;
	}



inline int BoxBlurFilter::getRadius() {
	return mRadius;
	}	
	
	
	
inline void BoxBlurFilter::apply( double *inChannel, 
                                  int inWidth, int inHeight ) {

    // the basis for this method was found in this article:
    // 
    // http://www.gamasutra.com/view/feature/3102/
    //        four_tricks_for_fast_blurring_in_.php?page=1
    //
    // however, the code provided seems to contain mistakes, which I have fixed


    // sum of all pixels to left and above each position 
    // (including that position)
    double *accumTotals = new double[ inWidth * inHeight ];
    

    double *accumPointer = accumTotals;
    double *sourcePointer = inChannel;
    for( int y=0; y<inHeight; y++ ) {
        for( int x=0; x<inWidth; x++ ) {
            double total = sourcePointer[0];

            if( x>0 ) {    
                total += accumPointer[-1];
                }
            
            if( y>0 ) {    
                total += accumPointer[ -inWidth ];
                }
            
            if( x>0 && y>0 ) {
                total -= accumPointer[ -inWidth - 1 ];
                }
            
            *accumPointer = total;
            accumPointer++;
            
            sourcePointer++;
            }
        }
    
    
    
    
    
    // sum boxes right into passed-in channel
    for( int y=0; y<inHeight; y++ ) {
        // seems like Gamasutra's code had a mistake
        // need to extend start of box 1 past actual box, for this to work
        // properly
        int boxYStart = y - mRadius - 1;
        int boxYEnd = y + mRadius;

        double yOutsideFactor = 1;
        
        int yDimensionExtra = 0;

        if( boxYStart < 0 ) {
            boxYStart = 0;
            yOutsideFactor = 0;
            yDimensionExtra = 1;
            }
        if( boxYEnd >= inHeight ) {
            boxYEnd = inHeight - 1;
            }
        int yDimension = boxYEnd - boxYStart + yDimensionExtra;


        for( int x=0; x<inWidth; x++ ) {
            
            int boxXStart = x - mRadius - 1;
            int boxXEnd = x + mRadius;
            
            double xOutsideFactor = 1;
            
            int xDimensionExtra = 0;
            
            if( boxXStart < 0 ) {
                boxXStart = 0;
                xOutsideFactor = 0;
                xDimensionExtra = 1;
                }
            if( boxXEnd >= inWidth ) {
                boxXEnd = inWidth - 1;
                }
            
            double outsideOverlapFactor = yOutsideFactor * xOutsideFactor;
            

            int xDimension = boxXEnd - boxXStart + xDimensionExtra;

            int numPixelsInBox = (yDimension) * (xDimension);
    
            double boxValueMultiplier = 1.0 / numPixelsInBox;

            
            inChannel[ y * inWidth + x ] = 
                boxValueMultiplier * (
                    // total sum of pixels in image from far corner
                    // of box back to (0,0)
                    accumTotals[ boxYEnd * inWidth + boxXEnd ]
                    // subtract regions outside of box, if any
                    -
                    yOutsideFactor * 
                    accumTotals[ boxYStart * inWidth + boxXEnd ]
                    -
                    xOutsideFactor * 
                    accumTotals[ boxYEnd * inWidth + boxXStart ]
                    // add back in region that was subtracted twice, if any
                    +
                    outsideOverlapFactor *
                    accumTotals[ boxYStart * inWidth + boxXStart ]
                    );
            
            }
        }
    
    delete [] accumTotals;    
    }


#endif
