/*
 * Modification History
 *
 * 2007-March-17    Jason Rohrer
 * Created.
 */
 
 
#ifndef SHADOW_IMAGE_BUTTON_GL_INCLUDED
#define SHADOW_IMAGE_BUTTON_GL_INCLUDED 

#include "ButtonGL.h"

#include "minorGems/graphics/Image.h"

#include "minorGems/graphics/openGL/SingleTextureGL.h"

#include <GL/gl.h>


/**
 * A textured button for GL-based GUIs that draws one image with a shadow
 *   to show pressed/unpressed status.
 *
 * @author Jason Rohrer
 */
class ShadowImageButtonGL : public ButtonGL {


	public:



		/**
		 * Constructs a button.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 * @param inUnpressedImage the image to display
		 *   when this button is unpressed.  Must have dimensions
		 *   that are powers of 2.
		 *   Will be destroyed when this class is destroyed.
		 */
		ShadowImageButtonGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, Image *inUnpressedImage );


		
		~ShadowImageButtonGL();


        // implements the ButtonGL interface
        virtual void drawPressed();
        virtual void drawUnpressed();

        
		
	protected:
		Image *mUnpressedImage;

		SingleTextureGL *mUnpressedTexture;

        
        /**
         * Draw this button with an optional z offset above it's shadow.
         *
         * @param inOffset true to draw the button above its shadow.
         */
        void drawWithOffset( char inOffset );
        
	};



inline ShadowImageButtonGL::ShadowImageButtonGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, Image *inUnpressedImage )
	: ButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mUnpressedImage( inUnpressedImage ) {

	mUnpressedTexture = new SingleTextureGL( mUnpressedImage );
	}



inline ShadowImageButtonGL::~ShadowImageButtonGL() {
	delete mUnpressedImage;

	delete mUnpressedTexture;
	}



inline void ShadowImageButtonGL::drawPressed() {
    drawWithOffset( false );
    }



inline void ShadowImageButtonGL::drawUnpressed() {
    drawWithOffset( true );
    }



inline void ShadowImageButtonGL::drawWithOffset( char inOffset ) {

    // calculate offset for shadow
    double xOffset = mWidth * 0.05;
    double yOffset = mHeight * 0.05;

    // use smaller
    double offset = xOffset;
    if( yOffset < xOffset ) {
        offset = yOffset;
        }

    
    // set our texture
	mUnpressedTexture->enable();

    // shadow first
	glColor4f( 0.0, 0.0, 0.0, 0.5 ); 
    
	// shadow in lower left corner 
	glBegin( GL_QUADS ); {
		
		glTexCoord2f( 0, 1.0f );
		glVertex2d( mAnchorX, mAnchorY );

		glTexCoord2f( 1.0f, 1.0f );
		glVertex2d( mAnchorX + mWidth - offset, mAnchorY ); 

		glTexCoord2f( 1.0f, 0 );
		glVertex2d( mAnchorX + mWidth - offset, mAnchorY + mHeight - offset );
		
		glTexCoord2f( 0, 0 );
		glVertex2d( mAnchorX, mAnchorY + mHeight - offset );
		}
	glEnd();

    // now button image
    glColor4f( 1.0, 1.0, 1.0, 10 );

    double anchorOffset = 0;
    if( inOffset ) {
        // put it in upper right corner
        anchorOffset = offset;
        offset = 0;
        }
    
    glBegin( GL_QUADS ); {
		
		glTexCoord2f( 0, 1.0f );
		glVertex2d( mAnchorX + anchorOffset,
                    mAnchorY + anchorOffset );

		glTexCoord2f( 1.0f, 1.0f );
		glVertex2d( mAnchorX + mWidth - offset,
                    mAnchorY + anchorOffset ); 

		glTexCoord2f( 1.0f, 0 );
		glVertex2d( mAnchorX + mWidth - offset,
                    mAnchorY + mHeight - offset );
		
		glTexCoord2f( 0, 0 );
		glVertex2d( mAnchorX + anchorOffset,
                    mAnchorY + mHeight - offset );
		}
	glEnd();
    
    
	mUnpressedTexture->disable();
    }



#endif



