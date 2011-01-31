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

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/FileInputStream.h"



#include "minorGems/game/game.h"
#include "minorGems/game/gameGraphics.h"



#include "demoCodePanel.h"



// some settings

// size of game image
int gameWidth = 320;
int gameHeight = 240;

// size of screen for fullscreen mode
int screenWidth = 640;
int screenHeight = 480;


int targetFrameRate = 60;


int soundSampleRate = 22050;
//int soundSampleRate = 44100;


char hardToQuitMode = false;


char demoMode = false;




// ^ and & keys to slow down and speed up for testing
//char enableSlowdownKeys = false;
char enableSlowdownKeys = true;


char mouseWorldCoordinates = true;



// should screenshot be taken at end of next redraw?
static char shouldTakeScreenshot = false;
static char *screenShotPrefix = NULL;

static void takeScreenShot();







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
        

        char mPaused;

        // reduce sleep time when user hits keys to restore responsiveness
        unsigned int mPausedSleepTime;

        
        
    protected:

        int mStartTimeSeconds;
        

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

    if( getUsesSound() ) {
        SDL_CloseAudio();
        }

    freeFrameDrawer();

    

    SDL_Quit();

    if( screenShotPrefix != NULL ) {
        delete [] screenShotPrefix;
        screenShotPrefix = NULL;
        }
    }





void audioCallback( void *inUserData, Uint8 *inStream, int inLengthToFill ) {
    getSoundSamples( inStream, inLengthToFill );
    }


int getSampleRate() {
    return soundSampleRate;
    }


void setSoundPlaying( char inPlaying ) {
    SDL_PauseAudio( !inPlaying );
    }



void lockAudio() {
    SDL_LockAudio();
    }



void unlockAudio() {
    SDL_UnlockAudio();
    }




int mainFunction( int inNumArgs, char **inArgs ) {




    // check result below, after opening log, so we can log failure
    Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
    if( getUsesSound() ) {
        flags |= SDL_INIT_AUDIO;
        }
    
    int sdlResult = SDL_Init( flags );


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
    
    if( fullscreenFound && readFullscreen == 0 ) {
        fullscreen = false;
        }


    char frameRateFound = false;
    int readFrameRate = SettingsManager::getIntSetting( "halfFrameRate", 
                                                         &frameRateFound );

    if( frameRateFound && readFrameRate >= 1 ) {
        // cut frame rate in half N times
        targetFrameRate /= (int)pow( 2, readFrameRate );
        }
    

    
    char recordFound = false;
    int readRecordFlag = SettingsManager::getIntSetting( "recordGame", 
                                                         &recordFound );
    
    char recordGame = false;
    
    if( recordFound && readRecordFlag == 1 ) {
        recordGame = true;
        }


    char *customData = getCustomRecordedGameData();

    screen =
        new ScreenGL( screenWidth, screenHeight, fullscreen, targetFrameRate,
                      recordGame,
                      customData,
                      getWindowTitle(), NULL, NULL, NULL );

    delete [] customData;
    

    // may change if specified resolution is not supported
    // or for event playback mode
    screenWidth = screen->getWidth();
    screenHeight = screen->getHeight();
    targetFrameRate = screen->getMaxFramerate();
    
    
    // adjust gameWidth to match available screen aspect ratio
    // keep gameHeight constant

    double aspectRatio = (double)screenWidth / (double)screenHeight;

    gameWidth = (int)( round( aspectRatio * gameHeight ) );


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
    

    screen->allowSlowdownKeysDuringPlayback( enableSlowdownKeys );

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


    
    // translation language
    File *languageNameFile = new File( NULL, "language.txt" );

    if( languageNameFile->exists() ) {
        char *languageNameText = languageNameFile->readFileContents();

        SimpleVector<char *> *tokens = tokenizeString( languageNameText );

        int numTokens = tokens->size();
        
        // first token is name
        if( numTokens > 0 ) {
            char *languageName = *( tokens->getElement( 0 ) );
        
            TranslationManager::setLanguage( languageName );
            }
        else {
            // default

            // TranslationManager already defaults to English, but
            // it looks for the language files at runtime before we have set
            // the current working directory.
            
            // Thus, we specify the default again here so that it looks
            // for its language files again.
            TranslationManager::setLanguage( "English" );
            }
        
        delete [] languageNameText;

        for( int t=0; t<numTokens; t++ ) {
            delete [] *( tokens->getElement( t ) );
            }
        delete tokens;
        }
    
    delete languageNameFile;
    

    
        
    // register cleanup function, since screen->start() will never return
    atexit( cleanUpAtExit );




    screen->switchTo2DMode();



    if( getUsesSound() ) {
        
        soundSampleRate = 
            SettingsManager::getIntSetting( "soundSampleRate", 22050 );

        int requestedBufferSize = 
            SettingsManager::getIntSetting( "soundBufferSize", 512 );
        
        
        // force user-specified value to closest (round up) power of 2
        int bufferSize = 2;
        while( bufferSize < requestedBufferSize ) {
            bufferSize *= 2;
            }
        

        SDL_AudioSpec audioFormat;

        /* Set 16-bit stereo audio at 22Khz */
        audioFormat.freq = soundSampleRate;
        audioFormat.format = AUDIO_S16;
        audioFormat.channels = 2;
        //audioFormat.samples = 512;        /* A good value for games */
        audioFormat.samples = bufferSize;     
        audioFormat.callback = audioCallback;
        audioFormat.userdata = NULL;
        
        SDL_AudioSpec actualFormat;

        /* Open the audio device and start playing sound! */
        if( SDL_OpenAudio( &audioFormat, &actualFormat ) < 0 ) {
            AppLog::getLog()->logPrintf( 
                Log::ERROR_LEVEL,
                "Unable to open audio: %s\n", SDL_GetError() );
            }
        else {

            if( actualFormat.format != AUDIO_S16 ||
                actualFormat.channels != 2 ) {
                
                
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "Able to open audio, "
                    "but stereo S16 samples not availabl\n");
                
                SDL_CloseAudio();
                }
            else {
                
                int desiredRate = soundSampleRate;
                

                AppLog::getLog()->logPrintf( 
                    Log::INFO_LEVEL,
                    "Successfully opened audio: %dHz (requested %dHz), "
                    "sample buffer size=%d (requested %d)\n", 
                    actualFormat.freq, desiredRate, actualFormat.samples,
                    bufferSize);
                
                soundSampleRate = actualFormat.freq;
                }
            }
        
        }
    


    demoMode = isDemoMode();
    
    
    //glLineWidth( pixelZoomFactor );

    initFrameDrawer( pixelZoomFactor * gameWidth, 
                     pixelZoomFactor * gameHeight, 
                     targetFrameRate,
                     screen->getCustomRecordedGameData(),
                     screen->isPlayingBack() );

    if( demoMode ) {    
        showDemoCodePanel( screen, getFontTGAFileName(), gameWidth,
                           gameHeight );
        
        // wait to start handling events
        // wait to start recording/playback
        }
    else {
        // handle events right away
        screen->addMouseHandler( sceneHandler );
        screen->addKeyboardHandler( sceneHandler );

        // start recording/playback right away
        screen->startRecordingOrPlayback();
        }
    
    screen->start();

    
    return 0;
    }




GameSceneHandler::GameSceneHandler( ScreenGL *inScreen )
    : mScreen( inScreen ),
      mPaused( false ),
      mPausedSleepTime( 0 ),
      mStartTimeSeconds( time( NULL ) ),
      mPrintFrameRate( true ),
      mNumFrames( 0 ), mFrameBatchSize( 100 ),
      mFrameBatchStartTimeSeconds( time( NULL ) ),
      mFrameBatchStartTimeMilliseconds( 0 ),
      mBackgroundColor( 0, 0, 0, 1 ) { 
    
    
    glClearColor( mBackgroundColor.r,
                  mBackgroundColor.g,
                  mBackgroundColor.b,
                  mBackgroundColor.a );
    

    // set external pointer so it can be used in calls below
    sceneHandler = this;

        
    mScreen->addSceneHandler( sceneHandler );
    mScreen->addRedrawListener( sceneHandler );
    }



GameSceneHandler::~GameSceneHandler() {
    mScreen->removeMouseHandler( this );
    mScreen->removeSceneHandler( this );
    mScreen->removeRedrawListener( this );

    if( demoMode ) {
        // panel has not freed itself yet
        freeDemoCodePanel();
        
        demoMode = false;
        }
    
    }




void GameSceneHandler::initFromFiles() {
    
    }




static float viewCenterX = 0;
static float viewCenterY = 0;
// default -1 to +1
static float viewSize = 2;


static void redoDrawMatrix() {
    // viewport square centered on screen (even if screen is rectangle)
    float hRadius = viewSize / 2;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( viewCenterX - hRadius, viewCenterX + hRadius, 
             viewCenterY - hRadius, viewCenterY + hRadius, -1.0f, 1.0f);
    
    
    glMatrixMode(GL_MODELVIEW);
    }



unsigned int getRandSeed() {
    return screen->getRandSeed();
    }



void pauseGame() {
    sceneHandler->mPaused = !( sceneHandler->mPaused );
    }


char isPaused() {
    return sceneHandler->mPaused;
    }


void wakeUpPauseFrameRate() {
    sceneHandler->mPausedSleepTime = 0;
    }



void mapKey( unsigned char inFromKey, unsigned char inToKey ) {
    screen->setKeyMapping( inFromKey, inToKey );
    }





void setViewCenterPosition( float inX, float inY ) {
    viewCenterX = inX;
    viewCenterY = inY;
    redoDrawMatrix();
    }
    

void setViewSize( float inSize ) {
    viewSize = inSize;
    redoDrawMatrix();
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


static char ignoreNextMouseEvent = false;
static int xCoordToIgnore, yCoordToIgnore;

void warpMouseToCenter( int *outNewMouseX, int *outNewMouseY ) {
    *outNewMouseX = screenWidth / 2;
    *outNewMouseY = screenHeight / 2;


    if( SDL_GetAppState() & SDL_APPMOUSEFOCUS ) {
        
        if( !demoMode ) {
            // not ignoring mouse events currently due to demo code panel
            ignoreNextMouseEvent = true;
            xCoordToIgnore = *outNewMouseX;
            yCoordToIgnore = *outNewMouseY;
            }    

        SDL_WarpMouse( *outNewMouseX, *outNewMouseY );
        }
    
    }





void GameSceneHandler::drawScene() {
    /*
    glClearColor( mBackgroundColor->r,
                  mBackgroundColor->g,
                  mBackgroundColor->b,
                  mBackgroundColor->a );
    */
	

    redoDrawMatrix();


	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );


    if( demoMode ) {
        
        if( ! isDemoCodePanelShowing() ) {
            
            // stop demo mode when panel done
            demoMode = false;

            mScreen->addMouseHandler( this );
            mScreen->addKeyboardHandler( this );

            screen->startRecordingOrPlayback();
            }
        }
    else {
        // demo mode done or was never enabled

        // carry on with game
        
        // don't update while paused
        char update = !mPaused;
        
        drawFrame( update );
        }

    if( shouldTakeScreenshot ) {
        takeScreenShot();
        shouldTakeScreenshot = false;
        }
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
    if( ignoreNextMouseEvent ) {
        if( inX == xCoordToIgnore && inY == yCoordToIgnore ) {
            // seeing the event that triggered the ignore
            ignoreNextMouseEvent = false;
            return;
            }
        else {
            // stale pending event before the ignore
            // skip it too
            return;
            }
        }
    
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerMove( x, y );
    }



void GameSceneHandler::mouseDragged( int inX, int inY ) {
    if( ignoreNextMouseEvent ) {
        if( inX == xCoordToIgnore && inY == yCoordToIgnore ) {
            // seeing the event that triggered the ignore
            ignoreNextMouseEvent = false;
            return;
            }
        else {
            // stale pending event before the ignore
            // skip it too
            return;
            }
        }
    
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

        if( mPausedSleepTime > (unsigned int)( 5 * targetFrameRate ) ) {
            // user has touched nothing for 5 seconds
            
            // sleep to avoid wasting CPU cycles
            Thread::staticSleep( mPausedSleepTime );
            }
        
        mPausedSleepTime++;
        
        return;
        }


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
            
            Time::getCurrentTime( &mFrameBatchStartTimeSeconds,
                                  &mFrameBatchStartTimeMilliseconds );
            }
        }
    }



static unsigned char lastKeyPressed = '\0';


void GameSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    // reset to become responsive while paused
    mPausedSleepTime = 0;
    

    if( enableSlowdownKeys ) {
        
        if( inKey == '^' ) {
            // slow
            mScreen->setMaxFrameRate( 2 );
            }
        if( inKey == '&' ) {
            // normal
            mScreen->setMaxFrameRate( targetFrameRate );
            }
        if( inKey == '*' ) {
            // fast forward
            mScreen->setMaxFrameRate( targetFrameRate * 2 );
            }            
        if( inKey == '(' ) {
            // fast forward
            mScreen->setMaxFrameRate( targetFrameRate * 4 );
            }            
        }
    
    if( !hardToQuitMode ) {
        // escape only

        if( inKey == 27 ) {
            // escape always toggles pause
            mPaused = !mPaused;
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


    if( mPaused && inKey == MG_KEY_F4 ) {
        // f4 to quit from pause
        exit( 0 );
        }
    
    specialKeyDown( inKey );
	}



void GameSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {

    specialKeyUp( inKey );
    } 



void GameSceneHandler::actionPerformed( GUIComponent *inTarget ) {
    }



Image *readTGAFile( const char *inTGAFileName ) {

    File tgaFile( new Path( "graphics" ), inTGAFileName );
    FileInputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    Image *result = converter.deformatImage( &tgaStream );

    if( result == NULL ) {        
        char *logString = autoSprintf( 
            "CRITICAL ERROR:  could not read TGA file %s, wrong format?",
            inTGAFileName );
        AppLog::criticalError( logString );
        delete [] logString;
        }
    
    return result;
    }


void writeTGAFile( const char *inTGAFileName, Image *inImage ) {
    File tgaFile( NULL, inTGAFileName );
    FileOutputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    return converter.formatImage( inImage, &tgaStream );
    }



SpriteHandle loadSprite( const char *inTGAFileName,
                         char inTransparentLowerLeftCorner ) {
    Image *result = readTGAFile( inTGAFileName );
    
    if( result == NULL ) {
        return NULL;
        }
    else {
        
        SpriteHandle sprite = fillSprite( result, 
                                          inTransparentLowerLeftCorner );

        delete result;
        return sprite;
        }
    }



const char *translate( const char *inTranslationKey ) {
    return TranslationManager::translate( inTranslationKey );
    }



void saveScreenShot( const char *inPrefix ) {
    if( screenShotPrefix != NULL ) {
        delete [] screenShotPrefix;
        }
    screenShotPrefix = stringDuplicate( inPrefix );
    shouldTakeScreenshot = true;
    }



static int nextShotNumber = -1;
static char shotDirExists = false;


void takeScreenShot() {
     
    
    File shotDir( NULL, "screenShots" );
    
    if( !shotDirExists && !shotDir.exists() ) {
        shotDir.makeDirectory();
        shotDirExists = shotDir.exists();
        }
    
    if( nextShotNumber < 1 ) {
        if( shotDir.exists() && shotDir.isDirectory() ) {
        
            int numFiles;
            File **childFiles = shotDir.getChildFiles( &numFiles );

            nextShotNumber = 1;

            char *formatString = autoSprintf( "%s%%d.tga", screenShotPrefix );

            for( int i=0; i<numFiles; i++ ) {
            
                char *name = childFiles[i]->getFileName();
                
                int number;
                
                int numRead = sscanf( name, formatString, &number );
                
                if( numRead == 1 ) {
                    
                    if( number >= nextShotNumber ) {
                        nextShotNumber = number + 1;
                        }
                    }
                delete [] name;
                
                delete childFiles[i];
                }
            
            delete [] formatString;
            
            delete [] childFiles;
            }
        }
    

    if( nextShotNumber < 1 ) {
        return;
        }
    
    char *fileName = autoSprintf( "%s%05d.tga", 
                                  screenShotPrefix, nextShotNumber );

    nextShotNumber++;
    

    File *file = shotDir.getChildFile( fileName );
    
    delete [] fileName;


    
    unsigned char *rgbBytes = 
        new unsigned char[ screenWidth * screenHeight * 3 ];

    // w and h might not be multiples of 4
    int oldAlignment;
    glGetIntegerv( GL_PACK_ALIGNMENT, &oldAlignment );
                
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    
    glReadPixels( 0, 0, screenWidth, screenHeight, 
                  GL_RGB, GL_UNSIGNED_BYTE, rgbBytes );
    
    glPixelStorei( GL_PACK_ALIGNMENT, oldAlignment );


    Image screenImage( screenWidth, screenHeight, 3, false );

    double *channels[3];
    int c;
    for( c=0; c<3; c++ ) {
        channels[c] = screenImage.getChannel( c );
        }
    
    // image of screen is upside down
    int outputRow = 0;
    for( int y=screenHeight-1; y>=0; y-- ) {
        for( int x=0; x<screenWidth; x++ ) {
                        
            int outputPixelIndex = outputRow * screenWidth + x;
            
            
            int screenPixelIndex = y * screenWidth + x;
            int byteIndex = screenPixelIndex * 3;
                        
            for( c=0; c<3; c++ ) {
                channels[c][outputPixelIndex] =
                    rgbBytes[ byteIndex + c ] / 255.0;
                }
            }
        outputRow++;
        }
    
    delete [] rgbBytes;


    FileOutputStream tgaStream( file );
    
    TGAImageConverter converter;
    
    converter.formatImage( &screenImage, &tgaStream );

    delete file;


    delete [] screenShotPrefix;
    screenShotPrefix = NULL;
    }








    



