/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2000-December-14		Jason Rohrer
 * Added implementation for all member functions.
 *
 * 2001-January-10		Jason Rohrer
 * Made class serializable. 
 *
 * 2001-January-24		Jason Rohrer
 * Fixed a bug in the deserialization function.
 *
 * 2001-February-2	Jason Rohrer
 * Added M_PI definition.
 *
 * 2001-April-14   Jason Rohrer
 * Added line segment and triangle intersection tests. 
 */
 
 
#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED 
 
#include "GeometricObject3D.h"
#include "LineSegment3D.h"
#include "Triangle3D.h"

#include "minorGems/io/Serializable.h"
 
#include <math.h>
#include <stdlib.h>
 
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif 
 
/**
 * Geometric sphere (in 3-space). 
 *
 * @author Jason Rohrer 
 */
class Sphere : public GeometricObject3D, public Serializable { 
	
	public:
	
		Vector3D *mCenter;
		double mRadius;
	
		/**
		 * Constructs a Sphere.
		 *
		 * @param inCenter a vector containing the coordinates of the
		 *   sphere's center.  The vector is copied, so the caller
		 *   is responsible for deallocating the passed in vector.
		 *
		 * @param inRadius the radius of the sphere.
		 */
		Sphere( Vector3D *inCenter, double inRadius );
		
		
		/**
		 * Constructs a Sphere by copying parameters from another Sphere.
		 *
		 * @param inOtherSphere the sphere to copy.
		 */
		Sphere( Sphere *inOtherSphere );
		
		
		~Sphere();
		
		
		/**
		 * Gets whether this sphere intersects with another.
		 *
		 * @param inOtherSphere the sphere to test for intersection 
		 *   with this sphere.
		 *
		 * @return true iff this sphere intersects with the other.
		 */
		char intersects( Sphere *inOtherSphere );
		
		
		/**
		 * Gets the normalized direction of intersection of this 
		 * sphere with another.
		 *
		 * @param inOtherSphere the sphere to test for intersection 
		 *   with this sphere.
		 *
		 * @return a normalized vector direction of the intersection, 
		 *   or NULL if there is no intersection.
		 */
		Vector3D *getIntersection( Sphere *inOtherSphere );

		
		/**
		 * Gets whether a line segment intersects or is inside this sphere.
		 *
		 * @param inSegment the segment to test.
		 *
		 * @return true if the segment intersects with or is contained by
		 *   this sphere.
		 */
		char intersects( LineSegment3D *inSegment );


		/**
		 * Gets whether a triangle intesects or is inside this sphere.
		 *
		 *
		 * @param inTriangle the triangle to test.
		 *
		 * @return true if the triangle intersects with or is contained by
		 *   this sphere.
		 */
		char intersects( Triangle3D *inTriangle );
		
		
		/**
		 * Gets the volume of this sphere.
		 *
		 * @return the volume of this sphere.
		 */
		double getVolume();
		
		
		// these implement the GeometricObject3D interface
		void move( Vector3D *inVector );
		void rotate( Angle3D *inAngle );
		void reverseRotate( Angle3D *inAngle );
		void scale( double inScalar );
		GeometricObject3D *copy();
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );

	};



inline Sphere::Sphere( Vector3D *inCenter, double inRadius )
	: mCenter( new Vector3D( inCenter ) ), mRadius( inRadius ) {
	
	}



inline Sphere::Sphere( Sphere *inOtherSphere ) 
	: mCenter( new Vector3D( inOtherSphere->mCenter ) ), 
	mRadius( inOtherSphere->mRadius ) {
	
	}



inline Sphere::~Sphere() {
	delete mCenter;
	}



inline char Sphere::intersects( Sphere *inOtherSphere ) {
	double distance = mCenter->getDistance( inOtherSphere->mCenter );
	if( distance <= mRadius + inOtherSphere->mRadius ) {
		return true;
		}
	else {	
		return false;
		}
	}



inline Vector3D *Sphere::getIntersection( Sphere *inOtherSphere ) {
	if( intersects( inOtherSphere ) ) {
		Vector3D * returnVector = new Vector3D( inOtherSphere->mCenter );
		returnVector->subtract( mCenter );
		returnVector->normalize();
		return returnVector;
		}
	else {
		return NULL;
		}
	}


/*
 * These intersection algorithms were found in the following paper:
 *
 * ERIT -- A Collection of Efficient and Reliable Intersection Tests
 * by Martin Held
 * Journal of Graphics Tools
 */

inline char Sphere::intersects( LineSegment3D *inSegment ) {
	// first check if either endpoint is inside this sphere
	if( inSegment->mEndpoints[0]->getDistance( mCenter ) <= mRadius ) {
		return true;
		}
	else if( inSegment->mEndpoints[1]->getDistance( mCenter ) <= mRadius ) {
		return true;
		}
	else {
		// both endpoints outside

		// project center onto the line
		Vector3D *projectedCenter = inSegment->projectPoint( mCenter );
		if( projectedCenter == NULL ) {
			// projection is outside of the segment
			return false;
			}
		else {
			// projection is inside of the segment
			// check distance from center
			double distance = projectedCenter->getDistance( mCenter );
			delete projectedCenter;
			
			if( distance <= mRadius ) {
				return true;
				}
			else {
				return false;
				}
			}
		}
	}



inline char Sphere::intersects( Triangle3D *inTriangle ) {
	char intersectionFound;
	
	// first, check each triangle edge segment for intersection
	LineSegment3D *segmentA = inTriangle->getLineSegment( 0 );
    intersectionFound = intersects( segmentA );
	delete segmentA;
	if( intersectionFound ) {
		return true;
		}
	LineSegment3D *segmentB = inTriangle->getLineSegment( 1 );
    intersectionFound = intersects( segmentB );
	delete segmentB;
	if( intersectionFound ) {
		return true;
		}
	LineSegment3D *segmentC = inTriangle->getLineSegment( 2 );
    intersectionFound = intersects( segmentC );
	delete segmentC;
	if( intersectionFound ) {
		return true;
		}

	// if we're at this point, none of the edges intersected.

	// we still need to check if the interior region of the triangle
	// is inside the sphere

	Vector3D *projectedCenter = inTriangle->projectOntoPlane( mCenter );

	if( inTriangle->isInBounds( projectedCenter ) ) {
		if( projectedCenter->getDistance( mCenter ) <= mRadius ) {
			// the projected center is inside the triangle and
			// inside the sphere
			intersectionFound = true;
			}
		else {
			// the projected center is inside the triangle, but outside
			// the sphere
			intersectionFound = false;
			}
		}
	else {
		// center projection not even in triangle bounds, so no intersection
		intersectionFound = false;
		}
	delete projectedCenter;

	return intersectionFound;
	}



inline double Sphere::getVolume() {
	return (4/3) * M_PI * mRadius * mRadius * mRadius;
	}



inline void Sphere::move( Vector3D *inVector ) {
	mCenter->add( inVector );
	}
	
	
	
inline void Sphere::rotate( Angle3D *inAngle ) {
	mCenter->rotate( inAngle );
	}
	
	
	
inline void Sphere::reverseRotate( Angle3D *inAngle ) {
	mCenter->reverseRotate( inAngle );
	}


	
inline void Sphere::scale( double inScalar ) {
	mCenter->scale( inScalar );
	mRadius *= inScalar;
	}
	
	
	
inline GeometricObject3D *Sphere::copy() {
	Sphere *copiedSphere = new Sphere( this );
	return (GeometricObject3D*)copiedSphere;
	}



inline int Sphere::serialize( OutputStream *inOutputStream ) {
	int numBytesWritten = 0;
	
	numBytesWritten += mCenter->serialize( inOutputStream );

	unsigned char* doublePointer = (unsigned char *)( &mRadius );
	numBytesWritten += inOutputStream->write( doublePointer, sizeof( double ) );

	return numBytesWritten;
	}
	
	
	
inline int Sphere::deserialize( InputStream *inInputStream ) {
	int numBytesRead = 0;
	
	numBytesRead += mCenter->deserialize( inInputStream );

	unsigned char* doublePointer = (unsigned char *)( &mRadius );
	numBytesRead += inInputStream->read( doublePointer, sizeof( double ) );
	
	return numBytesRead;
	}
	
	
	
#endif
