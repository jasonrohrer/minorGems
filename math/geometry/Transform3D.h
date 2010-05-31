/*
 * Modification History
 *
 * 2001-January-16		Jason Rohrer
 * Created.
 * Added a missing getMatrix() function.
 *
 * 2001-February-3		Jason Rohrer
 * Updated serialization code to use new interfaces.    
 */
 
 
#ifndef TRANSFORM_3D_INCLUDED
#define TRANSFORM_3D_INCLUDED 

#include <stdio.h>
#include <math.h>

#include "minorGems/io/Serializable.h"

#include "Vector3D.h"
#include "Angle3D.h"
 
/**
 * An affine transformation in 3D. 
 *
 * @author Jason Rohrer 
 */
class Transform3D : public Serializable { 
	
	public:
		
		/**
		 * Creates a new identity transform.
		 */
		Transform3D();
	
		/**
		 * Creates a new transform by copying another transform.
		 *
		 * @param inOther the transform to copy.
		 */
		Transform3D( Transform3D *inOther );
	
	
		/**
		 * Adds a rotation to the end of this transform.  Note that the
		 * rotations specified by inAngle are applied in the following order:
		 * rotX, rotY, rotZ
		 * 
		 * @param inAngle angle rotation to add.  Must be destructed by caller.
		 */
		void rotate( Angle3D *inAngle );
		
		
		/**
		 * Adds a scaling operation to the end of this transform.
		 * 
		 * @param inScale the uniform scale factor.
		 */
		void scale( double inScale );
		
		
		/**
		 * Adds a scaling operation to the end of this transform.
		 * 
		 * @param inScaleX the x direction scale factor.
		 * @param inScaleY the y direction scale factor.
		 * @param inScaleZ the z direction scale factor.
		 */
		void scale( double inScaleX, double inScaleY, double inScaleZ );
		
		
		/**
		 * Adds a translation to the end of this transform.
		 * 
		 * @param inTranslation angle rotation to add.  
		 *   Must be destructed by caller.
		 */
		void translate( Vector3D *inTranslation );
		
		
		
		/**
		 * Adds a transformation to the end of this transformation.
		 *
		 * @param inTransform the transform to add.  inTransform is no
		 *   modified by this call, and must be destroyed by the caller.
		 */
		void transform( Transform3D *inTransform );
		
		
		/**
		 * Transforms a vector using the built-up transformation.
		 *
		 * @param inTarget the vector to transform.  The object passed
		 *   in is modified directly, and must be destroyed by the caller.
		 */
		void apply( Vector3D *inTarget );
		
		
		/**
		 * Transforms a vector using the built-up transformation, but skips
		 * the translation part of the transform.  This is useful when
		 * applying the transform to normals, when translations will screw
		 * them up.
		 *
		 * @param inTarget the vector to transform.  The object passed
		 *   in is modified directly, and must be destroyed by the caller.
		 */
		void applyNoTranslation( Vector3D *inTarget );
		
		
		/**
		 * Gets the transformation matrix underlying this transform.
		 *
		 * @return the transformation matrix.
		 */
		double *getMatrix();
		
		
		/**
		 * Prints the transformation matrix to standard out.
		 */
		void print();
		
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );
	
	private:
		double mMatrix[4][4];
		
		/**
		 * Multiplies mMatrix by inMatrix, i.e., mMatrix = inMatrix * mMatrix.
		 *
		 * Note that this reversed multiplication order works for concatonating
		 * transformation matrices when we're using column vectors for our 3D
		 * points.  Thus, if after the above operation, we compute:
		 * 		point = mMatrix * point,
		 * we will be transformping point first by the original 
		 * mMatrix and then by inMatrix.
		 *
		 * @param inMatrix the matrix to multiply mMatrix by.
		 */
		void multiply( double inMatrix[][4] );
		
		/**
		 * Multiplies inMatrixA by inMatrixB, i.e., 
		 * inMatrixA = inMatrixA * inMatrixB.
		 *
		 * @param inMatrixA the first matrix in the multiplication, and
		 *   the destination of the result.
		 * @param inMatrixB the second matrix in the multiplication.
		 */
		//void multiply( double[4][4] inMatrixA, double[4][4] inMatrixB );
	};



inline Transform3D::Transform3D() {
	double tempM[4][4] = 	{	{ 1, 0, 0, 0 },
								{ 0, 1, 0, 0 },
								{ 0, 0, 1, 0 },
								{ 0, 0, 0, 1 } };
	memcpy( mMatrix, tempM, 16 * sizeof( double ) );	
	}



inline Transform3D::Transform3D( Transform3D *inOther ) {
	
	memcpy( mMatrix, inOther->getMatrix(), 16 * sizeof( double ) );	
	}



inline void Transform3D::rotate( Angle3D *inAngle ) {
	double aX = inAngle->mX;
	double rotX[4][4] = {	{ 1, 0, 0, 0 },
							{ 0, 0, 0, 0 },
							{ 0, 0, 0, 0 },
							{ 0, 0, 0, 1 } };
	rotX[1][1] = cos( aX );
	rotX[1][2] = -sin( aX );
	rotX[2][1] = sin( aX );
	rotX[2][2] = cos( aX );
	
	
	double aY = inAngle->mY;
	double rotY[4][4] = {	{ 0, 0, 0, 0 },
							{ 0, 1, 0, 0 },
							{ 0, 0, 0, 0 },
							{ 0, 0, 0, 1 } };
	rotY[0][0] = cos( aY );
	rotY[0][2] = sin( aY );
	rotY[2][0] = -sin( aY );
	rotY[2][2] = cos( aY );
	
	
	double aZ = inAngle->mZ;
	double rotZ[4][4] = {	{ 0, 0, 0, 0 },
							{ 0, 0, 0, 0 },
							{ 0, 0, 1, 0 },
							{ 0, 0, 0, 1 } };
	rotZ[0][0] = cos( aZ );
	rotZ[0][1] = -sin( aZ );
	rotZ[1][0] = sin( aZ );
	rotZ[1][1] = cos( aZ );
	
	multiply( rotX );
	multiply( rotY );
	multiply( rotZ );
	
	}



inline void Transform3D::scale( double inScale ) {
	scale( inScale, inScale, inScale );
	}



inline void Transform3D::scale( double inScaleX, 
	double inScaleY, double inScaleZ ) {
	
	double scaleM[4][4] = {	{ 0, 0, 0, 0 },
							{ 0, 0, 0, 0 },
							{ 0, 0, 0, 0 },
							{ 0, 0, 0, 1 } };
	scaleM[0][0] = inScaleX;
	scaleM[1][1] = inScaleY;
	scaleM[2][2] = inScaleZ;
	
	multiply( scaleM );
	}



inline void Transform3D::translate( Vector3D *inTranslation ) {
	double translateM[4][4] = {	{ 1, 0, 0, 0 },
								{ 0, 1, 0, 0 },
								{ 0, 0, 1, 0 },
								{ 0, 0, 0, 1 } };
	translateM[0][3] = inTranslation->mX;
	translateM[1][3] = inTranslation->mY;
	translateM[2][3] = inTranslation->mZ;
	
	multiply( translateM );
	}



inline void Transform3D::transform( Transform3D *inTransform ) {
	double tempM[4][4];
	memcpy( tempM, inTransform->getMatrix(), 16 * sizeof( double ) );
	multiply( tempM );
	}



inline void Transform3D::apply( Vector3D *inTarget ) {
	double x = inTarget->mX;
	double y = inTarget->mY;
	double z = inTarget->mZ;
	
	inTarget->mX = mMatrix[0][0] * x + mMatrix[0][1] * y + mMatrix[0][2] * z
		+ mMatrix[0][3];
	inTarget->mY = mMatrix[1][0] * x + mMatrix[1][1] * y + mMatrix[1][2] * z
		+ mMatrix[1][3];
	inTarget->mZ = mMatrix[2][0] * x + mMatrix[2][1] * y + mMatrix[2][2] * z
		+ mMatrix[2][3];	
	}


	
inline void Transform3D::applyNoTranslation( Vector3D *inTarget ) {
	double x = inTarget->mX;
	double y = inTarget->mY;
	double z = inTarget->mZ;
	
	inTarget->mX = mMatrix[0][0] * x + mMatrix[0][1] * y + mMatrix[0][2] * z;
	inTarget->mY = mMatrix[1][0] * x + mMatrix[1][1] * y + mMatrix[1][2] * z;
	inTarget->mZ = mMatrix[2][0] * x + mMatrix[2][1] * y + mMatrix[2][2] * z;
	}



inline void Transform3D::multiply( double inMatrix[][4] ) {
	double destM[4][4];
	
	for( int dY=0; dY<4; dY++ ) {
		for( int dX=0; dX<4; dX++ ) {
			
			// take a row of inMatrix corresponding to dY
			// take a column of mMatrix corresponding to dX
			destM[dY][dX] = 0;
			for( int i=0; i<4; i++ ) {
				destM[dY][dX] += inMatrix[dY][i] * mMatrix[i][dX];
				}
			
			}
		}
	
	memcpy( mMatrix, destM, 16 * sizeof( double ) );		
	}
	


inline double *Transform3D::getMatrix() {
	return &( mMatrix[0][0] );
	}



inline void Transform3D::print() {
	for( int y=0; y<4; y++ ) {
		for( int x=0; x<4; x++ ) {
			printf( "%f ", mMatrix[y][x] );
			}
		printf( "\n" );	
		}
	}
		
		
			
inline int Transform3D::serialize( OutputStream *inOutputStream ) {
	int numBytes = 0;
	
	for( int y=0; y<4; y++ ) {
		for( int x=0; x<4; x++ ) {
			numBytes += inOutputStream->writeDouble( mMatrix[y][x] );
			}
		}

	return numBytes;
	}
	
	
	
inline int Transform3D::deserialize( InputStream *inInputStream ) {
	int numBytes = 0;
	
	for( int y=0; y<4; y++ ) {
		for( int x=0; x<4; x++ ) {
			numBytes += inInputStream->readDouble( &( mMatrix[y][x] ) );
			}
		}

	return numBytes;
	}
		


#endif
