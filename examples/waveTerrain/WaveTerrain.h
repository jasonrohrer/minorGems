/*
 * Modification History
 *
 * 2001-April-20   Jason Rohrer
 * Created.  
 */
 
 
#ifndef WAVE_TERRAIN_INCLUDED
#define WAVE_TERRAIN_INCLUDED 

#include <stdio.h>

/**
 * Class that implements general interpolated wave terrain synthesis.
 *
 * @author Jason Rohrer.
 */
class WaveTerrain {
	public:

		/**
		 * Constructs a wave terrain, specifying a terrain height
		 * map.
		 *
		 * @param inHeightMap a two-dimensional array of height values,
		 *   each value in [0,1].  Is indexed as inTerrain[inHeight][inWidth].
		 *   Is destroyed when this class is destroyed.
		 * @param inWidth the width of the height array.
		 * @param inHeight the height of the height array.
		 */
		WaveTerrain( double **inHeightMap, int inWidth, int inHeight );

		~WaveTerrain();


		/**
		 * Gets the height map used by this terrain.
		 *
		 * @param outWidth a pointer to where the width of the map
		 *   will be returned.
		 * @param outHeight a pointer to where the height of the map
		 *   will be returned.
		 *
		 * @return the two-dimensional height map used by this terrain.
		 *   Is not a copy, and will be destroyed when this class is destroyed.
   		 */ 
		double **getHeightMap( int *outWidth, int *outHeight );



		/**
		 * Gets a sample from a position on the wave terrain.
		 *
		 * @param inX the x coordinate of the sample, in [0,1].
		 * @param inY the y coordinate of the sample, in [0,1].
		 *
		 * @return a sample from the terrain, in [0,1];
		 */
		double getSample( double inX, double inY );

		
	private:

		int mWidth;
		int mHeight;

		double **mHeightMap;
	};



inline WaveTerrain::WaveTerrain( double **inHeightMap,
								 int inWidth, int inHeight )
	: mHeightMap( inHeightMap ), mWidth( inWidth ), mHeight( inHeight ) {


	}



inline WaveTerrain::~WaveTerrain() {
	for( int y=0; y<mHeight; y++ ) {
		delete [] mHeightMap[y];
		}
	delete [] mHeightMap;
	}



inline double **WaveTerrain::getHeightMap( int *outWidth, int *outHeight ) {
	*outWidth = mWidth;
	*outHeight = mHeight;

	return mHeightMap;
	}



inline double WaveTerrain::getSample( double inX, double inY ) {
	// use bilinear interpolation for now

	// find closest integer coords above and below the passed in coords
	int lowX = (int)( inX * ( mWidth - 2 ) );
	int lowY = (int)( inY * ( mHeight - 2 ) );
	
	int highX = lowX + 1;
	int highY = lowY + 1;


	// weights for the high coords
	double highXWeight = ( inX * ( mWidth - 2 ) ) - lowX;
	double highYWeight = ( inY * ( mHeight - 2 ) ) - lowY;

	// average of two points with lowY coordinate
	double lowYAverage =
		( highXWeight * mHeightMap[lowY][highX] )
		+ ( ( 1 - highXWeight ) * mHeightMap[lowY][lowX] );

	// average of two points with highY coordinate
	double highYAverage =
		( highXWeight * mHeightMap[highY][highX] )
		+ ( ( 1 - highXWeight ) * mHeightMap[highY][lowX] );

	
	// average in y direction
	double returnValue =
		( highYWeight * highYAverage )
		+ ( ( 1 - highYWeight ) * lowYAverage );

	if( returnValue > 1.0 || returnValue < 0 ) {
		printf( "out of range\n" );
		}
	
	return returnValue;
	}



#endif
