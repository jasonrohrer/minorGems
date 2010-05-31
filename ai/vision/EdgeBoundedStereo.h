/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created.
 *
 * 2001-February-16		Jason Rohrer
 * Finished implementation. 
 *
 * 2001-February-21		Jason Rohrer
 * Fixed a bug in the channel indexing of the output image. 
 *
 * 2001-February-22		Jason Rohrer
 * Fixed another bug in the channel indexing of the output image.
 *
 * 2001-February-24		Jason Rohrer
 * Fixed a bug in picking random image points.
 *
 * 2001-February-25		Jason Rohrer
 * Fixed boundary checking bugs.     
 */
 
 
#ifndef EDGE_BOUNDED_STEREO_INCLUDED
#define EDGE_BOUNDED_STEREO_INCLUDED 


#include "Stereo.h"
#include "EdgeDetector.h"

#include "minorGems/util/random/RandomSource.h"

#include <float.h>

/**
 * Stereo implementation that uses edge-bounded local windows.
 *
 * Note that this stereo object only processes one channel from
 * each image in the pair ( as specified by getImageChannel() from superclass ).
 *
 * @author Jason Rohrer
 */
class EdgeBoundedStereo : public Stereo {
	
	public:
		
		/**
		 * Constructs an edge-bounded stereo object.
		 *
		 * @param inMaxDisparity the maximum disparity in pixels.
		 * @param inDetector the edge detector to use.  Note that
		 *   the edge detector is set by this constructor to
		 *   operate on the same channel as this stereo object.
		 *   Is destroyed when this class is destroyed.
		 * @param inRandSource the random source to use when 
		 *   selecting window points to expand.  Is not destroyed
		 *   by this class' destructor.
		 */
		EdgeBoundedStereo( int inMaxDisparity, EdgeDetector *inDetector,
			RandomSource *inRandSource );
		
		
		~EdgeBoundedStereo();
		
		
		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		virtual Stereo *copy();
		
	private:
		EdgeDetector *mEdgeDetector;
		RandomSource *mRandSource;
		
	};



inline EdgeBoundedStereo::EdgeBoundedStereo( int inMaxDisparity, 
	EdgeDetector *inDetector, RandomSource *inRandSource ) 
	: Stereo( inMaxDisparity ), mEdgeDetector( inDetector ), 
	mRandSource( inRandSource ) {

	}

		
		
inline EdgeBoundedStereo::~EdgeBoundedStereo() {
	delete mEdgeDetector;
	}



Stereo *EdgeBoundedStereo::copy() {

	return new EdgeBoundedStereo( mMaxDisparity, mEdgeDetector->copy(),
		mRandSource );
	}



// note that this code is a little messy, because it was copied
// from	edgeStereo.cpp without much modification.
inline Image *EdgeBoundedStereo::computeDepthMap( Image *inLeft, 
	Image *inRight ) {

	char *functionName = "EdgeBoundedStereo::computeDepthMap";
	
	int i;	// index counters
	
	int h = inLeft->getHeight();
	int w = inLeft->getWidth();
	
	int numPixels = h * w;
	
	if( h != inRight->getHeight() || w != inRight->getWidth() ) {
		// image sizes don't match
		printf( "%s: Left and right images must be the same size.\n", 
			functionName );
		return NULL;
		}
	
	// y offsets for fast array access
	int *yOffset = new int[h];
	for( i=0; i<h; i++ ) {
		yOffset[i] = i * w;
		}
	
	// images are okay....
	// continue processing
	
	// singled channeled stereo map
	Image *outImage = new Image( w, h, 1 );
	
	
	// get channels for all images
	double *leftChannel = inLeft->getChannel( mChannelNumber );
	double *rightChannel = inRight->getChannel( mChannelNumber );
	
	double *outChannel = outImage->getChannel( 0 );
	
	printf( "Detecting edges\n" );
	
	// detect edges in the left image	
	Image *edgeImage = mEdgeDetector->findEdges( inLeft );
	
	
	
	double *edgeChannel = edgeImage->getChannel( 0 );

	

	double marker = -1;
	
	// set output image to marker
	for( i=0; i<numPixels; i++ ) {
		outChannel[i] = marker;
		}

	
	printf( "%s: Computing disparities.\n", functionName );
	
	// now, for each pixel, find a window bounded on all sides by edges
	// do this as we fill in disparities for pixels
	
	// as we move through image, skip pixels that have already had disparities calculated
	
	
	// stochastically select pixels to expand windows around
	// keep going until numPixels pixels found
	
	int numPixelsDone = 0;
	
	int windowsFound = 0;
	
	while( numPixelsDone < numPixels ) {
		
		int y = mRandSource->getRandomBoundedInt( 0, h-1 );
		int x = mRandSource->getRandomBoundedInt( 0, w-1 );
		//int y = (int)( mRandSource->getRandomFloat() * (h-2) ) + 1;
		//int x = (int)( mRandSource->getRandomFloat() * (w-2) ) + 1;
	
		i = yOffset[y] + x;
		
		if( outChannel[i] != marker ) {
		
			while ( outChannel[i] != marker ) {
				// walk until we find a pixel still marked blank
				i = (i+1);
				if( i>=numPixels ) {
					i = 0;
					}
				}
			// found an i for a marked pixel
			// compute x and y from i
			x = i % w;
			y = i / w;
			}			
			
		windowsFound++;
		
		int delN = 0;
		int delS = 0;
		int delE = 0;
		int delW = 0;
		
		char delNRun = true;		// true if haven't hit edge yet in this direction
		char delSRun = true;
		char delERun = true;
		char delWRun = true;
		
		char delNBackup = false;	// should back up on this edge (and stop running)
		char delSBackup = false;
		char delEBackup = false;
		char delWBackup = false;
		
		// loop over 4 directions, expanding in each until edge hit in each direction
		while( delNRun || delSRun || delERun || delWRun ) {
			
			// first, expand window boundarys that are stull running
			if( delNRun ) {
				if( delNBackup ) {
					delN--;
					delNRun = false;
					delNBackup = false;
					}
				else {
					delN++;
					if( y - delN < 0 ) {
						delN = y;
						}
					}
				}
			if( delSRun ) {
				if( delSBackup ) {
					delS--;
					delSRun = false;
					delSBackup = false;
					}
				else {
					delS++;
					if( y + delS >= h ) {
						delS = h - y - 1;
						}
					}
				}
			if( delWRun ) {
				if( delWBackup ) {
					delW--;
					delWRun = false;
					delWBackup = false;
					}
				else {
					delW++;
					if( x - delW < 0 ) {
						delW = x;
						}
					}
				}
			if( delERun ) {
				if( delEBackup ) {
					delE--;
					delERun = false;
					delEBackup = false;
					}
				else {
					delE++;
					if( x + delE >= w ) {
						delE = w - x - 1;
						}
					}					
				}
			
			
			
			// now check all new rows/columns added to look for edge collisions
			// also look for collisions with edge of image
			
		
			if( delNRun ) {
				int rowY = y - delN;

				if( rowY <= 0 ) {
					// hit boundary, so stop
					delNRun = false;
					} 
				/*if( rowY < 0 ) {
					// watch for out of bounds case
					delN = delN - ( 0 - rowY );
					rowY = 0;
					}
				*/
				int rowYContrib = yOffset[rowY];
				for( int rowX = x - delW; rowX<= x + delE; rowX ++ ) {
					int rowIndex = rowYContrib + rowX;
					if( rowIndex < 0 || rowIndex >= w * h ) {
						printf( "N row index out of bounds, %d\n", rowIndex );
						}
					if( edgeChannel[ rowIndex ] == 1.0 ) {
						// edge here, so stop
						//printf( "edge hit in north at (%d, %d)\n", );
						delNRun = false;
						}
					if( outChannel[ rowIndex ] != marker ) {
						// hit an unmarked pixel, so backup
						delNRun = true;
						delNBackup = true;
						rowX = x + delE + 1;	// jump out of for loop
						}	
					}
				}	
			if( delSRun ) {
				int rowY = y + delS;
				
				if( rowY >= h-1 ) {
					// hit boundary, so stop
					delSRun = false;
					} 
				/*if( rowY > h-1 ) {
					// watch for out of bounds case
					delS = delS - ( rowY - ( h - 1 ) );
					rowY = h - 1;
					}
				*/
				int rowYContrib = yOffset[rowY];
				for( int rowX = x - delW; rowX<= x + delE; rowX ++ ) {
					int rowIndex = rowYContrib + rowX;
					if( rowIndex < 0 || rowIndex >= w * h ) {
						printf( "S row index out of bounds, %d\n", rowIndex );
						}
					if( edgeChannel[ rowIndex ] == 1.0 ) {
						// edge here, so stop
						delSRun = false;
						}
					if( outChannel[ rowIndex ] != marker ) {
						// hit an unmarked pixel, so backup
						delSRun = true;
						delSBackup = true;
						rowX = x + delE + 1;	// jump out of for loop
						}	
					}
				}
			if( delWRun ) {
				int rowX = x - delW;
				
				if( rowX <= 0 ) {
					// hit boundary, so stop
					delWRun = false;
					}
				/*if( rowX < 0 ) {
					// watch for out of bounds case
					delW = delW - ( 0 - rowX );
					rowX = 0;
					}
				*/
				for( int rowY = y - delN; rowY<= y + delS; rowY ++ ) {
					int rowIndex = yOffset[ rowY ] + rowX;
					if( rowIndex < 0 || rowIndex >= w * h ) {
						printf( "W row index out of bounds, %d\n", rowIndex );
						}
					if( edgeChannel[ rowIndex ] == 1.0 ) {
						// edge here, so stop
						delWRun = false;
						}
					if( outChannel[ rowIndex ] != marker ) {
						// hit an unmarked pixel, so backup
						delWRun = true;
						delWBackup = true;
						rowY = y + delS + 1;	// jump out of for loop
						}	
					}
				}	
			if( delERun ) {
				int rowX = x + delE;
				
				if( rowX >= w-1 ) {
					// hit boundary, so stop
					delERun = false;
					} 
				/*if( rowX > w-1 ) {
					// watch for out of bounds case
					delE = delE - ( rowX - ( w - 1 ) );
					rowX = w-1;
					}
				*/
				for( int rowY = y - delN; rowY<= y + delS; rowY ++ ) {
					int rowIndex = yOffset[ rowY ] + rowX;
					if( rowIndex < 0 || rowIndex >= w * h ) {
						printf( "E row index out of bounds, %d\n", rowIndex );
						}
					if( edgeChannel[ rowIndex ] == 1.0 ) {
						// edge here, so stop
						delERun = false;
						}
					if( outChannel[ rowIndex ] != marker ) {
						// hit an unmarked pixel, so backup
						delERun = true;
						delEBackup = true;
						rowY = y + delS + 1;	// jump out of for loop
						}	
					}
				}
				
				
									
			}	// end while loop over window directional expansions
	
			
		
		// now we have a window around this pixel that touches exactly one edge on each side
		//	also, window doesn't contain any unmarked pixels
		
		// compute best disparity for pixels in window together
		
		double costOfBestL2 = DBL_MAX;
		int bestDisp = 0;
		
		// look at each possible displacement for this pixel and it's window
		for( int d=0; d<=mMaxDisparity; d++ ) {
			double thisCostL2 = 0;
			
			// computed correlation error between each pixel in left image window and 
			// destination pixel in right image
			int windYMin = y-delN;
			int windYMax = y+delS;
			int windXMin = x-delW;
			int windXMax = x+delE;
			
			for( int windY = windYMin; windY <= windYMax; windY++ ) {
				int windYOffset = yOffset[ windY ];
				for( int windX = windXMin; windX <= windXMax; windX++ ) {
					
					int windowPixInd = windYOffset + windX;
				
					double valLeft = leftChannel[ windowPixInd ];
					
					double valRight = 0;
									
					if( windX - d > 0 ) {		// pixel lands in right image when moved by d
						valRight = rightChannel[ windowPixInd - d ];
						}
					// else leave valRight at 0

					double diff = valLeft - valRight;
					thisCostL2 += diff * diff;
					}	// end over all windX
				}	// end over all windY
			
			if( thisCostL2 < costOfBestL2 ) {		// check if new better L2 displ. found
				bestDisp = d;
				costOfBestL2 = thisCostL2;
				}
			}	// end over all disparities
		
		// found best displacement, write it into out image buffer
		//outImage[ yOffset[y] + x ] = bestDisp;
		
		// fill entire window in buffer with this disparity
		for( int windY = y-delN; windY <= y+delS; windY++ ) {
			int windYOffset = yOffset[ windY ];
			for( int windX = x-delW; windX <= x+delE; windX++ ) {
				outChannel[ windYOffset + windX ] = bestDisp;
				}
			}
		numPixelsDone += ( ( delN + delS + 1 ) * (delE + delW + 1 ) );

		}	// end while not all pix done
	
	
	//printf( "found %d windows\n", windowsFound );	
	
	
	delete [] yOffset;
	delete edgeImage;
		
	
	return outImage;
	}

		
#endif
