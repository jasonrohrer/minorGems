/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created. 
 *
 * 2001-January-31		Jason Rohrer
 * Fixed a bug in the fullscreen code.
 *
 * 2001-February-4		Jason Rohrer
 * Added support for keyboard up functions.
 * Added support for redraw listeners. 
 * Added missing initialization.
 *
 * 2001-August-29		Jason Rohrer
 * Added support for vectors of mouse, keyboard, and scene handlers.
 *
 * 2001-September-15    Jason Rohrer
 * Fixed a bug in passing redraw events to the redraw listeners.
 *
 * 2001-October-13   	Jason Rohrer
 * Added a function for applying the view matrix transformation.
 * Removed unneeded code from the glutResize function.
 *
 * 2001-October-29   	Jason Rohrer
 * Added support for focusable keyboard handlers.
 *
 * 2001-October-29   	Jason Rohrer
 * Added support for focusable keyboard handlers.
 *
 * 2004-August-30   	Jason Rohrer
 * Fixed distortion issues when screen aspect ratio is not 1:1.
 *
 * 2005-February-7   	Jason Rohrer
 * Fixed bug of incorrect GL matrix stack usage.  Now fog works correctly.
 *
 * 2005-February-21   	Jason Rohrer
 * Removed incorrect call to glPopMatrix.
 *
 * 2005-March-4   	Jason Rohrer
 * Changed to call redraw listeners before applying view transform.
 *
 * 2006-December-21   	Jason Rohrer
 * Added functions for changing window size and switching to full screen.
 *
 * 2008-September-12   	Jason Rohrer
 * Changed to use glutEnterGameMode for full screen at startup.
 * Added a 2D graphics mode.
 *
 * 2008-September-29   	Jason Rohrer
 * Enabled ignoreKeyRepeat.
 *
 * 2008-October-27   	Jason Rohrer
 * Prepared for alternate implementations besides GLUT.
 * Switched to implementation-independent keycodes.
 * Added support for setting viewport size separate from screen size.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2010-December-4    Jason Rohrer
 * Changed to support new constructor parameters, though they are unused.
 */


/*
 * Compile note:  For Linux, add these library flags:
 * -lGL -lglut -lGLU -L/usr/X11R6/lib
 */


#include "ScreenGL.h" 

#include <GL/gl.h>
#include <GL/glut.h>


#include <math.h>
#include <stdlib.h>

#include "minorGems/util/stringUtils.h"



/* ScreenGL to be accessed by callback functions.
 *
 * Note that this is a bit of a hack, but the callbacks
 * require a C-function (not a C++ member) and have fixed signatures,
 * so there's no way to pass the current screen into the functions.
 *
 * This hack prevents multiple instances of the ScreenGL class from
 * being used simultaneously.
 */
ScreenGL *currentScreenGL;


// maps GLUT-specific special key-codes (GLUT_KEY) to minorGems key codes
// (MG_KEY)
int mapGLUTKeyToMG( int inGLUTKey );

    



ScreenGL::ScreenGL( int inWide, int inHigh, char inFullScreen, 
                    unsigned int inMaxFrameRate,
                    char inRecordEvents,
					const char *inWindowName,
					KeyboardHandlerGL *inKeyHandler,
					MouseHandlerGL *inMouseHandler,
					SceneHandlerGL *inSceneHandler  ) 
	: mWide( inWide ), mHigh( inHigh ),
      mImageSizeSet( false ),
      mImageWide( inWide ), mImageHigh( inHigh ), 
      mFullScreen( inFullScreen ),
      m2DMode( false ),
	  mViewPosition( new Vector3D( 0, 0, 0 ) ),
	  mViewOrientation( new Angle3D( 0, 0, 0 ) ),
	  mMouseHandlerVector( new SimpleVector<MouseHandlerGL*>() ),
	  mKeyboardHandlerVector( new SimpleVector<KeyboardHandlerGL*>() ),
	  mSceneHandlerVector( new SimpleVector<SceneHandlerGL*>() ),
	  mRedrawListenerVector( new SimpleVector<RedrawListenerGL*>() ) {


	// add handlers if NULL (the default) was not passed in for them
	if( inMouseHandler != NULL ) {
		addMouseHandler( inMouseHandler );
		}
	if( inKeyHandler != NULL ) {
		addKeyboardHandler( inKeyHandler );
		}
	if( inSceneHandler != NULL ) {
		addSceneHandler( inSceneHandler );
		}

	
	if( mFullScreen ) {
		glutInitDisplayMode( 
			GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
            //| GLUT_FULLSCREEN );
		}
	else {
		glutInitDisplayMode( 
			GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
		}
	
    if( !mFullScreen ) {
        
        glutInitWindowSize( mWide, mHigh );
        glutCreateWindow( inWindowName );
        }
    else {
        // use game mode for full screen
        char *gameMode = autoSprintf( "%dx%d:32", mWide, mHigh );
        
        glutGameModeString( gameMode );
        
        if( glutGameModeGet( GLUT_GAME_MODE_POSSIBLE ) ) {
            glutEnterGameMode();
            delete [] gameMode;
            }
        else {
            printf( "The full-screen mode %s is not available\n", gameMode );
            delete [] gameMode;
            exit(1);
            }        
        }

    glutIgnoreKeyRepeat( 1 );
    

	glutKeyboardFunc( callbackKeyboard );
	glutKeyboardUpFunc( callbackKeyboardUp );
	glutSpecialFunc( callbackSpecialKeyboard );
	glutSpecialUpFunc( callbackSpecialKeyboardUp );
	glutReshapeFunc( callbackResize );
	glutMotionFunc( callbackMotion );
	glutMouseFunc( callbackMouse );
	glutPassiveMotionFunc( callbackPassiveMotion );
	glutDisplayFunc( callbackDisplay );
	glutIdleFunc( callbackIdle );
	
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	
	}



ScreenGL::~ScreenGL() {
    if( mFullScreen ) {
        glutLeaveGameMode();
        }
    

	delete mViewPosition;
	delete mViewOrientation;
	delete mRedrawListenerVector;
	delete mMouseHandlerVector;
	delete mKeyboardHandlerVector;
	delete mSceneHandlerVector;
	}



void ScreenGL::start() {
	currentScreenGL = this;
	glutMainLoop();
	}



void ScreenGL::switchTo2DMode() {
    m2DMode = true;    
    }



void ScreenGL::setFullScreen() {
    glutFullScreen();
    }



void ScreenGL::changeWindowSize( int inWidth, int inHeight ) {
    glutReshapeWindow( inWidth, inHeight );
    }



void ScreenGL::applyViewTransform() {
    // compute view angle

    // default angle is 90, but we want to force a 1:1 aspect ratio to avoid
    // distortion.
    // If our screen's width is different than its height, we need to decrease
    // the view angle so that the angle coresponds to the smaller dimension.
    // This keeps the zoom factor constant in the smaller dimension.

    // Of course, because of the way perspective works, only one Z-slice
    // will have a constant zoom factor
    // The zSlice variable sets the distance of this slice from the picture
    // plane
    double zSlice = .31;

    double maxDimension = mWide;
    if( mHigh > mWide ) {
        maxDimension = mHigh;
        }
    double aspectDifference = fabs( mWide / 2 - mHigh / 2 ) / maxDimension;
    // default angle is 90 degrees... half the angle is PI/4
    double angle = atan( tan( M_PI / 4 ) +
                         aspectDifference / zSlice );

    // double it to get the full angle
    angle *= 2;
    
    
    // convert to degrees
    angle = 360 * angle / ( 2 * M_PI );


    // set up the projection matrix
    glMatrixMode( GL_PROJECTION );

	glLoadIdentity();
    
    //gluPerspective( 90, mWide / mHigh, 1, 9999 );
    gluPerspective( angle,
                    1,
                    1, 9999 );

    
    // set up the model view matrix
    glMatrixMode( GL_MODELVIEW );
    
    glLoadIdentity();

	// create default view and up vectors, 
	// then rotate them by orientation angle
	Vector3D *viewDirection = new Vector3D( 0, 0, 1 );
	Vector3D *upDirection = new Vector3D( 0, 1, 0 );
	
	viewDirection->rotate( mViewOrientation );
	upDirection->rotate( mViewOrientation );
	
	// get a point out in front of us in the view direction
	viewDirection->add( mViewPosition );
	
	// look at takes a viewer position, 
	// a point to look at, and an up direction
	gluLookAt( mViewPosition->mX, 
				mViewPosition->mY, 
				mViewPosition->mZ,
				viewDirection->mX, 
				viewDirection->mY, 
				viewDirection->mZ,
				upDirection->mX, 
				upDirection->mY, 
				upDirection->mZ );
	
	delete viewDirection;
	delete upDirection;
    }



void callbackResize( int inW, int inH ) {	
	ScreenGL *s = currentScreenGL;
	s->mWide = inW;
	s->mHigh = inH;
    
    if( ! s->mImageSizeSet ) {
        // keep image size same as screen size
        s->mImageWide = inW;
        s->mImageHigh = inH;
        }
    

    int bigDimension = s->mImageWide;
    if( bigDimension < s->mImageHigh ) {
        bigDimension = s->mImageHigh;
        }
    
    int excessW = s->mWide - bigDimension;
    int excessH = s->mHigh - bigDimension;
    
    // viewport is square of biggest image dimension, centered on screen
    // (to ensure a 1:1 aspect ratio)
    glViewport( excessW / 2,
                excessH / 2, 
                bigDimension,
                bigDimension );

    /*
    if( s->mWide >= s->mHigh ) {
        int excess = s->mWide - s->mHigh;
        
        glViewport( 0, -excess / 2, s->mWide, s->mWide );
        }
    else {
        int excess = s->mHigh - s->mWide;
        
        glViewport( -excess / 2, 0, s->mHigh, s->mHigh );
        }
    */
	glutPostRedisplay();
	}



void callbackKeyboard( unsigned char inKey, int inX, int inY ) {
	char someFocused = currentScreenGL->isKeyboardHandlerFocused();
		
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );

		// if some are focused, only fire to this handler if it is one
		// of the focused handlers
		if( !someFocused || handler->isFocused() ) {
			handler->keyPressed( inKey, inX, inY );
			}
		}
	}



void callbackKeyboardUp( unsigned char inKey, int inX, int inY ) {
	char someFocused = currentScreenGL->isKeyboardHandlerFocused();
	
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );

		// if some are focused, only fire to this handler if it is one
		// of the focused handlers
		if( !someFocused || handler->isFocused() ) {
			handler->keyReleased( inKey, inX, inY );
			}
		}
	}	

	
	
void callbackSpecialKeyboard( int inKey, int inX, int inY ) {
	char someFocused = currentScreenGL->isKeyboardHandlerFocused();
	
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
		
		// if some are focused, only fire to this handler if it is one
		// of the focused handlers
		if( !someFocused || handler->isFocused() ) {
			handler->specialKeyPressed( mapGLUTKeyToMG( inKey ), inX, inY );
			}
		}
	}
	


void callbackSpecialKeyboardUp( int inKey, int inX, int inY ) {
	char someFocused = currentScreenGL->isKeyboardHandlerFocused();
	
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );

		// if some are focused, only fire to this handler if it is one
		// of the focused handlers
		if( !someFocused || handler->isFocused() ) {
			handler->specialKeyReleased( mapGLUTKeyToMG( inKey ), inX, inY );
			}
		}
	}	


	
void callbackMotion( int inX, int inY ) {
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
		MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
		handler->mouseDragged( inX, inY );
		}
	}
	


void callbackPassiveMotion( int inX, int inY ) {
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
		MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
		handler->mouseMoved( inX, inY );
		}
	}			
	
	
	
void callbackMouse( int inButton, int inState, int inX, int inY ) {
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
		MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
		handler->mouseMoved( inX, inY );
		if( inState == GLUT_DOWN ) {
			handler->mousePressed( inX, inY );
			}
		else if( inState == GLUT_UP ) {
			handler->mouseReleased( inX, inY );
			}
		else {
			printf( "Error:  Unknown mouse state received from OpenGL\n" );
			}	
		}
	}
	
	
	
void callbackDisplay() {
	ScreenGL *s = currentScreenGL;
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    // fire to all redraw listeners
    // do this first so that they can update our view transform
    // this makes control much more responsive
	for( int r=0; r<s->mRedrawListenerVector->size(); r++ ) {
		RedrawListenerGL *listener 
			= *( s->mRedrawListenerVector->getElement( r ) );
		listener->fireRedraw();
		}


	if( ! s->m2DMode ) {    
        // apply our view transform
        s->applyViewTransform();
        }
    
    
	// fire to all handlers
	for( int h=0; h<currentScreenGL->mSceneHandlerVector->size(); h++ ) {
		SceneHandlerGL *handler 
			= *( currentScreenGL->mSceneHandlerVector->getElement( h ) );
		handler->drawScene();
		}

	
	
	glutSwapBuffers();
	}



void callbackIdle() {
	glutPostRedisplay();
	}		



int mapGLUTKeyToMG( int inGLUTKey ) {
    switch( inGLUTKey ) {
        case GLUT_KEY_F1: return MG_KEY_F1;
        case GLUT_KEY_F2: return MG_KEY_F2;
        case GLUT_KEY_F3: return MG_KEY_F3;
        case GLUT_KEY_F4: return MG_KEY_F4;
        case GLUT_KEY_F5: return MG_KEY_F5;
        case GLUT_KEY_F6: return MG_KEY_F6;
        case GLUT_KEY_F7: return MG_KEY_F7;
        case GLUT_KEY_F8: return MG_KEY_F8;
        case GLUT_KEY_F9: return MG_KEY_F9;
        case GLUT_KEY_F10: return MG_KEY_F10;
        case GLUT_KEY_F11: return MG_KEY_F11;
        case GLUT_KEY_F12: return MG_KEY_F12;
        case GLUT_KEY_LEFT: return MG_KEY_LEFT;
        case GLUT_KEY_UP: return MG_KEY_UP;
        case GLUT_KEY_RIGHT: return MG_KEY_RIGHT;
        case GLUT_KEY_DOWN: return MG_KEY_DOWN;
        case GLUT_KEY_PAGE_UP: return MG_KEY_PAGE_UP;
        case GLUT_KEY_PAGE_DOWN: return MG_KEY_PAGE_DOWN;
        case GLUT_KEY_HOME: return MG_KEY_HOME;
        case GLUT_KEY_END: return MG_KEY_END;
        case GLUT_KEY_INSERT: return MG_KEY_INSERT;
        default: return 0;
        }
    }
