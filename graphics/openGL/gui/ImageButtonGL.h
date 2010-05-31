/*
 * Modification History
 *
 * 2006-July-3		Jason Rohrer
 * Created.
 *
 * 2007-March-17    Jason Rohrer
 * Enabled blending (why was it off?)
 *
 * 2010-March-31		Jason Rohrer
 * Changed glColor3f to 4f to fix driver bug on some video cards.
 */
 
 
#ifndef IMAGE_BUTTON_GL_INCLUDED
#define IMAGE_BUTTON_GL_INCLUDED 

#include "ButtonGL.h"

#include "minorGems/graphics/Image.h"

#include "minorGems/graphics/openGL/SingleTextureGL.h"

#include <GL/gl.h>


/**
 * A textured button for GL-based GUIs.
 *
 * @author Jason Rohrer
 */
class ImageButtonGL : public ButtonGL {


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
		 * @param inPressedImage the image to display
		 *   when this button is pressed.  Must have dimensions
		 *   that are powers of 2.
		 *   Will be destroyed when this class is destroyed.
		 */
		ImageButtonGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, Image *inUnpressedImage,
			Image *inPressedImage );


		
		~ImageButtonGL();


        // implements the ButtonGL interface
        virtual void drawPressed();
        virtual void drawUnpressed();

        
		
	protected:
		Image *mUnpressedImage;
		Image *mPressedImage;

		SingleTextureGL *mUnpressedTexture;
		SingleTextureGL *mPressedTexture;		

		SingleTextureGL *mCurrentTexture;

        
        /**
         * Draw this button using a specific texture.
         *
         * @param inTexture the texture.
         *   Destroyed by caller.
         */
        void drawWithTexture( SingleTextureGL *inTexture );
        
	};



inline ImageButtonGL::ImageButtonGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, Image *inUnpressedImage,
	Image *inPressedImage  )
	: ButtonGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mUnpressedImage( inUnpressedImage ),
	  mPressedImage( inPressedImage ) {

	mUnpressedTexture = new SingleTextureGL( mUnpressedImage );
	mPressedTexture = new SingleTextureGL( mPressedImage );

	mCurrentTexture = mUnpressedTexture;
	}



inline ImageButtonGL::~ImageButtonGL() {
	delete mUnpressedImage;
	delete mPressedImage;

	delete mUnpressedTexture;
	delete mPressedTexture;
	}



inline void ImageButtonGL::drawPressed() {
    drawWithTexture( mPressedTexture );
    }



inline void ImageButtonGL::drawUnpressed() {
    drawWithTexture( mUnpressedTexture );
    }



inline void ImageButtonGL::drawWithTexture( SingleTextureGL *inTexture ) {

    // set our texture
	inTexture->enable();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f ); 

	//glDisable( GL_BLEND );

	
	glBegin( GL_QUADS ); {
		
		glTexCoord2f( 0, 1.0f );
		glVertex2d( mAnchorX, mAnchorY );

		glTexCoord2f( 1.0f, 1.0f );
		glVertex2d( mAnchorX + mWidth, mAnchorY ); 

		glTexCoord2f( 1.0f, 0 );
		glVertex2d( mAnchorX + mWidth, mAnchorY + mHeight );
		
		glTexCoord2f( 0, 0 );
		glVertex2d( mAnchorX, mAnchorY + mHeight );
		}
	glEnd();

	
	inTexture->disable();
    }



#endif



