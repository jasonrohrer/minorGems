/*
 * Modification History
 *
 * 2010-September-3  Jason Rohrer
 * Fixed mouse to world translation function.
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


// let SDL override our main function with SDLMain
#include <SDL/SDL_main.h>


// must do this before SDL include to prevent WinMain linker errors on win32
int mainFunction( int inArgCount, char **inArgs );

int main( int inArgCount, char **inArgs ) {
    return mainFunction( inArgCount, inArgs );
    }


#include <SDL/SDL.h>



#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/graphics/openGL/SceneHandlerGL.h"
#include "minorGems/graphics/Color.h"

#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/graphics/openGL/gui/GUITranslatorGL.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/graphics/openGL/gui/LabelGL.h"
#include "minorGems/graphics/openGL/gui/TextFieldGL.h"
#include "minorGems/graphics/openGL/gui/SliderGL.h"



#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"

#include "minorGems/io/file/File.h"

#include "minorGems/network/HostAddress.h"

#include "minorGems/network/upnp/portMapping.h"


#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"


#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/log/FileLog.h"



#include "minorGems/game/game.h"





// some settings

// size of game image
int gameWidth = 320;
int gameHeight = 240;

// size of screen for fullscreen mode
int screenWidth = 640;
int screenHeight = 480;


char hardToQuitMode = false;



// ^ and & keys to slow down and speed up for testing
char enableSlowdownKeys = false;
//char enableSlowdownKeys = true;


char mouseWorldCoordinates = true;









class GameSceneHandler :
    public SceneHandlerGL, public MouseHandlerGL, public KeyboardHandlerGL,
    public RedrawListenerGL, public ActionListener  { 

	public:

        /**
         * Constructs a sceen handler.
         *
         * @param inScreen the screen to interact with.
         *   Must be destroyed by caller after this class is destroyed.
         */
        GameSceneHandler( ScreenGL *inScreen );

        virtual ~GameSceneHandler();


        
        /**
         * Executes necessary init code that reads from files.
         *
         * Must be called before using a newly-constructed GameSceneHandler.
         *
         * This call assumes that the needed files are in the current working
         * directory.
         */
        void initFromFiles();

        

        ScreenGL *mScreen;


        


        
        
        
		// implements the SceneHandlerGL interface
		virtual void drawScene();

        // implements the MouseHandlerGL interface
        virtual void mouseMoved( int inX, int inY );
        virtual void mouseDragged( int inX, int inY );
        virtual void mousePressed( int inX, int inY );
        virtual void mouseReleased( int inX, int inY );

        // implements the KeyboardHandlerGL interface
        virtual char isFocused() {
            // always focused
            return true;
            }        
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
		virtual void specialKeyPressed( int inKey, int inX, int inY );
		virtual void keyReleased( unsigned char inKey, int inX, int inY );
		virtual void specialKeyReleased( int inKey, int inX, int inY );
        
        // implements the RedrawListener interface
		virtual void fireRedraw();
        

        // implements the ActionListener interface
        virtual void actionPerformed( GUIComponent *inTarget );
        
        
        
    protected:

        
        // the time that the last frame was drawn
        unsigned long mLastFrameSeconds;
        unsigned long mLastFrameMilliseconds;

        // our current frame rate
        unsigned long mFrameMillisecondDelta;

        int mStartTimeSeconds;
        
        char mPaused;

        double mMaxFrameRate;

        char mPrintFrameRate;
        unsigned long mNumFrames;
        unsigned long mFrameBatchSize;
        unsigned long mFrameBatchStartTimeSeconds;
        unsigned long mFrameBatchStartTimeMilliseconds;
        


        Color mBackgroundColor;


	};



GameSceneHandler *sceneHandler;
ScreenGL *screen;


// how many pixels wide is each game pixel drawn as?
int pixelZoomFactor;







// function that destroys object when exit is called.
// exit is the only way to stop the loop in  ScreenGL
void cleanUpAtExit() {
    AppLog::info( "exiting\n" );

    delete sceneHandler;
    delete screen;

    freeFrameDrawer();


    SDL_Quit();
    }






int mainFunction( int inNumArgs, char **inArgs ) {




    // check result below, after opening log, so we can log failure
    int sdlResult = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE );


    // do this mac check after initing SDL,
    // since it causes various Mac frameworks to be loaded (which can
    // change the current working directory out from under us)
    #ifdef __mac__
        // make sure working directory is the same as the directory
        // that the app resides in
        // this is especially important on the mac platform, which
        // doesn't set a proper working directory for double-clicked
        // app bundles

        // arg 0 is the path to the app executable
        char *appDirectoryPath = stringDuplicate( inArgs[0] );
    
        char *appNamePointer = strstr( appDirectoryPath,
                                       "GameApp.app" );

        if( appNamePointer != NULL ) {
            // terminate full app path to get parent directory
            appNamePointer[0] = '\0';
            
            chdir( appDirectoryPath );
            }
        
        delete [] appDirectoryPath;
    #endif

        

    AppLog::setLog( new FileLog( "log.txt" ) );
    AppLog::setLoggingLevel( Log::DETAIL_LEVEL );
    
    AppLog::info( "New game starting up" );
    

    if( sdlResult < 0 ) {
        AppLog::getLog()->logPrintf( 
            Log::CRITICAL_ERROR_LEVEL,
            "Couldn't initialize SDL: %s\n", SDL_GetError() );
        return 0;
        }


    // read screen size from settings
    char widthFound = false;
    int readWidth = SettingsManager::getIntSetting( "screenWidth", 
                                                    &widthFound );
    char heightFound = false;
    int readHeight = SettingsManager::getIntSetting( "screenHeight", 
                                                    &heightFound );
    
    if( widthFound && heightFound ) {
        // override hard-coded defaults
        screenWidth = readWidth;
        screenHeight = readHeight;
        }
    
    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Screen dimensions for fullscreen mode:  %dx%d\n",
        screenWidth, screenHeight );


    char fullscreenFound = false;
    int readFullscreen = SettingsManager::getIntSetting( "fullscreen", 
                                                         &fullscreenFound );
    
    char fullscreen = true;
    
    if( readFullscreen == 0 ) {
        fullscreen = false;
        }
    

    screen =
        new ScreenGL( screenWidth, screenHeight, fullscreen, 
                      getWindowTitle(), NULL, NULL, NULL );

    // may change if specified resolution is not supported
    screenWidth = screen->getWidth();
    screenHeight = screen->getHeight();
    
    /*
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY,
                         SDL_DEFAULT_REPEAT_INTERVAL );
    */
    pixelZoomFactor = screenWidth / gameWidth;

    if( pixelZoomFactor * gameHeight > screenHeight ) {
        // screen too short
        pixelZoomFactor = screenHeight / gameHeight;
        }
    

    screen->setImageSize( pixelZoomFactor * gameWidth,
                          pixelZoomFactor * gameHeight );
    

    //SDL_ShowCursor( SDL_DISABLE );


    sceneHandler = new GameSceneHandler( screen );

    

    // also do file-dependent part of init for GameSceneHandler here
    // actually, constructor is file dependent anyway.
    sceneHandler->initFromFiles();
    

    // hard to quit mode?
    char hardToQuitFound = false;
    int readHardToQuit = SettingsManager::getIntSetting( "hardToQuitMode", 
                                                         &hardToQuitFound );
    
    if( readHardToQuit == 1 ) {
        hardToQuitMode = true;
        }
    
        
    // register cleanup function, since screen->start() will never return
    atexit( cleanUpAtExit );




    screen->switchTo2DMode();
    
    //glLineWidth( pixelZoomFactor );

    initFrameDrawer( screenWidth, screenHeight );

    screen->start();

    
    return 0;
    }




GameSceneHandler::GameSceneHandler( ScreenGL *inScreen )
    : mScreen( inScreen ),
      mFrameMillisecondDelta( 0 ),
      mStartTimeSeconds( time( NULL ) ),
      mPaused( false ),
      mMaxFrameRate( 30 ),  
      mPrintFrameRate( false ),
      mNumFrames( 0 ), mFrameBatchSize( 100 ),
      mFrameBatchStartTimeSeconds( time( NULL ) ),
      mFrameBatchStartTimeMilliseconds( 0 ),
      mBackgroundColor( 0, 0, 0.5, 1 ) { 
    
    
    glClearColor( mBackgroundColor.r,
                  mBackgroundColor.g,
                  mBackgroundColor.b,
                  mBackgroundColor.a );
    

    // set external pointer so it can be used in calls below
    sceneHandler = this;

    
    mScreen->addMouseHandler( this );
    mScreen->addKeyboardHandler( this );
    mScreen->addSceneHandler( this );
    mScreen->addRedrawListener( this );

    
    Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );


    }



GameSceneHandler::~GameSceneHandler() {
    mScreen->removeMouseHandler( this );
    mScreen->removeSceneHandler( this );
    mScreen->removeRedrawListener( this );
    }




void GameSceneHandler::initFromFiles() {
    
    }




static float viewCenterX = 0;
static float viewCenterY = 0;
// default -1 to +1
static float viewSize = 2;


void setViewCenterPosition( float inX, float inY ) {
    viewCenterX = inX;
    viewCenterY = inY;
    }
    

void setViewSize( float inSize ) {
    viewSize = inSize;
    }


void setCursorVisible( char inIsVisible ) {
    if( inIsVisible ) {
        SDL_ShowCursor( SDL_ENABLE );
        }
    else {
        SDL_ShowCursor( SDL_DISABLE );
        }
    }



void grabInput( char inGrabOn ) {
    if( inGrabOn ) {
        SDL_WM_GrabInput( SDL_GRAB_ON );
        }
    else {
        SDL_WM_GrabInput( SDL_GRAB_OFF );
        }
    }



void setMouseReportingMode( char inWorldCoordinates ) {
    mouseWorldCoordinates = inWorldCoordinates;
    }




void GameSceneHandler::drawScene() {
    /*
    glClearColor( mBackgroundColor->r,
                  mBackgroundColor->g,
                  mBackgroundColor->b,
                  mBackgroundColor->a );
    */
	
    // viewport square centered on screen (even if screen is rectangle)
    float hRadius = viewSize / 2;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( viewCenterX - hRadius, viewCenterX + hRadius, 
             viewCenterY - hRadius, viewCenterY + hRadius, -1.0f, 1.0f);
    
    
    glMatrixMode(GL_MODELVIEW);
    


	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );


    drawFrame();    
    }


static void screenToWorld( int inX, int inY, float *outX, float *outY ) {

    if( mouseWorldCoordinates ) {
        
        // relative to center,
        // viewSize spreads out across screenWidth only (a square on screen)
        float x = (float)( inX - (screenWidth/2) ) / (float)screenWidth;
        float y = -(float)( inY - (screenHeight/2) ) / (float)screenWidth;
        
        *outX = x * viewSize + viewCenterX;
        *outY = y * viewSize + viewCenterY;
        }
    else {
        // raw screen coordinates
        *outX = inX;
        *outY = inY;
        }
    
    }




void GameSceneHandler::mouseMoved( int inX, int inY ) {
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerMove( x, y );
    }



void GameSceneHandler::mouseDragged( int inX, int inY ) {
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerDrag( x, y );
    }




void GameSceneHandler::mousePressed( int inX, int inY ) {
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerDown( x, y );
    }



void GameSceneHandler::mouseReleased( int inX, int inY ) {
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerUp( x, y );
    }



void GameSceneHandler::fireRedraw() {
    
    if( mPaused ) {
        // ignore redraw event

        // sleep to avoid wasting CPU cycles
        Thread::staticSleep( 1000 );
        
        // also ignore time that passes while paused
        Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );
        
        return;
        }

    
    // deal with frame timing issues
    
    unsigned long lastMillisecondDelta = mFrameMillisecondDelta;
    
    // how many milliseconds have passed since the last frame
    mFrameMillisecondDelta =
        Time::getMillisecondsSince( mLastFrameSeconds,
                                    mLastFrameMilliseconds );

    
    // lock down to mMaxFrameRate frames per second
    unsigned long minFrameTime = (unsigned long)( 1000 / mMaxFrameRate );
    if( mFrameMillisecondDelta < minFrameTime ) {
        unsigned long timeToSleep = minFrameTime - mFrameMillisecondDelta;
        Thread::staticSleep( timeToSleep );

        // get new frame second delta, including sleep time
        mFrameMillisecondDelta =
            Time::getMillisecondsSince( mLastFrameSeconds,
                                        mLastFrameMilliseconds );
        }

    // avoid huge position "jumps" if we have a very large delay during a frame
    // (possibly caused by something going on in the background)
    // This will favor a slight visual slow down, but this is better than
    // a disorienting jump

    // skip this check if we are just starting up
    if( lastMillisecondDelta != 0 ) {
        if( mFrameMillisecondDelta > 6 * lastMillisecondDelta ) {
            // limit:  this frame represents at most twice the jump of the last
            // frame
            // printf( "Limiting time jump (requested=%lu ms, last=%lu ms)\n",
            //        mFrameMillisecondDelta, lastMillisecondDelta );

            if( mFrameMillisecondDelta > 10000 ) {
                AppLog::warning( 
                    "Time between frames more than 10 seconds:\n" );
                // way too big... investigate
                AppLog::getLog()->logPrintf( 
                    Log::WARNING_LEVEL,
                    "Last time = %lu s, %lu ms\n",
                    mLastFrameSeconds, mLastFrameMilliseconds );

                Time::getCurrentTime( &mLastFrameSeconds,
                                      &mLastFrameMilliseconds );
                AppLog::getLog()->logPrintf( 
                    Log::WARNING_LEVEL,
                    "current time = %lu s, %lu ms\n",
                    mLastFrameSeconds, mLastFrameMilliseconds );

                }
            
            mFrameMillisecondDelta = 2 * lastMillisecondDelta;
            
            }
        }
    
    
    // record the time that this frame was drawn
    Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );

    
    // for use with non-constant time-per-frame
    // this game is constant time-per-frame
    // double frameSecondsDelta = (double)mFrameMillisecondDelta / 1000.0;
    


    mNumFrames ++;

    if( mPrintFrameRate ) {
        
        if( mNumFrames % mFrameBatchSize == 0 ) {
            // finished a batch
            
            unsigned long timeDelta =
                Time::getMillisecondsSince( mFrameBatchStartTimeSeconds,
                                            mFrameBatchStartTimeMilliseconds );

            double frameRate =
                1000 * (double)mFrameBatchSize / (double)timeDelta;
            
            //AppLog::getLog()->logPrintf( 
            //    Log::DETAIL_LEVEL,
                printf(
                "Frame rate = %f frames/second\n", frameRate );
            
            mFrameBatchStartTimeSeconds = mLastFrameSeconds;
            mFrameBatchStartTimeMilliseconds = mLastFrameMilliseconds;
            }
        }
    }



static unsigned char lastKeyPressed = '\0';


void GameSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    if( enableSlowdownKeys ) {
        
        if( inKey == '^' ) {
            // slow
            mMaxFrameRate = 2;
            }
        if( inKey == '&' ) {
            // normal
            mMaxFrameRate = 30;
            }
        }
    
    if( !hardToQuitMode ) {
        // q or escape
        if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
            exit( 0 );
            }
        }
    else {
        // # followed by ESC
        if( lastKeyPressed == '#' && inKey == 27 ) {
            exit( 0 );
            }
        lastKeyPressed = inKey;
        }

    keyDown( inKey );
    }



void GameSceneHandler::keyReleased(
	unsigned char inKey, int inX, int inY ) {

    keyUp( inKey );
    }



void GameSceneHandler::specialKeyPressed(
	int inKey, int inX, int inY ) {

    specialKeyDown( inKey );
	}



void GameSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {

    specialKeyUp( inKey );
    } 



void GameSceneHandler::actionPerformed( GUIComponent *inTarget ) {
    }




    



