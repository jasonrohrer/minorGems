/*
 * Modification History
 *
 * 2001-September-16		Jason Rohrer
 * Created.
 *
 * 2010-March-31		Jason Rohrer
 * Changed glColor3f to 4f to fix driver bug on some video cards.
 */
 
 
#ifndef MOUSE_CURSOR_REGION_GL_INCLUDED
#define MOUSE_CURSOR_REGION_GL_INCLUDED 


#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/graphics/Color.h"

#include <GL/gl.h>

/**
 * A region in a GL-based gui where a mouse cursor
 * will be displayed.
 *
 * @author Jason Rohrer
 */
class MouseCursorRegionGL : public GUIComponentGL {

	public:


		/**
		 * Constructs a mouse cursor region.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 * @param inCursorFraction the fraction of the
		 *   region to be filled by the cursor.
		 * @param inUnpressedColor the color to draw the
		 *   cursor with when the mouse is unpressed.
		 *   Will be destroyed when this class is destroyed.
		 * @param inPressedColor the color to draw the
		 *   cursor with when the mouse is pressed.
		 *   Will be destroyed when this class is destroyed.
		 */
		MouseCursorRegionGL( double inAnchorX, double inAnchorY,
							 double inWidth,
							 double inHeight,
							 double inCursorFraction,
							 Color *inUnpressedColor,
							 Color *inPressedColor );

		~MouseCursorRegionGL();
			

			
		// override functions in GUIComponentGL
		virtual void mouseMoved( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
		virtual void fireRedraw();


		
	protected:

		double mCursorFraction;

		double mMouseLocationX;
		double mMouseLocationY;
		
		Color *mUnpressedColor;
		Color *mPressedColor;
		Color *mCurrentColor;
		
	};



inline MouseCursorRegionGL::MouseCursorRegionGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight,
	double inCursorFraction,
	Color *inUnpressedColor,
	Color *inPressedColor )
	: GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mCursorFraction( inCursorFraction ),
	  mUnpressedColor( inUnpressedColor ),
	  mPressedColor( inPressedColor ),
	  mMouseLocationX( -1 ), mMouseLocationY( -1 ) {

	mCurrentColor = mUnpressedColor;
	}



inline MouseCursorRegionGL::~MouseCursorRegionGL() {
	delete mUnpressedColor;
	delete mPressedColor;
	}



inline void MouseCursorRegionGL::mouseMoved( double inX, double inY ) {
	mMouseLocationX = inX;
	mMouseLocationY = inY;
	mCurrentColor = mUnpressedColor;
	}



inline void MouseCursorRegionGL::mouseReleased( double inX, double inY ) {
	mMouseLocationX = inX;
	mMouseLocationY = inY;
	mCurrentColor = mUnpressedColor;
	}



inline void MouseCursorRegionGL::mouseDragged( double inX, double inY ) {
	mMouseLocationX = inX;
	mMouseLocationY = inY;
	mCurrentColor = mPressedColor;
	}



inline void MouseCursorRegionGL::mousePressed( double inX, double inY ) {
	mMouseLocationX = inX;
	mMouseLocationY = inY;
	mCurrentColor = mPressedColor;
	}


		
inline void MouseCursorRegionGL::fireRedraw() {
	if( isInside( mMouseLocationX, mMouseLocationY ) ) {
		// set our color
		glColor4f( mCurrentColor->r, mCurrentColor->g,
				   mCurrentColor->b, 1.0f ); 

		double cursorLineLengthX = mCursorFraction * mWidth;
		double cursorLineLengthY = mCursorFraction * mHeight;

		// horizontal line
		glBegin( GL_LINES ); {
			glVertex2d( mMouseLocationX - 0.5 * cursorLineLengthX,
						mMouseLocationY );
			glVertex2d( mMouseLocationX + 0.5 * cursorLineLengthX,
						mMouseLocationY );
			}
		glEnd();

		// vertical line
		glBegin( GL_LINES ); {
			glVertex2d( mMouseLocationX,
						mMouseLocationY - 0.5 * cursorLineLengthY );
			glVertex2d( mMouseLocationX,
						mMouseLocationY + 0.5 * cursorLineLengthY );
			}
		glEnd();
		}
	}



#endif


