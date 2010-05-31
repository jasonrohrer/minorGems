/*
 * Modification History
 *
 * 2000-November-30		Jason Rohrer
 * Created.
 */


#include "minorGems/graphics/ScreenGraphics.h"
#include "minorGems/graphics/GraphicBuffer.h"
#include "minorGems/system/Thread.h"

#include "minorGems/util/random/StdRandomSource.h"


#include <signal.h>
#include <stdio.h>
#include <string.h>

char running = true;

void catch_int(int sig_num) {
	if( running ) {
		printf( "Quiting...\n" );
		running = false;
		signal( SIGINT, catch_int);
		}
	else {
		exit(1);
		}
	}


int width = 50;
int height = 50;

int pixelSize = 10;

int screenWidth = pixelSize * width;
int screenHeight = pixelSize * height;


int numPixels = screenWidth * screenHeight;
int numGridSpaces = width * height;

unsigned long *pixels;

unsigned long *grid;


void setPixelsFromGrid() {
	for( int x=0; x<screenWidth; x++ ) {
		for( int y=0; y<screenHeight; y++ ) {
			int gridX = x / pixelSize;
			int gridY = y / pixelSize;

			pixels[ y * screenWidth + x ] =
				grid[ gridY * width + gridX ];
			}
		}
	}

int main() {
	signal( SIGINT, catch_int);
	
	int i, j;

	StdRandomSource *randSource = new StdRandomSource();
	
	ScreenGraphics *screen = new ScreenGraphics( screenWidth, screenHeight );
	
	pixels = new unsigned long[ numPixels ];	

	grid = new unsigned long[ numGridSpaces ];

	for( i=0; i<numGridSpaces; i++ ) {
		// random
		/*
		if( randSource->getRandomFloat() > 0.999 ) {
			grid[i] = randSource->getRandomInt();
			}
		else {
			grid[i] = 0;
			}
		*/
		grid[i] = 0;
		}
	int color = randSource->getRandomInt();

	grid[ 10 * width + 10 ] = color;
	//color = randSource->getRandomInt();
	//grid[ 10 * width + 11 ] = color;
	//color = randSource->getRandomInt();
	grid[ 10 * width + 12 ] = color;

	//color = randSource->getRandomInt();
	//grid[ 11 * width + 11 ] = color;

	//color = randSource->getRandomInt();
	//grid[ 11 * width + 9 ] = color;

	//color = randSource->getRandomInt();
	//grid[ 11 * width + 13 ] = color;
	/*
	for( i=0; i<height; i++ ) {
		grid[ i * width + 10 ] = color;
		}
	color = randSource->getRandomInt();
	for( i=0; i<width; i++ ) {
		grid[ 10 * width + i ] = color;
		}
	color = randSource->getRandomInt();
	for( i=0; i<height; i++ ) {
		grid[ i * width + 11 ] = color;
		}
	*/
	
	
	// set one seed
	//grid[ randSource->getRandomBoundedInt( 0, numGridSpaces-1 ) ] =
	//	randSource->getRandomInt();

	//grid[ 10 * width + 10 ] = 0x00FF8080;
	//grid[ 20 * width + 10 ] = 0x0080FF80;
	//grid[ 30 * width + 10 ] = 0x008080FF;
	
	setPixelsFromGrid();
	
	GraphicBuffer *screenBuffer = new GraphicBuffer( pixels, 
		screenHeight, screenWidth );
	
	screen->swapBuffers( screenBuffer );

	int matrixRadius = 1;
	int matrixSize = 2 * matrixRadius + 1;
	int matrixCenterIndex = matrixRadius;
	
	double **matrix = new double*[matrixSize];
	
	for( i=0; i<matrixSize; i++ ) {
		matrix[i] = new double[matrixSize];
		}
	/*
	matrix[0][0] = 0;
	matrix[0][1] = -1;
	matrix[0][2] = 4;

	matrix[1][0] = 5;	
	matrix[1][1] = 1;
	matrix[1][2] = -3;
	
	matrix[2][0] = -2;
	matrix[2][1] = 3;
	matrix[2][2] = 2;
	
	
	matrix[0][0] = 4;
	matrix[0][1] = 3;
	matrix[0][2] = 14;

	matrix[1][0] = -3;	
	matrix[1][1] = -2;
	matrix[1][2] = 7;
	
	matrix[2][0] = -11;
	matrix[2][1] = -1;
	matrix[2][2] = -7;
	

	matrix[0][0] = 4;
	matrix[0][1] = 3;
	matrix[0][2] = 14;

	matrix[1][0] = 3;	
	matrix[1][1] = 2;
	matrix[1][2] = 7;
	
	matrix[2][0] = 11;
	matrix[2][1] = 1;
	matrix[2][2] = 7;
	*/

	// zero-sum magic square
	matrix[0][0] = -1;
	matrix[0][1] = -2;
	matrix[0][2] = 3;

	matrix[1][0] = 4;	
	matrix[1][1] = 0;
	matrix[1][2] = -4;
	
	matrix[2][0] = -3;
	matrix[2][1] = 2;
	matrix[2][2] = 1;
	
	// normalize the matrix
	// if the matrix doesn't sum to 1, then
	// the cell field will brown out or wash out
	double sum = 0;
	for( i=0; i<matrixSize; i++ ) {
		for( j=0; j<matrixSize; j++ ) {
			sum += matrix[i][j];
			}
		}
	for( i=0; i<matrixSize; i++ ) {
		for( j=0; j<matrixSize; j++ ) {
			//matrix[i][j] = matrix[i][j] / sum;
			}
		}
	

	while( running ) {
		int x,y;
		
		unsigned long *newGrid = new unsigned long[ numGridSpaces ];
		memcpy( newGrid, grid, sizeof( unsigned long ) * numGridSpaces );

		for( x=0; x<width; x++ ) {
			for( y=0; y<height; y++ ) {

				int p = y * width + x;
				

				double newGridRed = 0;
				double newGridGreen = 0;
				double newGridBlue = 0;
				
				// sum of matrix times corresponding grid spaces
				for( i=0; i<matrixSize; i++ ) {
					for( j=0; j<matrixSize; j++ ) {

						int gridX = x + i - matrixRadius;
						int gridY = y + j - matrixRadius;

						gridX = gridX % width;
						gridY = gridY % height;

						// handle negative wrap around
						if( gridX < 0 ) {
							gridX = width + gridX;
							}
						if( gridY < 0 ) {
							gridY = height + gridY;
							}
						
						unsigned long gridVal =
							grid[ gridY * width + gridX ];
						unsigned long gridRed =
							gridVal >> 16 & 0xFF;
						unsigned long gridGreen =
							gridVal >> 8 & 0xFF;
						unsigned long gridBlue =
							gridVal & 0xFF;

						double matrixVal = matrix[i][j];
						
						newGridRed += gridRed * matrixVal;
						newGridGreen += gridGreen * matrixVal;
						newGridBlue += gridBlue * matrixVal;				
						}
					}
				// threshold colors at 0 and 255
				/*
				if( newGridRed < 0 ) {
					newGridRed = 0;
					}
				if( newGridRed > 255 ) {
					newGridRed = 255;
					}
				if( newGridGreen < 0 ) {
					newGridGreen = 0;
					}
				if( newGridGreen > 255 ) {
					newGridGreen = 255;
					}
				if( newGridBlue < 0 ) {
					newGridBlue = 0;
					}
				if( newGridBlue > 255 ) {
					newGridBlue = 255;
					}
				*/
				int newRedInt = ( (int)newGridRed ) % 256;
				int newGreenInt = ( (int)newGridGreen ) % 256;
				int newBlueInt = ( (int)newGridBlue ) % 256;

				if( newRedInt < 0 ) {
					newRedInt += 256;
					}
				if( newGreenInt < 0 ) {
					newGreenInt += 256;
					}
				if( newBlueInt < 0 ) {
					newBlueInt += 256;
					}
				
				newGrid[ p ] =
					newRedInt << 16 |
					newGreenInt << 8 |
					newBlueInt;
				
				}
			}

		memcpy( grid, newGrid, sizeof( unsigned long ) * numGridSpaces );

		delete [] newGrid;

		setPixelsFromGrid();
		
		screen->swapBuffers( screenBuffer );

		//Thread::sleep( 1000 );
		}


	delete screenBuffer;
	delete screen;

	delete [] pixels;
	delete [] grid;
	
	for( i=0; i<matrixSize; i++ ) {
		delete [] matrix[i];
		}
	delete [] matrix;

	delete randSource;
	
	printf( "Done.\n" );
	
	return 0;
	}
