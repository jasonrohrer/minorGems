/*
 * Modification History
 *
 * 2001-October-29	Jason Rohrer
 * Created.
 *
 * 2001-November-2	Jason Rohrer
 * Added support for cursor position.
 * Added support for clicking to reposition the cursor.
 * Fixed an array out-of-bounds bug in the keyReleased function.
 *
 * 2008-September-29	Jason Rohrer
 * Improved text width handling and cursor spacing.
 * Added length limit.
 *
 * 2008-October-27	Jason Rohrer
 * Switched to implementation-independent keycodes.
 * Switched to key press (instead of release) events.
 *
 * 2008-November-1	Jason Rohrer
 * Added support for forced upper case.
 *
 * 2009-December-29	 Jason Rohrer
 * Added support for setting cursor position.
 *
 * 2010-January-1	 Jason Rohrer
 * Does NOT fire event when text changed externally.
 *
 * 2010-January-11	 Jason Rohrer
 * Fixed mouse-based cursor positioning.
 *
 * 2010-January-27	 Jason Rohrer
 * Fixed bug in text editing.
 *
 * 2010-January-29	 Jason Rohrer
 * Added parameter for border width.
 *
 * 2010-March-6	 Jason Rohrer
 * Added support for ignoring one key-press.
 *
 * 2010-March-31		Jason Rohrer
 * Changed glColor3f to 4f to fix driver bug on some video cards.
 *
 * 2010-April-6		Jason Rohrer
 * Enlarged click area.
 *
 * 2010-April-8		Jason Rohrer
 * Fixed behavior for presses that didn't start on this field.
 *
 * 2010-May-7   Jason Rohrer
 * Extended ASCII.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2010-May-25    Jason Rohrer
 * Based border offset on border width (instead of 1/8 height).
 */
 
 
#ifndef TEXT_FIELD_GL_INCLUDED
#define TEXT_FIELD_GL_INCLUDED 

#include "GUIComponentGL.h"
#include "TextGL.h"

#include "minorGems/graphics/Color.h"

#include "minorGems/util/stringUtils.h"

#include "minorGems/ui/event/ActionListenerList.h"




/**
 * A text field for OpenGL-based GUIs.
 * Will fire an event to listeners every time text changes
 *
 * @author Jason Rohrer
 */
class TextFieldGL : public GUIComponentGL, public ActionListenerList {


	public:



		/**
		 * Constructs a text field.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
         * @param inBorderWidth width of the border and the cursor.
		 * @param inString the text to display in this text field.
		 *    Is copied internally, so must be destroyed
		 *    by caller if not const.
		 * @param inText the text object to use when drawing
		 *   this textField.  Must be destroyed by caller after
		 *   this class is destroyed.
		 * @param inBorderColor the color of this field's border.
		 *   Will be destroyed when this class is destroyed.
		 * @param inFocusedBorderColor the color of this field's border
		 *   when the field is focused.
		 *   Will be destroyed when this class is destroyed.
		 * @param inBackgroundColor the color of this field's background.
		 *   Will be destroyed when this class is destroyed.
		 * @param inCursorColor the color of this field's cursor line.
		 *   Will be destroyed when this class is destroyed.
         * @param inLengthLimit the length limit for the entered string,
         *   or -1 for no limit.  Defaults to -1.
         * @param inForceUppercase true to force upper case for text entered.
         *   Defaults to false.
		 */
		TextFieldGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, double inBorderWidth,
            const char *inString, TextGL *inText,
			Color *inBorderColor, Color *inFocusedBorderColor,
			Color *inBackgroundColor, Color *inCursorColor,
            int inLengthLimit = -1,
            char inForceUppercase = false );


		
		~TextFieldGL();


		
		/**
		 * Sets the text displayed by this text field.
		 *
		 * @param inString the text to display in this text field.
		 *    Is copied internally, so must be destroyed
		 *    by caller if not const.
		 */
		void setText( const char *inString );


		
		/**
		 * Gets the text displayed by this text field.
		 *
		 * @return the text to display in this textField.
		 *    Must not be destroyed or modified by caller.
		 */
		char *getText();


        /**
         * Ignore next 1 key press.
         *
         * Useful for when this field becomes focused due to a key press
         * (like from a key-based menu selection) but we don't want that
         * key press passed immediately into the field.
         */
        void ignoreNextKey();
        


        void setCursorPosition( int inCharPosition );
        

		
		// override functions in GUIComponentGL
		virtual void setFocus( char inFocus );
		virtual char isFocused();
		virtual void keyPressed( unsigned char inKey, double inX, double inY );
		virtual void specialKeyPressed( int inKey, double inX, double inY );
		virtual void keyReleased( unsigned char inKey,
								  double inX, double inY );
		virtual void specialKeyReleased( int inKey, double inX, double inY );

        virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
		
		virtual void fireRedraw();



        // override this to make click area slightly bigger
        // (so it includes the border)
        virtual char isInside( double inX, double inY );
		
	protected:


		
		/**
		 * Adjusts the cursor position to bring it in bounds of the
		 * current string.
		 */
		void fixCursorPosition();

		
		
		TextGL *mText;

		char *mString;

		Color *mBorderColor;
		Color *mFocusedBorderColor;
		Color *mBackgroundColor;
		Color *mCursorColor;
		
		Color *mCurrentBorderColor;
		
        int mLengthLimit;
        char mForceUppercase;
        
		char mFocused;

		// cursor position, in number of characters
		int mCursorPosition;

        double mBorderWidth;

        char mIgnoreNextKey;

        char mPressStartedOnUs;
	};



inline TextFieldGL::TextFieldGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, double inBorderWidth,
    const char *inString, TextGL *inText,
	Color *inBorderColor, Color *inFocusedBorderColor,
	Color *inBackgroundColor, Color *inCursorColor,
    int inLengthLimit,
    char inForceUppercase )
	: GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mText( inText ), mString( NULL ),
	  mBorderColor( inBorderColor ),
	  mFocusedBorderColor( inFocusedBorderColor ),
	  mBackgroundColor( inBackgroundColor ),
	  mCursorColor( inCursorColor ),
	  mCurrentBorderColor( inBorderColor ),
      mLengthLimit( inLengthLimit ),
      mForceUppercase( inForceUppercase ),
	  mFocused( false ),
	  mCursorPosition( 0 ),
      mBorderWidth( inBorderWidth ),
      mIgnoreNextKey( false  ),
      mPressStartedOnUs( false ) {

	setText( inString );
	}



inline TextFieldGL::~TextFieldGL() {
	if( mString != NULL ) {
		delete [] mString;
		}

	delete mBorderColor;
	delete mFocusedBorderColor;
	delete mBackgroundColor;
	delete mCursorColor;
	}



inline void TextFieldGL::setText( const char *inString ) {
	if( mString != NULL ) {
		delete [] mString;
		}
	int length = strlen( inString ) + 1;
	
	mString = new char[ length ];

	memcpy( mString, inString, length );
    }



inline char *TextFieldGL::getText() {
	return mString;
	}



inline void TextFieldGL::ignoreNextKey() {
    mIgnoreNextKey = true;
    }



inline void TextFieldGL::setCursorPosition( int inCharPosition ) {
    int maxPosition = strlen( mString );
    if( inCharPosition > maxPosition ) {
        inCharPosition = maxPosition;
        }
    mCursorPosition = inCharPosition;
    }



inline void TextFieldGL::setFocus( char inFocus ) {
	mFocused = inFocus;

	if( mFocused ) {
		mCurrentBorderColor = mFocusedBorderColor;
		}
	else {
		mCurrentBorderColor = mBorderColor;
		}
	}



inline char TextFieldGL::isFocused() {
	return mFocused;
	}



inline void TextFieldGL::keyPressed(
	unsigned char inKey, double inX, double inY ) {

    if( mIgnoreNextKey ) {
        mIgnoreNextKey = false;
        return;
        }
    

    //printf( "key press: %d\n", inKey );
	
	// backspace and delete
	if( inKey == 127 || inKey == 8 ) {
		if( mCursorPosition != 0 ) {
			int length = strlen( mString );

			if( length != 0 ) {
				char *newString = new char[ length ];

				// copy mString up to the last char before the deleted char
				memcpy( newString, mString, mCursorPosition-1 );

				// copy the portion of mString after the deleted char
				// this will include the trailing \0
				memcpy( &( newString[mCursorPosition-1] ),
						&( mString[mCursorPosition] ),
						length - mCursorPosition + 1 );
			
				setText( newString );
                
				delete [] newString;

				mCursorPosition--;

                fireActionPerformed( this );
				}
			}
		}
	// allowable character key, from space up to tilde, then extended ascii
    // ignore after length limit reached
	else if( ( (inKey >= 32 && inKey <= 126)
               ||
               (inKey >= 160 ) )
             && 
             ( mLengthLimit < 0 || (int)strlen( mString ) < mLengthLimit ) ) {
		// add a character to our string

		int oldStringLength = strlen( mString ); 
		int length = oldStringLength + 2;
		
		char *newString = new char[ length ];

		
		if( mCursorPosition != 0 ) {
			// copy chars up to cursor position
			memcpy( newString, mString, mCursorPosition );
			}
		// copy chars after cursor position, including trailing \0
		memcpy( &( newString[mCursorPosition+1] ),
				&( mString[mCursorPosition] ),
				oldStringLength - mCursorPosition + 1 );

		

        if( mForceUppercase ) {
            inKey = toupper( inKey );
            }

        // now stick in the inserted char
		newString[ mCursorPosition ] = inKey;
		
		setText( newString );
		
		delete [] newString;

		mCursorPosition++;
        
        fireActionPerformed( this );
		}
	
	// else a non-valid key hit

	}



inline void TextFieldGL::specialKeyPressed(
	int inKey, double inX, double inY ) {
    
    if( mIgnoreNextKey ) {
        mIgnoreNextKey = false;
        return;
        }
    
	switch( inKey ) {
		case MG_KEY_RIGHT:
			mCursorPosition++;
			break;
		case MG_KEY_LEFT:
			mCursorPosition--;
		default:
			break;
		}

	fixCursorPosition();
	}



inline void TextFieldGL::keyReleased(
	unsigned char inKey, double inX, double inY ) {

	}



inline void TextFieldGL::specialKeyReleased(
	int inKey, double inX, double inY ) {

	}



inline char TextFieldGL::isInside( double inX, double inY ) {
    // offset to give text room
    double offset = mBorderWidth;//mHeight * 0.125;


    // draw border quad behind background
    double borderOffset = offset + mBorderWidth;

    if( inX >= mAnchorX - borderOffset && 
        inX < mAnchorX + mWidth + borderOffset
		&& 
        inY >= mAnchorY - borderOffset && 
        inY < mAnchorY + mHeight + borderOffset ) {

		return true;
		}
	else {
		return false;
		}
    
    }



inline void TextFieldGL::mousePressed( double inX, double inY ) {
    if( isEnabled() ) {
        // we'll only get pressed events if the mouse is pressed on us
        // so we don't need to check isInside status
        mPressStartedOnUs = true;
        }
    }



inline void TextFieldGL::mouseReleased(
	double inX, double inY ) {

	if( isInside( inX, inY ) && mPressStartedOnUs ) {

        double offset = mHeight * 0.1;

        int numChars = strlen( mString );
        
        // measure longer substrings until we find place where mouse clicked
        char found = false;
        for( int i=0; i<numChars + 1 && !found; i++ ) {
            
            // make a temp sub-string and measure its length
            char *substring = stringDuplicate( mString );
            substring[ i ] = '\0';
            

            double subWidth = mText->measureTextWidth( substring ) * mHeight 
                + offset;
			
            if( i < numChars ) {
                // if click is halfway into next char, put cursor before
                // next char
                char nextChar[2] = { mString[i], '\0' };
                subWidth += 
                    mText->measureTextWidth( nextChar ) * mHeight * 0.5;
                }
            

            delete [] substring;

            if( subWidth > inX - mAnchorX ) {
                mCursorPosition = i;
                found = true;
                }
            }
        
        if( !found ) {
            // stick cursor at very end
            mCursorPosition = numChars;
            }
        
        
        /*
		mCursorPosition = (int)( 0.5 + strlen( mString ) *
								 ( inX - mAnchorX ) / mWidth );
        */
		}

    // reset for next time
    mPressStartedOnUs = false;
	}



inline void TextFieldGL::fixCursorPosition() {
	if( mCursorPosition < 0 ) {
		mCursorPosition = 0;
		}
	else {
		int stringLength = strlen( mString );
		if( mCursorPosition > stringLength ) {
			mCursorPosition = stringLength;
			}
		}
	}


		
inline void TextFieldGL::fireRedraw() {
    
    // offset to give text room
    double offset = mBorderWidth;//mHeight * 0.125;


    // draw border quad behind background
    double borderOffset = offset + mBorderWidth;

    glColor4f( mCurrentBorderColor->r,
			   mCurrentBorderColor->g, mCurrentBorderColor->b, 1.0f );
	glBegin( GL_QUADS ); {
		glVertex2d( mAnchorX - borderOffset, mAnchorY - borderOffset );		
		glVertex2d( mAnchorX - borderOffset, 
                    mAnchorY + mHeight + borderOffset );

		glVertex2d( mAnchorX + mWidth + borderOffset, 
                    mAnchorY + mHeight + borderOffset );
		glVertex2d( mAnchorX + mWidth + borderOffset, 
                    mAnchorY - borderOffset ); 

		}
	glEnd();


	// draw the background 
	glColor4f( mBackgroundColor->r,
			   mBackgroundColor->g, mBackgroundColor->b, 1.0f );
	glBegin( GL_QUADS ); {
		glVertex2d( mAnchorX - offset, mAnchorY - offset );		
		glVertex2d( mAnchorX - offset, mAnchorY + mHeight + offset );

		glVertex2d( mAnchorX + mWidth + offset, mAnchorY + mHeight + offset );
		glVertex2d( mAnchorX + mWidth + offset, mAnchorY - offset ); 

		}
	glEnd();

    
    double charWidth = mHeight * strlen( mString );
    
	
	// draw the text
	mText->drawText( mString, mAnchorX, mAnchorY,
					 charWidth, mHeight );
    
    if( mFocused ) {
		// draw the cursor
		glColor4f( mCursorColor->r,
				   mCursorColor->g, mCursorColor->b, 1.0f );
		/*
        glBegin( GL_LINES ); {
            
            // make a temp sub-string and measure its length
            char *substring = stringDuplicate( mString );
            substring[ mCursorPosition ] = '\0';
            

            double subWidth = mText->measureTextWidth( substring ) * mHeight 
                + offset;
			
            delete [] substring;
            

            double cursorViewX = mAnchorX + subWidth;
            
            glVertex2d( cursorViewX, mAnchorY );		
			glVertex2d( cursorViewX, mAnchorY + mHeight );
			}
		glEnd();
        */
        // make a temp sub-string and measure its length
        char *substring = stringDuplicate( mString );
        substring[ mCursorPosition ] = '\0';
        
        
        double subWidth = mText->measureTextWidth( substring ) * mHeight 
            ;//+ offset;
        
        delete [] substring;
        
        
        double cursorViewX = mAnchorX + subWidth;
        
        double cursorOffset = mBorderWidth / 2;
        glBegin( GL_QUADS ); {
            glVertex2d( cursorViewX - cursorOffset, mAnchorY );		
			glVertex2d( cursorViewX - cursorOffset, mAnchorY + mHeight );
            glVertex2d( cursorViewX + cursorOffset, mAnchorY + mHeight );
            glVertex2d( cursorViewX + cursorOffset, mAnchorY );
            }
        glEnd();
		}
	
	
    /*
	// draw the border on top of the text and cursor
	glColor4f( mCurrentBorderColor->r,
			   mCurrentBorderColor->g, mCurrentBorderColor->b, 1.0f );
	glBegin( GL_LINE_LOOP ); {
		glVertex2d( mAnchorX - offset, mAnchorY - offset );		
		glVertex2d( mAnchorX - offset, mAnchorY + mHeight + offset );

		glVertex2d( mAnchorX + mWidth + offset, mAnchorY + mHeight + offset );
		glVertex2d( mAnchorX + mWidth + offset, mAnchorY - offset ); 
		}
	glEnd();
    */
	
	}



#endif



