// Jason Rohrer
// Noise.cpp

/**
*
*	Noise generation implementation
*
*
*	Created 11-3-99
*	Mods:	
*		Jason Rohrer	12-20-2000	Changed genFractalNoise2d function to make
*									it less blocky.
*
*/


#include "Noise.h"


// fills 2d image with ARGB noise
void genRandNoise2d(unsigned long *buff, int buffHigh, int buffWide) {
	
	int *yOffset = new int[buffHigh];
	
	// precalc y offsets
	for( int y=0; y<buffHigh; y++) {
		yOffset[y] = buffWide*y;
		}
	
	
	int red = (int)(255 * floatRand());
	int green = (int)(255 *  floatRand());
	int blue = (int)(255 *  floatRand());
	int alpha = (int)(255 *  floatRand());
	
	for( int y=0; y<buffHigh; y++) {
		int yContrib = yOffset[y];
		
		
		
		for( int x=0; x<buffWide; x++) {
		
		
/*			int red = (int)(255 * floatRand());
			int green = (int)(255 *  floatRand());
			int blue = (int)(255 *  floatRand());
			int alpha = (int)(255 *  floatRand());
*/			
			buff[ yContrib + x] = blue | green << 8 | red << 16 | alpha << 24;
		
			}
		}
		
	delete [] yOffset;
	}


// fills 2d image with ARGB fractal noise
void genFractalNoise2d(unsigned long *buff, int buffHigh, int buffWide) {
	
	int *yOffset = new int[buffHigh];
		
	// precalc y offsets
	for( int y=0; y<buffHigh; y++) {
		yOffset[y] = buffWide*y;
		}
	
	// first, zero out the buffer
	
	for( int y=0; y<buffHigh; y++) {
		int yContrib = yOffset[y];
		for( int x=0; x<buffWide; x++) {		
			buff[ yContrib + x] = 127 | 127 << 8 | 127 << 16 | 127 << 24;
			}
		}
	
	
	// walk through each frequency....
	// weight in sum of frequencies by 1/f
	
	// frequency in cycles per buffer width
	// stop when frequency == buffer width
	
	for( int f=1; f<=buffWide; f++) {		// for each frequency
		
		float weight = 1 / (float)(f);
		
		// number of pixels in each cycle
		int blockWide = (int)(buffWide * weight );	
		int blockHigh = (int)(buffHigh * weight );
		
		

		int buffY = 0;
		
		while( buffY < buffHigh ) {
			
			int startY = buffY;
			
			int buffX = 0;
			while( buffX < buffWide ) {
				
				buffY = startY;
				int startX = buffX;
				
				// color components for this block
				// weighted by frequency
				int red = (int)((255 * floatRand() - 127) * weight);
				int green = (int)((255 * floatRand() - 127) * weight);
				int blue = (int)((255 * floatRand() - 127) * weight);
				int alpha = (int)((255 * floatRand() - 127) * weight);
				
				
				int blockY = 0;
				while( blockY < blockHigh) {
					int blockX = 0;
					buffX = startX;
					while( blockX < blockWide) {
						
						unsigned long buffARGB = buff[ yOffset[buffY] + buffX];
						int buffAlpha = (buffARGB >> 24 & 0xFF) + alpha;
						int buffRed = (buffARGB >> 16 & 0xFF) + red;
						int buffGreen = (buffARGB >> 8 & 0xFF) + green;
						int buffBlue = (buffARGB & 0xFF) + blue;
						
						
						if( buffAlpha < 0) buffAlpha = 0;
						if( buffRed < 0) buffRed = 0;
						if( buffGreen < 0) buffGreen = 0;
						if( buffBlue < 0) buffBlue = 0;
						
						if( buffAlpha > 255) buffAlpha = 255;
						if( buffRed > 255) buffRed = 255;
						if( buffGreen > 255) buffGreen = 255;
						if( buffBlue > 255) buffBlue = 255;
						
						
						buff[ yOffset[buffY] + buffX] = buffBlue | buffGreen << 8 | buffRed << 16 | buffAlpha << 24;
						
						buffX++;
						blockX++;
						if( buffX >= buffWide ) blockX = blockWide;	// if this block hangs outside buffer
					
						}
					buffY++;
					blockY++;
					if( buffY >= buffHigh ) blockY = blockHigh;	// if this block hangs outside buffer
					
					}
				buffX = startX + blockWide;
				}
			buffY = startY + blockHigh;
			}
		}
		
	delete [] yOffset;
	}



void genFractalNoise2d( double *inBuffer, int inWidth, int inMaxFrequency, 
	double inFPower, char inInterpolate, RandomSource *inRandSource ) {
	
	RandomSource *r = inRandSource;
	
	int w = inWidth;
	
	int i, x, y, f;
	
	int numPoints = w * w;
	
	// first, fill surface with a uniform 0.5 value
	for( i=0; i<numPoints; i++ ) {
		inBuffer[i] = 0.5;
		}
		
	// for each frequency
	for( f=2; f<=inMaxFrequency; f = f * 2 ) {
		double weight = 1.0 / pow( f, inFPower );
		
		int blockSize = (int)( (double)w / (double)f + 1.0 );
		
		// one extra block to handle boundary case where x or y is 0
		int numBlocks = (f+1) * (f+1);
		double *blockValues = new double[ numBlocks ];
		
		// assign a random value to each block
		for( i=0; i<numBlocks; i++ ) {
			blockValues[i] = ( 2 * r->getRandomDouble() - 1 ) * weight;
			}
		
		// now walk though 2d array and perform 
		// bilinear interpolation between blocks
		for( y=0; y<w; y++ ) {
			// handle boundary case by skipping first row of blocks
			int yBlock = y / blockSize + 1;
			double yWeight; 
			yWeight = (double)(y % blockSize) / blockSize;

			
			for( x=0; x<w; x++ ) {
				// handle boundary case by skipping first column of blocks
				int xBlock = x / blockSize + 1;
				double xWeight; 
				xWeight = (double)(x % blockSize) / blockSize;

				
				// if interpolating take weighted sum with previous blocks
				double value; 
				if( inInterpolate ) {
					value =
						xWeight *
							( yWeight * 
								blockValues[ yBlock * f + xBlock ] +
							(1-yWeight) * 
								blockValues[ (yBlock-1) * f + xBlock ] ) +
						(1-xWeight) *
							( yWeight * 
								blockValues[ yBlock * f + xBlock - 1 ] +
							(1-yWeight) * 
								blockValues[ (yBlock-1) * f + xBlock - 1 ] );
					}
				else {
					value =
						blockValues[ yBlock * f + xBlock ];
					}
					
				// modulate current value by new value
				inBuffer[ y * w + x ] += value;
				
				// clip values as we go along
				if(	inBuffer[y * w + x] > 1.0 ) {
					inBuffer[y * w + x] = 1.0;
					}
				else if( inBuffer[y * w + x] < 0.0 ) {
					inBuffer[y * w + x] = 0.0;
					}
				}
			}
		
		delete [] blockValues;
		}	
	}
	

void genFractalNoise( double *inBuffer, int inWidth, int inMaxFrequency,
	double inFPower, char inInterpolate, RandomSource *inRandSource ) {
	
	RandomSource *r = inRandSource;
	
	int w = inWidth;
	
	int i, x, f;
	
	// first, fill array with uniform 0.5 values
	for( i=0; i<w; i++ ) {
		inBuffer[i] = 0.5;
		}
		
	// for each frequency
	for( f=2; f<=inMaxFrequency; f = f * 2 ) {
		double weight = 1.0 / pow( f, inFPower );
		
		int blockSize = (int)( (double)w / (double)f + 1.0 );
		
		// one extra block to handle boundary case where x is 0
		int numBlocks = (f+1);
		double *blockValues = new double[ numBlocks ];
		
		// assign a random value to each block
		for( i=0; i<numBlocks; i++ ) {
			blockValues[i] = ( 2 * r->getRandomDouble() - 1 ) * weight;
			}
		
		// now walk though array and perform linear interpolation between blocks

		for( x=0; x<w; x++ ) {
			// handle boundary case by skipping first column of blocks
			int xBlock = x / blockSize + 1;
			double xWeight; 
			xWeight = (double)(x % blockSize) / blockSize;


			// take weighted sum with previous blocks, but watch for
			// boundary cases
			double value; 
			if( inInterpolate ) {
				value =
					xWeight * blockValues[ xBlock ] +
					(1-xWeight) * blockValues[ xBlock - 1 ];
				}
			else {
				value =
					blockValues[ xBlock ];
				}

			// modulate current value by new value
			inBuffer[ x ] += value;

			// clip values as we go along
			if(	inBuffer[x] > 1.0 ) {
				inBuffer[x] = 1.0;
				}
			else if( inBuffer[x] < 0.0 ) {
				inBuffer[x] = 0.0;
				}
			}
		
		delete [] blockValues;
		}
	
	}	
