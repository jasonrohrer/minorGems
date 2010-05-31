/*
 * Modification History
 *
 * 2001-August-29		Jason Rohrer
 * Created.
 *
 * 2001-August-30		Jason Rohrer
 * Fixed some comments.
 *
 * 2001-October-29		Jason Rohrer
 * Changed to move and rotate view at a fixed
 * time rate, regardless of framerate.
 *
 * 2003-June-14   Jason Rohrer
 * Fixed namespace for exit call.
 * Added M_PI backup definition.
 */
 
 
#ifndef SCENE_NAVIGATOR_DISPLAY_GL_INCLUDED
#define SCENE_NAVIGATOR_DISPLAY_GL_INCLUDED 

#include "ScreenGL.h"

#include "MouseHandlerGL.h"
#include "KeyboardHandlerGL.h"
#include "SceneHandlerGL.h"

#include "RedrawListenerGL.h"

#include "minorGems/system/Time.h"


#include <stdlib.h>

#include <math.h>



// make sure M_PI is defined
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif




/**
 * Subclass of ScreenGL that handles general-purpose 3D scene navigation
 * rendered through OpenGL.
 *
 * Motion is unlimited, though forward-backward motion is confinded
 * to the x-z plane (the user can move up and down explicitly, however).
 *
 * To constrict motion, subclass SceneNavigatorDisplayGL and override
 * the moveView() function (a member of ScreenGL).
 *
 * Compile note:  For Linux, add these library flags:
 * -lGL -lglut -lGLU -L/usr/X11R6/lib
 * Be sure to include ScreenGL.cpp in the file list passed to the compiler.
 *
 * @author Jason Rohrer 
 */
class SceneNavigatorDisplayGL :
	public ScreenGL, public MouseHandlerGL,
	public KeyboardHandlerGL, public RedrawListenerGL { 
	
	public:


		
		/**
		 * Constructs a navigator.
		 *
		 * @param inWide the width of the screen in pixels.
		 * @param inHigh the height of the screen in pixels.
		 * @param inFullScreen true iff the display should
		 *   fill the screen.
		 * @param inWindowName the name of the display window.
		 * @param inSceenHandler the handler responsible for
		 *   drawing the scene.
		 *   Must be destroyed by caller.
		 * @param inMoveUnitsPerSecond the number of world
		 *   units to move per second when the user makes a move.
		 *   Defaults to 1.0 units per second.
		 * @param inRotateRadiansPerSecond the number of
		 *   radians to rotate per second when the user makes a rotation.
		 *   Defaults to 1.0 radians per second.
		 */
		SceneNavigatorDisplayGL( int inWide, int inHigh, char inFullScreen, 
								 char *inWindowName,
								 SceneHandlerGL *inSceneHandler,
								 double inMoveUnitsPerSecond = 1.0,
								 double inRotateRadiansPerSecond = 1.0 );



		~SceneNavigatorDisplayGL();
		


		// implements the MouseHandlerGL interface
		virtual void mouseMoved( int inX, int inY );
		virtual void mouseDragged( int inX, int inY );
		virtual void mousePressed( int inX, int inY );
		virtual void mouseReleased( int inX, int inY );
		
		// implements the KeyboardHandlerGL interface
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
		virtual void specialKeyPressed( int inKey, int inX, int inY );
		virtual void keyReleased( unsigned char inKey, int inX, int inY );
		virtual void specialKeyReleased( int inKey, int inX, int inY );
		
		// implements the RedrawListener interface
		virtual void fireRedraw();


		
		
		
		
	private :

		int mLastMouseX, mLastMouseY;
		
		// amount to move and rotate view during each redraw
		Vector3D *mMoveDirection;
		Angle3D *mRotationOffset;
		
		// keep y movement separate from view direction.
		double mYMovement;
		
		// true if a moving key is currently depressed
		char mMoving;
		// true if a rotation key is currently depressed
		char mRotating;


		unsigned long mTimeLastRedrawS;
		unsigned long mTimeLastRedrawMS;


		double mMoveUnitsPerSecond;
		double mRotateRadiansPerSecond;
		
	};



inline SceneNavigatorDisplayGL::SceneNavigatorDisplayGL(
	int inWide, int inHigh, char inFullScreen, char *inWindowName,
	SceneHandlerGL *inSceneHandler,
	double inMoveUnitsPerSecond,
	double inRotateRadiansPerSecond )
	
	: ScreenGL( inWide, inHigh, inFullScreen, inWindowName, NULL, NULL,
				inSceneHandler ),
	mMoveDirection( new Vector3D( 0, 0, 0 ) ),
	mRotationOffset( new Angle3D( 0, 0, 0 ) ),
	mMoving( false ), mRotating( false ),
	mYMovement( 0 ),
	mMoveUnitsPerSecond( inMoveUnitsPerSecond ),
	mRotateRadiansPerSecond( inRotateRadiansPerSecond ) {

	// add ourself to the superclass' listener lists
	addMouseHandler( this );
	addKeyboardHandler( this );
	addRedrawListener( this );


	
	Time::getCurrentTime( &mTimeLastRedrawS, &mTimeLastRedrawMS );

	}




inline void SceneNavigatorDisplayGL::mouseMoved( int inX, int inY ) {
	//printf( "mouse moved to (%d, %d)\n", inX, inY );
	
	}



inline void SceneNavigatorDisplayGL::mouseDragged( int inX, int inY ) {
	int delX = inX - mLastMouseX;
	int delY = inY - mLastMouseY;
	
	Angle3D *rotAngle = new Angle3D( delY / 50, delX/50, 0 );
	
	//mScreen->rotateView( rotAngle );
	
	delete rotAngle;
	
	mLastMouseX = inX;
	mLastMouseY = inY;
	}



inline void SceneNavigatorDisplayGL::mousePressed( int inX, int inY ) {
	
	}



inline void SceneNavigatorDisplayGL::mouseReleased( int inX, int inY ) {
	
	}



inline void SceneNavigatorDisplayGL::keyPressed(
	unsigned char inKey, int inX, int inY ) {

	if( inKey == 'y' || inKey == 'Y' ) {
		// turn view downwards
		mRotationOffset->mX = M_PI/20;
		}
	else if( inKey == 'h' || inKey == 'H' ) {
		// turn view upwards
		mRotationOffset->mX = -M_PI/20;
		}	
	
	if( inKey == 's' || inKey == 'S' ) {
		// turn to the left
		mRotationOffset->mY = M_PI/20;
		}
	else if( inKey == 'f' || inKey == 'F' ) {
		// turn to the right
		mRotationOffset->mY = -M_PI/20;
		}
	
	if( inKey == 'i' || inKey == 'I' ) {
		// move upwards
		mYMovement = 1;
		}
	else if( inKey == 'k' || inKey == 'K' ) {
		// move downwards
		mYMovement = -1;
		}
		
	if( inKey == 'j' || inKey == 'J' ) {
		// strafe to the left
		mMoveDirection->mX = 1;
		}
	else if( inKey == 'l' || inKey == 'L' ) {
		// strafe to the right
		mMoveDirection->mX = -1;
		}
	if( inKey == 'e' || inKey == 'E' ) {
		// move forward
		mMoveDirection->mZ = 1;
		}
	else if( inKey == 'd' || inKey == 'D' ) {
		// move backward
		mMoveDirection->mZ = -1;
		}
	else if( inKey == 'q' || inKey == 'Q' ) {
		// quit
		::exit( 0 );
		}		
	}



inline void SceneNavigatorDisplayGL::keyReleased(
	unsigned char inKey, int inX, int inY ) {

	if( inKey == 'e' || inKey == 'E' ) {
		mMoveDirection->mZ = 0;
		}
	else if( inKey == 'd' || inKey == 'D' ) {
		mMoveDirection->mZ = 0;
		}
	
	if( inKey == 'j' || inKey == 'J' ) {
		mMoveDirection->mX = 0;
		}
	else if( inKey == 'l' || inKey == 'L' ) {
		mMoveDirection->mX = 0;
		}
	
	if( inKey == 'i' || inKey == 'I' ) {
		mYMovement = 0;
		}
	else if( inKey == 'k' || inKey == 'K' ) {
		mYMovement = 0;
		}
	
	if( inKey == 's' || inKey == 'S' ) {
		mRotationOffset->mY = 0;
		}
	else if( inKey == 'f' || inKey == 'F' ) {
		mRotationOffset->mY = 0;
		}
	
	if( inKey == 'y' || inKey == 'Y' ) {
		mRotationOffset->mX = 0;
		}
	else if( inKey == 'h' || inKey == 'H' ) {
		mRotationOffset->mX = 0;
		}		
	}



inline void SceneNavigatorDisplayGL::specialKeyPressed(
	int inKey, int inX, int inY ) {
	
	}



inline void SceneNavigatorDisplayGL::specialKeyReleased(
	int inKey, int inX, int inY ) {
	
	}
	
	
	
inline void SceneNavigatorDisplayGL::fireRedraw() {

	unsigned long currentTimeS;
	unsigned long currentTimeMS;
	Time::getCurrentTime( &currentTimeS, &currentTimeMS );

	unsigned long deltaMS = Time::getMillisecondsSince(
		mTimeLastRedrawS, mTimeLastRedrawMS );

	mTimeLastRedrawS = currentTimeS;
	mTimeLastRedrawMS = currentTimeMS;

	// compute the number of units to move to maintain
	// a constant speed of mUnitsPerSecond
	double unitsToMove = mMoveUnitsPerSecond * ( deltaMS / 1000.0 );

	double radiansToRotate = mRotateRadiansPerSecond * ( deltaMS / 1000.0 ); 


	// scale our rotation offset
	Angle3D *scaledRotationOffset = new Angle3D( mRotationOffset );
	scaledRotationOffset->scale( radiansToRotate );

	
	// rotate the view in the super class
	rotateView( scaledRotationOffset );

	delete scaledRotationOffset;

	
	
	Vector3D *move = new Vector3D( mMoveDirection );

	// scale our move direction
	move->scale( unitsToMove );
	
	Angle3D *rotation = new Angle3D( getViewOrientation() );

	// only rotate movement direction about y axis
	// we don't want view y direction to affect the direction of movement
	rotation->mX = 0;
	rotation->mZ = 0;
	
	move->rotate( rotation );
	
	delete rotation;
		
	// now add in y movement, scaled
	move->mY = mYMovement * unitsToMove;
	
	
	// move the view in the superclass
	moveView( move );

	delete move;
	}



#endif

