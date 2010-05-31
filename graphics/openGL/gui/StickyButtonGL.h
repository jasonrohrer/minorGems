/*
 * Modification History
 *
 * 2001-September-16		Jason Rohrer
 * Created.
 */
 
 
#ifndef STICKY_BUTTON_GL_INCLUDED
#define STICKY_BUTTON_GL_INCLUDED 

#include "ButtonGL.h"

#include "minorGems/graphics/Image.h"

#include "minorGems/graphics/openGL/SingleTextureGL.h"



/**
 * A button that can be toggled between a pressed
 * and unpressed state.
 *
 * @author Jason Rohrer
 */
class StickyButtonGL : public ButtonGL {


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
		StickyButtonGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, Image *inUnpressedImage,
			Image *inPressedImage );



		/**
		 * Gets whether this button is toggled to a pressed state.
		 *
		 * @return true iff this button is pressed.
		 */
		virtual char isPressed();



		/**
		 * Sets this button's state.
		 *
		 * Note that if this function call causes
		 * this button's state to change, then all
		 * registered ActionListeners are notified.
		 *
		 * @param inPressed true iff this button should be
		 *   pressed.
		 */
		virtual void setPressed( char inPressed );

		
		
		// override funxtions in GUIComponentGL
		virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );

	};



inline StickyButtonGL::StickyButtonGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, Image *inUnpressedImage,
	Image *inPressedImage  )
	: ButtonGL( inAnchorX, inAnchorY, inWidth, inHeight,
	  inUnpressedImage, inPressedImage ) {

	}



inline char StickyButtonGL::isPressed() {
	if( mCurrentTexture == mPressedTexture ) {
		return true;
		}
	else {
		return false;
		}
	}



inline void StickyButtonGL::setPressed( char inPressed ) {
	SingleTextureGL *lastTexture = mCurrentTexture;

	if( inPressed ) {
		mCurrentTexture = mPressedTexture;
		}
	else {
		mCurrentTexture = mUnpressedTexture;
		}

	// check for state change
	if( lastTexture != mCurrentTexture ) {
		fireActionPerformed( this );
		}
	}



inline void StickyButtonGL::mouseDragged( double inX, double inY ) {
	// do nothing until the release
	}



inline void StickyButtonGL::mousePressed( double inX, double inY ) {
	// do nothing until the release
	}



inline void StickyButtonGL::mouseReleased( double inX, double inY ) {
	// Note the following situation:
	// If the mouse is pressed on another button and then
	// released on this button, this button will toggle...
	// This is non-standard behavior, but the situation occurs
	// rarely.  By ignoring this situation, we save a bit of coding,
	// but this may need to be fixed later.
	
	if( isInside( inX, inY ) ) {
		// toggle our state to the opposite of it's current setting
		setPressed( !isPressed() );
		// this will fire an action automatically
		}
	}



#endif



