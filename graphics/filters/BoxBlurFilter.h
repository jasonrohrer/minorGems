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
	
	double *blurredChannel = new double[ inWidth * inHeight ];


    // optimization:
    // The sum for a given pixel's box in row N is the sum for the same pixel
    // in row (N-1) minus one box-row of pixels and plus a box-row of pixels
    // We don't have to loop over the entire box for each pixel (instead,
    // we just add the new pixels and subtract the old as the box moves along)
    char lastRowSumsSet = false;
    double *lastRowSums = new double[ inWidth ];

    // track the sums from single rows of boxes that we have already seen
    // Thus, when we need to "subtract a row" from our box sum, we can use
    // the precomputed version
    double *singleRowBoxSums = new double[ inWidth * inHeight ];
    
    
	for( int y=0; y<inHeight; y++ ) {
		int yIndexContrib = y * inWidth;
		
		int startBoxY = y - mRadius;
		int endBoxY = y + mRadius;

        // deal with boundary cases (near top and bottom of image)
        // Near top, we only add rows to our sum
        // Near bottom, we only subtract rows to our sum
        char addARow = true;
        char subtractARow = true;
        
		if( startBoxY <= 0 ) {
			startBoxY = 0;
            
            // our box is at or hanging over top edge
            // no rows to subtract from sum
            subtractARow = false;
			}
		if( endBoxY >= inHeight ) {
			endBoxY = inHeight - 1; 

            // box hanging over bottom edge
            // no rows to add to sum
            addARow = false;
			}
		
		int boxSizeY = endBoxY - startBoxY + 1;
		
		for( int x=0; x<inWidth; x++ ) {

            int pixelIndex = yIndexContrib + x;

            // sum into this pixel in the blurred channel
            blurredChannel[ pixelIndex ] = 0;
            
            
			int startBoxX = x - mRadius;
			int endBoxX = x + mRadius;

            
            
			if( startBoxX < 0 ) {
				startBoxX = 0;
				}
			if( endBoxX >= inWidth ) {
				endBoxX = inWidth - 1;
				}
			
			int boxSizeX = endBoxX - startBoxX + 1;
			
			// sum all pixels in the box around this pixel
            double sum = 0;


            if( ! lastRowSumsSet ) {
                // do the "slow way" for the first row
                
                for( int boxY = startBoxY; boxY<=endBoxY; boxY++ ) {
                	int yBoxIndexContrib = boxY * inWidth;

                    double rowSum = 0;
                	for( int boxX = startBoxX; boxX<=endBoxX; boxX++ ) {	
                
                        rowSum += inChannel[ yBoxIndexContrib + boxX ];
                		}

                    sum += rowSum;

                    // store row sum for future use
                    singleRowBoxSums[ yBoxIndexContrib + x ] = rowSum;
                	}

                }
            else {
                // we have sum for this pixel from the previous row
                // use it to avoid looping over entire box again

                sum = lastRowSums[ x ];

                if( addARow ) {
                    // add pixels from row at endBoxY

                                    
                    int yBoxIndexContrib = endBoxY * inWidth;

                    double rowSum = 0;
                    for( int boxX = startBoxX; boxX<=endBoxX; boxX++ ) {	
                        rowSum += inChannel[ yBoxIndexContrib + boxX ];
                        }

                    sum += rowSum;

                    // store it for later when we will need to subtract it
                    singleRowBoxSums[ yBoxIndexContrib + x ] = rowSum;
                    }
                if( subtractARow ) {
                    // subtract pixels from startBoxY of previous row's box
                                    
                    int yBoxIndexContrib = (startBoxY - 1) * inWidth;

                    // use pre-computed sum for the row we're subtracting
                    sum -= singleRowBoxSums[ yBoxIndexContrib + x ];
                    }
                    
                }

            
			// divide by number of pixels to complete the average
			blurredChannel[ pixelIndex ] = sum / (boxSizeX * boxSizeY);

            // save to use when computing box sum for next row
            lastRowSums[ x ] = sum;
			}
        
        // we now have valid last row sums that we can use for
        // all the rest of the rows
        lastRowSumsSet = true;
        
		}
	
	// copy blurred image back into passed-in image
	memcpy( inChannel, blurredChannel, sizeof(double) * inWidth * inHeight );
	
	delete [] blurredChannel;
	delete [] lastRowSums;
    delete [] singleRowBoxSums;
	}
	
#endif
