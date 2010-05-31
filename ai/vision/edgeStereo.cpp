// Jason Rohrer
// edgesStereo.cpp

/**
*
*	Function for computing stereo disparity using SUSAN edge
*
*
*	Created 5-16-2000
*	Mods:
*/


#include "edgeStereo.h"

#include "susan_types.h"

#include "localStereo.h"


#include <float.h>
#include <string.h>	// for memcpy function
#include <stdlib.h>
#include <time.h>

// prototypes

extern "C" void susan_find_edges_thresh( uchar *in, int x_size, int y_size, uchar *binEdges, EDGE_ORIENTATION *orient, int thresh );
extern "C" void susan_find_edges( uchar *in, int x_size, int y_size, uchar *binEdges, EDGE_ORIENTATION *orient );

// dilate a binary image by radius
// pass in binary image, width and height, precalc'ed y offsets into pixel array, and dilation radius
void dilate( uchar *binImage, int w, int h, int *yOffset, int radius );


// inlines / prototypes

// L2 distance between two edge orientations
inline float orientDiff( EDGE_ORIENTATION a, EDGE_ORIENTATION b ) {
	float dif = a.z - b.z;
	return dif * dif;
	}
	
	
/*
*
*	Takes two input P_Ms (left and right),
*	one output P_M (disparity map),
*	and an integer describing max disparity
*
*/
void edgeStereoHausdorff( P_M *inL, P_M *inR, P_M *out, int windowSize, int maxDisparity, int edgeThreshold, int hdDilate ) {
	
	
	char *functionName = "edgeStereoHausdorff";
	
	int i;	// index counters
	
	char hybrid = true;	// if true, uses combined HD and local method
	char hybridA = false;	// if true, (and if hybrid true too) only uses HD for windows 
							//		centered on edge pixels (hybrid method A)
	char hybridB = true;	// if true, (and if hybrid true too) calculates HD method everywhere
							// 		but local method kicks in when HD fraction too low
	
	
	int h = inL->getHeight();
	int w = inL->getWidth();
	
	int numPixels = h * w;
	
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
	
	// images are okay....
	// continue processing
	
	
	// for now, just compute edges
	
	uchar *binLEdges = new uchar[numPixels];	// where binary edges will be returned (left)
	uchar *binREdges = new uchar[numPixels];	// where binary edges will be returned (right)
	
	if( binLEdges == NULL || binREdges == NULL ) {
		printf( "%s: Memory allocation failed.\n", functionName );
		return;
		}
	
	uchar *inputLImage = (uchar *)( inL->getBuffer() );
	uchar *inputRImage = (uchar *)( inR->getBuffer() );
	uchar *outImage = (uchar *)( out->getBuffer() );
	
	// set output image to 0
	for( i=0; i<numPixels; i++ ) {
		outImage[i] = 0;
		}
	
	int *yOffset = inL->getRowOffsets();
	
	// edge orientation data (where orientations returned)
	EDGE_ORIENTATION *lOrient = new EDGE_ORIENTATION[ numPixels ];
	EDGE_ORIENTATION *rOrient = new EDGE_ORIENTATION[ numPixels ];
	
	if( lOrient == NULL || rOrient == NULL ) {
		printf( "%s: Memory allocation failed.\n", functionName );
		return;
		}
	
	printf( "%s: Finding edges.\n", functionName );
	// find edges in left and right images
	susan_find_edges_thresh( inputLImage, w, h, binLEdges, lOrient, edgeThreshold );
	susan_find_edges_thresh( inputRImage, w, h, binREdges, rOrient, edgeThreshold );
	
	
	// write edge maps out to file for debugging
	P_M leftEdges( PGM, w, h, binLEdges, 1 );
	P_M rightEdges( PGM, w, h, binREdges, 1 );
	
	leftEdges.write( "dbg_edges_l.pgm" );
	rightEdges.write( "dbg_edges_r.pgm" );
	
	printf( "%s: Dilating edges in right image.\n", functionName );
	
	// dilate edges in right image
	dilate( binREdges, w, h, yOffset, hdDilate );
	
	// write dilated edge map out to file for debugging
	P_M rightDilatedEdges( PGM, w, h, binREdges, 1 );

	rightDilatedEdges.write( "dbg_dilated_edges_r.pgm" );
	
	
	
	printf( "%s: Finding disparities.\n", functionName );
	
	// now, for each pixel-centered window in left edge image
	
	// try different disparities that project window to left in right image
	
	int boxRad = (windowSize / 2 );		// radius of window
	int extra;
	if( (windowSize % 2) == 0 ) {
		boxRad = boxRad - 1;
		extra = 1;
		}
	else {
		extra = 0;
		}

	int startBox = boxRad + extra;
	
	int veryBig	= 1073741824;
	float INV_RAND_MAX = 1.0 / RAND_MAX;
	
	for( int y=startBox; y<h-boxRad; y++ ) {		// for each pixel in image
		printf( "row %d   ", y);
		for( int x=startBox; x<w-boxRad; x++ ) {
			
			int bestDisp = 0;
			
			float hdFractionOfBest = 0.0;
			int numEdges = 0;
			
			char hdComputed = false;
			
			// use HD method if conditions met for specified hybrid method
			if( !hybrid || hybridB || ( hybridA && binLEdges[ yOffset[y] + x ] ) ) {
				
				hdComputed = true;
				
				// first, count edges in left window
				for( int delY = -startBox; delY<=boxRad; delY++ ) {
					int thisYOffset = yOffset[  y + delY ];
					
					for( int delX = -startBox; delX<=boxRad; delX++ ) {
					
						int windowPixInd = thisYOffset + x + delX;
					
						if( binLEdges[ windowPixInd ] ) {
							numEdges++;
							}
						}
					}
				
				
				// look at each possible displacement for this pixel's window of edges
				
					// (d towards left, where pixel should land in R image)
				for( int d=0; d<maxDisparity; d++ ) {
				
					float thisFraction = 0.0;
					
					int numIntersect = 0;	// num edges in left that land on a dilated edge in right
					
					// for each pixel in window
					for( int delY = -startBox; delY<=boxRad; delY++ ) {
						int thisYOffset = yOffset[  y + delY ];
						
						for( int delX = -startBox; delX<=boxRad; delX++ ) {
						
							int windowPixInd = thisYOffset + x + delX;
						
							if( binLEdges[ windowPixInd ] ) {
								
								if( x + delX -d > 0 ) {	// pixel lands in right image when moved by d
									if( binREdges[ windowPixInd - d ] ) {
										// pixel lands on edge in right
										numIntersect++;
										}
									}
								}
							}	// end over all delX in window
						}	// end over all delY in window
					
					thisFraction = ((float)numIntersect) / ((float)numEdges);
					
					if( thisFraction > hdFractionOfBest ) {
						bestDisp = d;
						hdFractionOfBest = thisFraction;
						}
					}	// end loop over all displacements
				}
			if( hybrid && (( hybridA && !hdComputed ) || ( hybridB && ( numEdges == 0 || hdFractionOfBest < .75 ) )) ) {
				// center pixel is not an edge...
				// use ordinary l2 distance local window correlation method
				
				int costOfBestL2 = veryBig;
			
			
				// look at each possible displacement for this pixel and it's window
				
					// (d towards left, where pixel should land in R image)
				for( int d=0; d<maxDisparity; d++ ) {
				
					int thisCostL2 = 0;
					
					// for each pixel in window
					for( int delY = -startBox; delY<=boxRad; delY++ ) {
						int thisYOffset = yOffset[  y + delY ];
						
						for( int delX = -startBox; delX<=boxRad; delX++ ) {
						
							int windowPixInd = thisYOffset + x + delX;
						
							int valLeft = inputLImage[ windowPixInd ];
							
							int valRight;
							
						
							if( x + delX -d > 0 ) {		// pixel lands in right image when moved by d
								valRight = inputRImage[ windowPixInd - d];
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
						bestDisp = d;
						costOfBestL2 = thisCostL2;
						}
					}	// end loop over all displacements
				}
			
				
			// found best displacement, write it into out image buffer
			outImage[ yOffset[y] + x ] = bestDisp;
			
			}	// end loop over all x coords
		}	// end loop over all y coords
	
	
	delete [] binLEdges;
	delete [] binREdges;
	} 



void dilate( uchar *binImage, int w, int h, int *yOffset, int radius ) {
	
	char *functionName = "edgeStereo";
	
	int numPixels = w * h;
	
	// need to copy bin image before running
	uchar *binCopy = new uchar[ numPixels ];
	if( binCopy == NULL ) {
		printf( "%s: Memory allocation failed.\n", functionName );
		return;
		}
	
	// repeat 1-pixel dilation radius times to achieve dilation by radius
	for( int r=0; r<radius; r++ ) {
		
		// copy into temp array
		memcpy( (void *)binCopy, (void *)binImage, numPixels );
		
		for( int y=1; y<h-1; y++ ) {
			int yContrib = yOffset[y];
			for( int x=1; x<w-1; x++ ) {
				int i = yContrib + x;	// index into pix array
				
				// if pix at i not already colored (skip otherwise)
				if( ! binCopy[i] ) {
					
					// look at neighbors of i (in copy)
					if( binCopy[i-1] || binCopy[i+1] || binCopy[i-w] || binCopy[i+w] ) {
						binImage[i] = 1;
						}
					}
				}	// end for all x
			}	// end for all y
		}	// end for all radii
	
	delete [] binCopy;
	}	// end dilate()








char edgeStereoWindow( P_M *inL, P_M *inR, P_M *out, int maxDisparity, int edgeThreshold ) {
	
	char *functionName = "edgeStereoWindow";
	
	int i;	// index counters
	
	int h = inL->getHeight();
	int w = inL->getWidth();
	
	int numPixels = h * w;
	
	if( h != inR->getHeight() || w != inR->getWidth() ) {
		// image sizes don't match
		printf( "%s: Left and right images must be the same size.\n", functionName );
		return false;
		}
	
	if( h != out->getHeight() || w != out->getWidth() ) {
		// output image size doen't match input
		printf( "%s: Ouput and input images must be the same size.\n", functionName );
		return false;
		}
	
	// images are okay....
	// continue processing
	
	
	// for now, just compute edges
	
	uchar *binLEdges = new uchar[numPixels];	// where binary edges will be returned (left)
	
	if( binLEdges == NULL ) {
		printf( "%s: Memory allocation failed.\n", functionName );
		return false;
		}
	
	uchar *inputLImage = (uchar *)( inL->getBuffer() );
	uchar *inputRImage = (uchar *)( inR->getBuffer() );
	uchar *outImage = (uchar *)( out->getBuffer() );
	
	
	uchar marker = maxDisparity + 1;
	
	// set output image to marker
	for( i=0; i<numPixels; i++ ) {
		outImage[i] = marker;
		}
	
	int *yOffset = inL->getRowOffsets();
	
	// edge orientation data (where orientations returned)
	EDGE_ORIENTATION *lOrient = new EDGE_ORIENTATION[ numPixels ];
	
	if( lOrient == NULL ) {
		printf( "%s: Memory allocation failed.\n", functionName );
		return false;
		}
	
	printf( "%s: Finding edges.\n", functionName );
	// find edges in left and right images
	susan_find_edges_thresh( inputLImage, w, h, binLEdges, lOrient, edgeThreshold );
	
	// write edge maps out to file for debugging
	//P_M leftEdges( PGM, w, h, binLEdges, 1 );
	
	//leftEdges.write( "dbg_edges_l.pgm" );

	
	int veryBig	= 1073741824;
	float INV_RAND_MAX = 1.0 / RAND_MAX;
	
	
	printf( "%s: Computing disparities.\n", functionName );
	
	// now, for each pixel, find a window bounded on all sides by edges
	// do this as we fill in disparities for pixels
	
	// as we move through image, skip pixels that have already had disparities calculated
	
	
	// stochastically select pixels to expand windows around
	// keep going until numPixels pixels found
	
	int numPixelsDone = 0;
	
	int windowsFound = 0;
	
	while( numPixelsDone < numPixels ) {
		
		int y = (int)(((float)(rand()) * h-2) *INV_RAND_MAX) + 1;
		int x = (int)(((float)(rand()) * w-2) *INV_RAND_MAX) + 1;
	
		i = yOffset[y] + x;
		
		if( outImage[i] != marker ) {
		
			while ( outImage[i] != marker ) {
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
				
				int rowYContrib = yOffset[rowY];
				for( int rowX = x - delW; rowX<= x + delE; rowX ++ ) {
					int rowIndex = rowYContrib + rowX;
					if( binLEdges[ rowIndex ] ) {
						// edge here, so stop
						//printf( "edge hit in north at (%d, %d)\n", );
						delNRun = false;
						}
					if( outImage[ rowIndex ] != marker ) {
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
				
				int rowYContrib = yOffset[rowY];
				for( int rowX = x - delW; rowX<= x + delE; rowX ++ ) {
					int rowIndex = rowYContrib + rowX;
					if( binLEdges[ rowIndex ] ) {
						// edge here, so stop
						delSRun = false;
						}
					if( outImage[ rowIndex ] != marker ) {
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
				
				for( int rowY = y - delN; rowY<= y + delS; rowY ++ ) {
					int rowIndex = yOffset[ rowY ] + rowX;
					if( binLEdges[ rowIndex ] ) {
						// edge here, so stop
						delWRun = false;
						}
					if( outImage[ rowIndex ] != marker ) {
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
				
				for( int rowY = y - delN; rowY<= y + delS; rowY ++ ) {
					int rowIndex = yOffset[ rowY ] + rowX;
					if( binLEdges[ rowIndex ] ) {
						// edge here, so stop
						delERun = false;
						}
					if( outImage[ rowIndex ] != marker ) {
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
		
		int costOfBestL2 = veryBig;
		int bestDisp = 0;
		
		// look at each possible displacement for this pixel and it's window
		for( int d=0; d<=maxDisparity; d++ ) {
			int thisCostL2 = 0;
			
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
				
					int valLeft = inputLImage[ windowPixInd ];
					
					int valRight = 0;
									
					if( windX - d > 0 ) {		// pixel lands in right image when moved by d
						valRight = inputRImage[ windowPixInd - d];
						}
					// else leave valRight at 0

					int diff = valLeft - valRight;
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
				outImage[ windYOffset + windX ] = bestDisp;
				}
			}
		numPixelsDone += ( ( delN + delS + 1 ) * (delE + delW + 1 ) );

		}	// end while not all pix done
	
	
	printf( "found %d windows\n", windowsFound );	
	delete [] binLEdges;
	delete [] lOrient; 	
	
	return true;
	}


