/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.  Copied from LandscapePrimitiveGL, which this class will replace
 *
 * 2001-January-15		Jason Rohrer
 * Fixed a bug in the constructor.
 *
 * 2001-January-17		Jason Rohrer
 * Fliped how x and y in height map correspond to x and z in the terrain.
 *
 * 2001-January-19		Jason Rohrer
 * Changed to support multi-texturing.
 *
 * 2001-January-30		Jason Rohrer
 * Fixed a bug that occurs when the class defaults to no detail texture.
 *
 * 2001-March-11   Jason Rohrer
 * Fixed a bug in the texture map anchor points.  
 */

#ifndef LANDSCAPE_PRIMITIVE_3D_INCLUDED
#define LANDSCAPE_PRIMITIVE_3D_INCLUDED 
 
#include "Primitive3D.h"
 
/**
 * Primitive 3D lanscape object.
 *
 * Made from a height map.  Mesh generated spans both x and z between -1 and
 * 1, and can vary in height (y) from 0 to 1.
 *
 * @author Jason Rohrer 
 */
class LandscapePrimitive3D : public Primitive3D { 
	
	public:
		
		
		/**
		 * Constructs a LandscapePrimitive with a multi-layer texture.
		 *
		 * @param inWide width of mesh in number of vertices.
		 *   Must be even and at least 2.
		 * @param inHigh height of mesh in number of vertices.
		 * @param inHeights array of heights for each vertex, each in 
		 *   [0,1]. Must be destroyed by caller.  
		 * @param inTexture the texture to map onto the lanscape.
		 *   Texture is anchored by its corners to the corners of the 
		 *   lanscape.  inTexture is destroyed when primitive is destroyed.
		 * @param inDetailTexture the second layer in the multi-texture.
		 *   The alpha channel of inDetailTexture will be used as
		 *   the blending factor to mix the detail with the global texture.
		 *   Set to NULL to use only a single layer.
		 * @param inDetailScale the scale factor of the detailed texture.
		 *   Setting to 1.0 will anchor the detail at the corners of the 
		 *   mesh (just like inTexture).  Setting to 0.25 will cause
		 *   the detail texture to cycle 4 times across the surface of the
		 *   mesh.
		 */
		LandscapePrimitive3D( int inWide, int inHigh, double *inHeights,
			RGBAImage *inTexture, RGBAImage *inDetailTexture = NULL,
			double inDetailScale = 1.0 );
		
	};
	


inline LandscapePrimitive3D::LandscapePrimitive3D( int inWide, 
	int inHigh, double *inHeights,
	RGBAImage *inTexture, RGBAImage *inDetailTexture,
	double inDetailScale ) {
	
	// first, set Primitve3D members
	mHigh = inHigh;
	mWide = inWide;
	mNumVertices = mHigh * mWide;
	
	if( inDetailTexture == NULL ) {
		mNumTextures = 1;
		mTexture = new RGBAImage*[1];
		mTexture[0] = inTexture;
		
		mAnchorX = new double*[1];
		mAnchorY = new double*[1];
		mAnchorX[0] = new double[mNumVertices];
		mAnchorY[0] = new double[mNumVertices];
		}
	else {
		mNumTextures = 2;
		mTexture = new RGBAImage*[2];
		mTexture[0] = inTexture;
		mTexture[1] = inDetailTexture;	
		
		mAnchorX = new double*[2];
		mAnchorY = new double*[2];
		mAnchorX[0] = new double[mNumVertices];
		mAnchorY[0] = new double[mNumVertices];
		mAnchorX[1] = new double[mNumVertices];
		mAnchorY[1] = new double[mNumVertices];
		}
		
	
	mVertices = new Vector3D*[mNumVertices];
	
	// anchor at texture corners, and step linearly through texture
	double anchorYStep = 1.0 / ( mHigh - 1 );
	double anchorXStep = 1.0 / ( mWide - 1 );

	double detailAnchorYStep = 1.0 / ( ( mHigh - 1 ) * inDetailScale );
	double detailAnchorXStep = 1.0 / ( ( mWide - 1 ) * inDetailScale );
	
	for( int y=0; y<mHigh; y++ ) {
		for( int x=0; x<mWide; x++ ) {
			int index = y * mWide + x;
			
			mAnchorX[0][index] = anchorXStep * x;
			mAnchorY[0][index] = anchorYStep * y;
			
			if( mNumTextures == 2 ) {
				mAnchorX[1][index] = detailAnchorXStep * x;
				mAnchorY[1][index] = detailAnchorYStep * y;
				}
			
			// add height for this x and y position
			mVertices[index] = new Vector3D( 2 * mAnchorX[0][index] - 1, 
				inHeights[index], -2 * mAnchorY[0][index] + 1  );
			// note that y coordinates in the 2d height map must be
			// inverted before they can be used as z coordinates in the mesh
			}
		}	
	
	generateNormals();
	}

	
#endif 
