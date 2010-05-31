/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2000-December-13		Jason Rohrer
 * Added a function for getting the distance between two vectors. 
 * Added implementations for current functions.
 *
 * 2000-December-17		Jason Rohrer
 * Added a normalize function. 
 *
 * 2000-December-18		Jason Rohrer
 * Added a print function. 
 * Added a length function.
 *
 * 2000-December-20		Jason Rohrer
 * Added a cross product function.
 *
 * 2001-January-10		Jason Rohrer
 * Made class serializable.
 *
 * 2001-February-3		Jason Rohrer
 * Updated serialization code to use new interfaces.  
 *
 * 2001-February-10		Jason Rohrer
 * Added a linear sum function.  
 *
 * 2003-June-20		Jason Rohrer
 * Added function for getting Z angle between vectors.
 *
 * 2005-February-4		Jason Rohrer
 * Added setCoordinates functions.
 *
 * 2005-February-6		Jason Rohrer
 * Added equals function.
 *
 * 2005-February-15		Jason Rohrer
 * Added function for getting Y angle between vectors.
 *
 * 2005-February-22		Jason Rohrer
 * Added function for getting X angle between vectors.
 * Added function for rotating about an arbitrary axis.
 *
 * 2005-February-25		Jason Rohrer
 * Fixed bugs in get_AngleTo functions when vectors are close to equal.
 *
 * 2005-February-28		Jason Rohrer
 * Fixed bug in getXAngleTo.
 * Fixed bugs in get_AngleTo functions when vectors are close to opposite.
 *
 * 2005-March-3		Jason Rohrer
 * Fixed bug in getAngleTo when vectors are close to equal or opposite.
 *
 * 2005-March-18		Jason Rohrer
 * Added a getXZDistance function.
 *
 * 2006-August-6		Jason Rohrer
 * Added a no-arg constructor.
 */
 
 
#ifndef VECTOR_3D_INCLUDED
#define VECTOR_3D_INCLUDED 
 
#include <math.h> 
#include <stdio.h>

#include "Angle3D.h" 
#include "minorGems/io/Serializable.h"
 
/**
 * Geometric vector in 3-space. 
 *
 * @author Jason Rohrer 
 */
class Vector3D : public Serializable { 
	
	public:
		double mX, mY, mZ;
		
		/**
		 * Constructs a Vector3D.
		 */
		Vector3D( double inX, double inY, double inZ );

        

        /**
         * Constructs a zero-filled Vector3D.
         */
        Vector3D();
        

        
		/**
		 * Constructs a Vector3D by copying the parameters from
		 * another Vector3D.
		 *
		 * @param inOther vector to copy parameters from.
		 */
		Vector3D( Vector3D *inOther );

        

        /**
		 * Sets the values in this vector
		 */
		void setCoordinates( double inX, double inY, double inZ );

        

        /**
		 * Sets coordinates by copying the parameters from
		 * another Vector3D.
		 *
		 * @param inOther vector to copy parameters from.
         *   Must be destroyed by caller.
		 */
		void setCoordinates( Vector3D *inOther );

        
        
		/**
		 * Normalizes this vector so that it has a length of 1.
		 */
		void normalize();


        
        /**
		 * Tests if another vector is equal to this vector.
		 *
		 * @param inOther vector to test for equality with this vector.
         *
         * @return true if equal, false if not.
		 */
		char equals( Vector3D *inOther );

        
        
		/**
		 * Sums another vector with this vector.
		 *
		 * @param inOther vector to add to this vector.
		 */
		void add( Vector3D *inOther );
		
		
		/**
		 * Subtracts a vector from this vector.
		 *
		 * @param inOther vector to subtract from this vector.
		 */
		void subtract( Vector3D *inOther );
		
		
		/**
		 * Computes a dot product of this vector with another.
		 *
		 * @param inOther vector to perform the dot product with.
		 *
		 * @return the dot product of the two vectors.
		 */
		double dot( Vector3D *inOther );	

		
		/**
		 * Computes a cross product of this vector with another 
		 * ( this x other ).  The cross computed is right handed.
		 *
		 * @param inOther vector to perform the cross product with.
		 *
		 * @return the cross product of the two vectors.  Must be 
		 *   destroyed by caller.
		 */
		Vector3D *cross( Vector3D *inOther );


        
        /**
         * Computes the angle between this vector and another vector.
         *
         * @param inOther vector to find the angle to.
		 *
		 * @return the angle between the two vectors in radians.
         *   This angle is around the axis given by the cross of the two
         *   vectors.
         *   Must be destroyed by caller.
         */
        double getAngleTo( Vector3D *inOther );


        
        /**
         * Rotates this vector around an arbitrary axis.
         *
         * @param inAxis the axis of rotation.  Must be normalized.
         *   Must be destroyed by caller.
         * @param the angle in radians.
         */
        void rotate( Vector3D *inAxis, double inAngle );


        
		/**
		 * Computes the angle between this vector and another vector in
         * the x-y plane (in other words, the z-axis rotation angle).
		 *
		 * @param inOther vector to find the angle to.
		 *
		 * @return the angle between the two vectors in the x-y plane.
         *   Must be destroyed by caller.
		 */
		Angle3D *getZAngleTo( Vector3D *inOther );

        

        /**
		 * Computes the angle between this vector and another vector in
         * the x-z plane (in other words, the y-axis rotation angle).
		 *
		 * @param inOther vector to find the angle to.
		 *
		 * @return the angle between the two vectors in the x-z plane.
         *   Must be destroyed by caller.
		 */
		Angle3D *getYAngleTo( Vector3D *inOther );


        
        /**
         * Computes the angle between this vector and another vector in
         * the y-z plane (in other words, the x-axis rotation angle).
		 *
		 * @param inOther vector to find the angle to.
		 *
		 * @return the angle between the two vectors in the y-z plane.
         *   Must be destroyed by caller.
		 */
		Angle3D *getXAngleTo( Vector3D *inOther );

        
        
		/**
		 * Computes the linear weighted sum of two vectors.
		 *
		 * @param inFirst the first vector.
		 * @param inSecond the second vector.
		 * @param inFirstWeight the weight given to the first vector in the
		 *   sum.  The second vector is weighted (1-inFirstWeight).
		 *
		 * @return the sum vector.  Must be destroyed by caller.
		 */
		static Vector3D *linearSum( Vector3D *inFirst, Vector3D *inSecond,
			double inFirstWeight );
		
		
		/**
		 * Gets the length of this vector.
		 *
		 * @return this vector's length.
		 */
		double getLength();
		
		
		/**
		 * Multiplies this vector by a scalar.
		 *
		 * @param inScalar scalar to multiply this vector by.
		 */
		void scale( double inScalar );
	
		
		/**
		 * Gets the scalar distance between two vectors.
		 *
		 * @param inOther vector to compute the distance with.
		 *
		 * @return the distance between the two vectors.
		 */
		double getDistance( Vector3D *inOther );


        
        /**
		 * Gets the scalar distance between two vectors ignoring the y
         * components.
		 *
		 * @param inOther vector to compute the distance with.
		 *
		 * @return the xz distance between the two vectors.
		 */
		double getXZDistance( Vector3D *inOther );

        
		
		/**
		 * Rotates the vector about the origin.
		 *
		 * @param inAngle the angle to rotate the vector by.
		 */
		void rotate( Angle3D *inAngle );
		
		
		/**
		 * Rotates the vector about the origin in reverse direction.
		 *
		 * @param inAngle the angle to rotate the object by 
		 *   in reverse direction.
		 */
		void reverseRotate( Angle3D *inAngle );
		
		
		/**
		 * Prints this vector to standard out.
		 */		 
		void print();
		
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );
		
	};




inline Vector3D::Vector3D( double inX, double inY, double inZ ) 
	: mX( inX ), mY( inY ), mZ( inZ ) {
	
	}



inline Vector3D::Vector3D()
    : mX( 0 ), mY( 0 ), mZ( 0 ) {

    }


	
inline Vector3D::Vector3D( Vector3D *inOther ) 
	: mX( inOther->mX ), mY( inOther->mY ), mZ( inOther->mZ ) {
	
	}



inline void Vector3D::setCoordinates( double inX, double inY, double inZ ) { 
	mX = inX;
    mY = inY;
    mZ = inZ;
	}



inline void Vector3D::setCoordinates( Vector3D *inOther ) {
	setCoordinates( inOther->mX,
                    inOther->mY,
                    inOther->mZ );	
	}



inline void Vector3D::normalize() {
	scale( 1/sqrt( dot( this ) ) );
	}



inline char Vector3D::equals( Vector3D *inOther ) {
    return
        mX == inOther->mX &&
        mY == inOther->mY &&
        mZ == inOther->mZ;
    }



inline void Vector3D::add( Vector3D *inOther ) {
	mX += inOther->mX;
	mY += inOther->mY;
	mZ += inOther->mZ;
	}
		
		
		
inline void Vector3D::subtract( Vector3D *inOther ) {
	mX -= inOther->mX;
	mY -= inOther->mY;
	mZ -= inOther->mZ;
	}		
		


inline double Vector3D::dot( Vector3D *inOther ) {
	return mX * inOther->mX + mY * inOther->mY + mZ * inOther->mZ;
	}



inline Vector3D *Vector3D::cross( Vector3D *inOther ) {
	double i = this->mY * inOther->mZ - this->mZ * inOther->mY;
	double j = this->mZ * inOther->mX - this->mX * inOther->mZ;
	double k = this->mX * inOther->mY - this->mY * inOther->mX;
	
	return new Vector3D( i, j, k );
	}



inline double Vector3D::getAngleTo( Vector3D *inOther ) {
    // normalize and remove z component
    Vector3D *normalThis = new Vector3D( this );
    normalThis->normalize();
    
    Vector3D *normalOther = new Vector3D( inOther );
    normalOther->normalize();
    
    double cosineOfAngle = normalThis->dot( normalOther );


    // cosine is ambiguous (same for negative and positive angles)

    // compute cross product of vectors
    // the magnitude of the cross is the sine of the angle between the two
    // vectors

    Vector3D *crossVector = normalThis->cross( normalOther );
    double sineOfAngle = crossVector->getLength();

    delete crossVector;
    delete normalThis;
    delete normalOther;

    double angle = acos( cosineOfAngle );

    if( sineOfAngle > 0 ) {
        angle = -angle;
        }

    // if vectors are very close, our dot product above might give
    // a value greater than 1 due to round-off errors
    // this causes acos to return NAN
    if( cosineOfAngle >= 1 ) {
        angle = 0;
        }
    // also need to worry if vectors are complete opposites 
    else if( cosineOfAngle <= -1 ) {
        angle = M_PI;
        }
    
    return angle; 
    }



inline void Vector3D::rotate( Vector3D *inAxis, double inAngle ) {

    // this formula found here:
    // http://www.gamedev.net/reference/articles/article1199.asp
    
    double c = cos( inAngle );
    double s = sin( inAngle );
    double t = 1 - c;

    // we assume inAxis is a unit vector (normalized)
    double x = inAxis->mX;
    double y = inAxis->mY;
    double z = inAxis->mZ;

    double sx = s * x;
    double sy = s * y;
    double sz = s * z;

    double tx = t * x;
    double ty = t * y;

    double txx = tx * x;
    double txy = tx * y;
    double txz = tx * z;
    
    double tyy = ty * y;
    double tyz = ty * z;

    double tzz = t * z * z;

    /*
      The rotation matrix is:

      (txx + c)     (txy + sz)   (txz - sy)
      (txy - sz)    (tyy + c)    (tyz + sx)
      (txz + sy)    (tyz - sx)   (tzz + c)
    */
    
    
    double newX =
        (txx + c) * mX +
        (txy + sz) * mY +
        (txz - sy) * mZ;
    double newY =
        (txy - sz) * mX +
        (tyy + c) * mY +
        (tyz + sx) * mZ;
    double newZ =
        (txz + sy) * mX +
        (tyz - sx) * mY +
        (tzz + c) * mZ;

    mX = newX;
    mY = newY;
    mZ = newZ;
    }



inline Angle3D *Vector3D::getZAngleTo( Vector3D *inOther ) {
    // normalize and remove z component
    Vector3D *normalThis = new Vector3D( this );
    normalThis->mZ = 0;
    normalThis->normalize();
    
    Vector3D *normalOther = new Vector3D( inOther );
    normalOther->mZ = 0;
    normalOther->normalize();

    double cosineOfZAngle = normalThis->dot( normalOther );


    // cosine is ambiguous (same for negative and positive angles)

    // compute dot product with perpendicular vector to get sine
    // sign of sine will tell us whether angle is positive or negative
    
    Angle3D *rightAngleZ = new Angle3D( 0, 0, M_PI / 2 );

    normalThis->rotate( rightAngleZ );

    delete rightAngleZ;
    
    double sineOfZAngle = normalThis->dot( normalOther );
    
    delete normalThis;
    delete normalOther;

    double zAngle = acos( cosineOfZAngle );

    if( sineOfZAngle < 0 ) {
        zAngle = -zAngle;
        }

    // if vectors are very close, our dot product above might give
    // a value greater than 1 due to round-off errors
    // this causes acos to return NAN
    if( cosineOfZAngle >= 1 ) {
        zAngle = 0;
        }
    // also need to worry if vectors are complete opposites 
    else if( cosineOfZAngle <= -1 ) {
        zAngle = M_PI;
        }

    return new Angle3D( 0, 0, zAngle ); 
    }



inline Angle3D *Vector3D::getYAngleTo( Vector3D *inOther ) {
    // normalize and remove y component
    Vector3D *normalThis = new Vector3D( this );
    normalThis->mY = 0;
    normalThis->normalize();
    
    Vector3D *normalOther = new Vector3D( inOther );
    normalOther->mY = 0;
    normalOther->normalize();

    double cosineOfYAngle = normalThis->dot( normalOther );


    // cosine is ambiguous (same for negative and positive angles)

    // compute dot product with perpendicular vector to get sine
    // sign of sine will tell us whether angle is positive or negative
    
    Angle3D *rightAngleY = new Angle3D( 0, M_PI / 2, 0 );

    normalThis->rotate( rightAngleY );

    delete rightAngleY;
    
    double sineOfYAngle = normalThis->dot( normalOther );
    
    delete normalThis;
    delete normalOther;

    double yAngle = acos( cosineOfYAngle );

    if( sineOfYAngle < 0 ) {
        yAngle = -yAngle;
        }

    // if vectors are very close, our dot product above might give
    // a value greater than 1 due to round-off errors
    // this causes acos to return NAN
    if( cosineOfYAngle >= 1 ) {
        yAngle = 0;
        }
    // also need to worry if vectors are complete opposites 
    else if( cosineOfYAngle <= -1 ) {
        yAngle = M_PI;
        }
    
    return new Angle3D( 0, yAngle, 0 ); 
    }



inline Angle3D *Vector3D::getXAngleTo( Vector3D *inOther ) {
    // normalize and remove y component
    Vector3D *normalThis = new Vector3D( this );
    normalThis->mX = 0;
    normalThis->normalize();
    
    Vector3D *normalOther = new Vector3D( inOther );
    normalOther->mX = 0;
    normalOther->normalize();

    double cosineOfXAngle = normalThis->dot( normalOther );


    // cosine is ambiguous (same for negative and positive angles)

    // compute dot product with perpendicular vector to get sine
    // sign of sine will tell us whether angle is positive or negative
    
    Angle3D *rightAngleX = new Angle3D( M_PI / 2, 0, 0 );

    normalThis->rotate( rightAngleX );

    delete rightAngleX;
    
    double sineOfXAngle = normalThis->dot( normalOther );
    
    delete normalThis;
    delete normalOther;

    double xAngle = acos( cosineOfXAngle );

    if( sineOfXAngle < 0 ) {
        xAngle = -xAngle;
        }

    // if vectors are very close, our dot product above might give
    // a value greater than 1 due to round-off errors
    // this causes acos to return NAN
    if( cosineOfXAngle >= 1 ) {
        xAngle = 0;
        }
    // also need to worry if vectors are complete opposites 
    else if( cosineOfXAngle <= -1 ) {
        xAngle = M_PI;
        }

    return new Angle3D( xAngle, 0, 0 ); 
    }



inline Vector3D *Vector3D::linearSum( Vector3D *inFirst, Vector3D *inSecond,
	double inFirstWeight ) {
	
	double secondWeight = 1 - inFirstWeight;
	double x = inFirstWeight * inFirst->mX + secondWeight * inSecond->mX;
	double y = inFirstWeight * inFirst->mY + secondWeight * inSecond->mY;
	double z = inFirstWeight * inFirst->mZ + secondWeight * inSecond->mZ;
	
	return new Vector3D( x, y, z );
	}



inline double Vector3D::getLength() {
	return sqrt( dot( this ) );
	}


		
inline void Vector3D::scale( double inScalar ) {
	mX *= inScalar;
	mY *= inScalar;
	mZ *= inScalar;
	}
	


inline double Vector3D::getDistance( Vector3D *inOther ) {
	double delX = mX - inOther->mX;
	double delY = mY - inOther->mY;
	double delZ = mZ - inOther->mZ;

	return sqrt( delX * delX + delY * delY + delZ * delZ );
	}
	


inline double Vector3D::getXZDistance( Vector3D *inOther ) {
	double delX = mX - inOther->mX;
	double delZ = mZ - inOther->mZ;

	return sqrt( delX * delX + delZ * delZ );
	}



inline void Vector3D::rotate( Angle3D *inAngle ) {
	if( inAngle->mX != 0 ) {
		double cosTheta = cos( inAngle->mX );
		double sinTheta = sin( inAngle->mX );
		double oldY = mY;
		mY = mY * cosTheta - mZ * sinTheta;
		mZ = oldY * sinTheta + mZ * cosTheta;
		}
	if( inAngle->mY != 0 ) {
		double cosTheta = cos( inAngle->mY );
		double sinTheta = sin( inAngle->mY );
		double oldX = mX;
		mX = cosTheta * mX + sinTheta * mZ;
		mZ = -sinTheta * oldX + cosTheta * mZ;
		}
	if( inAngle->mZ != 0 ) {
		double cosTheta = cos( inAngle->mZ );
		double sinTheta = sin( inAngle->mZ );
		double oldX = mX;
		mX = cosTheta * mX - sinTheta * mY;
		mY = sinTheta * oldX + cosTheta * mY;
		}
	}



inline void Vector3D::reverseRotate( Angle3D *inAngle ) {
	Angle3D *actualAngle = 
		new Angle3D( -inAngle->mX, -inAngle->mY, -inAngle->mZ );
	
	rotate( actualAngle );
	delete actualAngle;
	}
	


inline void Vector3D::print() {
	printf( "(%f, %f, %f)", mX, mY, mZ );
	}



inline int Vector3D::serialize( OutputStream *inOutputStream ) {
	int numBytes = 0;
	
	numBytes += inOutputStream->writeDouble( mX );
	numBytes += inOutputStream->writeDouble( mY );
	numBytes += inOutputStream->writeDouble( mZ );
	
	return numBytes;
	}
	
	
	
inline int Vector3D::deserialize( InputStream *inInputStream ) {
	int numBytes = 0;
	
	numBytes += inInputStream->readDouble( &( mX ) );
	numBytes += inInputStream->readDouble( &( mY ) );
	numBytes += inInputStream->readDouble( &( mZ ) );
	
	return numBytes;
	}

			

#endif
