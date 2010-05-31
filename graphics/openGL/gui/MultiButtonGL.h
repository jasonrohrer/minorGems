/*
 * Modification History
 *
 * 2001-September-16		Jason Rohrer
 * Created.
 *
 * 2001-November-3		Jason Rohrer
 * Fixed a gutter size bug that occurred when there was
 * only one row or column.
 * Changed so that rows are filled before adding new columns.
 * Fixed a bug in setSelectedIndex.
 */
 
 
#ifndef MULTI_BUTTON_GL_INCLUDED
#define MULTI_BUTTON_GL_INCLUDED 

#include "GUIContainerGL.h"
#include "StickyButtonGL.h"

#include "minorGems/graphics/Image.h"

#include "minorGems/ui/event/ActionListenerList.h"
#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/ui/GUIComponent.h"

#include <math.h>
#include <stdio.h>

/**
 * A set of buttons that allows only one to be depressed
 * at a time.
 *
 * @author Jason Rohrer
 */
class MultiButtonGL : public GUIContainerGL,
					public ActionListener,
					public ActionListenerList {


	public:


		
		/**
		 * Constructs a set of buttons.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 * @param inNumButtons the number of buttons in the set.
		 * @param inUnpressedImages the images to display
		 *   when each button is unpressed.  Images must have dimensions
		 *   that are powers of 2.
		 *   Will be destroyed when this class is destroyed.
		 * @param inPressedImages the images to display
		 *   when each button is pressed.  Images must have dimensions
		 *   that are powers of 2.
		 *   Will be destroyed when this class is destroyed.
		 * @param inGutterFraction the fraction of each row
		 *   and column that should be taken up by the gutters
		 *   (the spaces between buttons).  In [0, 1.0].
		 */
		MultiButtonGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, int inNumButtons,
			Image **inUnpressedImages,
			Image **inPressedImages,
			double inGutterFraction );


		
		~MultiButtonGL();


		
		/**
		 * Sets the currently depressed button.
		 *
		 * Note that if this function causes a change
		 * in the state of the set, an action will
		 * be fired to all registered listeners.
		 *
		 * @param inButtonIndex the index of the button to depress.
		 */
		void setSelectedButton( int inButtonIndex );



		/**
		 * Gets the index of the currently depressed button.
		 *
		 * Note that if this function causes a change
		 * in the state of the set, an action will
		 * be fired to all registered listeners.
		 *
		 * @return the index of the button that is depressed.
		 */
		int getSelectedButton();


		
		// we don't need to override any mouse
		// or redraw functions, since the container
		// and the buttons should handle these correctly


		// implements the ActionListener interface
		virtual void actionPerformed( GUIComponent *inTarget );
		
		
	protected:
		int mNumButtons;
		
		StickyButtonGL **mButtons;


		int mSelectedIndex;


		char mIgnoreEvents;
		
		/**
		 * Maps a button in mButtons to its index in the
		 * mButtons array.
		 *
		 * @param inButton the button to get an index for.
		 *
		 * @return the index of inButton in the mButtons array.
		 */
		int buttonToIndex( StickyButtonGL *inButton );

		
		
	};



inline MultiButtonGL::MultiButtonGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, int inNumButtons,
	Image **inUnpressedImages,
	Image **inPressedImages,
	double inGutterFraction )
	: GUIContainerGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mNumButtons( inNumButtons ),
	  mButtons( new StickyButtonGL*[inNumButtons] ),
	  mSelectedIndex( 0 ),
	  mIgnoreEvents( false ) {

	int numColumns = (int)( sqrt( mNumButtons ) );
	int numRows = mNumButtons / numColumns;

	
	while( numRows * numColumns < mNumButtons ) {
		numRows++;
		}

	// determine gutter and button sizes
	
	double gutterSizeX;
	double gutterSizeY;
	
	if( numRows == 1 ) {
		gutterSizeY = 0;
		}
	else {
		gutterSizeY =
			( inHeight * inGutterFraction ) / ( numRows - 1 );
		}
	if( numColumns == 1 ) {
		gutterSizeX = 0;
		}
	else {
		gutterSizeX =
			( inWidth * inGutterFraction ) / ( numColumns - 1 );
		}
	
	
	double buttonSizeX =
		( inWidth * ( 1 - inGutterFraction ) ) / ( numColumns );
	double buttonSizeY =
		( inHeight * ( 1 - inGutterFraction ) ) / ( numRows );


	// setup each button
	
	int buttonIndex = 0;

	for( int r=0; r<numRows; r++ ) {
		for( int c=0; c<numColumns; c++ ) {
			
			double anchorX = inAnchorX +
				c * ( buttonSizeX + gutterSizeX );
			double anchorY = inAnchorY +
				r * ( buttonSizeY + gutterSizeY );

			mButtons[ buttonIndex ] =
				new StickyButtonGL( anchorX, anchorY,
									buttonSizeX, buttonSizeY,
									inUnpressedImages[ buttonIndex ],
									inPressedImages[ buttonIndex ] );

			GUIContainerGL::add( mButtons[ buttonIndex ] );
			
			buttonIndex++;

			if( buttonIndex >= mNumButtons ) {
				// jump out of our loop if we run out of buttons
				c = numColumns;
				r = numRows;
				}
			}
		}

	// now we've added all of our buttons

	// press one of them before we add any listeners
	mButtons[ mSelectedIndex ]->setPressed( true );
	

	// now add ourselves as an action listener to each button
	for( int i=0; i<mNumButtons; i++ ) {
		mButtons[ i ]->addActionListener( this );
		}

	// delete the arrays pointing to the images, since
	// they are no longer needed
	delete [] inUnpressedImages;
	delete [] inPressedImages;
	
	}


		
inline MultiButtonGL::~MultiButtonGL() {
	// note that we don't need to delete the buttons
	// themselves, since they will be deleted by GUIContainer
	// destructor
	
	delete [] mButtons;
	}



inline void MultiButtonGL::setSelectedButton( int inButtonIndex ) {
	// simply press the appropriate button, and let the
	// action handlers do the rest
	
	mButtons[ inButtonIndex ]->setPressed( true );
	}



inline int MultiButtonGL::getSelectedButton() {
	return mSelectedIndex;
	}



inline void MultiButtonGL::actionPerformed( GUIComponent *inTarget ) {
	if( !mIgnoreEvents ) {
		int buttonIndex = buttonToIndex( (StickyButtonGL*)inTarget );

		// if another button is being pressed
		if( buttonIndex != mSelectedIndex
			&& mButtons[ buttonIndex ]->isPressed() ) {

		
			// unpress the old button, ignoring the resulting event
			mIgnoreEvents = true;
			mButtons[ mSelectedIndex ]->setPressed( false );
			mIgnoreEvents = false;
			
			mSelectedIndex = buttonIndex;
			
			fireActionPerformed( this );
			}
		// else if our selected button is being unpressed
		else if( buttonIndex == mSelectedIndex
				 && !( mButtons[ buttonIndex ]->isPressed() ) ) {
			// don't allow it to become unpressed

			// re-press it, ignoring the resulting event
			mIgnoreEvents = true;
			mButtons[ mSelectedIndex ]->setPressed( true );
			mIgnoreEvents = false;

			// don't fire an action, since our state
			// has not changed
			}
		}
	}



inline int MultiButtonGL::buttonToIndex( StickyButtonGL *inButton ) {
	for( int i=0; i<mNumButtons; i++ ) {
		if( mButtons[i] == inButton ) {
			return i;
			}
		}
	
	// return the first button index by default
	printf( "MultiButtonGL:  no matching button found.\n" );
	return 0;
	}



#endif
