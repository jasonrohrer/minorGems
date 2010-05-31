/*
 * Modification History
 *
 * 2001-April-14   Jason Rohrer
 * Created.     
 *
 * 2005-February-15   Jason Rohrer
 * Made destructor virtual to quell warnings.
 */
 
 
#ifndef TRIANGLE_3D_INCLUDED
#define TRIANGLE_3D_INCLUDED 
 
#include "GeometricObject3D.h"
#include "LineSegment3D.h"

#include "minorGems/io/Serializable.h"
 
#include <math.h>
#include <stdlib.h>
 
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif 
 
/**
 * Triangle (in 3-space). 
 *
 * @author Jason Rohrer 
 */
class Triangle3D : public GeometricObject3D, public Serializable { 
	
	public:
	
		Vector3D *mPoints[3];

		/**
		 * Constructs a triangle.
		 *
		 * The vectors are copied, so the caller
		 * is responsible for deallocating the passed in vectors.
		 *
		 * @param inPointA the first point.
		 * @param inPointB the second point.
		 * @param inPointC the third point.
		 *
		 * The "front" face of this trangle is defined as the face
		 * viewed when the above vertices are in counter-clockwise order.
		 */
		Triangle3D( Vector3D *inPointA, Vector3D *inPointB,
					Vector3D *inPointC);
		
		
		/**
		 * Constructs a triangle by copying parameters
		 * from another triangle.
		 *
		 * @param inOtherTriangle the triangle to copy.
		 */
		Triangle3D( Triangle3D *inOtherTriangle );
		
		
		virtual ~Triangle3D();

		/**
		 * Gets a line segment from this triangle.
		 *
		 * @param inIndex the line segment to get, in [0,2].
		 *
		 * @return a new line segment, or NULL if the index is out of range.
		 *   Must be destroyed by caller.
		 */
		LineSegment3D *getLineSegment( int inIndex );
		

		/**
		 * Gets the normal for the plane containing this triangle.
		 *
		 * @return the normal vector.  Must be destroyed by caller.
		 */
		Vector3D *getNormal();

		
		/**
		 * Computes the normal projection of a point onto the plane
		 * defined by this triangle.  A point is returned even
		 * if it is not inside the bounds of the triangle.
		 *
		 * @param inPoint the point to project.  Must be destroyed by caller.
		 *
		 * @return a new vector representing the projection of the point.
		 *   Must be destroyed by caller.
		 */
		Vector3D *projectOntoPlane( Vector3D *inPoint );


		/**
		 * Checks if a point is inside the bounds of this triangle.
		 *
		 * @param inPoint the point to check.  Must be destroyed by caller.
		 *
		 * @return true iff the point is in bounds.
		 */
		char isInBounds( Vector3D *inPoint );
		
		
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



inline Triangle3D::Triangle3D( Vector3D *inEndpointA,
							   Vector3D *inEndpointB,
							   Vector3D *inEndpointC ) {

	mPoints[0] = new Vector3D( inEndpointA );
	mPoints[1] = new Vector3D( inEndpointB );
	mPoints[2] = new Vector3D( inEndpointC );
	
	}



inline Triangle3D::Triangle3D( Triangle3D *inOtherTriangle ) {
	
	mPoints[0] = new Vector3D( inOtherTriangle->mPoints[0] );
	mPoints[1] = new Vector3D( inOtherTriangle->mPoints[1] );
	mPoints[2] = new Vector3D( inOtherTriangle->mPoints[2] );

	}



inline Triangle3D::~Triangle3D() {
	delete mPoints[0];
	delete mPoints[1];
	delete mPoints[2];
	}



inline LineSegment3D *Triangle3D::getLineSegment( int inIndex ) {
	if( inIndex < 0 || inIndex > 2 ) {
		return NULL;
		}
	else {
		// can pass in mPoints directly, since constructor copies them
		return new LineSegment3D( mPoints[inIndex],
								  mPoints[ ( inIndex + 1 ) % 2 ] );
		}
	}



inline Vector3D *Triangle3D::getNormal() {
	// cross C-B with A-B to get a normal pointing towards
	// the viewer when seeing the points in counter-clockwise order

	Vector3D *firstLeg = new Vector3D( mPoints[2] );
	firstLeg->subtract( mPoints[1] );

	Vector3D *secondLeg = new Vector3D( mPoints[0] );
	secondLeg->subtract( mPoints[1] );

	Vector3D *normal = firstLeg->cross( secondLeg );

	normal->normalize();
	
	delete firstLeg;
	delete secondLeg;

	return normal;
	}



inline Vector3D *Triangle3D::projectOntoPlane( Vector3D *inPoint ) {

	// formula found at:
	// http://astronomy.swin.edu.au/pbourke/geometry/pointplane/
	//minimum distance =
	//(A (xa - xb) + B (ya - yb) + C (za - zb)) / sqrt(A^2 + B^2 + C^2)

	Vector3D *normal = getNormal();
	
	double minDistance =
		normal->mX * ( inPoint->mX - mPoints[0]->mX ) +
		normal->mY * ( inPoint->mY - mPoints[0]->mY ) +
		normal->mZ * ( inPoint->mZ - mPoints[0]->mZ );
	minDistance = minDistance /
		sqrt( normal->mX * normal->mX +
			  normal->mY * normal->mY +
			  normal->mZ * normal->mZ );

	double dot = inPoint->dot( normal );

	Vector3D *returnPoint = new Vector3D( inPoint );

	if( dot > 0 ) {
		// inPoint on front side of plane
		normal->scale( -minDistance );
		}
	else {
		// inPoint on back side of plane
		normal->scale( minDistance );
		}
	
	returnPoint->add( normal );
	
	delete normal;

	return returnPoint;
	}



inline char Triangle3D::isInBounds( Vector3D *inPoint ) {

	// this is a nice formula, found at
	// http://astronomy.swin.edu.au/pbourke/geometry/insidepoly/

	// compute the angle between inPoint and every pair of points in the
	// triangle (each edge).  If the sum is 2*pi, then the point
	// is inside the triangle.

	// note that we have a hard-coded epsilon value here
	double epsilon = 0.000001;

	double angleSum = 0;

	Vector3D *firstLeg = new Vector3D( mPoints[0] );
	firstLeg->subtract( inPoint );
	
	Vector3D *secondLeg = new Vector3D( mPoints[1] );
	secondLeg->subtract( inPoint );

	Vector3D *thirdLeg = new Vector3D( mPoints[2] );
	thirdLeg->subtract( inPoint );

	angleSum += acos( firstLeg->dot( secondLeg ) /
					  ( firstLeg->getLength() * secondLeg->getLength() ) );

	angleSum += acos( secondLeg->dot( thirdLeg ) /
					  ( secondLeg->getLength() * thirdLeg->getLength() ) );

	angleSum += acos( thirdLeg->dot( firstLeg ) /
					  ( thirdLeg->getLength() * firstLeg->getLength() ) );

	delete firstLeg;
	delete secondLeg;
	delete thirdLeg;
	
	if( angleSum < ( 2 * M_PI - epsilon ) ) {
		// angle too small for point to be inside plane
		return false;
		}
	else {
		return true;
		}
	}



inline void Triangle3D::move( Vector3D *inVector ) {
	mPoints[0]->add( inVector );
	mPoints[1]->add( inVector );
	mPoints[2]->add( inVector );
	}
	
	
	
inline void Triangle3D::rotate( Angle3D *inAngle ) {
	mPoints[0]->rotate( inAngle );
	mPoints[1]->rotate( inAngle );
	mPoints[2]->rotate( inAngle );
	}
	
	
	
inline void Triangle3D::reverseRotate( Angle3D *inAngle ) {
	mPoints[0]->reverseRotate( inAngle );
	mPoints[1]->reverseRotate( inAngle );
	mPoints[2]->reverseRotate( inAngle );
	}


	
inline void Triangle3D::scale( double inScalar ) {
	mPoints[0]->scale( inScalar );
	mPoints[1]->scale( inScalar );
	mPoints[2]->scale( inScalar );
	}
	
	
	
inline GeometricObject3D *Triangle3D::copy() {
	Triangle3D *copiedTriangle = new Triangle3D( this );
	return (GeometricObject3D*)copiedTriangle;
	}



inline int Triangle3D::serialize( OutputStream *inOutputStream ) {
	int numBytesWritten = 0;
	
	numBytesWritten += mPoints[0]->serialize( inOutputStream );
	numBytesWritten += mPoints[1]->serialize( inOutputStream );
	numBytesWritten += mPoints[2]->serialize( inOutputStream );
	
	return numBytesWritten;
	}
	
	
	
inline int Triangle3D::deserialize( InputStream *inInputStream ) {
	int numBytesRead = 0;

	numBytesRead += mPoints[0]->deserialize( inInputStream );
	numBytesRead += mPoints[1]->deserialize( inInputStream );
	numBytesRead += mPoints[2]->deserialize( inInputStream );
	
	return numBytesRead;
	}
	
	
	
#endif
