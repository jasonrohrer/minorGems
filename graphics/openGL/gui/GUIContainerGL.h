/*
 * Modification History
 *
 * 2001-September-15		Jason Rohrer
 * Created.
 * Changed to use normalized floating point coordinates.
 * Fixed some compile bugs.
 * Changed so that mouse released events are passed to all components.
 *
 * 2001-October-29		Jason Rohrer
 * Added support for focus, including mouse-release to focus.
 *
 * 2001-November-2		Jason Rohrer
 * Changed to send keyboard events only to focused components,
 * regardless of mouse position. 
 *
 * 2006-July-3		Jason Rohrer
 * Fixed warnings.
 *
 * 2006-September-8  Jason Rohrer
 * Switched to passing mouse motion events to all components.
 *
 * 2006-December-14  Jason Rohrer
 * Added function for testing containment.
 *
 * 2009-December-22  Jason Rohrer
 * New SimpleVector delete call.
 *
 * 2009-December-28    Jason Rohrer
 * Added support for locking focus.
 *
 * 2010-April-8		Jason Rohrer
 * Fixed focus behavior for presses that didn't start on a component.
 * Only pass focus to enabled components.
 *
 * 2010-April-9		Jason Rohrer
 * Fixed crash when components removed by a mouse event.
 */
 
 
#ifndef GUI_CONTAINER_GL_INCLUDED
#define GUI_CONTAINER_GL_INCLUDED 

#include "GUIComponentGL.h"
#include "minorGems/util/SimpleVector.h"



/**
 * A container full of gui components that delegates
 * redraw and ui events to the contained components.
 *
 * @author Jason Rohrer
 */
class GUIContainerGL : public GUIComponentGL {


	public:



		/**
		 * Constructs a container.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 */
		GUIContainerGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight );


		
		virtual ~GUIContainerGL();


			
		/**
		 * Adds a component to this container.
		 *
		 * Note that the added component should lie within the bounds
		 * of this container (or else event delegation may not function
		 * properly).
		 *
		 * @param inComponent the component to add.  Is destroyed
		 *   when this container is destroyed.
		 */
		virtual void add( GUIComponentGL *inComponent );


		
		/**
		 * Removes a component from this container.
		 *
		 * @param inComponent the component to remove.  Must be
		 *   destroyed by the caller.
		 *
		 * @return true iff the component was removed successfully.
		 */
		virtual char remove( GUIComponentGL *inComponent );


        
        /**
         * Gets whether this container contains a given component.
         *
         * @param inComponent the component to look for.
         *
         * @return true if inComponent is in this container.
         */
        virtual char contains( GUIComponentGL *inComponent );

        
		
		// the implementations below delegate events to appropriate
		// contained components
		
		// override functions in GUIComponentGL
		virtual void mouseMoved( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
		virtual char isFocused();
		virtual void keyPressed( unsigned char inKey, double inX, double inY );
		virtual void specialKeyPressed( int inKey, double inX, double inY );
		virtual void keyReleased( unsigned char inKey,
								  double inX, double inY );
		virtual void specialKeyReleased( int inKey, double inX, double inY );
		virtual void fireRedraw();


		
	protected:
		SimpleVector<GUIComponentGL*> *mComponentVector;
        
        // flags to track components in which a press started
        SimpleVector<char> *mPressStartedHereVector;
        
				

	};



inline GUIContainerGL::GUIContainerGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight )
	: GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mComponentVector( new SimpleVector<GUIComponentGL*>() ),
      mPressStartedHereVector( new SimpleVector<char>() ) {

	}



inline GUIContainerGL::~GUIContainerGL() {
	// delete each contained component
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		delete component;
		}
	
	delete mComponentVector;
    delete mPressStartedHereVector;
	}



inline void GUIContainerGL::add( GUIComponentGL *inComponent ) {
	mComponentVector->push_back( inComponent );
	mPressStartedHereVector->push_back( false );
    }



inline char GUIContainerGL::remove( GUIComponentGL *inComponent ) {
	int index = mComponentVector->getElementIndex( inComponent );
    
    if( index != -1 ) {
        
        mComponentVector->deleteElement( index );
        mPressStartedHereVector->deleteElement( index );
        return true;
        }
    return false;
    }



inline char GUIContainerGL::contains( GUIComponentGL *inComponent ) {
    int index = mComponentVector->getElementIndex( inComponent );

    if( index >= 0 ) {
        return true;
        }
    else {
        return false;
        }
    }



inline void GUIContainerGL::mouseMoved( double inX, double inY ) {
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
        component->mouseMoved( inX, inY );
		}
	}



inline void GUIContainerGL::mouseDragged( double inX, double inY ) {
	// released events should be passed to all components
	// so that a pressed component can react if the mouse
	// is dragged elsewhere
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		component->mouseDragged( inX, inY );
		}
	}



inline void GUIContainerGL::mousePressed( double inX, double inY ) {
	// only pass pressed events to components
	// that contain the coordinate pressed
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		if( component->isInside( inX, inY ) ) {
			component->mousePressed( inX, inY );

            // components MIGHT get removed by the event call, thus, our
            // vector might be too short for i now

            if( i < mComponentVector->size() ) {  
                *( mPressStartedHereVector->getElement( i ) ) = true;
                }
			}
        else {
            // press started elsewhere
            *( mPressStartedHereVector->getElement( i ) ) = false;
            }
		}
	}



inline void GUIContainerGL::mouseReleased( double inX, double inY ) {
	// first, unfocus all components, but skip those that are locked
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
        if( !component->isFocusLocked() ) {
            component->setFocus( false );
            }
		}
	
	// released events should be passed to all components
	// so that a pressed component can react if the mouse
	// is released elsewhere
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		component->mouseReleased( inX, inY );


        // components MIGHT get removed by the event call, thus, our
        // vector might be too short for i now

        if( i < mComponentVector->size() ) {    
            // focus any components hit by the release directly
            // IF press started there
            if( component->isInside( inX, inY ) &&
                component->isEnabled() &&
                *( mPressStartedHereVector->getElement(i) ) ) {
                component->setFocus( true );
                }
            }
        
		}

	}



inline char GUIContainerGL::isFocused() {
	// we are focused if any of our sub-components are focused	
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		if( component->isFocused() ) {
			return true;
			}
		}
	
	// else, none focused
	return false;
	}


		
inline void GUIContainerGL::keyPressed(
	unsigned char inKey, double inX, double inY ) {

	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		
		// only send events to focused components
		if( component->isFocused() ) {
			component->keyPressed( inKey, inX, inY );
			}
		}
	}



inline void GUIContainerGL::specialKeyPressed(
	int inKey, double inX, double inY ) {

	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );

		// only send events to focused components
		if( component->isFocused() ) {
			component->specialKeyPressed( inKey, inX, inY );
			}
		}
	}



inline void GUIContainerGL::keyReleased(
	unsigned char inKey, double inX, double inY ) {

	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );

		// only send events to focused components
		if( component->isFocused() ) {
			component->keyReleased( inKey, inX, inY );
			}
		}
	}



inline void GUIContainerGL::specialKeyReleased(
	int inKey, double inX, double inY ) {

	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );

		// only send events to focused components
		if( component->isFocused() ) {
			component->specialKeyReleased( inKey, inX, inY );
			}
		}
	}


		
inline void GUIContainerGL::fireRedraw() {
	for( int i=0; i<mComponentVector->size(); i++ ) {
		GUIComponentGL *component = *( mComponentVector->getElement( i ) );
		component->fireRedraw();
		}
	}



#endif



