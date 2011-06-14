/*
 * Modification History
 *
 * 2001-September-17		Jason Rohrer
 * Created.
 *
 * 2001-September-21		Jason Rohrer
 * Fixed a bug in thumb positioning in response to mouse position.
 *
 * 2001-September-23		Jason Rohrer
 * Fixed a bug in icon color clearing.
 *
 * 2009-December-25		Jason Rohrer
 * Added control over border/thumb width and thumb increments to fix rounding 
 * errors.
 *
 * 2010-January-30		Jason Rohrer
 * Improved response when clicking.
 *
 * 2010-February-8		Jason Rohrer
 * Distinguish between first press and subsequent dragging.
 *
 * 2010-March-31		Jason Rohrer
 * Changed glColor3f to 4f to fix driver bug on some video cards.
 *
 * 2010-April-2		Jason Rohrer
 * Added functions for setting bar colors after construction.
 *
 * 2010-April-15		Jason Rohrer
 * Distinguish release event.
 */
 
 
#ifndef SLIDER_GL_INCLUDED
#define SLIDER_GL_INCLUDED 

#include "GUIComponentGL.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/Color.h"

#include "minorGems/graphics/openGL/SingleTextureGL.h"

#include "minorGems/ui/event/ActionListenerList.h"

#include "minorGems/graphics/openGL/glInclude.h"


/**
 * A textured slider for GL-based GUIs.
 *
 * @author Jason Rohrer
 */
class SliderGL : public GUIComponentGL, public ActionListenerList {


	public:



		/**
		 * Constructs a slider.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 * @param inIconImage the image to display
		 *   to the left of this slider.  Must have dimensions
		 *   that are powers of 2.
		 *   Will be destroyed when this class is destroyed.
		 *   If set to NULL, then no icon will be drawn.
		 * @param inIconWidthFraction the fraction of the slider's
		 *   width that should be taken up by the icon.
		 * @param inBarStartColor the color on the left end of
		 *   the slider bar.
		 *   Will be destroyed when this class is destroyed.
		 * @param inBarEndColor the color on the right end of
		 *   the slider bar.
		 *   Will be destroyed when this class is destroyed.
		 * @param inThumbColor the color of the slider thumb.
		 *   Will be destroyed when this class is destroyed.
		 * @param inBorderColor the color of the slider border.
		 *   Will be destroyed when this class is destroyed.
         * @param inBorderWidth width of the border.
         * @param inThumbWidth width of the slider thumb.
         * @param inMinThumbIncrement the minimum increment
         *   at which to draw the slider thumb.
		 */
		SliderGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, Image *inIconImage,
			double inIconWidthFraction,
			Color *inBarStartColor, Color *inBarEndColor,
			Color *inThumbColor, Color *inBorderColor,
            double inBorderWidth,
            double inThumbWidth,
            double inMinThumbIncrement );


		
		~SliderGL();


        // both are destroyed by this class
        void setBarStartColor( Color *inColor );
        void setBarEndColor( Color *inColor );
        

		
		/**
		 * Gets the position of the slider thumb.
		 *
		 * @return the thumb position, in [0,1.0].
		 */
		double getThumbPosition();



		/**
		 * Sets the position of the slider thumb.
		 *
		 * Note that if the slider thumb position changes
		 * as a result of this call, then an action
		 * will be fired to all registered listeners.
		 *
		 * @param inPosition the thumb position, in [0,1.0].
		 */
		void setThumbPosition( double inPosition );

		
		
		// override funxtions in GUIComponentGL
		virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
		virtual void fireRedraw();


        // is set to true for action fired from first press on a slider 
        // adjustment, false for subsequent dragging for same adjustment
        char mJustPressed;

        // is set to true for action fired from mouse release on slider
        // at end of dragging
        char mJustReleased;
        

        // true iff the slider is currently being dragged
		char mDragging;


		
	protected:

        // common call for press and drag
        virtual void mouseInput( double inX, double inY );


		SingleTextureGL *mIconTexture;
		double mIconWidthFraction;
		
		Color *mBarStartColor;
		Color *mBarEndColor;
		Color *mThumbColor;
		Color *mBorderColor; 

		double mThumbPosition;

        double mBorderWidth;
        double mThumbWidth;
        double mMinThumbIncrement;
        

		


        // gets thumb position rounded to nearest increment
        double getPeggedThumbPosition();
        
	};



inline SliderGL::SliderGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, Image *inIconImage, double inIconWidthFraction,
	Color *inBarStartColor, Color *inBarEndColor,
	Color *inThumbColor, Color *inBorderColor,
    double inBorderWidth,
    double inThumbWidth,
    double inMinThumbIncrement )
	: GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
      mJustPressed( false ),
      mJustReleased( false ),
      mDragging( false ),
	  mIconWidthFraction( inIconWidthFraction ),
	  mBarStartColor( inBarStartColor ),
	  mBarEndColor( inBarEndColor ),
	  mThumbColor( inThumbColor ),
	  mBorderColor( inBorderColor ),
	  mThumbPosition( 0.5 ), 
      mBorderWidth( inBorderWidth ),
      mThumbWidth( inThumbWidth ),
      mMinThumbIncrement( inMinThumbIncrement ) {

    mThumbPosition = (int)( mThumbPosition / mMinThumbIncrement ) 
        * mMinThumbIncrement;
    
	if( inIconImage != NULL ) {
		mIconTexture = new SingleTextureGL( inIconImage );
		// the image is no longer needed
		delete inIconImage;
		}
	else {
		mIconTexture = NULL;
		}
	}



inline SliderGL::~SliderGL() {
	if( mIconTexture != NULL ) {
		delete mIconTexture;
		}

	delete mBarStartColor;
	delete mBarEndColor;
	delete mThumbColor;
	delete mBorderColor; 
	}



inline void SliderGL::setBarStartColor( Color *inColor ) {
    delete mBarStartColor;
    mBarStartColor = inColor;
    }



inline void SliderGL::setBarEndColor( Color *inColor ) {
    delete mBarEndColor;
    mBarEndColor = inColor;
    }



inline double SliderGL::getThumbPosition() {
	return mThumbPosition;
	}



inline void SliderGL::setThumbPosition( double inPosition ) {
	if( mThumbPosition != inPosition ) {
		mThumbPosition = inPosition;
		fireActionPerformed( this );
		}
	}


inline double SliderGL::getPeggedThumbPosition() {
    double barWidth = mWidth * ( 1 - mIconWidthFraction );
    double thumbUsableBarWidth = barWidth - mThumbWidth;

    return
        (int)( mThumbPosition * thumbUsableBarWidth / mMinThumbIncrement ) 
        * mMinThumbIncrement / thumbUsableBarWidth;
    }




inline void SliderGL::mouseInput( double inX, double inY ) {
    if( mDragging ) {

		double barWidth = mWidth * ( 1 - mIconWidthFraction );
		double iconEndX = mAnchorX + mWidth - barWidth;

		// we want the mouse centered over the thumb
		double thumbUsableBarWidth = barWidth - mThumbWidth;

		
		mThumbPosition = ( inX - ( iconEndX + 0.5 * mThumbWidth ) )
			/ thumbUsableBarWidth;

		if( mThumbPosition > 1 ) {
			mThumbPosition = 1;
			}
		else if( mThumbPosition < 0 ) {
			mThumbPosition = 0;
			}

		// fire to listeners
		fireActionPerformed( this );
		}
    }





inline void SliderGL::mouseDragged( double inX, double inY ) {
    mJustPressed = false;
    mJustReleased = false;
    mouseInput( inX, inY );
    }



inline void SliderGL::mousePressed( double inX, double inY ) {
	// presses passed to us only on direct hit 
    // (use to filter mouse activity that starts outside us)
    mDragging = true;
    mJustPressed = true;
    mJustReleased = false;
    mouseInput( inX, inY );
	}



inline void SliderGL::mouseReleased( double inX, double inY ) {
	// always stop dragging on a release
    
    mJustReleased = true;
    // one last event
    mouseInput( inX, inY );

    mJustPressed = false;
	mDragging = false;
	}


		
inline void SliderGL::fireRedraw() {

	// these values will change below if there is an icon
	double barStartX = mAnchorX;
	double barWidth = mWidth;

	if( mIconTexture != NULL ){
		// first, draw the icon

	
		// set our texture
		mIconTexture->enable();

		double textureStartX = mAnchorX;
		double textureStartY = mAnchorY;

		double textureEndX = mAnchorX + mIconWidthFraction * mWidth;
		double textureEndY = mAnchorY + mHeight;
	
		glBegin( GL_QUADS ); {
			// make sure our color is set to white for our texture
			// (to avoid leftover colors)
			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
			
			glTexCoord2f( 0, 1.0f );
			glVertex2d( textureStartX, textureStartY );

			glTexCoord2f( 1.0f, 1.0f );
			glVertex2d( textureEndX, textureStartY ); 

			glTexCoord2f( 1.0f, 0 );
			glVertex2d( textureEndX, textureEndY );
		
			glTexCoord2f( 0, 0 );
			glVertex2d( textureStartX, textureEndY );
			}
		glEnd();

		mIconTexture->disable();


		barWidth = mWidth * ( 1 - mIconWidthFraction );
		barStartX = textureEndX;

		} // end check for a non-NULL icon texture

	

	// now, draw the slider bar

	double barHeight = 0.5 * mHeight;

	// center the bar vertically
	double barStartY = mAnchorY + ( mHeight - barHeight ) * 0.5;


    // draw it's border rectangle underneath
	glColor4f( mBorderColor->r,
			   mBorderColor->g, mBorderColor->b, 1.0f );
	glBegin( GL_QUADS ); {
		glVertex2d( barStartX, barStartY );		
		glVertex2d( barStartX, barStartY + barHeight );
		
		glVertex2d( barStartX + barWidth, barStartY + barHeight );
		glVertex2d( barStartX + barWidth, barStartY ); 
		}
	glEnd();


	
	// draw its gradient-filled center, smaller
	glBegin( GL_QUADS ); {
		
		// start of bar
		glColor4f( mBarStartColor->r,
				   mBarStartColor->g, mBarStartColor->b, 1.0f );
		glVertex2d( barStartX + mBorderWidth, barStartY + mBorderWidth );
        glVertex2d( barStartX + mBorderWidth, 
                    barStartY + barHeight - mBorderWidth );

		// end of bar
		glColor4f( mBarEndColor->r,
				   mBarEndColor->g, mBarEndColor->b, 1.0f );
		glVertex2d( barStartX + barWidth - mBorderWidth, 
                    barStartY + barHeight - mBorderWidth );
		glVertex2d( barStartX + barWidth - mBorderWidth, 
                    barStartY + mBorderWidth ); 

		}
	glEnd();


	
	

	// now draw the thumb

	// we don't want the thumb going off the ends of the bar
	double thumbUsableBarWidth = barWidth - mThumbWidth;

	// round to nearest increment
    double thumbDrawPosition = getPeggedThumbPosition();
    
	double thumbStartX = thumbDrawPosition * thumbUsableBarWidth
		+ barStartX;
	double thumbEndX = thumbStartX + mThumbWidth;


    // draw its border, underneath
	glColor4f( mBorderColor->r,
			   mBorderColor->g, mBorderColor->b, 1.0f );
	glBegin( GL_QUADS ); {
		glVertex2d( thumbStartX, mAnchorY );		
		glVertex2d( thumbStartX, mAnchorY + mHeight );

		glVertex2d( thumbEndX, mAnchorY + mHeight );
		glVertex2d( thumbEndX, mAnchorY );
		}
	glEnd();


    // draw fill, smaller
	glColor4f( mThumbColor->r,
               mThumbColor->g, mThumbColor->b, 1.0f );
	glBegin( GL_QUADS ); {
		glVertex2d( thumbStartX + mBorderWidth, mAnchorY + mBorderWidth );
		glVertex2d( thumbStartX + mBorderWidth, 
                    mAnchorY + mHeight - mBorderWidth );

		glVertex2d( thumbEndX - mBorderWidth, 
                    mAnchorY + mHeight - mBorderWidth );
		glVertex2d( thumbEndX - mBorderWidth, mAnchorY + mBorderWidth ); 

		}
	glEnd();


	}



#endif



