/*
 * Modification History
 *
 * 2001-April-22   Jason Rohrer
 * Created.  
 */
 
 
#ifndef EPICYCLE_TERRAIN_PATH_INCLUDED
#define EPICYCLE_TERRAIN_PATH_INCLUDED 

#include "TerrainPath.h"

#include <math.h>
#include <stdio.h>

/**
 * Centered epicycle path implementation of a TerrainPath.
 *
 * An epicycle is the curve that results from rotating
 * a minor circle as it orbits on a circular path.
 *
 * A complete major circle traversal is made every 1.0 time unit.
 *
 * @author Jason Rohrer.
 */
class EpicycleTerrainPath : public TerrainPath {
	public:

		/**
		 * Constructs a epicycle path.  Path is centered at (0.5, 0.5).
		 *
		 * @param inRadius the maximum radius of the orbit sum,
		 *   in [0, 0.5].
		 * @param inRadiusRatio the ratio of minor orbit radius
		 *   to major orbit radius, in [0, DBL_MAX].
		 * @param inRotationRatio the ratio of rotation between the
		 *   minor and major orbits, in [0, DBL_MAX].  This is essentially
		 *   the number of minor rotations per major rotation.
		 * @param inRotationOffset how much the epicyle is rotated
		 *   from normal orientation, in [DBL_MIN, DBL_MAX].
		 */
		EpicycleTerrainPath( double inRadius,
							 double inRadiusRatio,
							 double inRotationRatio,
							 double inRotationOffset );

		
		/**
		 * Sets the maximum radius of the epicycle's orbit sum.
		 *
		 * @param inRadius the maximum radius of the orbit
		 *   sum, in [0, 0.5].
		 */
		void setRadius( double inRadius );


		/**
		 * Gets the maximum radius of the epicycle's orbit sum.
		 *
		 * @return the maximum radius of the orbit sum, in [0, 0.5].
		 */
		double getRadius();
		

		/**
		 * Sets the orbit ratio.
		 *
		 * @param inRadiusRatio the ratio of minor orbit radius
		 *   to major orbit radius, in [0, DBL_MAX].
		 */
		void setRadiusRatio( double inRadiusRatio );


		/**
		 * Gets the orbit ratio.
		 *
		 * @return the ratio of minor orbit radius
		 *   to major orbit radius, in [0, DBL_MAX].
		 */
		double getRadiusRatio();

		
		/**
		 * Sets the rotation rate ratio.
		 *
		 * @param inRotationRatio the ratio of minor orbit rotation rate
		 *   to major orbit rotation rate, in [0, DBL_MAX].
		 */
		void setRotationRatio( double inRotationRatio );


		/**
		 * Gets the rotation rate ratio.
		 *
		 * @return the ratio of minor orbit rotation rate
		 *   to major orbit rotation rate, in [0, DBL_MAX].
		 */
		double getRotationRatio();
		

		/**
		 * Sets the rotation offset of the epicycle.
		 *
		 * @param inOffset the rotation offset in [DBL_MIN, DBL_MAX].
		 */
		void setRotationOffset( double inOffset );

		
		/**
		 * Gets the rotation offset of the epicycle.
		 *
		 * @return the rotation offset in [DBL_MIN, DBL_MAX].
		 */
		double getRotationOffset();
		

		
		// implements the TerrainPath interface
		virtual void getPosition( double inTime,
								  double *outX,
								  double *outY );

	private:

		double mRadius;
		double mRadiusRatio;
		double mRotationRatio;

		double mRotationOffset;
		
		// optimization:  keep a precomputed value
		double mMinorRadiusFraction;
		
		/**
		 * Checks whether the current parameter values are in range.
		 *
		 * If a value is found to be out of range, an error message
		 * is printed and the value is corrected.
		 */
		void checkValues();
	};



inline EpicycleTerrainPath::EpicycleTerrainPath( double inRadius,
												 double inRadiusRatio,
												 double inRotationRatio,
												 double inRotationOffset )
	: mRadius( inRadius ),
	  mRadiusRatio( inRadiusRatio ),
	  mRotationRatio( inRotationRatio ),
	  mRotationOffset( 0.5 ) {

	// optimization:  keep a precomputed value
	mMinorRadiusFraction = mRadiusRatio / ( mRadiusRatio + 1 );
	
	checkValues();
	}



inline void EpicycleTerrainPath::setRadius( double inRadius ) {
	mRadius = inRadius;

	// optimization:  skip this check
	//checkValues();
	}



inline double EpicycleTerrainPath::getRadius() {
	return mRadius;
	}



inline void EpicycleTerrainPath::setRadiusRatio(  double inRadiusRatio ) {
	mRadiusRatio = inRadiusRatio;

	// optimization:  skip this check
	//checkValues();

	// optimization:  keep a precomputed value
	mMinorRadiusFraction = mRadiusRatio / ( mRadiusRatio + 1 );
	}



inline double EpicycleTerrainPath::getRadiusRatio() {
	return mRadiusRatio;
	}



inline void EpicycleTerrainPath::setRotationRatio(
	double inRotationRatio ) {

	mRotationRatio = inRotationRatio;

	// optimization:  skip this check
	//checkValues();
	}



inline double EpicycleTerrainPath::getRotationRatio() {
	return mRotationRatio;
	}



inline void EpicycleTerrainPath::setRotationOffset( double inOffset ) {
	mRotationOffset = inOffset;
	}



inline double EpicycleTerrainPath::getRotationOffset() {
	return mRotationOffset;
	}



inline void EpicycleTerrainPath::getPosition( double inTime,
											  double *outX,
											  double *outY ) {

	double minorRadius = mRadius * mMinorRadiusFraction;
	double majorRadius = mRadius - minorRadius;

	// optimization:  precompute these
	double majorScaledTime =
		2 * ( inTime + mRotationOffset ) * M_PI;
	double minorScaledTime =
		2 * ( inTime ) * M_PI * mRotationRatio;
	
	// first, rotate around the minor orbit
	double minorX =
		( cos( minorScaledTime ) * minorRadius );
	double minorY =
		( sin( minorScaledTime ) * minorRadius );

	// compute the major contribution
	double majorX =
		( cos( majorScaledTime ) * majorRadius );
	double majorY =
		( sin( majorScaledTime ) * majorRadius );
	
	// now translate by the major orbit contribution
	// also, offset by 0.5 to center the epicycle
	*outX = minorX + majorX + 0.5;
	*outY = minorY + majorY + 0.5;
	}



inline void EpicycleTerrainPath::checkValues() {
	if( mRadius > 0.5 || mRadius < 0 ) {
		printf(
			"Radius of a EpicycleTerrainPath must be in [0, 0.5]\n" );

		if( mRadius > 0.5 ) {
			mRadius = 0.5;
			}
		else if( mRadius < 0 ) {
			mRadius = 0;
			}
		}

	if( mRadiusRatio < 0 ) {
		printf(
			"Radius ratio of a EpicycleTerrainPath must be" );
		printf( " in [0, DBL_MAX]\n" );

		mRadiusRatio = 0;
		}

	if( mRotationRatio < 0 ) {
		printf(
			"Rotation ratio of a EpicycleTerrainPath must be" );
		printf( " in [0, DBL_MAX]\n" );

		mRotationRatio = 0;
		}
	
	}



#endif
