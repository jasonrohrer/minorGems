/*
 * Modification History
 *
 * 2001-October-13	Jason Rohrer
 * Created.
 *
 * 2001-October-29	Jason Rohrer
 * Added a missing destructor.
 *
 * 2006-December-19	Jason Rohrer
 * Made destructor virtual.
 *
 * 2008-October-1    Jason Rohrer
 * Added function for getting TextGL.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */
 
 
#ifndef LABEL_GL_INCLUDED
#define LABEL_GL_INCLUDED 

#include "GUIComponentGL.h"
#include "TextGL.h"



/**
 * A text label for OpenGL-based GUIs.
 *
 * @author Jason Rohrer
 */
class LabelGL : public GUIComponentGL {


	public:



		/**
		 * Constructs a label.
		 *
		 * @param inAnchorX the x position of the upper left corner
		 *   of this component.
		 * @param inAnchorY the y position of the upper left corner
		 *   of this component.
		 * @param inWidth the width of this component.
		 * @param inHeight the height of this component.
		 * @param inString the text to display in this label.
		 *    Is copied internally, so must be destroyed
		 *    by caller if not const.
		 * @param inText the text object to use when drawing
		 *   this label.  Must be destroyed by caller after
		 *   this class is destroyed.
		 */
		LabelGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, const char *inString, TextGL *inText );



		virtual ~LabelGL();

		
		
		/**
		 * Sets the text displayed by this label.
		 *
		 * @param inString the text to display in this label.
		 *    Is copied internally, so must be destroyed
		 *    by caller if not const.
		 */
		void setText( const char *inString );


		
		/**
		 * Gets the text displayed by this label.
		 *
		 * @return the text to display in this label.
		 *    Must not be destroyed or modified by caller.
		 */
		char *getText();



        /**
         * Gets the TextGL object used to draw this label.
         *
         * @return the TextGL object. 
         *   Must not be destroyed by caller until after this class is 
         *   destroyed.
         */
        TextGL *getTextGL() {
            return mText;
            }
        

		
		// override fireRedraw in GUIComponentGL
		virtual void fireRedraw();
        

		
	protected:
		TextGL *mText;

		char *mString;
	};



inline LabelGL::LabelGL(
	double inAnchorX, double inAnchorY, double inWidth,
	double inHeight, const char *inString, TextGL *inText )
	: GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
	  mText( inText ), mString( NULL ) {

	setText( inString );
	}



inline LabelGL::~LabelGL() {
	if( mString != NULL ) {
		delete [] mString;
		}
	}



inline void LabelGL::setText( const char *inString ) {
	if( mString != NULL ) {
		delete [] mString;
		}
	int length = strlen( inString ) + 1;
	
	mString = new char[ length ];

	memcpy( mString, inString, length );
	}



inline char *LabelGL::getText() {
	return mString;
	}


		
inline void LabelGL::fireRedraw() {
	
	mText->drawText( mString, mAnchorX, mAnchorY,
					 mWidth, mHeight );
	}



#endif



