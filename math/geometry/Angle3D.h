/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2000-December-14		Jason Rohrer
 * Added data members and constructors.
 *
 * 2001-January-10		Jason Rohrer
 * Made class serializable. 
 *
 * 2001-January-15		Jason Rohrer
 * Added a print() function.  
 *
 * 2001-February-10		Jason Rohrer
 * Added a linear sum function.
 *
 * 2001-March-11		Jason Rohrer
 * Added a scale function. 
 *
 * 2004-February-13		Jason Rohrer
 * Added setComponents functions.
 *
 * 2006-September-3		Jason Rohrer
 * Added zero-angle constructor.
 */
 
 
#ifndef ANGLE_3D_INCLUDED
#define ANGLE_3D_INCLUDED 

#include <stdio.h>

#include "minorGems/io/Serializable.h"
 
/**
 * Angle in 3-space. 
 *
 * @author Jason Rohrer 
 */
class Angle3D : public Serializable { 
	
	public:
		
		// rotations in radians around x, y, and z axis
		double mX, mY, mZ;
	
		/**
		 * Constructs an Angle3D.
		 */
		Angle3D( double inX, double inY, double inZ );


        
        /**
         * Constructs a zero angle.
         */
        Angle3D();

        
		
		/**
		 * Constructs an Angle3D by copying the parameters from
		 * another Angle3D.
		 *
		 * @param inOther angle to copy parameters from.
		 */
		Angle3D( Angle3D *inOther );


        
        /**
		 * Sets the components of this angle.
		 */
		void setComponents( double inX, double inY, double inZ );

        

        /**
		 * Sets components by copying the parameters from
		 * another Angle3D.
		 *
		 * @param inOther vector to copy parameters from.
         *   Must be destroyed by caller.
		 */
		void setComponents( Angle3D *inOther );


        
		/**
		 * Sums another angle with this angle.
		 *
		 * @param inOther angle to add to this angle.
		 */
		void add( Angle3D *inOther );
		
		
		/**
		 * Subtracts a angle from this angle.
		 *
		 * @param inOther angle to subtract from this angle.
		 */
		void subtract( Angle3D *inOther );
		

		/**
		 * Multiplies this angle by a scalar.
		 *
		 * @param inScalar scalar to multiply this angle by.
		 */
		void scale( double inScalar );

		
		/**
		 * Computes the linear weighted sum of two angles.
		 *
		 * @param inFirst the first angle.
		 * @param inSecond the second angle.
		 * @param inFirstWeight the weight given to the first angle in the
		 *   sum.  The second angle is weighted (1-inFirstWeight).
		 *
		 * @return the sum angle.  Must be destroyed by caller.
		 */
		static Angle3D *linearSum( Angle3D *inFirst, Angle3D *inSecond,
			double inFirstWeight );
			
		
		/**
		 * Prints this angle to standard out.
		 */		 
		void print();
		
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );
		
	};



inline Angle3D::Angle3D( double inX, double inY, double inZ ) 
	: mX( inX ), mY( inY ), mZ( inZ ) {
	
	}



inline Angle3D::Angle3D() 
	: mX( 0 ), mY( 0 ), mZ( 0 ) {
	
	}

	
inline Angle3D::Angle3D( Angle3D *inOther ) 
	: mX( inOther->mX ), mY( inOther->mY ), mZ( inOther->mZ ) {
	
	}



inline void Angle3D::setComponents( double inX, double inY, double inZ ) { 
	mX = inX;
    mY = inY;
    mZ = inZ;
	}



inline void Angle3D::setComponents( Angle3D *inOther ) {
	setComponents( inOther->mX,
                   inOther->mY,
                   inOther->mZ );	
	}



inline void Angle3D::add( Angle3D *inOther ) {
	mX += inOther->mX;
	mY += inOther->mY;
	mZ += inOther->mZ;
	}



inline void Angle3D::subtract( Angle3D *inOther ) {
	mX -= inOther->mX;
	mY -= inOther->mY;
	mZ -= inOther->mZ;
	}


inline void Angle3D::scale( double inScalar ) {
	mX *= inScalar;
	mY *= inScalar;
	mZ *= inScalar;
	}
	
	
	
inline Angle3D *Angle3D::linearSum( Angle3D *inFirst, Angle3D *inSecond,
	double inFirstWeight ) {
	
	double secondWeight = 1 - inFirstWeight;
	double x = inFirstWeight * inFirst->mX + secondWeight * inSecond->mX;
	double y = inFirstWeight * inFirst->mY + secondWeight * inSecond->mY;
	double z = inFirstWeight * inFirst->mZ + secondWeight * inSecond->mZ;
	
	return new Angle3D( x, y, z );
	}	
	
	
			
inline int Angle3D::serialize( OutputStream *inOutputStream ) {
	int numBytes = 0;
	
	numBytes += inOutputStream->writeDouble( mX );
	numBytes += inOutputStream->writeDouble( mY );
	numBytes += inOutputStream->writeDouble( mZ );
	
	return numBytes;
	}
	
	
	
inline int Angle3D::deserialize( InputStream *inInputStream ) {
	int numBytes = 0;
	
	numBytes += inInputStream->readDouble( &( mX ) );
	numBytes += inInputStream->readDouble( &( mY ) );
	numBytes += inInputStream->readDouble( &( mZ ) );
	
	return numBytes;
	}
	


inline void Angle3D::print() {
	printf( "(%f, %f, %f)", mX, mY, mZ );
	}	


	
#endif
