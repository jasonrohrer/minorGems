/*
 * Modification History
 *
 * 2000-December-20		Jason Rohrer
 * Created. 
 *
 * 2001-January-9		Jason Rohrer
 * Changed to contain a 3d/Primitive3D.  Containment was used instead of 
 * subclassing so that GL-independent primitive subclasses could all be 
 * used with the same PrimitiveGL.
 *
 * 2001-January-16		Jason Rohrer
 * Changed the draw() function to take a Transform3D.  
 *
 * 2001-January-19		Jason Rohrer
 * Changed to use the altered TextureGL class, which now supports
 * multi-texturing.
 *
 * 2001-January-30		Jason Rohrer
 * Changed to check for GL version 1.2 before using 1.2 features.
 * Implemented first generation of a non-ARB draw function.
 * Now we assume multi-texture is defined (because of the availability
 * of glext), but we check that it is supported at runtime. 
 *
 * 2001-January-31		Jason Rohrer
 * Changed glMultiTexCoord2dARB to glMultiTexCoord2fARB to support
 * the non-1.2 version of multitexture.
 * Fixed a return error affecting the windows compile.
 * Fixed an ambiguous variable declaration.
 *
 * 2001-February-1		Jason Rohrer
 * Worked on the non-ARB multitexturing.
 *
 * 2001-February-2		Jason Rohrer
 * Decided to skip non-ARB multitexturing for now and just
 * use the first texture layer when built-in multitexturing isn't available.
 *
 * 2001-March-14   Jason Rohrer
 * Added support for paramatization and temporal animations.
 *
 * 2001-March-17   Jason Rohrer
 * Fixed a bug in some of the wrapper functions.
 *
 * 2001-September-15   Jason Rohrer
 * Fixed a memory leak in texture creation. 
 */
 
 
#ifndef PRIMITIVE_GL_INCLUDED
#define PRIMITIVE_GL_INCLUDED 

#include <GL/gl.h>
#include <stdio.h>

#include "TextureGL.h"
#include "LightingGL.h"

#include "minorGems/math/geometry/Transform3D.h" 

#include "minorGems/graphics/RGBAImage.h"
#include "minorGems/graphics/3d/Primitive3D.h"

#include "minorGems/graphics/openGL/NoLightingGL.h"

// try and include extensions if multitexture isn't
// built-in to gl.h
#ifndef GL_ARB_multitexture
#include <GL/glext.h>
#endif

 
/**
 * OpenGL primitive object.
 *
 * Comprised of a triangle mesh, texture map, and anchor points.
 *
 * @author Jason Rohrer 
 */
class PrimitiveGL { 
	
	public:
		
		
		/**
		 * Constructs a PrimitiveGL.
		 *
		 * No parameters are copied, so they should not be destroyed
		 * or re-accessed by caller.  All are destroyed when the 
		 * primitive is destroyed.
		 *
		 * @param inPrimitive the primitive to base this GL version on.
		 */
		PrimitiveGL( Primitive3D *inPrimitive );
		
		
		
		~PrimitiveGL();
		
		
		/**
		 * Draws the primitive (assumes an active openGL screen).
		 *
		 * @param inTransform the transform to perform on the primitive
		 *   before drawing it. 
		 * @param inLighting the lighting to use when drawing the primitive.
		 */
		void draw( Transform3D *inTransform, LightingGL *inLighting ); 

		
		/**
		 * Gets the number of parameters associated with this primitive.
		 *
		 * @return the number of parameters.
		 */
		virtual int getNumParameters();

		
		/**
		 * Gets the number of animations associated with this primitive.
		 *
		 * @return the number of animations.
		 */
		virtual int getNumAnimations();


		/**
		 * Sets a parameter for this primative.
		 *
		 * @param inParameterIndex the index of the parameter to set.
		 *   If an index for a non-existing parameter is specified,
		 *   this call has no effect.
		 * @param inValue the value to set the parameter to, in [-1, 1].
		 *   The default value for all parameters is 0.
		 */
		virtual void setParameter( int inParameterIndex, double inValue ); 


		/**
		 * Gets a parameter for this primative.
		 *
		 * @param inParameterIndex the index of the parameter to get.
		 *   If an index for a non-existing parameter is specified,
		 *   0 is returned.
		 *
		 * @return the value of the parameter, in [-1, 1].
		 *   The default value for all parameters is 0.
		 */
		virtual double getParameter( int inParameterIndex );

		
		/**
		 * Steps this primitive forward in time.
		 *
		 * @param inStepSize the size of the timestep to take.
		 */
		virtual void step( double inStepSize );


		/**
		 * Starts a temporal animation of this primitive.
		 * The animation progresses as step() is called repeatedly.
		 * If called for a non-existent animation or for one that is
		 * already running, this function has no effect.
		 */
		virtual void startAnimation( int inAnimationIndex );

		
		/**
		 * Stops a temporal animation of this primitive.  If called
		 * for a non-existent animation or for one that is not currently
		 * running, this function has no effect.
		 */
		virtual void stopAnimation( int inAnimationIndex );

		
	protected:
		
		Primitive3D *mPrimitive;
		
		TextureGL *mTextureGL;
		
		
		// Equivalent to the public draw(), but does manual multi-texturing
		// by multi-pass rendering.  Does not depend on ARB calls.
		void drawNoMultitexture( Transform3D *inTransform, 
			LightingGL *inLighting );
		
	};



inline PrimitiveGL::PrimitiveGL( Primitive3D *inPrimitive ) 
	: mPrimitive( inPrimitive ) {
	
	
	int numTextures = mPrimitive->mNumTextures;	
	mTextureGL = new TextureGL( numTextures );	
	for( int i=0; i<numTextures; i++ ) {
		unsigned char *rgba = mPrimitive->mTexture[i]->getRGBABytes();
		mTextureGL->setTextureData( i, 
		rgba,
		mPrimitive->mTexture[i]->getWidth(),
		mPrimitive->mTexture[i]->getHeight() );

		delete [] rgba;
		}		
	}



inline PrimitiveGL::~PrimitiveGL() {
	delete mPrimitive;
	delete mTextureGL;
	}



// this is the ARB version of draw
inline void PrimitiveGL::draw( Transform3D *inTransform, 
	LightingGL *inLighting ) {
	
	// check for multi-texture availability before proceeding
	if( !TextureGL::isMultiTexturingSupported() ) {
		drawNoMultitexture( inTransform, inLighting );
		return;
		}
	
	// first, copy the vertices and translate/rotate/scale them
	Vector3D **worldVertices = new Vector3D*[ mPrimitive->mNumVertices ];
	Vector3D **worldNormals = new Vector3D*[ mPrimitive->mNumVertices ]; 
	for( int i=0; i<mPrimitive->mNumVertices; i++ ) {
		// copy
		worldVertices[i] = new Vector3D( mPrimitive->mVertices[i] );
		worldNormals[i] = new Vector3D( mPrimitive->mNormals[i] );
		
		// translate/rotate/scale
		inTransform->apply( worldVertices[i] );
		
		// only rotate normals
		inTransform->applyNoTranslation( worldNormals[i] );
		// normalize to get rid of any scaling
		worldNormals[i]->normalize();
		} 
	
	
	// now draw vertices as triangle strips
	
	mTextureGL->enable();
	int numTextureLayers = mTextureGL->getNumLayers();
	int t;
	
	if( mPrimitive->isTransparent() ) {
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	else {
		glDisable( GL_BLEND );
		}
	
	if( mPrimitive->isBackVisible() ) {
		glDisable( GL_CULL_FACE );
		}
	else {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glFrontFace( GL_CCW );
		}	
		
	
	Color *lightColor = new Color( 0, 0, 0, 1.0 );
	
	// for each strip of triangles
	for( int y=0; y<mPrimitive->mHigh-1; y++ ) {
		// note that we skip the last row of verts, since they are the bottom
		// of the last strip.
		
		int thisRow = y * mPrimitive->mWide;
		int nextRow = ( y + 1 ) * mPrimitive->mWide;
		
		glBegin( GL_TRIANGLE_STRIP );
			// draw first triangle
			
			// first vert in next row
			int index = nextRow + 0;
			
			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );
			
			// pass in each layer's anchor points
			for(  t=0; t<numTextureLayers; t++ ) {
				glMultiTexCoord2fARB( TextureGL::sMultiTextureEnum[t],
					mPrimitive->mAnchorX[t][ index ], 
					mPrimitive->mAnchorY[t][ index ] );
				}
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );
			
			index = thisRow + 0;
			
			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );
			
			// pass in each layer's anchor points
			for(  t=0; t<numTextureLayers; t++ ) {
				glMultiTexCoord2fARB( TextureGL::sMultiTextureEnum[t],
					mPrimitive->mAnchorX[t][ index ], 
					mPrimitive->mAnchorY[t][ index ] );
				}
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );
			
			index = nextRow + 1;
			
			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );
			
			// pass in each layer's anchor points
			for(  t=0; t<numTextureLayers; t++ ) {
				glMultiTexCoord2fARB( TextureGL::sMultiTextureEnum[t],
					mPrimitive->mAnchorX[t][ index ], 
					mPrimitive->mAnchorY[t][ index ] );
				}
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );
			
			// draw next vertex to complete first "rectangle"
			
			index = thisRow + 1;
			
			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );
			
			// pass in each layer's anchor points
			for(  t=0; t<numTextureLayers; t++ ) {
				glMultiTexCoord2fARB( TextureGL::sMultiTextureEnum[t],
					mPrimitive->mAnchorX[t][ index ], 
					mPrimitive->mAnchorY[t][ index ] );
				}
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );
			
			
			// then add rest of vertices as part of strip
			for( int x=2; x<mPrimitive->mWide; x++ ) {
				
				// every additional pair of vertices completes
				// another "rectangle"
				index = nextRow + x;
				
				inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
				glColor4f( lightColor->r, lightColor->g, 
					lightColor->b, 1.0 );
			
				// pass in each layer's anchor points
				for(  t=0; t<numTextureLayers; t++ ) {
					glMultiTexCoord2fARB( TextureGL::sMultiTextureEnum[t],
						mPrimitive->mAnchorX[t][ index ], 
						mPrimitive->mAnchorY[t][ index ] );
					}
	    		glVertex3d( worldVertices[index]->mX, 
					worldVertices[index]->mY, worldVertices[index]->mZ );

				index = thisRow + x;

				inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
				glColor4f( lightColor->r, lightColor->g, 
					lightColor->b, 1.0 );

				// pass in each layer's anchor points
				for(  t=0; t<numTextureLayers; t++ ) {
					glMultiTexCoord2fARB( TextureGL::sMultiTextureEnum[t],
						mPrimitive->mAnchorX[t][ index ], 
						mPrimitive->mAnchorY[t][ index ] );
					}
	    		glVertex3d( worldVertices[index]->mX, 
					worldVertices[index]->mY, worldVertices[index]->mZ );
				}
		glEnd();		
		}
	
	
	mTextureGL->disable();
	
	// cleanup
	for( int i=0; i<mPrimitive->mNumVertices; i++ ) {
		delete worldVertices[i];
		delete worldNormals[i];
		}
	delete [] worldVertices;
	delete [] worldNormals;
	delete lightColor;
	}	
	


// this is the non-ARB version of draw
inline void PrimitiveGL::drawNoMultitexture( Transform3D *inTransform, 
	LightingGL *inLighting ) {
	
	// first, copy the vertices and translate/rotate/scale them
	Vector3D **worldVertices = new Vector3D*[ mPrimitive->mNumVertices ];
	Vector3D **worldNormals = new Vector3D*[ mPrimitive->mNumVertices ]; 
	for( int i=0; i<mPrimitive->mNumVertices; i++ ) {
		// copy
		worldVertices[i] = new Vector3D( mPrimitive->mVertices[i] );
		worldNormals[i] = new Vector3D( mPrimitive->mNormals[i] );
		
		// translate/rotate/scale
		inTransform->apply( worldVertices[i] );
		
		// only rotate normals
		inTransform->applyNoTranslation( worldNormals[i] );
		// normalize to get rid of any scaling
		worldNormals[i]->normalize();
		} 
		
	Color *lightColor = new Color( 0, 0, 0, 1.0 );
	
	// now draw vertices as triangle strips
		
	mTextureGL->enable();

	if( mPrimitive->isTransparent() ) {
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	else {
		glDisable( GL_BLEND );
		}

	if( mPrimitive->isBackVisible() ) {
		glDisable( GL_CULL_FACE );
		}
	else {
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glFrontFace( GL_CCW );
		}	


	// for each strip of triangles
	for( int y=0; y<mPrimitive->mHigh-1; y++ ) {
		// note that we skip the last row of verts, since they are the bottom
		// of the last strip.

		int thisRow = y * mPrimitive->mWide;
		int nextRow = ( y + 1 ) * mPrimitive->mWide;

		glBegin( GL_TRIANGLE_STRIP );
			// draw first triangle

			// first vert in next row
			int index = nextRow + 0;

			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );


			glTexCoord2d( mPrimitive->mAnchorX[0][ index ], 
                		mPrimitive->mAnchorY[0][ index ] );
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );

			index = thisRow + 0;

			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );


			glTexCoord2d( mPrimitive->mAnchorX[0][ index ], 
                		mPrimitive->mAnchorY[0][ index ] );
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );

			index = nextRow + 1;

			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );


			glTexCoord2d( mPrimitive->mAnchorX[0][ index ], 
                		mPrimitive->mAnchorY[0][ index ] );	
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );

			// draw next vertex to complete first "rectangle"

			index = thisRow + 1;

			inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
			glColor4f( lightColor->r, lightColor->g, lightColor->b, 1.0 );


			glTexCoord2d( mPrimitive->mAnchorX[0][ index ], 
                		mPrimitive->mAnchorY[0][ index ] );
	    	glVertex3d( worldVertices[index]->mX, 
				worldVertices[index]->mY, worldVertices[index]->mZ );


			// then add rest of vertices as part of strip
			for( int x=2; x<mPrimitive->mWide; x++ ) {

				// every additional pair of vertices completes
				// another "rectangle"
				index = nextRow + x;

				inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
				glColor4f( lightColor->r, lightColor->g, 
					lightColor->b, 1.0 );


				glTexCoord2d( mPrimitive->mAnchorX[0][ index ], 
                		mPrimitive->mAnchorY[0][ index ] );
	    		glVertex3d( worldVertices[index]->mX, 
					worldVertices[index]->mY, worldVertices[index]->mZ );

				index = thisRow + x;

				inLighting->getLighting( worldVertices[index],
					worldNormals[index], lightColor );
				glColor4f( lightColor->r, lightColor->g, 
					lightColor->b, 1.0 );


				glTexCoord2d( mPrimitive->mAnchorX[0][ index ], 
                		mPrimitive->mAnchorY[0][ index ] );
	    		glVertex3d( worldVertices[index]->mX, 
					worldVertices[index]->mY, worldVertices[index]->mZ );
				}
		glEnd();		
		}


	mTextureGL->disable();
	
	// cleanup
	for( int i=0; i<mPrimitive->mNumVertices; i++ ) {
		delete worldVertices[i];
		delete worldNormals[i];
		}
	delete [] worldVertices;
	delete [] worldNormals;
	delete lightColor;
	
	}


// these all just wrap the functions of the underlying Primitive3D

inline int PrimitiveGL::getNumParameters() {
	return mPrimitive->getNumParameters();
	}



inline int PrimitiveGL::getNumAnimations() {
	return mPrimitive->getNumAnimations();
	}



inline void PrimitiveGL::setParameter( int inParameterIndex, double inValue ) {
	mPrimitive->setParameter( inParameterIndex, inValue );
	}



inline double PrimitiveGL::getParameter( int inParameterIndex ) {

	return mPrimitive->getParameter( inParameterIndex );
	}



inline void PrimitiveGL::step( double inStepSize ) {
	mPrimitive->step( inStepSize );
	}



inline void PrimitiveGL::startAnimation( int inAnimationIndex ) {
	mPrimitive->startAnimation( inAnimationIndex );
	}



inline void PrimitiveGL::stopAnimation( int inAnimationIndex ) {
	mPrimitive->stopAnimation( inAnimationIndex );
	}



#endif
