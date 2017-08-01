/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created. 
 *
 * 2001-January-15		Jason Rohrer
 * Added compile instructions for Linux to the comments.
 *
 * 2001-February-4		Jason Rohrer
 * Added a function for getting the view angle.
 * Added support for keyboard up functions. 
 * Added support for redraw listeners.
 * Added a missing destructor.
 *
 * 2001-August-29		Jason Rohrer
 * Added support for adding and removing mouse, keyboard, and scene handlers.
 *
 * 2001-August-30		Jason Rohrer
 * Fixed some comments.
 *
 * 2001-September-15   	Jason Rohrer
 * Added functions for accessing the screen size.
 *
 * 2001-October-13   	Jason Rohrer
 * Added a function for applying the view matrix transformation.
 *
 * 2001-October-29   	Jason Rohrer
 * Added a private function that checks for focused keyboard handlers.
 *
 * 2004-June-11   	Jason Rohrer
 * Added functions for getting/setting view position.
 *
 * 2004-June-14   	Jason Rohrer
 * Added comment about need for glutInit call.
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
 * 2010-April-20   	Jason Rohrer
 * Alt-Enter to leave fullscreen mode.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2010-September-6   Jason Rohrer
 * Split display callback into two parts to handle events after frame sleep.
 *
 * 2010-September-9   Jason Rohrer
 * Moved frame rate limit into ScreenGL class.
 *
 * 2010-November-2   Jason Rohrer
 * Support for controlling order of keyboard handlers.
 *
 * 2010-November-17   Jason Rohrer
 * Added input recording and playback.
 *
 * 2010-November-18   Jason Rohrer
 * Record and playback rand seed.
 * ASCII key mapping.
 *
 * 2010-December-23   Jason Rohrer
 * Support for delaying start of event playback or recording.
 *
 * 2010-December-27   Jason Rohrer
 * Support for slowdown keys during playback.
 *
 * 2011-January-3   Jason Rohrer
 * Added custom data to recorded game files.
 * Support for detecting playback mode.
 * 
 * 2011-February-6   Jason Rohrer
 * Support for getting rough playback done fraction.
 * 
 * 2011-February-7   Jason Rohrer
 * Support for minimizing on Alt-tab out of fullscreen mode.
 * 
 * 2011-February-9   Jason Rohrer
 * Hash checking of custom data in recorded game files.
 * 
 * 2011-February-12   Jason Rohrer
 * Playback display toggle.
 * 
 * 2011-February-22   Jason Rohrer
 * Added a force-dimensions flag.
 * 
 * 2011-March-14   Jason Rohrer
 * Changed Alt-Tab to explicitly release mouse.  
 * 
 * 2012-July-6   Jason Rohrer
 * Added support for toggling key mapping.  
 * 
 * 2013-January-29   Jason Rohrer
 * Added support for replay of time() values.  
 *
 * 2014-January-3   Jason Rohrer
 * Added recording and playback of socket events.
 *
 * 2014-November-25   Jason Rohrer
 * Added support for obscuring sensitive typing in recorded event file.
 */
 
 
#ifndef SCREEN_GL_INCLUDED
#define SCREEN_GL_INCLUDED 


#include "MouseHandlerGL.h"
#include "KeyboardHandlerGL.h"
#include "SceneHandlerGL.h"

#include "RedrawListenerGL.h"

#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"

#include "minorGems/util/SimpleVector.h"


#include "minorGems/system/Time.h"


// prototypes
void callbackResize( int inW, int inH );
void callbackKeyboard( unsigned char  inKey, int inX, int inY );
void callbackKeyboardUp( unsigned char  inKey, int inX, int inY );
void callbackSpecialKeyboard( int inKey, int inX, int inY );
void callbackSpecialKeyboardUp( int inKey, int inX, int inY );
void callbackMotion( int inX, int inY );
void callbackPassiveMotion( int inX, int inY );
void callbackMouse( int inButton, int inState, int inX, int inY );
void callbackPreDisplay();
void callbackDisplay();
void callbackIdle();



typedef struct WebEvent {
        int handle;
        int type;
        char *bodyText;
        int bodyLength;
    } WebEvent;


typedef struct SocketEvent {
        int handle;
        int type;
        int numBodyBytes;
        // can be NULL even if numBodyBytes not 0 (in case of
        // recorded send, where we don't need to record what was sent)
        char *bodyBytesHex;
    } SocketEvent;




/**
 * Object that handles general initialization of an OpenGL screen.
 *
 * @author Jason Rohrer 
 */
class ScreenGL { 
	
	public:
		
		/**
		 * Constructs a ScreenGL.
         *
         * GLUT implementation only:
         * Before calling this constructor, glutInit must be called with
         * the application's command-line arguments.  For example, if
         * the application's main function looks like:
         *
         * int main( int inNumArgs, char **inArgs ) { ... }
         *
         * Then you must call glutInit( &inNumArgs, inArgs ) before
         * constructing a screen.
         *
         * SDL implementation:
         * Must call SDL_Init() with at least SDL_INIT_VIDEO
         * as a parameter.
		 *
		 * @param inWide width of screen.
		 * @param inHigh height of screen.
		 * @param inFullScreen set to true for full screen mode.
         *   NOTE that full screen mode requires inWide and inHigh to match
         *   an available screen resolution.
         * @param inDoNotChangeNativeResolution if true, the screen resolution
         *   is left alone.
		 * @param inWindowName name to be displayed on title bar of window.
         * @param inMaxFrameRate in frames per second.
         * @param inRecordEvents true to record events to file.
         * @param inCustomRecordedGameData custom data string to add to 
         *   header of file.  Must contain no whitespace.
         *   Destroyed by caller.
         * @param inHashSalt secret salt data to use when authenticating
         *   custom recorded data.
         *   Destroyed by caller.
		 * @param inKeyHandler object that will receive keyboard events.
		 *   NULL specifies no handler (defaults to NULL).
		 *   Must be destroyed by caller.
		 * @param inMouseHandler object that will receive mouse events.
		 *   NULL specifies no handler (defaults to NULL).
		 *   Must be destroyed by caller.
		 * @param inSceneHandler object that will be called to draw
		 *   the scene during in response to redraw events.
		 *   NULL specifies no handler (defaults to NULL).
		 *   Must be destroyed by caller.
		 */
		ScreenGL( int inWide, int inHigh, char inFullScreen,
				  char inDoNotChangeNativeResolution,
				  unsigned int inMaxFrameRate,
                  char inRecordEvents,
                  const char *inCustomRecordedGameData,
                  const char *inHashSalt,
                  const char *inWindowName,
				  KeyboardHandlerGL *inKeyHandler = NULL,
				  MouseHandlerGL *inMouseHandler = NULL,
				  SceneHandlerGL *inSceneHandler = NULL );
		
		
		/**
		 * Destructor closes and releases the screen.
		 */	
		~ScreenGL();	
		


        /**
         * Gets data read from a recorded game file.
         *
         * Not destroyed by caller.
         */
        const char *getCustomRecordedGameData();


        /**
         * True if currently in playback mode.
         */
        char isPlayingBack();
        

        /**
         * Returns an estimate of playback fraction complete.
         */
        float getPlaybackDoneFraction();
        

        /**
         * Returns whether playback display is on or off.
         */
        char shouldShowPlaybackDisplay();
        
        


        /**
         * True if minimized.
         */
        char isMinimized();
        
        
		
		/**
		 * Starts the GLUT main loop.
		 * 
		 * Note that this function call never returns.
		 */
		void start();
		

        // can avoid recording/playback during certain "front matter"
        // activities like menu navigation
        // (has no effect if no recording or playback pending)
        void startRecordingOrPlayback();
        
        // our current max
        // can be changed with speed control keys, if enabled
        void setMaxFrameRate( unsigned int inMaxFrameRate );

        // our absolute max
        // speed control key adjustments are relative to this full rate
        void setFullFrameRate( unsigned int inFullFrameRate );
        
        
        unsigned int getMaxFramerate();
        
        // set to true to enable frame sleeping to enforce max frame rate
        // (sleeping is default)
        // Otherwise, if false, no max is enforced (though we may be able
        // to depend on vsync to enforce it for us)
        void useFrameSleep( char inUse );
        


        // should ^ and % be allowed to slowdown and resume normal speed
        // during event playback?
        // setMaxFrameRate also can be used to create a slowdown, but
        // ^ and % keys are not passed through during playback
        void allowSlowdownKeysDuringPlayback( char inAllow );
        

        // enables rand seed to be recorded and played back with
        // event playback
        unsigned int getRandSeed();
        

        // enables Time::timeSec() values to be recorded and played back
        timeSec_t getTimeSec();
        double getCurrentTime();

        
        // to save FPS in recording files
        void registerActualFrameRate( double inFrameRate );

        // these can be played back
        double getRecordedFrameRate();
        
        
        // sets mapping so that when inFromKey is pressed, an
        // event for inToKey is generated instead
        void setKeyMapping( unsigned char inFromKey,
                            unsigned char inToKey );
        

        // turns key mapping on (default) or off
        void toggleKeyMapping( char inMappingOn );
        


        // passes in a web event to be (possibly) added to the current
        // game recording
        // type encodes :
        //    0 = normal request step completed (NULL body)
        //    1 = request done step completed (NULL body)
        //   -1 = error-returning request step completed (NULL body)
        //    2 = result-fetch step completed (non-NULL body)
        //   >2 = web progress event, showing number of bytes fetched
        //        (NULL body)
        void registerWebEvent( int inHandle,
                               int inType,
                               const char *inBodyString = NULL,
                               // -1 means use string length
                               // provide length for binary bodies
                               int inBodyLength = -1 );
        
        
        // gets the type of the next pending web event (from playback)
        // if the event has no result body, this call removes the event
        // from the list.
        // If it has a result body, getWebEventResultBody must be called.
        int getWebEventType( int inHandle );

        // gets a recorded event body text from the current frame matching 
        //   inHandle
        // result NULL if there is no event matching inHandle
        // result destroyed by caller.
        //
        // This call removes the web event from the list of pending events.
        char *getWebEventResultBody( int inHandle,
                                     int *outSize = NULL );
        




        // passes in a socket event to be (possibly) added to the current
        // game recording
        // type encodes :
        //    0 = send with number sent (NULL body)
        //    1 = send resulting in error (NULL body)
        //    2 = read with number read (non-NULL body if inNumBodyBytes != 0)
        //    3 = read resulting in error (NULL body)
        void registerSocketEvent( int inHandle,
                                  int inType,
                                  int inNumBodyBytes,
                                  unsigned char *inBodyBytes = NULL );
        
        
        // gets the type of the next pending socket event (from playback)
        // if the event has no body bytes 
        // (type 0, 1, or 3, OR type 2 with 0-length body), 
        // this call removes the event from the list.
        //
        // In case of type 2 with outNumBodyBytes > 0, 
        // getWebEventResultBody must be called.
        //
        // 
        void getSocketEventTypeAndSize( int inHandle, 
                                        int *outType, 
                                        int *outNumBodyBytes );
        
        // gets a recorded socket event body bytes from the current 
        // frame matching inHandle
        // result NULL if there is no event matching inHandle
        // result destroyed by caller.
        //
        // return value has length as set by last getSocketEventTypeAndSize
        // call
        //
        // This call removes the socket event from the list of pending events.
        unsigned char *getSocketEventBodyBytes( int inHandle );

        
        void registerAsyncFileDone( int inHandle );
        
        char getAsyncFileDone( int inHandle );
        


        /**
         * Switches to 2D mode, where no view transforms are applied
         *
         * Must be called before start();
         */
        void switchTo2DMode();
        

		
		/**
		 * Moves the view position.
		 *
		 * @param inPositionChange directional vector describing motion.
		 *   Must be destroyed by caller.
		 */
		void moveView( Vector3D *inPositionChange );
		
		
		/**
		 * Rotates the view.
		 *
		 * @param inOrientationChange angle to rotate view by.
		 *   Must be destroyed by caller.
		 */
		void rotateView( Angle3D *inOrientationChange );
		
		
		/**
		 * Gets the angle of the current view direction.
		 *
		 * @return the angle of the current view direction.
		 *   Not a copy, so shouldn't be modified or destroyed by caller.
		 */
		Angle3D *getViewOrientation();


		/**
		 * Gets the current view position.
		 *
		 * @return the position of the current view.
		 *  Must be destroyed by caller.
		 */
		Vector3D *getViewPosition();

        
		/**
		 * Sets the current view position.
		 *
		 * @param inPosition the new position.
		 *   Must be destroyed by caller.
		 */
		void setViewPosition( Vector3D *inPosition );
        
        
		/**
		 * Gets the width of the screen.
		 *
		 * @return the width of the screen, in pixels.
		 */
		int getWidth();



		/**
		 * Gets the height of the screen.
		 *
		 * @return the height of the screen, in pixels.
		 */
		int getHeight();



        /**
         * Switches into full screen mode.
         *
         * Use changeWindowSize to switch back out of full screen mode.
         */
        void setFullScreen();


        
        /**
         * Sets the size of the viewport image in the window.
         *
         * Defaults to window size.
         *
         * Must be called before screen is started.
         *
         * @param inWidth, inHeight the new dimensions, in pixels.
         */
        void setImageSize( int inWidth, int inHeight );



        /**
		 * Gets the width of the viewport image.
		 *
		 * @return the width of the viewport, in pixels.
		 */
		int getImageWidth();



		/**
		 * Gets the height of the viewport image.
		 *
		 * @return the height of the viewport, in pixels.
		 */
		int getImageHeight();


        
        /**
         * Change the window size.
         *
         * @param inWidth, inHeight the new dimensions, in pixels.
         */
        void changeWindowSize( int inWidth, int inHeight );


        
		/**
		 * Adds a mouse handler.
		 *
		 * @param inHandler the handler to add  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void addMouseHandler( MouseHandlerGL *inHandler );
		

		
		/**
		 * Removes a mouse handler.
		 *
		 * @param inHandler the handler to remove.  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void removeMouseHandler( MouseHandlerGL *inHandler );


		/**
		 * Adds a keyboard handler.
		 *
		 * @param inHandler the handler to add  Must 
		 *   be destroyed by caller.
         * @param inFirstHandler true to put this handler ahead of
         *   existing handlers in the list.
		 *
		 * Must not be called after calling start().
		 */
		void addKeyboardHandler( KeyboardHandlerGL *inHandler,
                                 char inFirstHandler = false );
		

		
		/**
		 * Removes a keyboard handler.
		 *
		 * @param inHandler the handler to remove.  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void removeKeyboardHandler( KeyboardHandlerGL *inHandler );
		

		/**
		 * Adds a scene handler.
		 *
		 * @param inHandler the handler to add  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void addSceneHandler( SceneHandlerGL *inHandler );
		

		
		/**
		 * Removes a scene handler.
		 *
		 * @param inHandler the handler to remove.  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void removeSceneHandler( SceneHandlerGL *inHandler );
		

		
		/**
		 * Adds a redraw listener.
		 *
		 * @param inListener the listener to add.  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void addRedrawListener( RedrawListenerGL *inListener );
		
		
		/**
		 * Removes a redraw listener.
		 *
		 * @param inListener the listener to remove.  Must 
		 *   be destroyed by caller.
		 *
		 * Must not be called after calling start().
		 */
		void removeRedrawListener( RedrawListenerGL *inListener );
		


		/**
		 * Applies the current view matrix transformation
		 * to the matrix at the top of the GL_PROJECTION stack.
		 */
		void applyViewTransform();


        
        void obscureRecordedNumericTyping( char inObscure, 
                                           char inCharToRecordInstead ) {
            mObscureRecordedNumericTyping = inObscure;
            mCharToRecordInstead = inCharToRecordInstead;
            }
		
		
		/**
		 * Access the various handlers.
		 */
		//KeyboardHandlerGL *getKeyHandler();
		//MouseHandlerGL *getMouseHandler();
		//SceneHandlerGL *getSceneHandler();
        

        char isLastMouseButtonRight() {
            return mLastMouseButtonRight;
            }



    private :

        void setupSurface();
        
	
        // used by various implementations
		// callbacks (external C functions that can access private members)
		
		friend void callbackResize( int inW, int inH );
		friend void callbackKeyboard( unsigned char inKey, int inX, int inY );
		friend void callbackKeyboardUp( unsigned char inKey, 
                                        int inX, int inY );
		friend void callbackSpecialKeyboard( int inKey, 
			int inX, int inY );
		friend void callbackSpecialKeyboardUp( int inKey, 
			int inX, int inY );	
		friend void callbackMotion( int inX, int inY );
		friend void callbackPassiveMotion( int inX, int inY );
		friend void callbackMouse( int inButton, int inState, 
                                   int inX, int inY );
		friend void callbackPreDisplay();
        friend void callbackDisplay();
		friend void callbackIdle();
		



        // our private members

		int mWide;
		int mHigh;

        // forces requested aspect ratio, if it's available, even
        // if it doesn't match screen's current ratio
        char mForceAspectRatio;
		
        // goes beyond just forcing the aspect ratio
        char mForceSpecifiedDimensions;
        
        // if true, when resolution not forced, we don't
        // change the user's resolution at all
        char mDoNotChangeNativeResolution;
        

        // for an viewport image that can be smaller than our screen
        char mImageSizeSet;
        int mImageWide;
        int mImageHigh;
        

		char mFullScreen;
		
        char mWantToMimimize;
        char mMinimized;
        char mWasFullScreenBeforeMinimize;
        char mWasInputGrabbedBeforeMinimize;
        
        // only allow ALT-Enter to toggle fullscreen if it started there
        char mStartedFullScreen;
        
        // current target framerate, may involve slowdown mode (for testing)
        unsigned int mMaxFrameRate;
        
        char mUseFrameSleep;
        

        // full frame rate when not in slowdown mode
        unsigned int mFullFrameRate;
        
        char mAllowSlowdownKeysDuringPlayback;
        


        char m2DMode;
        
		Vector3D *mViewPosition;
		
		// orientation of 0,0,0 means looking in the direction (0,0,1)
		// with an up direction of (0,1,0)
		Angle3D *mViewOrientation;
		
		// vectors of handlers and listeners
		SimpleVector<MouseHandlerGL*> *mMouseHandlerVector;
		SimpleVector<KeyboardHandlerGL*> *mKeyboardHandlerVector;
		SimpleVector<SceneHandlerGL*> *mSceneHandlerVector;
		SimpleVector<RedrawListenerGL*> *mRedrawListenerVector;

		

		/**
		 * Gets whether at least one of our keyboard handlers is focused.
		 *
		 * @return true iff at least one keyboard handler is focused.
		 */
		char isKeyboardHandlerFocused();
        

        // for event recording
        SimpleVector<char*> mUserEventBatch;
        // these are written to file before user events
        // so that they can be played back first
        SimpleVector<char*> mEventBatch;
        char mRecordingEvents;
        char mPlaybackEvents;
        FILE *mEventFile;

        char mObscureRecordedNumericTyping;
        char mCharToRecordInstead;
        

        

        // length of open playback file
        int mEventFileNumBatches;
        int mNumBatchesPlayed;
        
        char mShouldShowPlaybackDisplay;


        char mRecordingOrPlaybackStarted;
        
        char *mCustomRecordedGameData;
        char *mHashSalt;
        

        void writeEventBatchToFile();
        void writeEventBatchToFile( SimpleVector<char*> *inBatch );

        void playNextEventBatch();
        

        // recording file may contain gaps between web event sequence
        // numbers (if recording was trimmed by hand)
        // adjust these so that they don't have gaps
        int mLastReadWebEventHandle;
        int mCurrentWebEventHandle;
        int mNextUnusedWebEventHandle;
        
        SimpleVector<WebEvent> mPendingWebEvents;
        SimpleVector<SocketEvent> mPendingSocketEvents;


        // since async file reads happen to handles in order
        // only need to track largest handle done so far
        int mLastAsyncFileHandleDone;


        unsigned int mRandSeed;
        
        timeSec_t mLastTimeValue;
        double mLastCurrentTimeValue;

        double mLastActualFrameRate;
        

        timeSec_t mLastRecordedTimeValue;
        double mLastRecordedCurrentTimeValue;
        
        // for emulating time() values during playback where
        // time values were not recorded
        
        // detect whether any time values have been played back so far
        // if so, we assume that more will come and that we should stick
        // to those values.  If none have been played back yet, we simulate
        // played back time() values locked to the playback-specified
        // frame rate
        char mTimeValuePlayedBack;
        unsigned int mFramesSinceLastTimeTick;


        char mLastMouseButtonRight;
        
        // for playing back minimized window state
        char mLastMinimizedStatus;
        
	};



inline void ScreenGL::moveView( Vector3D *inPositionChange ) {
	mViewPosition->add( inPositionChange );
	}



inline void ScreenGL::rotateView( Angle3D *inOrientationChange ) {
	mViewOrientation->add( inOrientationChange );
	}



inline Angle3D *ScreenGL::getViewOrientation() {
	return mViewOrientation;
	}



inline Vector3D *ScreenGL::getViewPosition() {
	return new Vector3D( mViewPosition );
	}



inline void ScreenGL::setViewPosition( Vector3D *inPosition ) {
    delete mViewPosition;
    mViewPosition = new Vector3D( inPosition );
    }



inline int ScreenGL::getWidth() {
	return mWide;
	}



inline int ScreenGL::getHeight() {
	return mHigh;
	}



inline void ScreenGL::setImageSize( int inWidth, int inHeight ) {
    mImageSizeSet = true;
    
    mImageWide = inWidth;
    mImageHigh = inHeight;
    }



inline int ScreenGL::getImageWidth() {
	return mImageWide;
	}



inline int ScreenGL::getImageHeight() {
	return mImageHigh;
	}



inline void ScreenGL::addRedrawListener( RedrawListenerGL *inListener ) {
	mRedrawListenerVector->push_back( inListener );
	}
		


inline void ScreenGL::removeRedrawListener( RedrawListenerGL *inListener ) {
	mRedrawListenerVector->deleteElementEqualTo( inListener );
	}



inline void ScreenGL::addMouseHandler( MouseHandlerGL *inListener ) {
	mMouseHandlerVector->push_back( inListener );
	}
		


inline void ScreenGL::removeMouseHandler( MouseHandlerGL *inListener ) {
	mMouseHandlerVector->deleteElementEqualTo( inListener );
	}



inline void ScreenGL::addKeyboardHandler( KeyboardHandlerGL *inListener,
                                          char inFirstHandler ) {
	if( !inFirstHandler ) {
        mKeyboardHandlerVector->push_back( inListener );
        }
    else {
        int numExisting= mKeyboardHandlerVector->size();
        KeyboardHandlerGL **oldHandlers = 
            mKeyboardHandlerVector->getElementArray();
        
        mKeyboardHandlerVector->deleteAll();
        
        mKeyboardHandlerVector->push_back( inListener );
        mKeyboardHandlerVector->appendArray( oldHandlers, numExisting );
        delete [] oldHandlers;
        }
	}
		


inline void ScreenGL::removeKeyboardHandler( KeyboardHandlerGL *inListener ) {
	mKeyboardHandlerVector->deleteElementEqualTo( inListener );
	}



inline void ScreenGL::addSceneHandler( SceneHandlerGL *inListener ) {
	mSceneHandlerVector->push_back( inListener );
	}
		


inline void ScreenGL::removeSceneHandler( SceneHandlerGL *inListener ) {
	mSceneHandlerVector->deleteElementEqualTo( inListener );
	}



inline char ScreenGL::isKeyboardHandlerFocused() {
	for( int h=0; h<mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( mKeyboardHandlerVector->getElement( h ) );
		if( handler->isFocused() ) {
			return true;
			}
		}
	
	// else none were focused
	return false;
	}


inline void ScreenGL::allowSlowdownKeysDuringPlayback( char inAllow ) {
    mAllowSlowdownKeysDuringPlayback = inAllow;
    }



#endif
