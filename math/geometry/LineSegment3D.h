/*
 * Modification History
 *
 * 2001-April-14   Jason Rohrer
 * Created.     
 *
 * 2005-February-15   Jason Rohrer
 * Made destructor virtual to quell warnings.
 */
 
 
#ifndef LINE_SEGMENT_3D_INCLUDED
#define LINE_SEGMENT_3D_INCLUDED 
 
#include "GeometricObject3D.h" 

#include "minorGems/io/Serializable.h"
 
#include <math.h>
#include <stdlib.h>
 
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif 
 
/**
 * Line segment (in 3-space). 
 *
 * @author Jason Rohrer 
 */
class LineSegment3D : public GeometricObject3D, public Serializable { 
	
	public:
	
		Vector3D *mEndpoints[2];

		/**
		 * Constructs a line segment.
		 *
		 * The vectors are copied, so the caller
		 * is responsible for deallocating the passed in vectors.
		 *
		 * @param inEndpointA the first endpoint.
		 * @param inEndpointB the second endpoint.
		 */
		LineSegment3D( Vector3D *inEndpointA, Vector3D *inEndpointB );
		
		
		/**
		 * Constructs a line segment by copying parameters
		 * from another line segment.
		 *
		 * @param inOtherSegment the line segment to copy.
		 */
		LineSegment3D( LineSegment3D *inOtherSegment );
		
		
		virtual ~LineSegment3D();
		

		/**
		 * Gets the projection of a point onto this line segment
		 *
		 * @param inPoint the point to project.  Must be destroyed
		 *   by caller.
		 *
		 * @return a new vector representing the projected point, or
		 *   NULL if the projection is not on this line segment.
		 */
		Vector3D *projectPoint( Vector3D *inPoint );

		
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



inline LineSegment3D::LineSegment3D( Vector3D *inEndpointA,
									 Vector3D *inEndpointB ) {
	mEndpoints[0] = new Vector3D( inEndpointA );
	mEndpoints[1] = new Vector3D( inEndpointB );
	}



inline LineSegment3D::LineSegment3D( LineSegment3D *inOtherSegment ) {

	mEndpoints[0] = new Vector3D( inOtherSegment->mEndpoints[0] );
	mEndpoints[1] = new Vector3D( inOtherSegment->mEndpoints[1] );

	}



inline LineSegment3D::~LineSegment3D() {
	delete mEndpoints[0];
	delete mEndpoints[1];
	}



inline Vector3D *LineSegment3D::projectPoint( Vector3D *inPoint ) {

	// compute the vector for the underlying line of this segment
	Vector3D *supportingVector = new Vector3D( mEndpoints[0] );
	supportingVector->subtract( mEndpoints[1] );

	supportingVector->normalize();
	
	// now find the lengths of the projection of each endpoint
	// onto this supporting vector
	double lengthA = supportingVector->dot( mEndpoints[0] );
	double lengthB = supportingVector->dot( mEndpoints[1] );
	
	// find the length of the projection of inPoint
	double lengthInPoint = supportingVector->dot( inPoint );

	// projection is off the segment if lengthInPoint is not
	// between lengthA and lengthB
	if( lengthInPoint > lengthA && lengthInPoint > lengthB ) {
		delete supportingVector;
		return NULL;
		}
	else if( lengthInPoint < lengthA && lengthInPoint < lengthB ) {
		delete supportingVector;
		return NULL;
		}
	else {
		// length is in between the two

		// compute difference from A's projection
		double difference = lengthInPoint - lengthA;
		Vector3D *returnVector = new Vector3D( mEndpoints[0] );

		supportingVector->scale( difference );
		returnVector->add( supportingVector );

		delete supportingVector;
		return returnVector;
		}
	}



inline void LineSegment3D::move( Vector3D *inVector ) {
	mEndpoints[0]->add( inVector );
	mEndpoints[1]->add( inVector );
	}
	
	
	
inline void LineSegment3D::rotate( Angle3D *inAngle ) {
	mEndpoints[0]->rotate( inAngle );
	mEndpoints[1]->rotate( inAngle );
	}
	
	
	
inline void LineSegment3D::reverseRotate( Angle3D *inAngle ) {
	mEndpoints[0]->reverseRotate( inAngle );
	mEndpoints[1]->reverseRotate( inAngle );
	}


	
inline void LineSegment3D::scale( double inScalar ) {
	mEndpoints[0]->scale( inScalar );
	mEndpoints[1]->scale( inScalar );
	}
	
	
	
inline GeometricObject3D *LineSegment3D::copy() {
	LineSegment3D *copiedSegment = new LineSegment3D( this );
	return (GeometricObject3D*)copiedSegment;
	}



inline int LineSegment3D::serialize( OutputStream *inOutputStream ) {
	int numBytesWritten = 0;
	
	numBytesWritten += mEndpoints[0]->serialize( inOutputStream );
	numBytesWritten += mEndpoints[1]->serialize( inOutputStream );

	return numBytesWritten;
	}
	
	
	
inline int LineSegment3D::deserialize( InputStream *inInputStream ) {
	int numBytesRead = 0;

	numBytesRead += mEndpoints[0]->deserialize( inInputStream );
	numBytesRead += mEndpoints[1]->deserialize( inInputStream );
	
	return numBytesRead;
	}
	
	
	
#endif
