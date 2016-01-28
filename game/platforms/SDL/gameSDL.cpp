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

#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/network/web/WebRequest.h"

#include "minorGems/graphics/openGL/glInclude.h"

#include "minorGems/graphics/openGL/SceneHandlerGL.h"
#include "minorGems/graphics/openGL/MouseHandlerGL.h"
#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"
#include "minorGems/ui/event/ActionListener.h"


#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"

#include "minorGems/io/file/File.h"

#include "minorGems/network/HostAddress.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketClient.h"

#include "minorGems/network/upnp/portMapping.h"


#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"


#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/log/FileLog.h"

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/FileInputStream.h"


#include "minorGems/sound/formats/aiff.h"



#include "minorGems/game/game.h"
#include "minorGems/game/gameGraphics.h"

#include "minorGems/game/diffBundle/client/diffBundleClient.h"





#ifdef RASPBIAN

#include "minorGems/graphics/openGL/RaspbianGLSurface.cpp"

// demo code panel uses non-GLES code
void showDemoCodePanel( ScreenGL *inScreen, const char *inFontTGAFileName,
                        int inWidth, int inHeight ) {
    }
char isDemoCodePanelShowing() {
    return false;
    }
void freeDemoCodePanel() {
    }
void showWriteFailedPanel( ScreenGL *inScreen, const char *inFontTGAFileName,
                           int inWidth, int inHeight ) {
    }
void freeWriteFailedPanel() {
    }


#else

#include "demoCodePanel.h"

#endif



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

char soundRunning = false;


char hardToQuitMode = false;


char demoMode = false;

char writeFailed = false;
char loadingFailedFlag = false;
char *loadingFailedMessage = NULL;


char loadingMessageShown = false;
char frameDrawerInited = false;


// ^ and & keys to slow down and speed up for testing
// read from settings folder
char enableSpeedControlKeys = false;

// should each and every frame be saved to disk?
// useful for making videos
char outputAllFrames = false;

// should output only every other frame, and blend in dropped frames?
char blendOutputFramePairs = false;

float blendOutputFrameFraction = 0;


char *webProxy = NULL;



static unsigned char *lastFrame_rgbaBytes = NULL;


static unsigned int frameNumber = 0;


static char recordAudio = false;

static FILE *aiffOutFile = NULL;

static int samplesLeftToRecord = 0;




char mouseWorldCoordinates = true;


#ifdef USE_JPEG
    #include "minorGems/graphics/converters/JPEGImageConverter.h"
    static JPEGImageConverter screenShotConverter( 90 );
    static const char *screenShotExtension = "jpg";
#elif defined(USE_PNG)
    #include "minorGems/graphics/converters/PNGImageConverter.h"
    static PNGImageConverter screenShotConverter;
    static const char *screenShotExtension = "png";
#else
    static TGAImageConverter screenShotConverter;
    static const char *screenShotExtension = "tga";
#endif


// should screenshot be taken at end of next redraw?
static char shouldTakeScreenshot = false;
static char manualScreenShot = false;

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


        char mBlockQuitting;

        double mLastFrameRate;

        
    protected:

        int mStartTimeSeconds;
        

        char mPrintFrameRate;
        unsigned long mNumFrames;
        unsigned long mFrameBatchSize;
        double mFrameBatchStartTimeSeconds;
        


        Color mBackgroundColor;


	};



GameSceneHandler *sceneHandler;
ScreenGL *screen;


// how many pixels wide is each game pixel drawn as?
int pixelZoomFactor;




typedef struct WebRequestRecord {
        int handle;
        WebRequest *request;
    } WebRequestRecord;

SimpleVector<WebRequestRecord> webRequestRecords;




typedef struct SocketConnectionRecord {
        int handle;
        Socket *sock;
    } SocketConnectionRecord;

SimpleVector<SocketConnectionRecord> socketConnectionRecords;




// function that destroys object when exit is called.
// exit is the only way to stop the loop in  ScreenGL
void cleanUpAtExit() {
    AppLog::info( "exiting...\n" );

    AppLog::info( "exiting: Deleting sceneHandler\n" );
    delete sceneHandler;


    if( soundRunning ) {
        AppLog::info( "exiting: calling SDL_CloseAudio\n" );
        SDL_CloseAudio();
        }
    soundRunning = false;

    if( frameDrawerInited ) {
        AppLog::info( "exiting: freeing frameDrawer\n" );
        freeFrameDrawer();
        }
    
    
    AppLog::info( "exiting: Deleting screen\n" );
    delete screen;


    AppLog::info( "exiting: freeing drawString\n" );
    freeDrawString();
    
#ifdef RASPBIAN
    raspbianReleaseSurface();
#endif

    AppLog::info( "exiting: calling SDL_Quit()\n" );
    SDL_Quit();

    if( screenShotPrefix != NULL ) {
        AppLog::info( "exiting: deleting screenShotPrefix\n" );

        delete [] screenShotPrefix;
        screenShotPrefix = NULL;
        }

    if( lastFrame_rgbaBytes != NULL ) {
        AppLog::info( "exiting: deleting lastFrame_rgbaBytes\n" ); 
        
        delete [] lastFrame_rgbaBytes;
        lastFrame_rgbaBytes = NULL;
        }


    if( recordAudio ) {
        AppLog::info( "exiting: closing audio output file\n" ); 

        recordAudio = false;
        fclose( aiffOutFile );
        aiffOutFile = NULL;
        }
    
    for( int i=0; i<webRequestRecords.size(); i++ ) {
        AppLog::infoF( "exiting: Deleting lingering web request %d\n", i ); 
                
        WebRequestRecord *r = webRequestRecords.getElement( i );
        
        delete r->request;
        }

    if( webProxy != NULL ) {
        delete [] webProxy;
        webProxy = NULL;
        }
    
    AppLog::info( "exiting: Done.\n" );
    }


static int nextSoundSpriteHandle;

typedef struct SoundSprite {
        int handle;
        int numSamples;
        int samplesPlayed;
        
        Sint16 *samples;
    } SoundSprite;



static SimpleVector<SoundSprite> soundSprites;

static SimpleVector<SoundSprite> playingSoundSprites;

static float soundLoudness = 1.0f;


void setSoundLoudness( float inLoudness ) {
    soundLoudness = inLoudness;
    }



int loadSoundSprite( const char *inAIFFFileName ) {
    File aiffFile( new Path( "sounds" ), inAIFFFileName );

    if( ! aiffFile.exists() ) {
        printf( "File does not exist in sounds folder: %s\n", inAIFFFileName );
        return -1;
        }
    
    char *fileName = aiffFile.getFullFileName();

    FILE *file = fopen( fileName, "rb" );
    
    delete [] fileName;

    if( file == NULL ) {
        printf( "Failed to open sound file for reading: %s\n", 
                inAIFFFileName );
        return -1;
        }
    
    // skip 20 bytes of header to get to num channels
    fseek( file, 20, SEEK_SET );
    
    unsigned char readBuffer[4];
    
    fread( readBuffer, 1, 2, file );
    
    if( readBuffer[0] != 0 || readBuffer[1] != 1 ) {
        printf( "Sound file not mono: %s\n", inAIFFFileName );
        fclose( file );
        return -1;
        }

    // num sample frames
    fread( readBuffer, 1, 4, file );

    SoundSprite s;
    
    s.handle = nextSoundSpriteHandle ++;
    s.numSamples = 
        readBuffer[0] << 24 |
        readBuffer[1] << 16 |
        readBuffer[2] << 8 |
        readBuffer[3];

    s.samplesPlayed = 0;


    // bits per sample
    fread( readBuffer, 1, 2, file );
    
    if( readBuffer[0] != 0 || readBuffer[1] != 16 ) {
        printf( "Sound file not 16-bit: %s\n", inAIFFFileName );
        fclose( file );
        return -1;
        }

    // 26 more bytes of header before samples
    fseek( file, 26, SEEK_CUR );

    unsigned char *rawSamples = new unsigned char[ 2 * s.numSamples ];

    int numRead = fread( rawSamples, 1, s.numSamples * 2, file );
    

    if( numRead != s.numSamples * 2 ) {
        printf( "Failed to read %d samples from file: %s\n", 
                s.numSamples, inAIFFFileName );
        
        delete [] rawSamples;
        fclose( file );
        return -1;
        }
    

    s.samples = new Sint16[ s.numSamples ];

    int r = 0;
    for( int i=0; i<s.numSamples; i++ ) {
        s.samples[i] = 
            ( rawSamples[r] << 8 ) |
            rawSamples[r+1];
        
        r += 2;
        }
    delete [] rawSamples;
    
    

    fclose( file );
    
    soundSprites.push_back( s );
    
    return s.handle;
    }



// plays sound sprite now
void playSoundSprite( int inHandle ) {
    for( int i=0; i<soundSprites.size(); i++ ) {
        SoundSprite *s = soundSprites.getElement( i );
        if( s->handle == inHandle ) {
            s->samplesPlayed = 0;
            
            lockAudio();
            playingSoundSprites.push_back( *s );
            unlockAudio();
            
            break;
            }
        }
    }



void freeSoundSprite( int inHandle ) {
    // make sure this sprite isn't playing
    lockAudio();
    
    for( int i=playingSoundSprites.size()-1; i>=0; i-- ) {
        SoundSprite *s = playingSoundSprites.getElement( i );
        if( s->handle == inHandle ) {
            // stop it abruptly
            playingSoundSprites.deleteElement( i );
            }
        }
    
    unlockAudio();


    for( int i=0; i<soundSprites.size(); i++ ) {
        SoundSprite *s = soundSprites.getElement( i );
        if( s->handle == inHandle ) {
            delete [] s->samples;
            soundSprites.deleteElement( i );
            }
        }
    }




void audioCallback( void *inUserData, Uint8 *inStream, int inLengthToFill ) {
    getSoundSamples( inStream, inLengthToFill );
    
    int numSamples = inLengthToFill / 4;


    if( playingSoundSprites.size() > 0 ) {
        
        for( int i=0; i<playingSoundSprites.size(); i++ ) {
            SoundSprite *s = playingSoundSprites.getElement( i );
            
            int filled = 0;
            
            int filledBytes = 0;
            
            int samplesPlayed = s->samplesPlayed;
            int spriteNumSamples = s->numSamples;
            
            while( filled < numSamples &&
                   samplesPlayed < spriteNumSamples  ) {
                
                Sint16 sample = s->samples[ samplesPlayed ];
                
                Sint16 lSample = 
                    (Sint16)( (inStream[filledBytes+1] << 8) | 
                             inStream[filledBytes] );
                Sint16 rSample = 
                    (Sint16)( (inStream[filledBytes+3] << 8) | 
                             inStream[filledBytes+2] );
                
                lSample += sample;
                rSample += sample;

                inStream[filledBytes++] = (Uint8)( lSample & 0xFF );
                inStream[filledBytes++] = (Uint8)( ( lSample >> 8 ) & 0xFF );
                inStream[filledBytes++] = (Uint8)( rSample & 0xFF );
                inStream[filledBytes++] = (Uint8)( ( rSample >> 8 ) & 0xFF );
        
                filled ++;
                samplesPlayed ++;
                }

            s->samplesPlayed = samplesPlayed;
            }

        // walk backward, removing any that are done
        for( int i=playingSoundSprites.size()-1; i>=0; i-- ) {
            SoundSprite *s = playingSoundSprites.getElement( i );
            if( s->samplesPlayed >= s->numSamples ) {
                playingSoundSprites.deleteElement( i );
                }
            }
        }

    if( soundLoudness != 1.0f ) {
        int nextByte = 0;
        for( int i=0; i<numSamples; i++ ) {
            Sint16 lSample = 
                inStream[nextByte] | 
                ( inStream[nextByte + 1] << 8 );
            
            Sint16 rSample = 
                inStream[nextByte + 2] | 
                ( inStream[nextByte + 3] << 8 );
            
            lSample = (Sint16)( lSample * soundLoudness );
            rSample = (Sint16)( rSample * soundLoudness );
            

            inStream[nextByte++] = (Uint8)( lSample & 0xFF );
            inStream[nextByte++] = (Uint8)( ( lSample >> 8 ) & 0xFF );
            inStream[nextByte++] = (Uint8)( rSample & 0xFF );
            inStream[nextByte++] = (Uint8)( ( rSample >> 8 ) & 0xFF );
            }
        }
    
    
    if( recordAudio ) {    
        
        
        if( numSamples > samplesLeftToRecord ) {
            numSamples = samplesLeftToRecord;
            }

        // reverse byte order
        int nextByte = 0;
        for( int i=0; i<numSamples; i++ ) {
                    
            fwrite( &( inStream[ nextByte + 1 ] ), 1, 1, aiffOutFile );
            fwrite( &( inStream[ nextByte ] ), 1, 1, aiffOutFile );
    
            fwrite( &( inStream[ nextByte + 3 ] ), 1, 1, aiffOutFile );
            fwrite( &( inStream[ nextByte + 2 ] ), 1, 1, aiffOutFile );
            nextByte += 4;
            }
        

        samplesLeftToRecord -= numSamples;
        
    
        if( samplesLeftToRecord <= 0 ) {
            recordAudio = false;
            fclose( aiffOutFile );
            aiffOutFile = NULL;
            }            
        }
    
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



char isSoundRunning() {
    return soundRunning;
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
    
        char *bundleName = autoSprintf( "%s.app", getAppName() );

        char *appNamePointer = strstr( appDirectoryPath, bundleName );

        if( appNamePointer != NULL ) {
            // terminate full app path to get parent directory
            appNamePointer[0] = '\0';
            
            chdir( appDirectoryPath );
            }
        
        delete [] bundleName;
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


    if( doesOverrideGameImageSize() ) {
        getGameImageSize( &gameWidth, &gameHeight );
        }
    
    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Target game image size:  %dx%d\n",
        gameWidth, gameHeight );


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


    if( screenWidth < gameWidth ) {
        AppLog::info( 
            "Screen width smaller than target game width, fixing" );
        screenWidth = gameWidth;
        }
    if( screenHeight < gameHeight ) {
        AppLog::info( 
            "Screen height smaller than target game height, fixing" );
        screenHeight = gameHeight;
        }
    


    char fullscreenFound = false;
    int readFullscreen = SettingsManager::getIntSetting( "fullscreen", 
                                                         &fullscreenFound );
    
    char fullscreen = true;
    
    if( fullscreenFound && readFullscreen == 0 ) {
        fullscreen = false;
        }


    char useLargestWindowFound = false;
    int readUseLargestWindow = 
        SettingsManager::getIntSetting( "useLargestWindow", 
                                        &useLargestWindowFound );
    
    char useLargestWindow = true;
    
    if( useLargestWindowFound && readUseLargestWindow == 0 ) {
        useLargestWindow = false;
        }


    if( !fullscreen && useLargestWindow ) {
        
        const SDL_VideoInfo* currentScreenInfo = SDL_GetVideoInfo();
        
        int currentW = currentScreenInfo->current_w;
        int currentH = currentScreenInfo->current_h;
        
        int blowUpFactor = 1;
        
        while( gameWidth * blowUpFactor < currentW &&
               gameHeight * blowUpFactor < currentH ) {
            
            blowUpFactor++;
            }

        while( blowUpFactor > 1 &&
               ( gameWidth * blowUpFactor >= currentW * 0.85 ||
                 gameHeight * blowUpFactor >= currentH * 0.85 ) ) {
            
            // scale back, because we don't want to totally
            // fill the screen (annoying to manage such a big window)
            
            // stop at a window that fills < 85% of screen in either direction
            blowUpFactor --;
            }
        
        screenWidth = blowUpFactor * gameWidth;
        screenHeight = blowUpFactor * gameHeight;
            

        AppLog::getLog()->logPrintf( 
            Log::INFO_LEVEL,
            "Screen dimensions for largest-window mode:  %dx%d\n",
            screenWidth, screenHeight );
        }
    



    char frameRateFound = false;
    int readFrameRate = SettingsManager::getIntSetting( "halfFrameRate", 
                                                         &frameRateFound );

    if( frameRateFound && readFrameRate >= 1 ) {
        // cut frame rate in half N times
        targetFrameRate /= (int)pow( 2, readFrameRate );
        }
    
    // can't draw less than 1 frame per second
    if( targetFrameRate < 1 ) {
        targetFrameRate = 1;
        }
    

    
    char recordFound = false;
    int readRecordFlag = SettingsManager::getIntSetting( "recordGame", 
                                                         &recordFound );
    
    char recordGame = false;
    
    if( recordFound && readRecordFlag == 1 ) {
        recordGame = true;
        }


    int speedControlKeysFlag = 
        SettingsManager::getIntSetting( "enableSpeedControlKeys", 0 );
    
    if( speedControlKeysFlag == 1 ) {
        enableSpeedControlKeys = true;
        }
    


    int outputAllFramesFlag = 
        SettingsManager::getIntSetting( "outputAllFrames", 0 );
    
    if( outputAllFramesFlag == 1 ) {
        outputAllFrames = true;
        // start with very first frame
        shouldTakeScreenshot = true;

        screenShotPrefix = stringDuplicate( "frame" );
        }

    int blendOutputFramePairsFlag = 
        SettingsManager::getIntSetting( "blendOutputFramePairs", 0 );
    
    if( blendOutputFramePairsFlag == 1 ) {
        blendOutputFramePairs = true;
        }

    blendOutputFrameFraction = 
        SettingsManager::getFloatSetting( "blendOutputFrameFraction", 0.0f );

    webProxy = SettingsManager::getStringSetting( "webProxy" );
    
    if( webProxy != NULL && 
        strcmp( webProxy, "" ) == 0 ) {
        
        delete [] webProxy;
        webProxy = NULL;
        }
    

    // make sure dir is writeable
    FILE *testFile = fopen( "testWrite.txt", "w" );
    
    if( testFile == NULL ) {
        writeFailed = true;
        }
    else {
        fclose( testFile );
        
        remove( "testWrite.txt" );
        
        writeFailed = false;
        }


    // don't try to record games if we can't write to dir
    // can cause a crash.
    if( writeFailed ) {
        recordGame = false;
        }
    





    char *customData = getCustomRecordedGameData();

    char *hashSalt = getHashSalt();

    screen =
        new ScreenGL( screenWidth, screenHeight, fullscreen, 
                      shouldNativeScreenResolutionBeUsed(),
                      targetFrameRate,
                      recordGame,
                      customData,
                      hashSalt,
                      getWindowTitle(), NULL, NULL, NULL );

    delete [] customData;
    delete [] hashSalt;
    

    // may change if specified resolution is not supported
    // or for event playback mode
    screenWidth = screen->getWidth();
    screenHeight = screen->getHeight();
    targetFrameRate = screen->getMaxFramerate();
    
    
    // adjust gameWidth to match available screen space
    // keep gameHeight constant


    /*
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY,
                         SDL_DEFAULT_REPEAT_INTERVAL );
    */
    
    // never cut off top/bottom of image, and always try to use largest
    // whole multiples of screen pixels per game pixel to fill
    // the screen vertically as well as we can
    pixelZoomFactor = screenHeight / gameHeight;
    
    if( pixelZoomFactor < 1 ) {
        pixelZoomFactor = 1;
        }
    
    // make sure game width fills the screen at this pixel zoom, even if game
    // height does not (letterbox on top/bottom, but never on left/rigtht)

    // closest number of whole pixels
    // may be *slight* black bars on left/right
    gameWidth = screenWidth / pixelZoomFactor;
    


    screen->setImageSize( pixelZoomFactor * gameWidth,
                          pixelZoomFactor * gameHeight );
    

    screen->allowSlowdownKeysDuringPlayback( enableSpeedControlKeys );

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
        
            TranslationManager::setLanguage( languageName, true );

            // augment translation by adding other languages listed
            // to fill in missing keys in top-line language
            
            for( int i=1; i<numTokens; i++ ) {
                languageName = *( tokens->getElement( i ) );
        
                TranslationManager::setLanguage( languageName, false );
                }
            }
        else {
            // default

            // TranslationManager already defaults to English, but
            // it looks for the language files at runtime before we have set
            // the current working directory.
            
            // Thus, we specify the default again here so that it looks
            // for its language files again.
            TranslationManager::setLanguage( "English", true );
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



        SDL_LockAudio();


        /* Open the audio device and start playing sound! */
        if( SDL_OpenAudio( &audioFormat, &actualFormat ) < 0 ) {
            AppLog::getLog()->logPrintf( 
                Log::ERROR_LEVEL,
                "Unable to open audio: %s\n", SDL_GetError() );
            soundRunning = false;
            }
        else {

            if( actualFormat.format != AUDIO_S16 ||
                actualFormat.channels != 2 ) {
                
                
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "Able to open audio, "
                    "but stereo S16 samples not availabl\n");
                
                SDL_CloseAudio();
                soundRunning = false;
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
                soundRunning = true;

                int recordAudioFlag = 
                    SettingsManager::getIntSetting( "recordAudio", 0 );
                int recordAudioLengthInSeconds = 
                    SettingsManager::getIntSetting( 
                        "recordAudioLengthInSeconds", 0 );

                if( recordAudioFlag == 1 && recordAudioLengthInSeconds > 0 ) {
                    recordAudio = true;
                    
                    samplesLeftToRecord = 
                        recordAudioLengthInSeconds * soundSampleRate;

                    aiffOutFile = fopen( "recordedAudio.aiff", "wb" );
        
                    int headerLength;
                    
                    unsigned char *header =
                        getAIFFHeader( 2, 16,
                                       soundSampleRate,
                                       samplesLeftToRecord, 
                                       &headerLength );

                    fwrite( header, 1, headerLength, aiffOutFile );

                    delete [] header;
                    }



                }
            }

        SDL_UnlockAudio();        
        }
    


    


    if( ! writeFailed ) {    
        demoMode = isDemoMode();
        }
    

    
    initDrawString( pixelZoomFactor * gameWidth, 
                    pixelZoomFactor * gameHeight );
        

    
    //glLineWidth( pixelZoomFactor );
    
    
    if( demoMode ) {    
        showDemoCodePanel( screen, getFontTGAFileName(), gameWidth,
                           gameHeight );
        
        // wait to start handling events
        // wait to start recording/playback
        }
    else if( writeFailed ) {
        showWriteFailedPanel( screen, getFontTGAFileName(), gameWidth,
                              gameHeight );
        // handle key events right away to listen for ESC
        screen->addKeyboardHandler( sceneHandler );
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
      mBlockQuitting( false ),
      mLastFrameRate( targetFrameRate ),
      mStartTimeSeconds( time( NULL ) ),
      mPrintFrameRate( true ),
      mNumFrames( 0 ), mFrameBatchSize( 100 ),
      mFrameBatchStartTimeSeconds( -1 ),
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

    if( writeFailed ) {
        freeWriteFailedPanel();
        }
    if( loadingFailedMessage != NULL ) {
        delete [] loadingFailedMessage;
        loadingFailedMessage = NULL;
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



void blockQuitting( char inNoQuitting ) {
    sceneHandler->mBlockQuitting = inNoQuitting;
    }



char isQuittingBlocked() {
    return sceneHandler->mBlockQuitting;
    }



void wakeUpPauseFrameRate() {
    sceneHandler->mPausedSleepTime = 0;
    }


// returns true if we're currently executing a recorded game
char isGamePlayingBack() {
    return screen->isPlayingBack();
    }







void mapKey( unsigned char inFromKey, unsigned char inToKey ) {
    screen->setKeyMapping( inFromKey, inToKey );
    }


void toggleKeyMapping( char inMappingOn ) {
    screen->toggleKeyMapping( inMappingOn );
    }





void setViewCenterPosition( float inX, float inY ) {
    viewCenterX = inX;
    viewCenterY = inY;
    redoDrawMatrix();
    }


    
doublePair getViewCenterPosition() {
    doublePair p = { viewCenterX, viewCenterY };
    
    return p;
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



static int lastMouseX = 0;
static int lastMouseY = 0;
static int lastMouseDownX = 0;
static int lastMouseDownY = 0;

static char mouseDown = false;
// start with last click expired
static int mouseDownSteps = 1000;


static char ignoreNextMouseEvent = false;
static int xCoordToIgnore, yCoordToIgnore;

void warpMouseToCenter( int *outNewMouseX, int *outNewMouseY ) {
    *outNewMouseX = screenWidth / 2;
    *outNewMouseY = screenHeight / 2;

    if( *outNewMouseX == lastMouseX && *outNewMouseY == lastMouseY ) {
        // mouse already there, no need to warp
        // (and warping when already there may or may not generate
        //  an event on some platforms, which causes trouble when we
        //  try to ignore the event)
        return;
        }

    if( SDL_GetAppState() & SDL_APPINPUTFOCUS ) {
        
        if( frameDrawerInited ) {
            // not ignoring mouse events currently due to demo code panel
            // or loading message... frame drawer not inited yet
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




    // do this here, because it involves game_getCurrentTime() calls
    // which are recorded, and aren't available for playback in fireRedraw
    mNumFrames ++;

    if( mPrintFrameRate ) {
        
        if( mFrameBatchStartTimeSeconds == -1 ) {
            mFrameBatchStartTimeSeconds = game_getCurrentTime();
            }

        if( mNumFrames % mFrameBatchSize == 0 ) {
            // finished a batch
            
            double timeDelta = 
                game_getCurrentTime() - mFrameBatchStartTimeSeconds;
            
            mLastFrameRate =
                (double)mFrameBatchSize / (double)timeDelta;
            
            //AppLog::getLog()->logPrintf( 
            //    Log::DETAIL_LEVEL,
            printf(
                "Frame rate = %f frames/second\n", mLastFrameRate );
            
            mFrameBatchStartTimeSeconds = game_getCurrentTime();
            }
        }

	

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
    else if( !loadingMessageShown ) {
        drawString( translate( "loading" ), true );

        loadingMessageShown = true;
        }
    else if( loadingFailedFlag ) {
        drawString( loadingFailedMessage, true );
        }
    else if( !writeFailed && !loadingFailedFlag && !frameDrawerInited ) {
        drawString( translate( "loading" ), true );

        initFrameDrawer( pixelZoomFactor * gameWidth, 
                         pixelZoomFactor * gameHeight, 
                         targetFrameRate,
                         screen->getCustomRecordedGameData(),
                         screen->isPlayingBack() );

        frameDrawerInited = true;
        
        // this is a good time, a while after launch, to do the post
        // update step
        postUpdate();
        }
    else if( !writeFailed && !loadingFailedFlag  ) {
        // demo mode done or was never enabled

        // carry on with game
        

        // auto-pause when minimized
        if( screen->isMinimized() ) {
            mPaused = true;
            }


        // don't update while paused
        char update = !mPaused;
        
        drawFrame( update );


        if( screen->isPlayingBack() && screen->shouldShowPlaybackDisplay() ) {

            char *progressString = autoSprintf( 
                "%s %.1f\n%s\n%s",
                translate( "playbackTag" ),
                screen->getPlaybackDoneFraction() * 100,
                translate( "playbackToggleMessage" ),
                translate( "playbackEndMessage" ) );
            
            drawString( progressString );

            delete [] progressString;
            
            }
        

        if( screen->isPlayingBack() && 
            screen->shouldShowPlaybackDisplay() &&
            showMouseDuringPlayback() ) {


            // draw mouse position info
            
            if( mouseDown ) {
                setDrawColor( 1, 0, 0, 0.5 );
                }
            else {
                setDrawColor( 1, 1, 1, 0.5 );
                }
            
            // step mouse click animation even after mouse released
            // (too hard to see it otherwise for fast clicks)
            mouseDownSteps ++;


            float sizeFactor = 5.0f;
            float clickSizeFactor = 5.0f;
            char showClick = false;
            float clickFade = 1.0f;
            
            int mouseClickDisplayDuration = 20 * 60 / targetFrameRate;

            if( mouseDownSteps < mouseClickDisplayDuration ) {
                
                float mouseClickProgress = 
                    mouseDownSteps / (float)mouseClickDisplayDuration;

                clickSizeFactor *= 5 * mouseClickProgress;
                showClick = true;

                clickFade *= 1.0f - mouseClickProgress;
                }
            
            
            // mouse coordinates in screen space
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            
            
            // viewport is square of largest dimension, centered on screen

            int bigDimension = screenWidth;
            
            if( screenHeight > bigDimension ) {
                bigDimension = screenHeight;
                }

            float excessX = ( bigDimension - screenWidth ) / 2;
            float excessY = ( bigDimension - screenHeight ) / 2;

            glOrtho( -excessX, -excessX + bigDimension, 
                     -excessY + bigDimension, -excessY, 
                     -1.0f, 1.0f );
            
            glMatrixMode(GL_MODELVIEW);


            double verts[8] = 
                {lastMouseX - sizeFactor, lastMouseY - sizeFactor,
                 lastMouseX - sizeFactor, lastMouseY + sizeFactor,
                 lastMouseX + sizeFactor, lastMouseY + sizeFactor,
                 lastMouseX + sizeFactor, lastMouseY - sizeFactor};

            drawQuads( 1, verts );


            double centerSize = 2;


            if( showClick ) {
                double clickVerts[8] = 
                    {lastMouseDownX - clickSizeFactor, 
                     lastMouseDownY - clickSizeFactor,
                     lastMouseDownX - clickSizeFactor, 
                     lastMouseDownY + clickSizeFactor,
                     lastMouseDownX + clickSizeFactor, 
                     lastMouseDownY + clickSizeFactor,
                     lastMouseDownX + clickSizeFactor, 
                     lastMouseDownY - clickSizeFactor};

                if( mouseDown ) {
                    setDrawColor( 1, 0, 0, clickFade );
                    }
                else {
                    setDrawColor( 0, 1, 0, clickFade );
                    }

                drawQuads( 1, clickVerts );

                // draw pin-point at center of click
                double clickCenterVerts[8] = 
                    {lastMouseDownX - centerSize, 
                     lastMouseDownY - centerSize,
                     lastMouseDownX - centerSize, 
                     lastMouseDownY + centerSize,
                     lastMouseDownX + centerSize, 
                     lastMouseDownY + centerSize,
                     lastMouseDownX + centerSize, 
                     lastMouseDownY - centerSize};
                
                drawQuads( 1, clickCenterVerts );
                }
            

            // finally, darker black center over whole thing
            double centerVerts[8] = 
                {lastMouseX - centerSize, lastMouseY - centerSize,
                 lastMouseX - centerSize, lastMouseY + centerSize,
                 lastMouseX + centerSize, lastMouseY + centerSize,
                 lastMouseX + centerSize, lastMouseY - centerSize};

            setDrawColor( 0, 0, 0, 0.5 );
            drawQuads( 1, centerVerts );
                
            }
        
        }

    if( shouldTakeScreenshot ) {
        takeScreenShot();

        if( !outputAllFrames ) {
            // just one
            shouldTakeScreenshot = false;
            }
        manualScreenShot = false;
        }

    frameNumber ++;
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
    
    lastMouseX = inX;
    lastMouseY = inY;
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

    lastMouseX = inX;
    lastMouseY = inY;
    }




void GameSceneHandler::mousePressed( int inX, int inY ) {
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerDown( x, y );

    lastMouseX = inX;
    lastMouseY = inY;

    mouseDown = true;
    mouseDownSteps = 0;
    lastMouseDownX = inX;
    lastMouseDownY = inY;
    }



void GameSceneHandler::mouseReleased( int inX, int inY ) {
    float x, y;
    screenToWorld( inX, inY, &x, &y );
    pointerUp( x, y );

    lastMouseX = inX;
    lastMouseY = inY;
    mouseDown = false;

    // start new animation for release
    mouseDownSteps = 0;
    lastMouseDownX = inX;
    lastMouseDownY = inY;
    }



void GameSceneHandler::fireRedraw() {

    
    if( mPaused ) {
        // ignore redraw event

        if( mPausedSleepTime > (unsigned int)( 5 * targetFrameRate ) ) {
            // user has touched nothing for 5 seconds
            
            // sleep to avoid wasting CPU cycles
            Thread::staticSleep( 500 );
            }
        
        mPausedSleepTime++;
        
        return;
        }


    }



static unsigned char lastKeyPressed = '\0';


void GameSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    if( writeFailed || loadingFailedFlag ) {
        exit( 0 );
        }
    

    // reset to become responsive while paused
    mPausedSleepTime = 0;
    

    if( mPaused && inKey == '%' && ! mBlockQuitting ) {
        // % to quit from pause
        exit( 0 );
        }
    

    if( inKey == 9 && isCommandKeyDown() &&
        screen->isPlayingBack() ) {
        
        printf( "Caught alt-tab during playback, pausing\n" );
        
        // alt-tab pressed during playback
        // but we aren't actually being minimized during playback
        // (because there's nothing to bring us back)
        // Still, force a pause, so that user's unpause action after
        // tabbing back in replays correctly
        mPaused = true;
        }
    

    if( enableSpeedControlKeys ) {
        
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
            // fast fast forward
            mScreen->setMaxFrameRate( targetFrameRate * 4 );
            }            
        if( inKey == ')' ) {
            // fast fast fast forward
            mScreen->setMaxFrameRate( targetFrameRate * 8 );
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



// need to track these separately from SDL_GetModState so that
// we replay isCommandKeyDown properly during recorded game playback
static char rCtrlDown = false;
static char lCtrlDown = false;

static char rAltDown = false;
static char lAltDown = false;

static char rMetaDown = false;
static char lMetaDown = false;



void GameSceneHandler::specialKeyPressed(
	int inKey, int inX, int inY ) {
    
    if( writeFailed || loadingFailedFlag ) {
        exit( 0 );
        }
    
    switch( inKey ) {
        case MG_KEY_RCTRL:
            rCtrlDown = true;
            break;
        case MG_KEY_LCTRL:
            lCtrlDown = true;
            break;
        case MG_KEY_RALT:
            rAltDown = true;
            break;
        case MG_KEY_LALT:
            lAltDown = true;
            break;
        case MG_KEY_RMETA:
            rMetaDown = true;
            break;
        case MG_KEY_LMETA:
            lMetaDown = true;
            break;
        }
    
    
    specialKeyDown( inKey );
	}



void GameSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {
    

    switch( inKey ) {
        case MG_KEY_RCTRL:
            rCtrlDown = false;
            break;
        case MG_KEY_LCTRL:
            lCtrlDown = false;
            break;
        case MG_KEY_RALT:
            rAltDown = false;
            break;
        case MG_KEY_LALT:
            lAltDown = false;
            break;
        case MG_KEY_RMETA:
            rMetaDown = false;
            break;
        case MG_KEY_LMETA:
            lMetaDown = false;
            break;
        }


    specialKeyUp( inKey );
    } 



char isCommandKeyDown() {
    SDLMod modState = SDL_GetModState();
    

    if( ( modState & KMOD_CTRL )
        ||
        ( modState & KMOD_ALT )
        ||
        ( modState & KMOD_META ) ) {
        
        return true;
        }
    
    if( screen->isPlayingBack() ) {
        // ignore these, saved internally, unless we're playing back
        // they can fall out of sync with keyboard reality as the user
        // alt-tabs between windows and release events are lost.
        if( rCtrlDown || lCtrlDown ||
            rAltDown || lAltDown ||
            rMetaDown || lMetaDown ) {
            return true;
            }
        }
    
    
    return false;
    }



char isLastMouseButtonRight() {
    return screen->isLastMouseButtonRight();
    }



void obscureRecordedNumericTyping( char inObscure, 
                                   char inCharToRecordInstead ) {
    
    screen->obscureRecordedNumericTyping( inObscure, inCharToRecordInstead );
    }




void GameSceneHandler::actionPerformed( GUIComponent *inTarget ) {
    }



static Image *readTGAFile( File *inFile ) {
    
    if( !inFile->exists() ) {
        char *fileName = inFile->getFullFileName();
        
        char *logString = autoSprintf( 
            "CRITICAL ERROR:  TGA file %s does not exist",
            fileName );
        delete [] fileName;
        
        AppLog::criticalError( logString );
        delete [] logString;
    
        return NULL;
        }    


    FileInputStream tgaStream( inFile );
    
    TGAImageConverter converter;
    
    Image *result = converter.deformatImage( &tgaStream );

    if( result == NULL ) {        
        char *fileName = inFile->getFullFileName();
        
        char *logString = autoSprintf( 
            "CRITICAL ERROR:  could not read TGA file %s, wrong format?",
            fileName );
        delete [] fileName;
        
        AppLog::criticalError( logString );
        delete [] logString;
        }
    
    return result;
    }



Image *readTGAFile( const char *inTGAFileName ) {

    File tgaFile( new Path( "graphics" ), inTGAFileName );
    
    return readTGAFile( &tgaFile );
    }



Image *readTGAFileBase( const char *inTGAFileName ) {

    File tgaFile( NULL, inTGAFileName );
    
    return readTGAFile( &tgaFile );
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



SpriteHandle loadSpriteBase( const char *inTGAFileName,
                             char inTransparentLowerLeftCorner ) {
    Image *result = readTGAFileBase( inTGAFileName );
    
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
    manualScreenShot = true;
    }




// if manualScreenShot false, then any blend settings (for saving blended 
// double-frames) are applied
// can return NULL in this case (when this frame should not be output
// according to blending settings)
//
// Region in screen pixels
static Image *getScreenRegionInternal( 
    int inStartX, int inStartY, int inWidth, int inHeight ) {    
        
    int numBytes = inWidth * inHeight * 3;
    
    unsigned char *rgbBytes = 
        new unsigned char[ numBytes ];

    // w and h might not be multiples of 4
    GLint oldAlignment;
    glGetIntegerv( GL_PACK_ALIGNMENT, &oldAlignment );
                
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    
    glReadPixels( inStartX, inStartY, inWidth, inHeight, 
                  GL_RGB, GL_UNSIGNED_BYTE, rgbBytes );
    
    glPixelStorei( GL_PACK_ALIGNMENT, oldAlignment );


    if( ! manualScreenShot && 
        blendOutputFramePairs && 
        frameNumber % 2 != 0 && 
        lastFrame_rgbaBytes != NULL ) {
        
        // save blended frames on odd frames
        if( blendOutputFrameFraction > 0 ) {
            float blendA = 1 - blendOutputFrameFraction;
            float blendB = blendOutputFrameFraction;
            
            for( int i=0; i<numBytes; i++ ) {
                rgbBytes[i] = 
                    (unsigned char)(
                        blendA * rgbBytes[i] + 
                        blendB * lastFrame_rgbaBytes[i] );
                }
            }
        
        }
    else if( !manualScreenShot &&
             blendOutputFramePairs &&
             frameNumber % 2 == 0 ) {
        
        // skip even frames, but save them for next blending
        
        if( lastFrame_rgbaBytes != NULL ) {
            delete [] lastFrame_rgbaBytes;
            lastFrame_rgbaBytes = NULL;
            }

        lastFrame_rgbaBytes = rgbBytes;
        
        return NULL;
        }
    

    Image *screenImage = new Image( inWidth, inHeight, 3, false );

    double *channelOne = screenImage->getChannel( 0 );
    double *channelTwo = screenImage->getChannel( 1 );
    double *channelThree = screenImage->getChannel( 2 );
        
    // image of screen is upside down
    int outputRow = 0;
    for( int y=inHeight - 1; y>=0; y-- ) {
        for( int x=0; x<inWidth; x++ ) {
                        
            int outputPixelIndex = outputRow * inWidth + x;
            
            
            int regionPixelIndex = y * inWidth + x;
            int byteIndex = regionPixelIndex * 3;
                
            // optimization found:  should unroll this loop over 3 channels
            // divide by 255, with a multiply
            channelOne[outputPixelIndex] = 
                rgbBytes[ byteIndex++ ] * 0.003921569;
            channelTwo[outputPixelIndex] = 
                rgbBytes[ byteIndex++ ] * 0.003921569;
            channelThree[outputPixelIndex] = 
                rgbBytes[ byteIndex++ ] * 0.003921569;
            }
        outputRow++;
        }
    
    delete [] rgbBytes;

    return screenImage;
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

            char *formatString = autoSprintf( "%s%%d.%s", screenShotPrefix,
                                              screenShotExtension );

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
    
    char *fileName = autoSprintf( "%s%05d.%s", 
                                  screenShotPrefix, nextShotNumber,
                                  screenShotExtension );

    

    File *file = shotDir.getChildFile( fileName );
    
    delete [] fileName;

    
    

    Image *screenImage = 
        getScreenRegionInternal( 0, 0, screenWidth, screenHeight );

    if( screenImage == NULL ) {
        // a skipped frame due to blending settings
        delete file;
        
        return;
        }
    


    FileOutputStream tgaStream( file );
    
    screenShotConverter.formatImage( screenImage, &tgaStream );

    delete screenImage;
    
    delete file;

    nextShotNumber++;
    }




Image *getScreenRegion( double inX, double inY, 
                        double inWidth, double inHeight ) {
    
    double endX = inX + inWidth;
    double endY = inY + inHeight;
    
    // rectangle specified in integer screen coordinates

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
 
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    GLdouble winStartX, winStartY, winStartZ;
    GLdouble winEndX, winEndY, winEndZ;

    gluProject( inX, inY, 0, 
                modelview, projection, viewport, 
                &winStartX, &winStartY, &winStartZ );

    gluProject( endX, endY, 0, 
                modelview, projection, viewport, 
                &winEndX, &winEndY, &winEndZ );





    manualScreenShot = true;

    
    Image *result = 
            getScreenRegionInternal( 
                lrint( winStartX ), lrint( winStartY ), 
                lrint( winEndX - winStartX ), lrint( winEndY - winStartY ) );

    manualScreenShot = false;
    
    return result;
    }






int nextWebRequestHandle = 0;




int startWebRequest( const char *inMethod, const char *inURL,
                     const char *inBody ) {
    
    WebRequestRecord r;
    
    r.handle = nextWebRequestHandle;
    nextWebRequestHandle ++;
    

    if( screen->isPlayingBack() ) {
        // stop here, don't actually start a real web request
        return r.handle;
        }


    r.request = new WebRequest( inMethod, inURL, inBody, webProxy );
    
    webRequestRecords.push_back( r );
    
    return r.handle;
    }



static WebRequest *getRequestByHandle( int inHandle ) {
    for( int i=0; i<webRequestRecords.size(); i++ ) {
        WebRequestRecord *r = webRequestRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            return r->request;
            }
        }

    // else not found?
    AppLog::error( "gameSDL - getRequestByHandle:  "
                   "Requested WebRequest handle not found\n" );
    return NULL;
    }



int stepWebRequest( int inHandle ) {
    
    if( screen->isPlayingBack() ) {
        // don't step request, because we're only simulating the response
        // of the server
        
        int nextType = screen->getWebEventType( inHandle );
        
        if( nextType == 2 ) {
            return 1;
            }
        else {        
            return nextType;
            }
        }
    

    WebRequest *r = getRequestByHandle( inHandle );
    
    if( r != NULL ) {
        
        int stepResult = r->step();
        
        screen->registerWebEvent( inHandle, stepResult );
        
        return stepResult;
        }
    
    return -1;
    }



// gets the response body as a \0-terminated string, destroyed by caller
char *getWebResult( int inHandle ) {
    if( screen->isPlayingBack() ) {
        // return a recorded server result
        
        int nextType = screen->getWebEventType( inHandle );
        
        if( nextType == 2 ) {
            return screen->getWebEventResultBody( inHandle );
            }
        else {        
            AppLog::error( "Expecting a web result body in playback file, "
                           "but found none." );
            
            return NULL;
            }
        }



    WebRequest *r = getRequestByHandle( inHandle );
    
    if( r != NULL ) {
        char *result = r->getResult();

        if( result != NULL ) {    
            screen->registerWebEvent( inHandle,
                                      // the type for "result" is 2
                                      2,
                                      result );
            }
        
        return result;
        }
    
    return NULL;
    }




unsigned char *getWebResult( int inHandle, int *outSize ) {
    if( screen->isPlayingBack() ) {
        // return a recorded server result
        
        int nextType = screen->getWebEventType( inHandle );
        
        if( nextType == 2 ) {
            return (unsigned char *)
                screen->getWebEventResultBody( inHandle, outSize );
            }
        else {        
            AppLog::error( "Expecting a web result body in playback file, "
                           "but found none." );
            
            return NULL;
            }
        }



    WebRequest *r = getRequestByHandle( inHandle );
    
    if( r != NULL ) {
        unsigned char *result = r->getResult( outSize );

        if( result != NULL ) {    
            screen->registerWebEvent( inHandle,
                                      // the type for "result" is 2
                                      2,
                                      (char*)result,
                                      *outSize );
            }
        
        return result;
        }
    
    return NULL;
    }



int getWebProgressSize( int inHandle ) {
    if( screen->isPlayingBack() ) {
        // return a recorded server result
        
        int nextType = screen->getWebEventType( inHandle );
        
        if( nextType > 2 ) {
            return nextType;
            }
        else {        
            AppLog::error( 
                "Expecting a web result progress event in playback file, "
                "but found none." );
            
            return 0;
            }
        }
    


    WebRequest *r = getRequestByHandle( inHandle );
    
    if( r != NULL ) {
        int progress = r->getProgressSize();
        if( progress > 2 ) {    
            screen->registerWebEvent( inHandle,
                                      // the type for "progress" is 
                                      // the actual size
                                      progress );
            return progress;
            }
        else {
            // progress of 2 or less is returned as 0, to keep consistency
            // for recording and playback
            return 0;
            }
        }
    
    return 0;
    }

    


// frees resources associated with a web request
// if request is not complete, this cancels it
// if hostname lookup is not complete, this call might block.
void clearWebRequest( int inHandle ) {
    
    if( screen->isPlayingBack() ) {
        // not a real request, do nothing
        return;
        }
        

    for( int i=0; i<webRequestRecords.size(); i++ ) {
        WebRequestRecord *r = webRequestRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            delete r->request;
            
            webRequestRecords.deleteElement( i );
            
            // found, done
            return;
            }
        }

    // else not found?
    AppLog::error( "gameSDL - clearWebRequest:  "
                   "Requested WebRequest handle not found\n" );
    }





int nextSocketConnectionHandle = 0;



int openSocketConnection( const char *inNumericalAddress, int inPort ) {
    SocketConnectionRecord r;
    
    r.handle = nextSocketConnectionHandle;
    nextSocketConnectionHandle++;


    if( screen->isPlayingBack() ) {
        // stop here, don't actually open a real socket
        return r.handle;
        }

    HostAddress address( stringDuplicate( inNumericalAddress ), inPort );


    char timedOut;
    
    // non-blocking connet
    r.sock = SocketClient::connectToServer( &address, 0, &timedOut );
    
    if( r.sock != NULL ) {
        socketConnectionRecords.push_back( r );
        
        return r.handle;
        }
    else {
        return -1;
        }
    }



static Socket *getSocketByHandle( int inHandle ) {
    for( int i=0; i<socketConnectionRecords.size(); i++ ) {
        SocketConnectionRecord *r = socketConnectionRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            return r->sock;
            }
        }

    // else not found?
    AppLog::error( "gameSDL - getSocketByHandle:  "
                   "Requested Socket handle not found\n" );
    return NULL;
    }

    



// non-blocking send
// returns number sent (maybe 0) on success, -1 on error
int sendToSocket( int inHandle, unsigned char *inData, int inDataLength ) {
    if( screen->isPlayingBack() ) {
        // play back result of this send

        int nextType, nextNumBodyBytes;
        screen->getSocketEventTypeAndSize( inHandle, 
                                           &nextType, &nextNumBodyBytes );
        
        if( nextType == 0 ) {
            return nextNumBodyBytes;
            }
        else {        
            return -1;
            }
        }
    

    Socket *sock = getSocketByHandle( inHandle );
    
    if( sock != NULL ) {
        
        int numSent = 0;
        

        if( sock->isConnected() ) {
            
            numSent = sock->send( inData, inDataLength, false, false );
            
            if( numSent == -2 ) {
                // would block
                numSent = 0;
                }
            }
        
        int type = 0;
        
        if( numSent == -1 ) {
            type = 1;
            numSent = 0;
            }
        
        screen->registerSocketEvent( inHandle, type, numSent, NULL );
        
        return numSent;
        }
    
    return -1;
    }



// non-blocking read
// returns number of bytes read (maybe 0), -1 on error 
int readFromSocket( int inHandle, 
                    unsigned char *inDataBuffer, int inBytesToRead ) {
    
    if( screen->isPlayingBack() ) {
        // play back result of this read

        int nextType, nextNumBodyBytes;
        screen->getSocketEventTypeAndSize( inHandle, 
                                           &nextType, &nextNumBodyBytes );
        
        if( nextType == 2 ) {
            
            if( nextNumBodyBytes == 0 ) {
                return 0;
                }
            // else there are body bytes waiting

            if( nextNumBodyBytes > inBytesToRead ) {
                AppLog::errorF( "gameSDL - readFromSocket:  "
                                "Expecting to read at most %d bytes, but "
                                "recording has %d bytes waiting\n",
                                inBytesToRead, nextNumBodyBytes );
                return -1;
                }
            
            unsigned char *bodyBytes = 
                screen->getSocketEventBodyBytes( inHandle );
            
            memcpy( inDataBuffer, bodyBytes, nextNumBodyBytes );
            delete [] bodyBytes;
            
            
            return nextNumBodyBytes;
            }
        else {        
            return -1;
            }
        }
    

    Socket *sock = getSocketByHandle( inHandle );
    
    if( sock != NULL ) {
        
        int numRead = 0;

        if( sock->isConnected() ) {
            
            numRead = sock->receive( inDataBuffer, inBytesToRead, 0 );
            
            if( numRead == -2 ) {
                // would block
                numRead = 0;
                }
            }
        
        int type = 2;
        if( numRead == -1 ) {
            type = 3;
            }
        
        unsigned char *bodyBytes = NULL;
        if( numRead > 0 ) {
            bodyBytes = inDataBuffer;
	    }
        
        screen->registerSocketEvent( inHandle, type, numRead, bodyBytes );
        
        return numRead;
        }
    
    return -1;
    }



void closeSocket( int inHandle ) {
    
    if( screen->isPlayingBack() ) {
        // not a real socket, do nothing
        return;
        }
    
    for( int i=0; i<socketConnectionRecords.size(); i++ ) {
        SocketConnectionRecord *r = socketConnectionRecords.getElement( i );
        
        if( r->handle == inHandle ) {
            delete r->sock;
            
            socketConnectionRecords.deleteElement( i );
            
            // found, done
            return;
            }
        }

    // else not found?
    AppLog::error( "gameSDL - closeSocket:  "
                   "Requested Socket handle not found\n" );
    }





time_t game_time( time_t *__timer ) {
    return screen->getTime( __timer );
    }



double game_getCurrentTime() {
    return screen->getCurrentTime();
    }


double getRecentFrameRate() {
    return sceneHandler->mLastFrameRate;
    }



void loadingFailed( const char *inFailureMessage ) {
    loadingFailedFlag = true;

    if( loadingFailedMessage != NULL ) {
        delete [] loadingFailedMessage;
        }
    loadingFailedMessage = stringDuplicate( inFailureMessage );
    }




// platform-specific clipboard code




char isClipboardSupported() {
#ifdef LINUX
    return true;
#elif defined(__mac__)
    return true;
#elif defined(WIN_32)
    return true;
#else
    return false;
#endif
    }







#ifdef LINUX
// X windows clipboard tutorial found here
// http://michael.toren.net/mirrors/doc/X-copy+paste.txt

// X11 has it's own Time type
// avoid conflicts with our Time class from above by replacing the word
// (Trick found here:  http://stackoverflow.com/questions/8865744 )
#define Time X11_Time

#include <X11/Xlib.h> 
#include <X11/Xatom.h>


char *getClipboardText() {
    char *fromClipboard = NULL;
    

    Display *display = XOpenDisplay( NULL );
	
    Atom XA_CLIPBOARD = XInternAtom( display, "CLIPBOARD", 0 );

	
    Window selectionOwner = XGetSelectionOwner( display, XA_CLIPBOARD );
    
    if( selectionOwner != None ) {
        Atom ourSelection = XInternAtom( display, "OUR_SELECTION", false );

        XConvertSelection( display, XA_CLIPBOARD, XA_STRING, ourSelection,
                           selectionOwner, CurrentTime );

        Atom type;
        int format;
        unsigned long length, bytesLeft, dummy;
        unsigned char *data;
        
        // check how long the data is
        XGetWindowProperty( display, selectionOwner, 
                            ourSelection,
                            0, 0,	  	  // offset, len
                            0, 	 	  // Delete 0==FALSE
                            XA_STRING,  // type
                            &type,		  // return type
                            &format,	  // return format
                            &length, &bytesLeft,  //that 
                            &data );				

        if( bytesLeft > 0 ) {
            int result = XGetWindowProperty( display, selectionOwner, 
                                             ourSelection, 
                                             0, bytesLeft, 
                                             0,
                                             XA_STRING, 
                                             &type, 
                                             &format,
                                             &length, &dummy, 
                                             &data );
            if( result == Success ) {
                fromClipboard = stringDuplicate( (char*)data );
                XFree (data);
                }
            }
        }
    
    XCloseDisplay( display );

    if( fromClipboard != NULL ) {
        return fromClipboard;
        }
    else { 
        return stringDuplicate( "" );
        }
    }


void setClipboardText( const char *inText  ) {
    // x copy paste is a MESS
    // after claiming ownership of the clipboard, application needs
    // to listen to x events forever to handle any consumers of the clipboard
    // data.  Yuck!

    // farm this out to xclip with -silent flag
    // it forks its own process and keeps it live as long as the clipboard
    // data is still needed (kills itself when the clipboard is claimed
    // by someone else with new data)
    
    FILE* pipe = popen( "xclip -silent -selection clipboard -i", "w");
    if( pipe == NULL ) {
        return;
        }
    fputs( inText, pipe );
    
    pclose( pipe );
    }



#elif defined(__mac__)

// pbpaste command line trick found here:
// http://www.alecjacobson.com/weblog/?p=2376

char *getClipboardText() {
    FILE* pipe = popen( "pbpaste", "r");
    if( pipe == NULL ) {
        return stringDuplicate( "" );
        }
    
    char buffer[ 128 ];
    
    char *result = stringDuplicate( "" );
    
    // read until pipe closed
    while( ! feof( pipe ) ) {
        if( fgets( buffer, 128, pipe ) != NULL ) {            
            char *newResult = concatonate( result, buffer );
            delete [] result;
            result = newResult;
            }
        }
    pclose( pipe );

    
    return result;
    }



void setClipboardText( const char *inText  ) {
    FILE* pipe = popen( "pbcopy", "w");
    if( pipe == NULL ) {
        return;
        }
    fputs( inText, pipe );
    
    pclose( pipe );
    }



#elif defined(WIN_32)

// simple windows clipboard solution found here:
// https://www.allegro.cc/forums/thread/606034

#include <windows.h>

char *getClipboardText() {
    char *fromClipboard = NULL;
    if( OpenClipboard( NULL ) ) {
        HANDLE hData = GetClipboardData( CF_TEXT );
        char *buffer = (char*)GlobalLock( hData );
        if( buffer != NULL ) {
            fromClipboard = stringDuplicate( buffer );
            }
        GlobalUnlock( hData );
        CloseClipboard();
        }

    if( fromClipboard == NULL ) {
        fromClipboard = stringDuplicate( "" );
        }

    return fromClipboard;
    }


void setClipboardText( const char *inText  ) {
    if (OpenClipboard(NULL)) {

        EmptyClipboard();
        HGLOBAL clipBuffer = GlobalAlloc( GMEM_DDESHARE, strlen(inText) + 1 );
        char *buffer = (char*)GlobalLock( clipBuffer );
        
        strcpy( buffer, inText );
        GlobalUnlock( clipBuffer );
        SetClipboardData( CF_TEXT, clipBuffer );
        CloseClipboard();
        }
    }



#else
// unsupported platform
char *getClipboardText() {
    return stringDuplicate( "" );
    }
#endif





#define macLaunchExtension ".app"
#define winLaunchExtension ".exe"


#ifdef LINUX

#include <unistd.h>
#include <stdarg.h>

char relaunchGame() {
    char *launchTarget = 
        autoSprintf( "./%s", getLinuxAppName() );

    AppLog::infoF( "Relaunching game %s", launchTarget );

    int forkValue = fork();

    if( forkValue == 0 ) {
        // we're in child process, so exec command
        char *arguments[2] = { launchTarget, NULL };
        
        execvp( launchTarget, arguments );

        // we'll never return from this call
                
        // small memory leak here, but okay
        delete [] launchTarget;
        }
    
    delete [] launchTarget;
    printf( "Returning from relaunching game, exiting this process\n" );
    exit( 0 );
    return true;
    }
    

#elif defined(__mac__)

#include <unistd.h>
#include <stdarg.h>

char relaunchGame() {
    char *launchTarget = 
        autoSprintf( "%s%s", getAppName(), macLaunchExtension );
    
    AppLog::infoF( "Relaunching game %s", launchTarget );
    
    int forkValue = fork();

    if( forkValue == 0 ) {
        // we're in child process, so exec command
        char *arguments[4] = { (char*)"open", (char*)"-n",
                               launchTarget, NULL };        

        execvp( "open", arguments );
        // we'll never return from this call
        
        // small memory leak here, but okay
        delete [] launchTarget;
        }
    
    delete [] launchTarget;
    
    printf( "Returning from relaunching game, exiting this process\n" );
    exit( 0 );
    return true;
    }


#elif defined(WIN_32)

#include <windows.h>
#include <process.h>

char relaunchGame() {
    char *launchTarget = 
        autoSprintf( "%s%s", getAppName(), winLaunchExtension );
    
    AppLog::infoF( "Relaunching game %s", launchTarget );

    char *arguments[2] = { (char*)launchTarget, NULL };
    
    _spawnvp( _P_NOWAIT, launchTarget, arguments );

    delete [] launchTarget;
    
    printf( "Returning from relaunching game, exiting this process\n" );
    exit( 0 );
    return true;
    }

#else
// unsupported platform
char relaunchGame() {
    return false;
    }
#endif




void quitGame() {
    exit( 0 );
    }
