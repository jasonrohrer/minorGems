/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.  Copied from LathePrimitiveGL, which this class will replace.
 *
 * 2001-January-19		Jason Rohrer
 * Changed to support multi-texturing internally, though this primitive
 * type doesn't use it.
 *
 * 2001-January-21		Jason Rohrer
 * Fixed a bug in the constructor.
 *
 * 2001-January-31		Jason Rohrer
 * Got rid of an unused variable.
 *
 * 2001-March-11   Jason Rohrer
 * Fixed a bug in the texture map anchor points. 
 */
 
 
#ifndef LATHE_PRIMITIVE_3D_INCLUDED
#define LATHE_PRIMITIVE_3D_INCLUDED 

#include "Primitive3D.h"
 
/**
 * Primitive 3D lathe object.
 *
 * Made of a curve rotated around an axis.
 *
 * @author Jason Rohrer 
 */
class LathePrimitive3D : public Primitive3D { 
	
	public:
		
		
		/**
		 * Constructs a LathePrimitive.  The lathe curve is rotated
		 * about the y axis.
		 *
		 * No parameters are copied, so they should not be destroyed
		 * or re-accessed by caller.  All are destroyed when the 
		 * primitive is destroyed.
		 *
		 * @param inNumCurvePoints number of points in curve to be lathed.
		 * @param inCurvePoints points to be lathed.  All z components
		 *   are set to 0 before lathing, so only x and y values matter.
		 * @param inNumLatheSteps the number of quad segments around
		 *   the circumference of the lathed object.  For example,
		 *   setting to 4 (with a lathe angle of 2pi)
		 *   will produce an extruded square object.
		 * @param inNetLatheAngle total angle to sweep during lathing,
		 *   in (0,2pi].
		 * @param inTexture the texture to map onto the lathed object.
		 *   Texture is anchored by its corners to the ends of the lath
		 *   curve at the beginning and end of the lathe sweep
		 */
		LathePrimitive3D( int inNumCurvePoints, Vector3D **inCurvePoints,
			int inNumLatheSteps, double inNetLatheAngle,
			RGBAImage *inTexture );
		
	};



inline LathePrimitive3D::LathePrimitive3D( int inNumCurvePoints, 
	Vector3D **inCurvePoints,
	int inNumLatheSteps, double inNetLatheAngle,
	RGBAImage *inTexture ) {
	
	// first, set Primitve3D members
	mHigh = inNumCurvePoints;
	mWide = inNumLatheSteps + 1;
	mNumVertices = mHigh * mWide;
	
	mNumTextures = 1;
	mTexture = new RGBAImage*[1];
	mTexture[0] = inTexture;
	
	
	double stepAngle = inNetLatheAngle / inNumLatheSteps;
	
	int i;
	
	// first, set all z values for control points to 0
	for( i=0; i<inNumCurvePoints; i++ ) {
		inCurvePoints[i]->mZ = 0;
		}
	
	mVertices = new Vector3D*[mNumVertices];
	
	mAnchorX = new double*[1];
	mAnchorY = new double*[1];
	mAnchorX[0] = new double[mNumVertices];
	mAnchorY[0] = new double[mNumVertices];
	
	// anchor at texture corners, and step linearly through texture
	double anchorYStep = 1.0 / ( mHigh - 1 );
	double anchorXStep = 1.0 / ( mWide - 1 );
	
	for( int y=0; y<mHigh; y++ ) {
		for( int x=0; x<mWide; x++ ) {
			int index = y * mWide + x;
			
			mAnchorX[0][index] = anchorXStep * x;
			mAnchorY[0][index] = anchorYStep * y;
			
			// copy curve into each lathe step
			mVertices[index] = new Vector3D( inCurvePoints[y] );
			
			// rotate the copied curve by x steps around y-axis
			Angle3D *latheRotation = new Angle3D( 0, stepAngle * x, 0 );
			mVertices[index]->reverseRotate( latheRotation );
			
			delete latheRotation;
			}
		
		// cleanup as we go along
		delete inCurvePoints[y];
		}
		 
	delete [] inCurvePoints;
	
	generateNormals();	
	}

#endif
