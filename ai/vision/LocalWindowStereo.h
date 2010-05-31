/*
 * Modification History
 *
 * 2001-February-25		Jason Rohrer
 * Created. 
 *
 * 2001-February-26		Jason Rohrer
 * Optimized.     
 */
 
 
#ifndef LOCAL_WINDOW_STEREO_INCLUDED
#define LOCAL_WINDOW_STEREO_INCLUDED 


#include "PartialStereo.h"
#include "EdgeDetector.h"

#include "minorGems/util/random/RandomSource.h"

#include "minorGems/graphics/ImageColorConverter.h"

#include <float.h>
#include <limits.h>
#include <stdio.h>

/**
 * Local window stereo implementation.
 *
 * @author Jason Rohrer
 */
class LocalWindowStereo : public PartialStereo {
	
	public:
		
		/**
		 * Constructs a local window stereo object.
		 *
		 * @param inMaxDisparity the maximum disparity in pixels.
		 * @param inWindowSize the diameter of each square pixel window.
		 * @param inRandSource the random source to use when 
		 *   setting error values for pixel windows that displace outside of
		 *   the right image.
		 */
		LocalWindowStereo( int inMaxDisparity, int inWindowSize,
			RandomSource *inRandSource );
		
		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		virtual Stereo *copy();
		
	private:
		int mWindowSize;
		
		RandomSource *mRandSource;
	};



inline LocalWindowStereo::LocalWindowStereo( 
	int inMaxDisparity, int inWindowSize, RandomSource *inRandSource )
	: PartialStereo( inMaxDisparity ), mWindowSize( inWindowSize ),
	mRandSource( inRandSource ) {
	
	}



inline Stereo *LocalWindowStereo::copy() {

	LocalWindowStereo *returnValue = 
		new LocalWindowStereo( mMaxDisparity, mWindowSize, mRandSource );
	returnValue->setRange( mXStart, mXEnd, mYStart, mYEnd );
	
	return returnValue;
	}



// note that this code is a little messy, because it was copied
// from	localStereo.cpp without much modification.
inline Image *LocalWindowStereo::computeDepthMap( Image *inLeft, 
	Image *inRight ) {

	char *functionName = "localStereo";
	
	
	int w = inLeft->getWidth();
	int h = inLeft->getHeight();

	//printf( "%s: Computing stereo.\n", 
	//		functionName );

	if( h != inRight->getHeight() || w != inRight->getWidth() ) {
		// image sizes don't match
		printf( "%s: Left and right images must be the same size.\n", 
			functionName );
		return NULL;
		}


	int numPixels = w*h;
	
	int boxRad = ( mWindowSize / 2 );		// radius of window
	
	int extra;
	
	if( ( mWindowSize % 2 ) == 0 ) {
		boxRad = boxRad - 1;
		extra = 1;
		}
	else {
		extra = 0;
		}


	// singled channeled stereo map
	Image *outImage = new Image( w, h, 1 );
	
	
	// output channel
	double *outChannel = outImage->getChannel( 0 );

	// convert incoming double channels to byte arrays
	// this should improve running time
	unsigned char *leftChannel = 
		ImageColorConverter::grayscaleToByteArray( inLeft, mChannelNumber );
	unsigned char *rightChannel = 
		ImageColorConverter::grayscaleToByteArray( inRight, mChannelNumber );


	// y offsets for fast array access
	int *yOffset = new int[h];
	for( int i=0; i<h; i++ ) {
		yOffset[i] = i * w;
		}
	
	
	int yStart = (int)( mYStart * h );
	int yEnd = (int)( mYEnd * h ) - 1;
	int xStart = (int)( mXStart * w );
	int xEnd = (int)( mXEnd * w ) - 1;
	
	int startBox = boxRad + extra;
	
	// leave room at image edges
	if( yStart < startBox ) {
		yStart = startBox;
		}
	if( yEnd > h - boxRad - 1 ) {
		yEnd = h - boxRad - 1;
		}
	if( xStart < startBox ) {
		xStart = startBox;
		}
	if( xEnd > w - boxRad - 1 ) {
		xEnd = w - boxRad - 1;
		}
	
	
	for( int y=yStart; y<=yEnd; y++ ) {		// for each pixel in image
		//printf( "row %d\n", y);
		for( int x=xStart; x<=xEnd; x++ ) {
			
			int bestDispL2 = 0;
			long costOfBestL2 = LONG_MAX;
			
			
			// look at each possible displacement for this pixel and it's window
			
				// (d towards left, where pixel should land in R image)
			for( int d=0; d<=mMaxDisparity; d++ ) {
			
				long thisCostL2 = 0;
				
				// for each pixel in window
				for( int delY = -startBox; delY<=boxRad; delY++ ) {
					int thisYOffset = yOffset[  y + delY ];
					
					for( int delX = -startBox; delX<=boxRad; delX++ ) {
						
						// optimization
						register int xPlusDelX = x + delX;
						
						int windowPixInd = thisYOffset + xPlusDelX;
					
						long valLeft = leftChannel[ windowPixInd ];
						
						long valRight;
						
					
						if( xPlusDelX - d > 0 ) {		// pixel lands in right image when moved by d
							valRight = rightChannel[ windowPixInd - d ];
							}
						else {		// pixel lands outside R-image when displaced by d
							// shouldn't let this slip through with no cost, or pixels
							// will prefer to be displaced outside the image
							// give them random intensity value.
							
							valRight = 
								mRandSource->getRandomBoundedInt( 
									0, 255 );
							}	
						
						// calculate L1 for this landing place in right image
						register long diff = valLeft - valRight;
						
						// calculate L2 for this landing place in right image
						// add to total L2 cost for this displacement of this window
						thisCostL2 += diff * diff;
						}
					}
				
				if( thisCostL2 < costOfBestL2 ) {		// check if new better L2 displ. found
					bestDispL2 = d;
					costOfBestL2 = thisCostL2;
					}
				}	// end loop over all displacements
				
			
			// found best displacement, write it into L1 and L2 image buffers
			outChannel[ yOffset[y] + x ] = 
				(double)bestDispL2 / (double)mMaxDisparity;
			
			}	// end loop over all x coords
		}	// end loop over all y coords

	delete [] yOffset;
	delete [] leftChannel;
	delete [] rightChannel;
	
	return outImage;
	}
		
		
#endif
