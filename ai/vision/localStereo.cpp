// Jason Rohrer
// localStereo.cpp

/**
*
*	Function for computing stereo disparity using local window
*	Uses L2 distance only
*
*
*	Created 5-17-2000
*	Mods:
*/

#include <stdlib.h>

#include "localStereo.h"

int veryBig	= 1073741824;

void localStereo( P_M *inL, P_M *inR, P_M *out, int windowSize, int maxDisparity ) {
	
	char *functionName = "localStereo";
	
	
	int w = inL->getWidth();
	int h = inL->getHeight();


	if( h != inR->getHeight() || w != inR->getWidth() ) {
		// image sizes don't match
		printf( "%s: Left and right images must be the same size.\n", functionName );
		return;
		}
	
	if( h != out->getHeight() || w != out->getWidth() ) {
		// output image size doen't match input
		printf( "%s: Ouput and input images must be the same size.\n", functionName );
		return;
		}


	int numPixels = w*h;
	
	int boxRad = (windowSize / 2 );		// radius of window
	
	int extra;
	
	if( (windowSize % 2) == 0 ) {
		boxRad = boxRad - 1;
		extra = 1;
		}
	else {
		extra = 0;
		}


	unsigned char *leftBuffer = inL->getBuffer();
	unsigned char *rightBuffer = inR->getBuffer();
	unsigned char *outBuffer = out->getBuffer();
	
	int *offset = inL->getRowOffsets();		// same for all images
	
	
	int startBox = boxRad + extra;
	
	float INV_RAND_MAX = 1.0 / RAND_MAX;
	
	for( int y=startBox; y<h-boxRad; y++ ) {		// for each pixel in image
		printf( "row %d   ", y);
		for( int x=startBox; x<w-boxRad; x++ ) {
			
			int bestDispL2 = 0;
			int costOfBestL2 = veryBig;
			
			
			// look at each possible displacement for this pixel and it's window
			
				// (d towards left, where pixel should land in R image)
			for( int d=0; d<maxDisparity; d++ ) {
			
				int thisCostL2 = 0;
				
				// for each pixel in window
				for( int delY = -startBox; delY<=boxRad; delY++ ) {
					int thisYOffset = offset[  y + delY ];
					
					for( int delX = -startBox; delX<=boxRad; delX++ ) {
					
						int windowPixInd = thisYOffset + x + delX;
					
						int valLeft = leftBuffer[ windowPixInd ];
						
						int valRight;
						
					
						if( x + delX -d > 0 ) {		// pixel lands in right image when moved by d
							valRight = rightBuffer[ windowPixInd - d];
							}
						else {		// pixel lands outside R-image when displaced by d
							// shouldn't let this slip through with no cost, or pixels
							// will prefer to be displaced outside the image
							// give them random intensity value.
							
							valRight = (int)(((float)(rand()) * 255) *INV_RAND_MAX);
							}	
						
						// calculate L1 for this landing place in right image
						int diff = valLeft - valRight;
						
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
			outBuffer[ offset[y] + x ] = bestDispL2;
			
			}	// end loop over all x coords
		}	// end loop over all y coords

	}