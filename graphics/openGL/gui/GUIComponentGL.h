/*
 * Modification History
 *
 * 2001-September-15		Jason Rohrer
 * Created.
 * Changed to use normalized floating point coordinates.
 * Fixed some compile bugs.
 * Changed to extend the GUIComponent class.
 *
 * 2001-October-29		Jason Rohrer
 * Added support for focus.
 *
 * 2006-July-4    Jason Rohrer
 * Added support for disabled components.
 *
 * 2008-October-1    Jason Rohrer
 * Added position changing and gettting functions.
 *
 * 2009-December-28    Jason Rohrer
 * Added support for locking focus.
 */
 
 
#ifndef GUI_COMPONENT_GL_INCLUDED
#define GUI_COMPONENT_GL_INCLUDED 


#include "minorGems/graphics/openGL/RedrawListenerGL.h"

#include "minorGems/ui/GUIComponent.h"


// for key codes
#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"


/**
 * A base class for all OpenGL GUI components.
 *
 * Note that all coordinates are specified in a normalized
 * [0,1.0] space, where the screen is spanned by this [0,1.0] range.
 *
 * @author Jason Rohrer
 */
class GUIComponentGL : public GUIComponent, public RedrawListenerGL {


	public:


        virtual ~GUIComponentGL();



        /**
		 * Sets the position of this component.
		 *
         * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 */
		void setPosition( double inAnchorX, double inAnchorY, double inWidth,
                          double inHeight );


        // functions for getting position
        double getAnchorX() {
            return mAnchorX;
            }

        double getAnchorY() {
            return mAnchorY;
            }

        double getWidth() {
            return mWidth;
            }

        double getHeight() {
            return mHeight;
            }
        
        
        
        
        /**
         * Sets the enabled state of this component.
         *
         * @param inEnabled true to enable, or false to disable.
         */
        virtual void setEnabled( char inEnabled );


        
        /**
         * Gets the enabled state of this component.
         *
         * @return true to enable, or false to disable.
         */
        virtual char isEnabled();

        
		
		/**
		 * Tests whether a point is inside this component.
		 *
		 * @param inX the x value to check.
		 * @param inY the y value to check.
		 *
		 * @return true iff the point is insided this component.
		 */
		virtual char isInside( double inX, double inY );

		

		/**
		 * Sets this component's focus status.
		 *
		 * Note that this component may ignore this function call
		 * and configure its own focus (for example, some
		 * components are never focused).
		 *
		 * The default implementation ignores this call
		 * and is never focused.
		 *
		 * @param inFocus true iff this component should be focused.
		 */
		virtual void setFocus( char inFocus );


        /**
         * Locks the focus onto this component so that it keeps it
         * despite mouse clicks that would cause it to lose focus.
         *
         * @param inFocusLocked true to lock focus onto this component.
         */
        virtual void lockFocus( char inFocusLocked );
        
        virtual char isFocusLocked();
        
		

		// the implementations below do nothing, but they allow
		// subclasses to pick which input they care about (and which
		// functions they want to override)
		
		// implements a normalized (to [0,1.0] version
		// of a MouseHandlerGL-like interface
		virtual void mouseMoved( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
		
		// implements a normalized (to [0,1.0] version
		// of a KeyboardHandlerGL-like interface
		virtual char isFocused();
		virtual void keyPressed( unsigned char inKey, double inX, double inY );
		virtual void specialKeyPressed( int inKey, double inX, double inY );
		virtual void keyReleased( unsigned char inKey,
								  double inX, double inY );
		virtual void specialKeyReleased( int inKey, double inX, double inY );

		
		// implements the RedrawListenerGL interface
		virtual void fireRedraw();


		
	protected:



		/**
		 * Constructs a component.
		 *
		 * Should only be called by subclass constructors.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 */
		GUIComponentGL( double inAnchorX, double inAnchorY, double inWidth,
						double inHeight );


		
		double mAnchorX;
		double mAnchorY;
		double mWidth;
		double mHeight;

        char mEnabled;

        char mFocusLocked;
        
		
	};



inline GUIComponentGL::GUIComponentGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight )
	: mAnchorX( inAnchorX ), mAnchorY( inAnchorY ), mWidth( inWidth ),
	  mHeight( inHeight ),
      mEnabled( true ),
      mFocusLocked( false ) {

	}



inline GUIComponentGL::~GUIComponentGL() {
    }



inline void GUIComponentGL::setPosition( 
    double inAnchorX, double inAnchorY, double inWidth, double inHeight ) {
    
    mAnchorX = inAnchorX;
    mAnchorY = inAnchorY;
    mWidth = inWidth;
    mHeight = inHeight;
    }



inline void GUIComponentGL::setEnabled( char inEnabled ) {
    mEnabled = inEnabled;
    }



inline char GUIComponentGL::isEnabled() {
    return mEnabled;
    }



inline char GUIComponentGL::isInside( double inX, double inY ) {
	if( inX >= mAnchorX && inX < mAnchorX + mWidth
		&& inY >= mAnchorY && inY < mAnchorY + mHeight ) {

		return true;
		}
	else {
		return false;
		}
	}

		

inline void GUIComponentGL::mouseMoved( double inX, double inY ) {
	}



inline void GUIComponentGL::mouseDragged( double inX, double inY ) {
	}



inline void GUIComponentGL::mousePressed( double inX, double inY ) {
	}



inline void GUIComponentGL::mouseReleased( double inX, double inY ) {
	}



inline void GUIComponentGL::setFocus( char inFocus ) {
	// default implementation ignores this call
	}


inline void GUIComponentGL::lockFocus( char inFocusLocked ) {
    mFocusLocked = inFocusLocked;
    }



inline char GUIComponentGL::isFocusLocked() {
	return mFocusLocked;
	}



inline char GUIComponentGL::isFocused() {
	return false;
	}



inline void GUIComponentGL::keyPressed(
	unsigned char inKey, double inX, double inY ) {
	}



inline void GUIComponentGL::specialKeyPressed(
	int inKey, double inX, double inY ) {
	}



inline void GUIComponentGL::keyReleased(
	unsigned char inKey, double inX, double inY ) {
	}



inline void GUIComponentGL::specialKeyReleased(
	int inKey, double inX, double inY ) {
	}


		
inline void GUIComponentGL::fireRedraw() {
	}



#endif


