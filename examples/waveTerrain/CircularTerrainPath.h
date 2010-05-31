/*
 * Modification History
 *
 * 2001-April-20   Jason Rohrer
 * Created.
 *
 * 2001-April-22   Jason Rohrer
 * Fixed radius range checking.
 */
 
 
#ifndef CIRCULAR_TERRAIN_PATH_INCLUDED
#define CIRCULAR_TERRAIN_PATH_INCLUDED 

#include "TerrainPath.h"

#include <math.h>
#include <stdio.h>

/**
 * Centered Circular path implementation of a TerrainPath.
 *
 * A complete circle traversal is made every 1.0 time unit.
 *
 * @author Jason Rohrer.
 */
class CircularTerrainPath : public TerrainPath {
	public:

		/**
		 * Constructs a circular path.  Path is centered at (0.5, 0.5).
		 *
		 * @param inRadius the radius of the circle, in [0, 0.5].
		 */
		CircularTerrainPath( double inRadius );


		/**
		 * Sets the radius of the circle.
		 *
		 * @param inRadius the radius of the circle, in [0, 0.5].
		 */
		void setRadius( double inRadius );


		/**
		 * Gets the radius of the circle.
		 *
		 * @return the radius of the circle, in [0, 0.5].
		 */
		double getRadius();


		// implements the TerrainPath interface
		virtual void getPosition( double inTime,
								  double *outX,
								  double *outY );

	private:

		double mRadius;


		/**
		 * Checks whether the current radius is in the range [0, 0.5].
		 *
		 * If radius is found to be out of range, an error message
		 * is printed and the radius is corrected.
		 */
		void checkRadius();
	};



inline CircularTerrainPath::CircularTerrainPath( double inRadius )
	: mRadius( inRadius ) {

	checkRadius();
	}



inline void CircularTerrainPath::setRadius( double inRadius ) {
	mRadius = inRadius;
	checkRadius();
	}



inline double CircularTerrainPath::getRadius() {
	return mRadius;
	}



inline void CircularTerrainPath::getPosition( double inTime,
												  double *outX,
												  double *outY ) {
	
	*outX = ( cos( inTime * 2 * M_PI ) * mRadius ) + 0.5;
	*outY = ( sin( inTime * 2 * M_PI ) * mRadius ) + 0.5;
	}



inline void CircularTerrainPath::checkRadius() {
	if( mRadius > 0.5 || mRadius < 0 ) {
		printf(
			"Radius of a CircularTerrainPath must be in [0, 0.5]\n" );

		if( mRadius > 0.5 ) {
			mRadius = 0.5;
			}
		else if( mRadius < 0 ) {
			mRadius = 0;
			}
		}
	}



#endif
