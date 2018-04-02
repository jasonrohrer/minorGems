/*
 * Modification History
 *
 * 2008-October-27		Jason Rohrer
 * Created.  Copied structure from GLUT version (ScreenGL.cpp). 
 *
 * 2009-March-19		Jason Rohrer
 * UNICODE support for keyboard input of symbols. 
 *
 * 2010-March-17	Jason Rohrer
 * Support for Ctrl, Alt, and Meta.
 *
 * 2010-April-6 	Jason Rohrer
 * Blocked event passing to handlers that were added by event.
 *
 * 2010-April-8 	Jason Rohrer
 * Added post-redraw events.
 * Don't treat wheel events as mouse clicks.
 *
 * 2010-April-9 	Jason Rohrer
 * Checks for closest matching resolution.
 *
 * 2010-April-20   	Jason Rohrer
 * Alt-Enter to leave fullscreen mode.
 * Reload all SingleTextures after GL context change.
 *
 * 2010-May-7   	Jason Rohrer
 * Mapped window close button to ESC key.
 * Extended ASCII.
 *
 * 2010-May-12   	Jason Rohrer
 * Use full 8 lower bits of unicode to support extended ASCII.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2010-May-19    Jason Rohrer
 * Added support for 1-bit stencil buffer.
 *
 * 2010-May-21    Jason Rohrer
 * Mapped ctrl-q and alt-q to ESC key.
 *
 * 2010-August-26    Jason Rohrer
 * Fixed a parens warning.
 *
 * 2010-September-6   Jason Rohrer
 * Split display callback into two parts to handle events after frame sleep.
 *
 * 2010-September-8   Jason Rohrer
 * Fixed bug in ASCII key release event.
 * Swap control to eliminate tearing.
 *
 * 2010-September-9   Jason Rohrer
 * Moved frame rate limit into ScreenGL class.
 *
 * 2010-November-2 	Jason Rohrer
 * Support for eating key events.
 *
 * 2010-November-3 	Jason Rohrer
 * Fixed frame timing on platforms where sleeps can be shorter than requested.
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
 * 2010-December-30   Jason Rohrer
 * Fixed to ignore hidden files in playbackGame directory.
 *
 * 2011-January-3   Jason Rohrer
 * Fast-forward key support.
 * Added custom data to recorded game files.  
 * Fixed bug in hidden file skipping.
 * Support for detecting playback mode.
 *
 * 2011-January-25   Jason Rohrer
 * Now respects current screen's aspect ratio when picking a resolution.
 *
 * 2011-January-26   Jason Rohrer
 * Switched to integer representation for aspect ratio comparisons.
 *
 * 2011-January-31   Jason Rohrer
 * Fixed bugs in aspect ration calculations.
 *
 * 2011-February-3   Jason Rohrer
 * Now always picks resolution at least as big as what is requested.
 * 
 * 2011-February-6   Jason Rohrer
 * Fixed to stop playback when end of recorded event file is reached.
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
 * Windowed mode forced if fullscreen dimensions specified in playback file 
 * not available.
 * Got Alt-Tab working for windowed mode too.
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
 * 2013-November-3   Jason Rohrer
 * Fixed bug in frame timing (negative frame time measurements) if system time
 * changes while we're running.
 *
 * 2013-November-21   Jason Rohrer
 * Added work-around for improper window de-minimization detection on some
 * platforms so that isMinimized will always report true minimization state
 * (thanks Joshua Collins).
 *
 * 2014-January-3   Jason Rohrer
 * Added recording and playback of socket events.
 *
 * 2014-November-25   Jason Rohrer
 * Added support for obscuring sensitive typing in recorded event file.
 */


#include "ScreenGL.h" 
#include "SingleTextureGL.h" 

#include "minorGems/graphics/openGL/glInclude.h"

#include <SDL/SDL.h>


#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>


#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/io/file/File.h"

#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"

#include "minorGems/crypto/hashes/sha1.h"
#include "minorGems/formats/encodingUtils.h"

#ifdef __mac__
#include "minorGems/game/platforms/SDL/mac/SDLMain_Ext.h"
#endif

#ifdef RASPBIAN

#include "RaspbianGLSurface.cpp"

#endif



/* ScreenGL to be accessed by callback functions.
 *
 * Note that this is a bit of a hack, but the callbacks
 * require a C-function (not a C++ member) and have fixed signatures,
 * so there's no way to pass the current screen into the functions.
 *
 * This hack prevents multiple instances of the ScreenGL class from
 * being used simultaneously.
 *
 * Even worse for SLD, because this hack is carried over from GLUT.
 * SDL doesn't even require C callbacks (you provide the event loop).
 */
ScreenGL *currentScreenGL;



// maps SDL-specific special (non-ASCII) key-codes (SDLK) to minorGems key 
// codes (MG_KEY)
int mapSDLSpecialKeyToMG( int inSDLKey );

// for ascii key
char mapSDLKeyToASCII( int inSDLKey );


static unsigned char keyMap[256];
static char keyMapOn = true;


// prototypes
/*
void callbackResize( int inW, int inH );
void callbackKeyboard( unsigned char  inKey, int inX, int inY );
void callbackMotion( int inX, int inY );
void callbackPassiveMotion( int inX, int inY );
void callbackMouse( int inButton, int inState, int inX, int inY );
void callbackPreDisplay();
void callbackDisplay();
void callbackIdle();
*/

ScreenGL::ScreenGL( int inWide, int inHigh, char inFullScreen,
                    char inDoNotChangeNativeResolution,
                    unsigned int inMaxFrameRate,
                    char inRecordEvents,
                    const char *inCustomRecordedGameData,
                    const char *inHashSalt,
                    const char *inWindowName,
					KeyboardHandlerGL *inKeyHandler,
					MouseHandlerGL *inMouseHandler,
					SceneHandlerGL *inSceneHandler  ) 
	: mWide( inWide ), mHigh( inHigh ),
      mForceAspectRatio( false ),
      mForceSpecifiedDimensions( false ),
      mDoNotChangeNativeResolution( inDoNotChangeNativeResolution ),
      mImageWide( inWide ), mImageHigh( inHigh ),
      mFullScreen( inFullScreen ),
      mMaxFrameRate( inMaxFrameRate ),
      mUseFrameSleep( true ),
      mFullFrameRate( inMaxFrameRate ),
      m2DMode( false ),
	  mViewPosition( new Vector3D( 0, 0, 0 ) ),
	  mViewOrientation( new Angle3D( 0, 0, 0 ) ),
	  mMouseHandlerVector( new SimpleVector<MouseHandlerGL*>() ),
	  mKeyboardHandlerVector( new SimpleVector<KeyboardHandlerGL*>() ),
	  mSceneHandlerVector( new SimpleVector<SceneHandlerGL*>() ),
	  mRedrawListenerVector( new SimpleVector<RedrawListenerGL*>() ) {

    mWantToMimimize = false;
    mMinimized = false;
    mWasFullScreenBeforeMinimize = false;
    
    mCustomRecordedGameData = stringDuplicate( inCustomRecordedGameData );
    
    mHashSalt = stringDuplicate( inHashSalt );
    

    mLastReadWebEventHandle = -1;
    mCurrentWebEventHandle = 0;
    mNextUnusedWebEventHandle = 0;
    
    mLastAsyncFileHandleDone = -1;
    
    mLastMinimizedStatus = false;

    mAllowSlowdownKeysDuringPlayback = false;

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

    
    mRandSeed = (unsigned int)fmod( Time::timeSec(), UINT_MAX );
    
    mLastTimeValue = Time::timeSec();
    mLastRecordedTimeValue = 0;

    mLastCurrentTimeValue = Time::getCurrentTime();
    mLastRecordedCurrentTimeValue = 0;

    mLastActualFrameRate = inMaxFrameRate;

    mTimeValuePlayedBack = false;
    mFramesSinceLastTimeTick = 0;

    mShouldShowPlaybackDisplay = true;

    int hidePlaybackDisplayFlag = 
        SettingsManager::getIntSetting( "hidePlaybackDisplay", 0 );
    
    if( hidePlaybackDisplayFlag == 1 ) {
        mShouldShowPlaybackDisplay = false;
        }


    mRecordingOrPlaybackStarted = false;
    
    mRecordingEvents = inRecordEvents;
    mPlaybackEvents = false;
    mEventFile = NULL;
    mEventFileNumBatches = 0;
    mNumBatchesPlayed = 0;
    
    mObscureRecordedNumericTyping = false;

    // playback overrides recording, check for it first
    // do this before setting up surface
    
    File playbackDir( NULL, "playbackGame" );
    
    if( !playbackDir.exists() ) {
        playbackDir.makeDirectory();
        }
    
    int numChildren;
    File **childFiles = playbackDir.getChildFiles( &numChildren );

    if( numChildren > 0 ) {

        // take first
        char *fullFileName = childFiles[0]->getFullFileName();
        char *partialFileName = childFiles[0]->getFileName();
        
        // skip hidden files
        int i = 0;
        while( partialFileName != NULL &&
               partialFileName[i] == '.' ) {

            delete [] fullFileName;
            fullFileName = NULL;

            delete [] partialFileName;
            partialFileName = NULL;

            i++;
            if( i < numChildren ) {
                fullFileName = childFiles[i]->getFullFileName();
                partialFileName = childFiles[i]->getFileName();
                }
            }


        if( fullFileName != NULL ) {
            delete [] partialFileName;
            
            

            mEventFile = fopen( fullFileName, "r" );

            if( mEventFile == NULL ) {
                AppLog::error( "Failed to open event playback file" );
                }
            else {

                // count number of newlines in file (close to the number
                // of batches in the file)
                char *fileContents = childFiles[i]->readFileContents();
                
                int fileLength = strlen( fileContents );
                
                for( int j=0; j<fileLength; j++ ) {
                    if( fileContents[j] == '\n' ) {
                        mEventFileNumBatches ++;
                        }
                    }
                delete [] fileContents;
                

                AppLog::getLog()->logPrintf( 
                    Log::INFO_LEVEL,
                    "Playing back game from file %s", fullFileName );
            

                // first, determine max possible length of custom data
                int maxCustomLength = 0;
                
                int readChar = fgetc( mEventFile );
                
                while( readChar != EOF && readChar != '\n' ) {
                    maxCustomLength++;
                    readChar = fgetc( mEventFile );
                    }
                
                // back to start
                rewind( mEventFile );
                
                char *readCustomGameData = new char[ maxCustomLength ];

                char hashString[41];

                
                
                int fullScreenFlag;
                unsigned int readRandSeed;
                unsigned int readMaxFrameRate;
                int readWide;
                int readHigh;
                
                int numScanned =
                    fscanf( 
                        mEventFile, 
                        "%u seed, %u fps, %dx%d, fullScreen=%d, %s %40s\n",
                        &readRandSeed,
                        &readMaxFrameRate,
                        &readWide, &readHigh, &fullScreenFlag, 
                        readCustomGameData,
                        hashString );
                
                if( numScanned == 7 ) {

                    char *stringToHash = autoSprintf( "%s%s",
                                                      readCustomGameData,
                                                      mHashSalt );

                    char *correctHash = computeSHA1Digest( stringToHash );

                    delete [] stringToHash;
                    
                    int difference = strcmp( correctHash, hashString );
                    
                    delete [] correctHash;

                    if( difference == 0 ) {

                        mRecordingEvents = false;
                        mPlaybackEvents = true;
                        
                        mRandSeed = readRandSeed;
                        mMaxFrameRate = readMaxFrameRate;
                        mWide = readWide;
                        mHigh = readHigh,
                        
                        mFullFrameRate = mMaxFrameRate;
                    
                        mImageWide = mWide;
                        mImageHigh = mHigh;
                        
                        AppLog::info( 
                          "Forcing dimensions specified in playback file" );
                        mForceSpecifiedDimensions = true;
                        
                        
                        if( fullScreenFlag ) {
                            mFullScreen = true;
                            }
                        else {
                            mFullScreen = false;
                            }

                        delete [] mCustomRecordedGameData;
                        mCustomRecordedGameData = 
                            stringDuplicate( readCustomGameData );
                        }
                    else {
                        AppLog::error( 
                        "Hash check failed for custom data in playback file" );
                        }
                    }
                else {
                    AppLog::error( 
                        "Failed to parse playback header data" );

                    }
                delete [] readCustomGameData;                
                }
            delete [] fullFileName;
            }
        

        for( int i=0; i<numChildren; i++ ) {
            delete childFiles[i];
            }
        }
    delete [] childFiles;





    mStartedFullScreen = mFullScreen;

    setupSurface();
    

    SDL_WM_SetCaption( inWindowName, NULL );
    

    // turn off repeat
    SDL_EnableKeyRepeat( 0, 0 );

    SDL_EnableUNICODE( true );
    
    
    

    
    


    

    for( int i=0; i<256; i++ ) {
        keyMap[i] = (unsigned char)i;
        }
    

    }



ScreenGL::~ScreenGL() {
	delete mViewPosition;
	delete mViewOrientation;
	delete mRedrawListenerVector;
	delete mMouseHandlerVector;
	delete mKeyboardHandlerVector;
	delete mSceneHandlerVector;

    if( mRecordingEvents && mRecordingOrPlaybackStarted ) {    
        writeEventBatchToFile();
        }
    
    if( mEventFile != NULL ) {
        fclose( mEventFile );
        mEventFile = NULL;
        }
        

    delete [] mCustomRecordedGameData;    
    delete [] mHashSalt;
    

    for( int i=0; i<mPendingWebEvents.size(); i++ ) {
        WebEvent *e = mPendingWebEvents.getElement( i );
        
        if( e->bodyText != NULL ) {
            
            delete [] e->bodyText;
        
            e->bodyText = NULL;
            }
        
        }
    for( int i=0; i<mPendingSocketEvents.size(); i++ ) {
        SocketEvent *e = mPendingSocketEvents.getElement( i );
        
        if( e->bodyBytesHex != NULL ) {
            
            delete [] e->bodyBytesHex;
        
            e->bodyBytesHex = NULL;
            }
        
        }

	}



void ScreenGL::startRecordingOrPlayback() {
    if( mRecordingEvents ) {
        File recordedGameDir( NULL, "recordedGames" );
    
        if( !recordedGameDir.exists() ) {
            recordedGameDir.makeDirectory();
            }


        // find next event recording file
        int fileNumber = 0;
        
        char hit = true;

        while( hit ) {
            fileNumber++;
            char *fileName = autoSprintf( "recordedGame%05d.txt", 
                                          fileNumber );
            File *file = recordedGameDir.getChildFile( fileName );
            
            delete [] fileName;
            
            if( !file->exists() ) {
                hit = false;
            
                char *fullFileName = file->getFullFileName();
                
                mEventFile = fopen( fullFileName, "w" );

                if( mEventFile == NULL ) {
                    AppLog::error( "Failed to open event recording file" );
                    }
                else {
                    AppLog::getLog()->logPrintf( 
                        Log::INFO_LEVEL,
                        "Recording game into file %s", fullFileName );

                    int fullScreenFlag = 0;
                    if( mFullScreen ) {
                        fullScreenFlag = 1;
                        }

                    char *stringToHash = autoSprintf( "%s%s",
                                                      mCustomRecordedGameData,
                                                      mHashSalt );

                    char *correctHash = computeSHA1Digest( stringToHash );
                    
                    delete [] stringToHash;
                    

                    fprintf( mEventFile, 
                             "%u seed, %u fps, %dx%d, fullScreen=%d, %s %s\n",
                             mRandSeed,
                             mMaxFrameRate, mWide, mHigh, fullScreenFlag,
                             mCustomRecordedGameData,
                             correctHash );

                    delete [] correctHash;
                    }

                delete [] fullFileName;                
                }
            delete file;
            }
        }



    mRecordingOrPlaybackStarted = true;
    }



char screenGLStencilBufferSupported = false;




// aspect ratio rounded to nearest 1/100
// (16:9  is 177)
// Some screen resolutions, like 854x480, are not exact matches to their
// aspect ratio
int computeAspectRatio( int inW, int inH ) {

    int intRatio = (100 * inW ) / inH;

    return intRatio;
    }




void ScreenGL::setupSurface() {
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
    int flags = 0;

#ifndef RASPBIAN    
    // don't have SDL create the GL surface
    // (the SDL one is a software surface on Raspbian) 
    flags = SDL_OPENGL;
#endif

    // NOTE:  flags are also adjusted below if fullscreen resolution not
    // available
    int borderless = 0;
    
	if( mFullScreen ) {
#ifdef __mac__
        borderless = 1;
        NSMenu_setMenuBarVisible(0);
#else
        borderless = SettingsManager::getIntSetting( "borderless", 0 );
#endif

        if( borderless ) {
            AppLog::info( "Setting borderless mode for fullscreen" );
            SDL_putenv( (char*)"SDL_VIDEO_WINDOW_POS=0,0" );
            
            flags = flags | SDL_NOFRAME;
            }
        else {
            AppLog::info( "Setting real (not borderless) fullscreen mode" );
            flags = flags | SDL_FULLSCREEN;
            }
        }

    const SDL_VideoInfo* currentScreenInfo = SDL_GetVideoInfo();
    
    int currentW = currentScreenInfo->current_w;
    int currentH = currentScreenInfo->current_h;
    
    // aspect ratio
    int currentAspectRatio = computeAspectRatio( currentW, currentH );

    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Current screen configuration is %dx%d with aspect ratio %.2f",
        currentW, currentH, currentAspectRatio / 100.0f );



    // check for available modes
    SDL_Rect** modes;

    
    // Get available fullscreen/hardware modes
    modes = SDL_ListModes( NULL, flags);

    // Check if there are any modes available
    if( modes == NULL ) {
        AppLog::criticalError( "ERROR:  No video modes available");
        exit(-1);
        }

    // Check if our resolution is restricted
    if( modes == (SDL_Rect**)-1 ) {
        AppLog::info( "All resolutions available" );
        
        if( mFullScreen && mDoNotChangeNativeResolution ) {
            AppLog::info( "Sticking with user's current screen resolution" );
            
            int borderlessHeightAdjust = 
                SettingsManager::getIntSetting( "borderlessHeightAdjust", 0 );

            mWide = currentW;
            mHigh = currentH;
            
            if( borderless && borderlessHeightAdjust != 0 ) {
                AppLog::getLog()->logPrintf( 
                    Log::INFO_LEVEL,
                    "Adding %d to borderless window height of %d "
                    "resulting in total height of %d",
                    borderlessHeightAdjust, mHigh,
                    mHigh + borderlessHeightAdjust );
                mHigh += borderlessHeightAdjust;
                }
            }
        }
    else if( mForceSpecifiedDimensions && mFullScreen ) {
        
        AppLog::info( "Requested video mode is forced (playback?)" );

        // check if specified dimension available in fullscreen
        
        char match = false;
        
        for( int i=0; modes[i] && ! match; ++i ) {
            if( mWide == modes[i]->w && 
                mHigh == modes[i]->h ) {
                match = true;
                }
            }
        
        if( !match ) {
            AppLog::getLog()->logPrintf( 
                Log::WARNING_LEVEL,
                "  Could not find a full-screen match for the forced screen "
                "dimensions %d x %d\n", mWide, mHigh );
            AppLog::warning( "Reverting to windowed mode" );
            
            mFullScreen = false;
            
            flags = SDL_OPENGL;
#ifdef RASPBIAN  
            flags = 0;
#endif
            }
        }
    else{
        
        if( mFullScreen && mDoNotChangeNativeResolution ) {
            AppLog::info( "Sticking with user's current screen resolution" );
        
            mWide = currentW;
            mHigh = currentH;        
            }


        AppLog::getLog()->logPrintf( 
            Log::INFO_LEVEL,
            "Checking if requested video mode (%dx%d) is available",
            mWide, mHigh );

        
        // Print valid modes

        // only count a match of BOTH resolution and aspect ratio
        char match = false;
        
        AppLog::info( "Available video modes:" );
        for( int i=0; modes[i]; ++i ) {
            AppLog::getLog()->logPrintf( Log::DETAIL_LEVEL,
                                         "   %d x %d\n", 
                                         modes[i]->w, 
                                         modes[i]->h );

            int thisAspectRatio = computeAspectRatio( modes[i]->w,
                                                      modes[i]->h );
            
            if( !mForceAspectRatio && thisAspectRatio == currentAspectRatio ) {
                AppLog::info( "   ^^^^ this mode matches current "
                              "aspect ratio" );
                }
            
            if( mWide == modes[i]->w && mHigh == modes[i]->h ) {
                AppLog::info( "   ^^^^ this mode matches requested mode" );
                
                if( ! mForceAspectRatio && 
                    thisAspectRatio != currentAspectRatio ) {
                    AppLog::info( "        but it doesn't match current "
                                  "aspect ratio" );
                    }
                else {
                    match = true;
                    }
                }
            }

        if( !match ) {
            AppLog::warning( "Warning:  No match for requested video mode" );
            AppLog::info( "Trying to find the closest match" );
            
            int bestDistance = 99999999;
            
            int bestIndex = -1;
            
            for( int i=0; modes[i]; ++i ) {
                // don't even consider modes that are SMALLER than our 
                // requested mode in either dimension
                if( modes[i]->w >= mWide &&
                    modes[i]->h >= mHigh ) {
                
                    int distance = (int)(
                        fabs( modes[i]->w - mWide ) +
                        fabs( modes[i]->h - mHigh ) );
                    
                    int thisAspectRatio = computeAspectRatio( modes[i]->w,
                                                              modes[i]->h );

                    if( ( mForceAspectRatio ||
                          thisAspectRatio == currentAspectRatio ) 
                        && 
                        distance < bestDistance ) {
                        
                        bestIndex = i;
                        bestDistance = distance;
                        }
                    }
                
                }
                    

            if( bestIndex != -1 ) {
                
                if( mForceAspectRatio ) {
                    AppLog::getLog()->logPrintf( 
                        Log::INFO_LEVEL,
                        "Picking closest available large-enough resolution:  "
                        "%d x %d\n", 
                        modes[bestIndex]->w, 
                        modes[bestIndex]->h );
                    }
                else {
                    AppLog::getLog()->logPrintf( 
                        Log::INFO_LEVEL,
                        "Picking closest available large-enough resolution "
                        "that matches current aspect ratio:  %d x %d\n", 
                        modes[bestIndex]->w, 
                        modes[bestIndex]->h );
                    }
                }
            else {
                // search again, ignoring aspect match

                if( !mForceAspectRatio ) {
                    
                    AppLog::warning( 
                        "Warning:  No match for current aspect ratio" );
                    AppLog::info( 
                        "Trying to find the closest off-ratio match" );

                
                    for( int i=0; modes[i]; ++i ) {
                        // don't even consider modes that are SMALLER than our 
                        // requested mode in either dimension
                        if( modes[i]->w >= mWide &&
                            modes[i]->h >= mHigh ) {
                            
                            int distance = (int)(
                                fabs( modes[i]->w - mWide ) +
                                fabs( modes[i]->h - mHigh ) );
                            
                            if( distance < bestDistance ) {
                                bestIndex = i;
                                bestDistance = distance;
                                }
                            }
                        }
                    }
                
                
                if( bestIndex != -1 ) {
                    AppLog::getLog()->logPrintf( 
                        Log::INFO_LEVEL,
                        "Picking closest available large-enough resolution:  "
                        "%d x %d\n", 
                        modes[bestIndex]->w, 
                        modes[bestIndex]->h );
                    }
                else {
                    AppLog::warning( 
                        "Warning:  No sufficiently sized resolution found" );
                    AppLog::info( 
                        "Considering closest-match smaller resolution" );
                    
                    for( int i=0; modes[i]; ++i ) {
                        int distance = (int)(
                            fabs( modes[i]->w - mWide ) +
                            fabs( modes[i]->h - mHigh ) );
                        
                        if( distance < bestDistance ) {
                            bestIndex = i;
                            bestDistance = distance;
                            }
                        }
                    
                    if( bestIndex != -1 ) {
                        AppLog::getLog()->logPrintf( 
                            Log::INFO_LEVEL,
                            "Picking closest available resolution:  "
                            "%d x %d\n", 
                            modes[bestIndex]->w, 
                            modes[bestIndex]->h );
                        }
                    else {
                        AppLog::criticalError( 
                            "ERROR:  No video modes available");
                        exit(-1);
                        }
                    }
                
                }


            mWide = modes[bestIndex]->w;
            mHigh = modes[bestIndex]->h;
            }
        
        }


    // 1-bit stencil buffer
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );

    // vsync to avoid tearing
    SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );

    // current color depth
    SDL_Surface *screen = SDL_SetVideoMode( mWide, mHigh, 0, flags);

#ifdef RASPBIAN
    raspbianCreateSurface();
#endif


    if ( screen == NULL ) {
        printf( "Couldn't set %dx%d GL video mode: %s\n", 
                mWide, 
                mHigh,
                SDL_GetError() );
        }

#ifdef RASPBIAN
    screenGLStencilBufferSupported = true;
#else
    int setStencilSize;
    SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &setStencilSize );
    if( setStencilSize > 0 ) {
        // we have a stencil buffer
        screenGLStencilBufferSupported = true;
        }
#endif


    glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
    }




void ScreenGL::registerWebEvent( int inHandle,
                                 int inType,
                                 const char *inBodyString,
                                 int inBodyLength ) {

    if( ! currentScreenGL->mRecordingEvents || 
        ! currentScreenGL->mRecordingOrPlaybackStarted ) {
    
        // not recording!
        return;
        }
    
        
    char *eventString;

    // only event type 2 has a body text payload
    if( inType == 2 ) {
        if( inBodyLength == -1 ) {
            
            eventString = autoSprintf( "wb %u %d %u %s", inHandle, 
                                       inType, 
                                       strlen( inBodyString ), inBodyString );
            }
        else {

            char *bodyHex = hexEncode( (unsigned char*)inBodyString, 
                                       inBodyLength );

            eventString = autoSprintf( "wx %u %d %u %s", inHandle, 
                                       inType, 
                                       strlen( bodyHex ), bodyHex );
            
            delete [] bodyHex;
            }
        }
    else {
        eventString = autoSprintf( "wb %u %d", inHandle, inType );
        }

    
    mEventBatch.push_back( eventString );
    }



int ScreenGL::getWebEventType( int inHandle ) {
    for( int i=0; i<mPendingWebEvents.size(); i++ ) {
        WebEvent *e = mPendingWebEvents.getElement( i );
        
        if( e->handle == inHandle ) {
            
            int type = e->type;
            
            if( type != 2 ) {
                mPendingWebEvents.deleteElement( i );
                }

            return type;
            }
        }

    // no event record present?
    // assume a normal, non-finished step occurred
    return 0;
    }



char *ScreenGL::getWebEventResultBody( int inHandle, int *outSize ) {
    
    for( int i=0; i<mPendingWebEvents.size(); i++ ) {
        WebEvent *e = mPendingWebEvents.getElement( i );
        
        if( e->handle == inHandle ) {
            
            char *returnValue = e->bodyText;
            
            if( outSize != NULL ) {
                *outSize = e->bodyLength;
                }

            mPendingWebEvents.deleteElement( i );

            return returnValue;
            }
        }
    
    return NULL;
    }




void ScreenGL::registerSocketEvent( int inHandle,
                                    int inType,
                                    int inNumBodyBytes,
                                    unsigned char *inBodyBytes ) {

    if( ! currentScreenGL->mRecordingEvents || 
        ! currentScreenGL->mRecordingOrPlaybackStarted ) {
    
        // not recording!
        return;
        }
    
        
    char *eventString;
    
    // only event type 2 has a body byte payload
    if( inType == 2 && inNumBodyBytes != 0 ) {

        char *bodyHex = hexEncode( inBodyBytes, inNumBodyBytes );

        eventString = autoSprintf( "xs %u %d %u %s", inHandle, 
                                   inType, 
                                   inNumBodyBytes, bodyHex );
        delete [] bodyHex;
        }
    else {
        eventString = autoSprintf( "xs %u %d %d", inHandle, inType,
                                   inNumBodyBytes );
        }

    
    mEventBatch.push_back( eventString );
    }



void ScreenGL::getSocketEventTypeAndSize( int inHandle, int *outType,
                                          int *outNumBodyBytes ) {
    for( int i=0; i<mPendingSocketEvents.size(); i++ ) {
        SocketEvent *e = mPendingSocketEvents.getElement( i );
        
        if( e->handle == inHandle ) {
            
            *outType = e->type;
            *outNumBodyBytes = e->numBodyBytes;
            
            if( *outType != 2 || *outNumBodyBytes == 0 ) {
                mPendingSocketEvents.deleteElement( i );
                }

            return;
            }
        }

    // no event record present?
    *outType = -1;
    *outNumBodyBytes = 0;
    return;
    }



unsigned char *ScreenGL::getSocketEventBodyBytes( int inHandle ) {
    
    for( int i=0; i<mPendingSocketEvents.size(); i++ ) {
        SocketEvent *e = mPendingSocketEvents.getElement( i );
        
        if( e->handle == inHandle ) {
            
            
            unsigned char *returnValue = NULL;

            if( e->bodyBytesHex != NULL ) {
                returnValue = hexDecode( e->bodyBytesHex );
            
                delete [] e->bodyBytesHex;
                }
            
            mPendingSocketEvents.deleteElement( i );

            return returnValue;
            }
        }
    
    return NULL;
    }




void ScreenGL::registerAsyncFileDone( int inHandle ) {

    if( ! currentScreenGL->mRecordingEvents || 
        ! currentScreenGL->mRecordingOrPlaybackStarted ) {
    
        // not recording!
        return;
        }
    
        
    char *eventString = autoSprintf( "af %d", inHandle );
    
    mEventBatch.push_back( eventString );
    }



char ScreenGL::getAsyncFileDone( int inHandle ) {
    return ( mLastAsyncFileHandleDone >= inHandle );
    }




void ScreenGL::writeEventBatchToFile( SimpleVector<char*> *inBatch ) {
    int numInBatch = inBatch->size();
        
    if( mEventFile != NULL ) {
        if( numInBatch > 0 ) {
            
            char **allEvents = inBatch->getElementArray();
            char *eventString = join( allEvents, numInBatch, " " );
            
        
            int eventStringLength = strlen( eventString );
            
            int numWritten = fwrite( eventString, 1, eventStringLength, 
                                     mEventFile );
            
            if( numWritten != eventStringLength ) {
                printf( "Failed to write %d-event batch of length %d "
                        "to recording file\n", numInBatch, eventStringLength );
                }

            delete [] allEvents;
            delete [] eventString;
            }
        
        }
            
    for( int i=0; i<numInBatch; i++ ) {
        delete [] *( inBatch->getElement( i ) );
        }
    inBatch->deleteAll();
    }




void ScreenGL::writeEventBatchToFile() {
    int num = mEventBatch.size() + mUserEventBatch.size();
    
    fprintf( mEventFile, "%d ", num );

    writeEventBatchToFile( &mEventBatch );
    
    fprintf( mEventFile, " " );

    writeEventBatchToFile( &mUserEventBatch );
    
    fprintf( mEventFile, "\n" );

    fflush( mEventFile );
    }



void ScreenGL::playNextEventBatch() {
    // we get a minimized event every frame that we're minimized
    mLastMinimizedStatus = false;
    

    // read and playback next batch
    int batchSize = 0;
    int numRead = fscanf( mEventFile, "%d", &batchSize );
            
    if( numRead == 0 || numRead == EOF ) {
        printf( "Reached end of recorded event file during playback\n" );
        // stop playback
        mPlaybackEvents = false;
        }
    

    for( int i=0; i<batchSize; i++ ) {
                
        char code[3];
        code[0] = '\0';
                
        fscanf( mEventFile, "%2s", code );
                
        switch( code[0] ) {
            case 'm':
                switch( code[1] ) {
                    case 'm': {
                        int x, y;
                        fscanf( mEventFile, "%d %d", &x, &y );
                                
                        callbackPassiveMotion( x, y );
                        }
                        break;
                    case 'd': {
                        int x, y;
                        fscanf( mEventFile, "%d %d", &x, &y );
                                
                        callbackMotion( x, y );
                        }
                        break;
                    case 'b': {
                        int button, state, x, y;
                        fscanf( mEventFile, "%d %d %d %d", 
                                &button, &state, &x, &y );
                                
                        if( state == 1 ) {
                            state = SDL_PRESSED;
                            }
                        else {
                            state = SDL_RELEASED;
                            }
                        
                        callbackMouse( button, state, x, y );
                        }
                        break;
                    }
                break;
            case 'k': {
                int c, x, y;
                fscanf( mEventFile, "%d %d %d", &c, &x, &y );

                switch( code[1] ) {
                    case 'd':          
                        callbackKeyboard( c, x, y );
                        break;
                    case 'u':
                        callbackKeyboardUp( c, x, y );                        
                        break;
                    }
                }
                break;
            case 's': {
                int c, x, y;
                fscanf( mEventFile, "%d %d %d", &c, &x, &y );

                switch( code[1] ) {
                    case 'd':          
                        callbackSpecialKeyboard( c, x, y );
                        break;
                    case 'u':
                        callbackSpecialKeyboardUp( c, x, y );
                        break;
                    }
                }
                break;
            case 't': {
                fscanf( mEventFile, "%lf", &mLastTimeValue );
                mTimeValuePlayedBack = true;
                }
                break;
            case 'T': {
                double t;
                fscanf( mEventFile, "%lf", &t );
                mLastCurrentTimeValue = t;
                mTimeValuePlayedBack = true;
                }
                break;
            case 'F': {
                double fps;
                fscanf( mEventFile, "%lf", &fps );
                mLastActualFrameRate = fps;
                }
                break;
            case 'v': {
                mLastMinimizedStatus = true;
                }
                break;
            case 'w': {
                // special case:  incoming web event
                // (simulating response from a web server during playback)
                
                WebEvent e;
                fscanf( mEventFile, "%d %d", &( e.handle ), &( e.type ) );
                
                if( e.handle > mLastReadWebEventHandle ) {
                    mLastReadWebEventHandle = e.handle;
                    e.handle = mNextUnusedWebEventHandle;
                    mCurrentWebEventHandle = e.handle;
                    
                    mNextUnusedWebEventHandle++;
                    }
                else {
                    e.handle = mCurrentWebEventHandle;
                    }

                e.bodyText = NULL;
                e.bodyLength = 0;
                
                if( e.type == 2 ) {
                    // includes a body payload

                    unsigned int length;
                    
                    fscanf( mEventFile, "%u", &length );

                    // skip the space after length
                    fgetc( mEventFile );
                
                
                    if( code[1] == 'b' ) {
                        // plain text body
                        e.bodyLength = length;
                        e.bodyText = new char[ length + 1 ];
                
                        unsigned int numRead = 
                            fread( e.bodyText, 1, length, mEventFile );
                
                        e.bodyText[ length ] = '\0';

                        if( numRead != length ) {
                            AppLog::error( 
                                "Failed to read web event body text from "
                                "playback file" );
                            delete [] e.bodyText;
                            e.bodyText = NULL;
                            e.bodyLength = 0;
                            }
                        }
                    else if( code[1] == 'x' ) {
                        // hex-encoded body
                        char *bodyHex = new char[ length + 1 ];
                        
                        unsigned int numRead = 
                            fread( bodyHex, 1, length, mEventFile );
                
                        bodyHex[ length ] = '\0';

                        if( numRead != length ) {
                            AppLog::error( 
                                "Failed to read web event body hex from "
                                "playback file" );
                            e.bodyText = NULL;
                            e.bodyLength = 0;
                            }
                        else {
                            unsigned char *bodyDecoded 
                                = hexDecode( bodyHex );
                            
                            e.bodyLength = length / 2;
                            e.bodyText = new char[ e.bodyLength + 1 ];
                            
                            memcpy( e.bodyText, bodyDecoded, e.bodyLength );
                            
                            e.bodyText[ e.bodyLength ] = '\0';
                            
                            delete [] bodyDecoded;
                            }
                        delete [] bodyHex;
                        }
                    }
                
                mPendingWebEvents.push_back( e );
                break;
                }
            case 'x': {
                // special case:  incoming socket event
                // (simulating response from a socket server during playback)
                
                SocketEvent e;
                fscanf( mEventFile, "%d %d %d", 
                        &( e.handle ), &( e.type ), &( e.numBodyBytes ) );

                e.bodyBytesHex = NULL;

                if( e.type == 2 && e.numBodyBytes != 0 ) {
                    // includes a body payload

                    // skip the space after numBodyBytes
                    fgetc( mEventFile );
                
                    unsigned int hexLength = e.numBodyBytes * 2;
                    

                    e.bodyBytesHex = new char[ hexLength + 1 ];
                
                    unsigned int numRead = 
                        fread( e.bodyBytesHex, 1, hexLength, mEventFile );
                
                    e.bodyBytesHex[ hexLength ] = '\0';

                    if( numRead != hexLength ) {
                        AppLog::error( 
                            "Failed to read socket event body hex from "
                            "playback file" );
                        delete [] e.bodyBytesHex;
                        e.bodyBytesHex = NULL;
                        }
                    }
                
                mPendingSocketEvents.push_back( e );
                break;
                }
            case 'a': {
                int nextHandle;
                fscanf( mEventFile, "%d", &nextHandle );
                
                if( nextHandle > mLastAsyncFileHandleDone ) {
                    // track the largest handle seen done so far
                    // (async files are ready in handle order)
                    mLastAsyncFileHandleDone = nextHandle;
                    }
                }
                break;
            default:
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL, 
                    "Unknown code '%s' in playback file\n",
                    code );
            }            
        
        }


    mNumBatchesPlayed++;
    }




const char *ScreenGL::getCustomRecordedGameData() {
    return mCustomRecordedGameData;
    }



char ScreenGL::isPlayingBack() {
    return mPlaybackEvents;
    }


float ScreenGL::getPlaybackDoneFraction() {
    if( mEventFileNumBatches == 0 || mEventFile == NULL ) {
        return 0;
        }
    
    return mNumBatchesPlayed / (float)mEventFileNumBatches;    
    }



char ScreenGL::shouldShowPlaybackDisplay() {
    return mShouldShowPlaybackDisplay;
    }


char ScreenGL::isMinimized() {
    // we use mMinimized internally to keep track of whether we ever
    // minimized ourself and were properly restored from that minimization
    // (to do the things that we need to do when we come out of minimization)

    // HOWEVER, on some platforms, we don't receive proper events when we
    // come out of minimization, so we can't count on mMinimized to be
    // correct.

    // Actually test for real here.



    if( mPlaybackEvents && mRecordingOrPlaybackStarted && 
        mEventFile != NULL ) {
        

        return mLastMinimizedStatus;
        }
    

    // non-playback behavior
    char isMin = ( ( SDL_GetAppState() & SDL_APPACTIVE ) == 0 );

    if( isMin && 
        mRecordingEvents && 
        mRecordingOrPlaybackStarted ) {
        
        // record it 
        
        char *eventString = stringDuplicate( "v" );
        
        mEventBatch.push_back( eventString );
        }
    

    return isMin;
    }





void ScreenGL::start() {
	currentScreenGL = this;

    // call our resize callback (GLUT used to do this for us when the
    // window was created)
    callbackResize( mWide, mHigh );


    // oversleep on last loop (discount it from next sleep)
    // can be negative (add to next sleep)
    int oversleepMSec = 0;


    
    // main loop
    while( true ) {
        
        timeSec_t frameStartSec;
        unsigned long frameStartMSec;
        
        Time::getCurrentTime( &frameStartSec, &frameStartMSec );


        // pre-display first, this might involve a sleep for frame timing
        // purposes
        callbackPreDisplay();
        


        // now handle pending events BEFORE actually drawing the screen.
        // Thus, screen reflects all the latest events (not just those
        // that happened before any sleep called during the pre-display).
        // This makes controls much more responsive.

        

        SDL_Event event;
        
        while( !( mPlaybackEvents && mRecordingOrPlaybackStarted )
               && SDL_PollEvent( &event ) ) {
            
            SDLMod mods = SDL_GetModState();

            // alt-enter, toggle fullscreen (but only if we started there,
            // to prevent window content centering issues due to mWidth and
            // mHeight changes mid-game)
            if( mStartedFullScreen && 
                event.type == SDL_KEYDOWN && 
                event.key.keysym.sym == SDLK_RETURN && 
                ( ( mods & KMOD_META ) || ( mods & KMOD_ALT ) ) ) {  
                printf( "Toggling fullscreen\n" );
                
                mFullScreen = !mFullScreen;
                
                setupSurface();

                callbackResize( mWide, mHigh );

                // reload all textures into OpenGL
                SingleTextureGL::contextChanged();
                }
            // alt-tab when not in fullscreen mode
            else if( ! mFullScreen &&
                     ! mMinimized &&
                     event.type == SDL_KEYDOWN && 
                     event.key.keysym.sym == SDLK_TAB && 
                     ( ( mods & KMOD_META ) || ( mods & KMOD_ALT ) ) ) {
                
                mWantToMimimize = true;
                mWasFullScreenBeforeMinimize = false;

                if( SDL_WM_GrabInput( SDL_GRAB_QUERY ) == SDL_GRAB_ON ) {
                    mWasInputGrabbedBeforeMinimize = true;
                    }
                else {
                    mWasInputGrabbedBeforeMinimize = false;
                    }
                SDL_WM_GrabInput( SDL_GRAB_OFF );
                
                // record TAB keystroke so that it's properly
                // played back
                if( mRecordingEvents && 
                    mRecordingOrPlaybackStarted ) {
        
                    int mouseX, mouseY;
                    SDL_GetMouseState( &mouseX, &mouseY );
                    char *eventString = autoSprintf( "kd %d %d %d", 
                                                     9, mouseX, mouseY );
        
                    mUserEventBatch.push_back( eventString );
                    }
                }
            // handle alt-tab to minimize out of full-screen mode
            else if( mFullScreen &&
                     ! mMinimized &&
                     event.type == SDL_KEYDOWN && 
                     event.key.keysym.sym == SDLK_TAB && 
                     ( ( mods & KMOD_META ) || ( mods & KMOD_ALT ) ) ) { 
                
                printf( "Minimizing from fullscreen on Alt-tab\n" );

                mFullScreen = false;
                
                setupSurface();

                callbackResize( mWide, mHigh );

                // reload all textures into OpenGL
                SingleTextureGL::contextChanged();

                mWantToMimimize = true;
                mWasFullScreenBeforeMinimize = true;

                if( SDL_WM_GrabInput( SDL_GRAB_QUERY ) == SDL_GRAB_ON ) {
                    mWasInputGrabbedBeforeMinimize = true;
                    }
                else {
                    mWasInputGrabbedBeforeMinimize = false;
                    }
                SDL_WM_GrabInput( SDL_GRAB_OFF );
                
                // record TAB keystroke so that it's properly
                // played back
                if( mRecordingEvents && 
                    mRecordingOrPlaybackStarted ) {
                    
                    int mouseX, mouseY;
                    SDL_GetMouseState( &mouseX, &mouseY );
                    char *eventString = autoSprintf( "kd %d %d %d", 
                                                     9, mouseX, mouseY );
        
                    mUserEventBatch.push_back( eventString );
                    }
                }
            // active event after minimizing from windowed mode
            else if( mMinimized && 
                     ! mWasFullScreenBeforeMinimize &&
                     event.type == SDL_ACTIVEEVENT && 
                     event.active.gain && 
                     event.active.state == SDL_APPACTIVE ) {
                // window becoming active out of minimization, needs
                // to return to full-screen mode

                printf( "Restoring to window after Alt-tab\n" );
                
                mWantToMimimize = false;
                mWasFullScreenBeforeMinimize = false;
                mMinimized = false;

                if( mWasInputGrabbedBeforeMinimize ) {
                    SDL_WM_GrabInput( SDL_GRAB_ON );
                    }
                }
            // active event after minimizing from fullscreen mode
            else if( mMinimized && 
                     mWasFullScreenBeforeMinimize &&
                     event.type == SDL_ACTIVEEVENT && 
                     event.active.gain && 
                     event.active.state == SDL_APPACTIVE ) {
                // window becoming active out of minimization, needs
                // to return to full-screen mode

                printf( "Restoring to fullscreen after Alt-tab\n" );

                mFullScreen = true;
                
                setupSurface();

                callbackResize( mWide, mHigh );

                // reload all textures into OpenGL
                SingleTextureGL::contextChanged();
                
                mWantToMimimize = false;
                mWasFullScreenBeforeMinimize = false;
                mMinimized = false;

                if( mWasInputGrabbedBeforeMinimize ) {
                    SDL_WM_GrabInput( SDL_GRAB_ON );
                    }
                }
            // map CTRL-q to ESC
            // 17 is "DC1" which is ctrl-q on some platforms
            else if( event.type == SDL_KEYDOWN &&
                     ( ( event.key.keysym.sym == SDLK_q
                         &&
                         ( ( mods & KMOD_META ) || ( mods & KMOD_ALT )
                           || ( mods & KMOD_CTRL ) ) )
                       ||
                       ( ( event.key.keysym.unicode & 0xFF ) == 17 ) ) ) {
                
                // map to 27, escape
                int mouseX, mouseY;
                SDL_GetMouseState( &mouseX, &mouseY );
                
                callbackKeyboard( 27, mouseX, mouseY );    
                }
            else {
                

            switch( event.type ) {
                case SDL_QUIT: {
                    // map to 27, escape
                    int mouseX, mouseY;
                    SDL_GetMouseState( &mouseX, &mouseY );

                    callbackKeyboard( 27, mouseX, mouseY );
                    }
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    int mouseX, mouseY;
                    SDL_GetMouseState( &mouseX, &mouseY );
                    
                    
                    // check if special key
                    int mgKey = mapSDLSpecialKeyToMG( event.key.keysym.sym );
                    
                    if( mgKey != 0 ) {
                        if( event.type == SDL_KEYDOWN ) {
                            callbackSpecialKeyboard( mgKey, mouseX, mouseY );
                            }
                        else {
                            callbackSpecialKeyboardUp( mgKey, mouseX, mouseY );
                            }
                        }
                    else {
                        unsigned char asciiKey;

                        // try unicode first, if 8-bit clean (extended ASCII)
                        if( ( event.key.keysym.unicode & 0xFF00 ) == 0 &&
                            ( event.key.keysym.unicode & 0x00FF ) != 0 ) {
                            asciiKey = event.key.keysym.unicode & 0xFF;
                            }
                        else {
                            // else unicode-to-ascii failed

                            // fall back
                            asciiKey = 
                                mapSDLKeyToASCII( event.key.keysym.sym );
                            }

                      
                        if( asciiKey != 0 ) {
                            // shift and caps cancel each other
                            if( ( ( event.key.keysym.mod & KMOD_SHIFT )
                                  &&
                                  !( event.key.keysym.mod & KMOD_CAPS ) )
                                ||
                                ( !( event.key.keysym.mod & KMOD_SHIFT )
                                  &&
                                  ( event.key.keysym.mod & KMOD_CAPS ) ) ) {
                                
                                asciiKey = toupper( asciiKey );
                                }
                        
                            if( event.type == SDL_KEYDOWN ) {
                                callbackKeyboard( asciiKey, mouseX, mouseY );
                                }
                            else {
                                callbackKeyboardUp( asciiKey, mouseX, mouseY );
                                }
                            }
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if( event.motion.state & SDL_BUTTON( 1 )
                        || 
                        event.motion.state & SDL_BUTTON( 2 )
                        ||
                        event.motion.state & SDL_BUTTON( 3 ) ) {
                        
                        callbackMotion( event.motion.x, event.motion.y );
                        }
                    else {
                        callbackPassiveMotion( event.motion.x, 
                                               event.motion.y );
                        }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    callbackMouse( event.button.button,
                                   event.button.state,
                                   event.button.x, 
                                   event.button.y );
                    break;
                }
                }
            
            }

        

        

        if( mPlaybackEvents && mRecordingOrPlaybackStarted && 
            mEventFile != NULL ) {
            
            
            if( !mTimeValuePlayedBack ) {
                
                // so far, no time values have been played back yet.
                // (as a fix for earlier release that did not
                // record time), fix time() values to go along with specified
                // frame rate in recording file (so that a game played on a 
                // machine fast enough for 60fps will behave close to the 
                // same, time()-wise, on a machine that can't play the game 
                // back at 60fps).

                mFramesSinceLastTimeTick ++;
            
                if( mFramesSinceLastTimeTick >= mFullFrameRate ) {
                    mFramesSinceLastTimeTick = 0;
                    mLastTimeValue ++;
                    mLastCurrentTimeValue += 1.0;
                    }
                }
            

            // this may overwrite the mLastTimeValue that we're emulating
            // if this recorded frame involved a recorded time() call.
            playNextEventBatch();


            // dump events, but responde to ESC to stop playback
            // let player take over from that point
            while( SDL_PollEvent( &event ) ) {
                SDLMod mods = SDL_GetModState();
                // map CTRL-q to ESC
                // 17 is "DC1" which is ctrl-q on some platforms
                if( event.type == SDL_KEYDOWN &&
                    ( ( event.key.keysym.sym == SDLK_q
                        &&
                        ( ( mods & KMOD_META ) || ( mods & KMOD_ALT )
                          || ( mods & KMOD_CTRL ) ) )
                      ||
                      ( ( event.key.keysym.unicode & 0xFF ) == 17 ) ) ) {
                    
                    // map to 27, escape
                    int mouseX, mouseY;
                    SDL_GetMouseState( &mouseX, &mouseY );
                    
                    printf( "User terminated recorded event playback "
                            "with ESC\n" );
        
                    // stop playback
                    mPlaybackEvents = false;
                    }
                else {
                    switch( event.type ) {
                        case SDL_QUIT: {
                            // map to 27, escape
                            int mouseX, mouseY;
                            SDL_GetMouseState( &mouseX, &mouseY );
                            
                            // actual quit event, still pass through 
                            // as ESC to signal a full quit
                            callbackKeyboard( 27, mouseX, mouseY );
                            }
                            break;
                        case SDL_KEYDOWN: {
                            
                            unsigned char asciiKey;
                            
                            // try unicode first, 
                            // if 8-bit clean (extended ASCII)
                            if( ( event.key.keysym.unicode & 0xFF00 ) == 0 &&
                                ( event.key.keysym.unicode & 0x00FF ) != 0 ) {
                                asciiKey = event.key.keysym.unicode & 0xFF;
                                }
                            else {
                                // else unicode-to-ascii failed

                                // fall back
                                asciiKey = 
                                    mapSDLKeyToASCII( event.key.keysym.sym );
                                }
                            if( asciiKey == 27 ) {
                                // pass ESC through
                                // map to 27, escape
                                int mouseX, mouseY;
                                SDL_GetMouseState( &mouseX, &mouseY );
                                
                                printf( 
                                    "User terminated recorded event playback "
                                    "with ESC\n" );

                                // stop playback
                                mPlaybackEvents = false;
                                }
                            else if( asciiKey == '%' ) {
                                mShouldShowPlaybackDisplay =
                                    ! mShouldShowPlaybackDisplay;
                                }
                            if( mAllowSlowdownKeysDuringPlayback ) {
                                
                                if( asciiKey == '^' ) {
                                    setMaxFrameRate( 2 );
                                    }
                                else if( asciiKey == '&' ) {
                                    setMaxFrameRate( mFullFrameRate );
                                    }
                                else if( asciiKey == '*' ) {
                                    // fast forward
                                    setMaxFrameRate( mFullFrameRate * 2 );
                                    }
                                else if( asciiKey == '(' ) {
                                    // fast fast forward
                                    setMaxFrameRate( mFullFrameRate * 4 );
                                    }
                                else if( asciiKey == ')' ) {
                                    // fast fast fast forward
                                    setMaxFrameRate( mFullFrameRate * 8 );
                                    }
                                }
                            }
                        }                    
                    }
                
                }
            
            if( !mPlaybackEvents ) {
                // playback ended
                // send through full spectrum of release events
                // so no presses linger after playback end
               
                int mouseX, mouseY;
                SDL_GetMouseState( &mouseX, &mouseY );
                callbackMouse( SDL_BUTTON_LEFT, 
                               SDL_RELEASED, mouseX, mouseY );

                callbackMouse( SDL_BUTTON_MIDDLE, 
                               SDL_RELEASED, mouseX, mouseY );

                callbackMouse( SDL_BUTTON_RIGHT, 
                               SDL_RELEASED, mouseX, mouseY );

                callbackMouse( SDL_BUTTON_WHEELUP, 
                               SDL_RELEASED, mouseX, mouseY );

                callbackMouse( SDL_BUTTON_WHEELDOWN, 
                               SDL_RELEASED, mouseX, mouseY );

                for( int i=0; i<255; i++ ) {
                    callbackKeyboardUp( i, mouseX, mouseY );
                    }
                for( int i=MG_KEY_FIRST_CODE; i<=MG_KEY_LAST_CODE; i++ ) {
                    callbackSpecialKeyboardUp( i, mouseX, mouseY );
                    }
                }
            

            }
        
        



        // now all events handled, actually draw the screen
        callbackDisplay();


        // record them?
        // do this down here, AFTER display, since some events might be
        // triggered by the drawing code (example:  web requests and results)
        if( mRecordingEvents && mRecordingOrPlaybackStarted ) {
            writeEventBatchToFile();
            }


        int frameTime =
            Time::getMillisecondsSince( frameStartSec, frameStartMSec );
        

        // frame time should never be negative
        // BUT it can be if system time changes while game is running
        // (example:  automatic daylight savings time adjustment)
        if( frameTime < 0 ) {
            frameTime = 0;
            }
        
        
        if( mUseFrameSleep ) {    
            // lock down to mMaxFrameRate frames per second
            int minFrameTime = 1000 / mMaxFrameRate;
            if( ( frameTime + oversleepMSec ) < minFrameTime ) {
                int timeToSleep = 
                    minFrameTime - ( frameTime + oversleepMSec );
                
                //SDL_Delay( timeToSleep );
                timeSec_t sleepStartSec;
                unsigned long sleepStartMSec;
                Time::getCurrentTime( &sleepStartSec, &sleepStartMSec );
                
                Thread::staticSleep( timeToSleep );
                
                int actualSleepTime = 
                    Time::getMillisecondsSince( sleepStartSec, sleepStartMSec );
                
                oversleepMSec = actualSleepTime - timeToSleep;
                }
            else { 
                oversleepMSec = 0;
                }
            }
        
        
        }
    
    }



void ScreenGL::setMaxFrameRate( unsigned int inMaxFrameRate ) {
    mMaxFrameRate = inMaxFrameRate;
    }


void ScreenGL::setFullFrameRate( unsigned int inFullFrameRate ) {
    mFullFrameRate = inFullFrameRate;
    mMaxFrameRate = mFullFrameRate;
    }




unsigned int ScreenGL::getMaxFramerate() {
    return mMaxFrameRate;
    }


void ScreenGL::useFrameSleep( char inUse ) {
    mUseFrameSleep = inUse;
    }


unsigned int ScreenGL::getRandSeed() {
    return mRandSeed;
    }



timeSec_t ScreenGL::getTimeSec() {
    
    if( mPlaybackEvents && mRecordingOrPlaybackStarted && 
        mEventFile != NULL ) {
        
        return mLastTimeValue;
        }
    

    // else just normal behavior (platform's time() output)
    timeSec_t currentTime = Time::timeSec();

    if( mRecordingEvents && 
        mRecordingOrPlaybackStarted ) {
        
        // record it if it is different from the last value that we recorded
        // thus, 't' events are sparse in our event file (and not repeated
        // for every frame, or multiple times per frame, even if get_time()
        // is called a lot).

        if( currentTime != mLastRecordedTimeValue ) {
            
            char *eventString = autoSprintf( "t %.f", currentTime );
            
            mEventBatch.push_back( eventString );
            
            mLastRecordedTimeValue = currentTime;
            }
        }
    

    return currentTime;
    }




double ScreenGL::getCurrentTime() {
    
    if( mPlaybackEvents && mRecordingOrPlaybackStarted && 
        mEventFile != NULL ) {
        

        return mLastCurrentTimeValue;
        }
    

    // else just normal behavior (platform's output)
    double currentTime = Time::getCurrentTime();

    if( mRecordingEvents && 
        mRecordingOrPlaybackStarted ) {
        
        // record it if it is different from the last value that we recorded
        // thus, 't' events are sparse in our event file (and not repeated
        // for every frame, or multiple times per frame, even if get_time()
        // is called a lot).

        if( currentTime != mLastRecordedCurrentTimeValue ) {
            
            char *eventString = autoSprintf( "T %f", currentTime );
            
            mEventBatch.push_back( eventString );
            
            mLastRecordedCurrentTimeValue = currentTime;
            }
        }
    

    return currentTime;
    }



void ScreenGL::registerActualFrameRate( double inFrameRate ) {
    if( mRecordingEvents && 
        mRecordingOrPlaybackStarted ) {
        
        char *eventString = autoSprintf( "F %lf", inFrameRate );
            
        mEventBatch.push_back( eventString );
        }
    }



double ScreenGL::getRecordedFrameRate() {
    return mLastActualFrameRate;
    }




void ScreenGL::setKeyMapping( unsigned char inFromKey,
                              unsigned char inToKey ) {
    keyMap[ inFromKey ] = inToKey;

    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Mapping key '%c' to '%c'", inFromKey, inToKey );
    }




void ScreenGL::toggleKeyMapping( char inMappingOn ) {
    keyMapOn = inMappingOn;
    }


        



void ScreenGL::switchTo2DMode() {
    m2DMode = true;    
    }



void ScreenGL::setFullScreen() {
    //glutFullScreen();
    }



void ScreenGL::changeWindowSize( int inWidth, int inHeight ) {
    //glutReshapeWindow( inWidth, inHeight );
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

    
    int bigDimension = s->mImageWide;
    if( bigDimension < s->mImageHigh ) {
        bigDimension = s->mImageHigh;
        }
    
    int excessW = s->mWide - bigDimension;
    int excessH = s->mHigh - bigDimension;
    
    // viewport is square of biggest image dimension, centered on screen
    glViewport( excessW / 2,
                excessH / 2, 
                bigDimension,
                bigDimension );
    }



void callbackKeyboard( unsigned char inKey, int inX, int inY ) {
    // all playback events are already mapped
    if( ! ( currentScreenGL->mPlaybackEvents && 
            currentScreenGL->mRecordingOrPlaybackStarted )
        && keyMapOn ) {
        
        inKey = keyMap[inKey];
        }
    
    if( currentScreenGL->mRecordingEvents && 
        currentScreenGL->mRecordingOrPlaybackStarted ) {
        
        unsigned char keyToRecord = inKey;
        
        if( currentScreenGL->mObscureRecordedNumericTyping &&
            inKey >= '0' && inKey <= '9' ) {
            keyToRecord = currentScreenGL->mCharToRecordInstead;
            }

        char *eventString = autoSprintf( "kd %d %d %d", 
                                         keyToRecord, inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }


	char someFocused = currentScreenGL->isKeyboardHandlerFocused();

    int h;
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callback was called
    
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }    
		
	// fire to all handlers, stop if eaten
	for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );

        if( handler->mHandlerFlagged ) {
            // if some are focused, only fire to this handler if it is one
            // of the focused handlers
            if( !someFocused || handler->isFocused() ) {
                handler->keyPressed( inKey, inX, inY );
                if( handler->mEatEvent ) {
                    handler->mEatEvent = false;
                    goto down_eaten;                  
                    }
                }
            }
		}

    down_eaten:
    


    // deflag for next time
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }
	}



void callbackKeyboardUp( unsigned char inKey, int inX, int inY ) {
    // all playback events are already mapped
    if( ! ( currentScreenGL->mPlaybackEvents &&
            currentScreenGL->mRecordingOrPlaybackStarted )
        && keyMapOn ) {
        
        inKey = keyMap[inKey];
        }
    
    if( currentScreenGL->mRecordingEvents &&
        currentScreenGL->mRecordingOrPlaybackStarted ) {

        unsigned char keyToRecord = inKey;
        
        if( currentScreenGL->mObscureRecordedNumericTyping  &&
            inKey >= '0' && inKey <= '9' ) {
            keyToRecord = currentScreenGL->mCharToRecordInstead;
            }

        char *eventString = autoSprintf( "ku %d %d %d", 
                                         keyToRecord, inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }

	char someFocused = currentScreenGL->isKeyboardHandlerFocused();
	
    int h;
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callback was called
    
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }

	// fire to all handlers, stop if eaten
	for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );

        if( handler->mHandlerFlagged ) {
            
            // if some are focused, only fire to this handler if it is one
            // of the focused handlers
            if( !someFocused || handler->isFocused() ) {
                handler->keyReleased( inKey, inX, inY );
                if( handler->mEatEvent ) {
                    handler->mEatEvent = false;
                    goto up_eaten;                  
                    }
                }
            }
        }

    up_eaten:
    

    // deflag for next time
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }
    
	}	

	
	
void callbackSpecialKeyboard( int inKey, int inX, int inY ) {
    if( currentScreenGL->mRecordingEvents &&
        currentScreenGL->mRecordingOrPlaybackStarted ) {

        char *eventString = autoSprintf( "sd %d %d %d", inKey, inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }


	char someFocused = currentScreenGL->isKeyboardHandlerFocused();
	
    int h;
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callback was called
    
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }


	// fire to all handlers, stop if eaten
	for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
		
        if( handler->mHandlerFlagged ) {
            
            // if some are focused, only fire to this handler if it is one
            // of the focused handlers
            if( !someFocused || handler->isFocused() ) {
                handler->specialKeyPressed( inKey, inX, inY );
                if( handler->mEatEvent ) {
                    handler->mEatEvent = false;
                    goto special_down_eaten;                  
                    }
                }
            }
        }

    special_down_eaten:
    
    // deflag for next time
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }
	}
	


void callbackSpecialKeyboardUp( int inKey, int inX, int inY ) {
    if( currentScreenGL->mRecordingEvents &&
        currentScreenGL->mRecordingOrPlaybackStarted ) {

        char *eventString = autoSprintf( "su %d %d %d", inKey, inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }


	char someFocused = currentScreenGL->isKeyboardHandlerFocused();

    int h;
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callback was called
    
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }
	
	// fire to all handlers, stop if eaten
	for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
		KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );

        if( handler->mHandlerFlagged ) {
            
            // if some are focused, only fire to this handler if it is one
            // of the focused handlers
            if( !someFocused || handler->isFocused() ) {
                handler->specialKeyReleased( inKey, inX, inY );
                if( handler->mEatEvent ) {
                    handler->mEatEvent = false;
                    goto special_up_eaten;                  
                    }
                }
            }
        }

    special_up_eaten:
    
    // deflag for next time
    for( h=0; h<currentScreenGL->mKeyboardHandlerVector->size(); h++ ) {
        KeyboardHandlerGL *handler 
			= *( currentScreenGL->mKeyboardHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }
    
	}	


	
void callbackMotion( int inX, int inY ) {
    if( currentScreenGL->mRecordingEvents && 
        currentScreenGL->mRecordingOrPlaybackStarted ) {

        char *eventString = autoSprintf( "md %d %d", inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }

	// fire to all handlers
    int h;
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callback was called
    
    for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
        MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }

	for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
		MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
		if( handler->mHandlerFlagged ) {
            handler->mouseDragged( inX, inY );
            }
		}

    // deflag for next time
    for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
        MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }
	}
	


void callbackPassiveMotion( int inX, int inY ) {

    if( currentScreenGL->mRecordingEvents &&
        currentScreenGL->mRecordingOrPlaybackStarted ) {

        char *eventString = autoSprintf( "mm %d %d", inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }

	// fire to all handlers
    int h;
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callback was called
    
    for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
        MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }

	for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
		MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
		
        if( handler->mHandlerFlagged ) {
            handler->mouseMoved( inX, inY );
            }
		}

    // deflag for next time
    for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
        MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }
	}			
	
	
	
void callbackMouse( int inButton, int inState, int inX, int inY ) {
	
    // ignore wheel events
    if( inButton == SDL_BUTTON_WHEELUP ||
        inButton == SDL_BUTTON_WHEELDOWN ) {
        return;
        }
    
    if( inButton == SDL_BUTTON_RIGHT ) {
        currentScreenGL->mLastMouseButtonRight = true;
        }
    else {
        currentScreenGL->mLastMouseButtonRight = false;
        }
    

    if( currentScreenGL->mRecordingEvents &&
        currentScreenGL->mRecordingOrPlaybackStarted ) {

        int stateEncoding = 0;
        if( inState == SDL_PRESSED ) {
            stateEncoding = 1;
            }
        
        char *eventString = autoSprintf( "mb %d %d %d %d",
                                         inButton, stateEncoding, inX, inY );
        
        currentScreenGL->mUserEventBatch.push_back( eventString );
        }
    

    // fire to all handlers
    int h;
    
    // flag those that exist right now
    // because handlers might remove themselves or add new handlers,
    // and we don't want to fire to those that weren't present when
    // callbackMouse was called
    
    for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
        MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = true;
        }
    
	for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
		MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
		
        if( handler->mHandlerFlagged ) {
            
            handler->mouseMoved( inX, inY );
            if( inState == SDL_PRESSED ) {
                handler->mousePressed( inX, inY );
                }
            else if( inState == SDL_RELEASED ) {
                handler->mouseReleased( inX, inY );
                }
            else {
                printf( "Error:  Unknown mouse state received from SDL\n" );
                }	
            }
		}

    // deflag for next time
    for( h=0; h<currentScreenGL->mMouseHandlerVector->size(); h++ ) {
        MouseHandlerGL *handler 
			= *( currentScreenGL->mMouseHandlerVector->getElement( h ) );
        handler->mHandlerFlagged = false;
        }

	}
	
	
	
void callbackPreDisplay() {
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
    }



void callbackDisplay() {
    ScreenGL *s = currentScreenGL;

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

    for( int r=0; r<s->mRedrawListenerVector->size(); r++ ) {
		RedrawListenerGL *listener 
			= *( s->mRedrawListenerVector->getElement( r ) );
		listener->postRedraw();
		}

#ifdef RASPBIAN
    raspbianSwapBuffers();
#else
	SDL_GL_SwapBuffers();
#endif

    // thanks to Andrew McClure for the idea of doing this AFTER
    // the next redraw (for pretty minimization)
    if( s->mWantToMimimize ) {
        s->mWantToMimimize = false;
        SDL_WM_IconifyWindow();
        s->mMinimized = true;
        }
    }



void callbackIdle() {
	//glutPostRedisplay();
	}		



int mapSDLSpecialKeyToMG( int inSDLKey ) {
    switch( inSDLKey ) {
        case SDLK_F1: return MG_KEY_F1;
        case SDLK_F2: return MG_KEY_F2;
        case SDLK_F3: return MG_KEY_F3;
        case SDLK_F4: return MG_KEY_F4;
        case SDLK_F5: return MG_KEY_F5;
        case SDLK_F6: return MG_KEY_F6;
        case SDLK_F7: return MG_KEY_F7;
        case SDLK_F8: return MG_KEY_F8;
        case SDLK_F9: return MG_KEY_F9;
        case SDLK_F10: return MG_KEY_F10;
        case SDLK_F11: return MG_KEY_F11;
        case SDLK_F12: return MG_KEY_F12;
        case SDLK_LEFT: return MG_KEY_LEFT;
        case SDLK_UP: return MG_KEY_UP;
        case SDLK_RIGHT: return MG_KEY_RIGHT;
        case SDLK_DOWN: return MG_KEY_DOWN;
        case SDLK_PAGEUP: return MG_KEY_PAGE_UP;
        case SDLK_PAGEDOWN: return MG_KEY_PAGE_DOWN;
        case SDLK_HOME: return MG_KEY_HOME;
        case SDLK_END: return MG_KEY_END;
        case SDLK_INSERT: return MG_KEY_INSERT;
        case SDLK_RSHIFT: return MG_KEY_RSHIFT;
        case SDLK_LSHIFT: return MG_KEY_LSHIFT;
        case SDLK_RCTRL: return MG_KEY_RCTRL;
        case SDLK_LCTRL: return MG_KEY_LCTRL;
        case SDLK_RALT: return MG_KEY_RALT;
        case SDLK_LALT: return MG_KEY_LALT;
        case SDLK_RMETA: return MG_KEY_RMETA;
        case SDLK_LMETA: return MG_KEY_LMETA;
        default: return 0;
        }
    }


char mapSDLKeyToASCII( int inSDLKey ) {
    // map world keys  (SDLK_WORLD_X) directly to ASCII
    if( inSDLKey >= 160 && inSDLKey <=255 ) {
        return inSDLKey;
        }
    

    switch( inSDLKey ) {
        case SDLK_UNKNOWN: return 0;
        case SDLK_BACKSPACE: return 8;
        case SDLK_TAB: return 9;
        case SDLK_CLEAR: return 12;
        case SDLK_RETURN: return 13;
        case SDLK_PAUSE: return 19;
        case SDLK_ESCAPE: return 27;
        case SDLK_SPACE: return ' ';
        case SDLK_EXCLAIM: return '!';
        case SDLK_QUOTEDBL: return '"';
        case SDLK_HASH: return '#';
        case SDLK_DOLLAR: return '$';
        case SDLK_AMPERSAND: return '&';
        case SDLK_QUOTE: return '\'';
        case SDLK_LEFTPAREN: return '(';
        case SDLK_RIGHTPAREN: return ')';
        case SDLK_ASTERISK: return '*';
        case SDLK_PLUS: return '+';
        case SDLK_COMMA: return ',';
        case SDLK_MINUS: return '-';
        case SDLK_PERIOD: return '.';
        case SDLK_SLASH: return '/';
        case SDLK_0: return '0';
        case SDLK_1: return '1';
        case SDLK_2: return '2';
        case SDLK_3: return '3';
        case SDLK_4: return '4';
        case SDLK_5: return '5';
        case SDLK_6: return '6';
        case SDLK_7: return '7';
        case SDLK_8: return '8';
        case SDLK_9: return '9';
        case SDLK_COLON: return ':';
        case SDLK_SEMICOLON: return ';';
        case SDLK_LESS: return '<';
        case SDLK_EQUALS: return '=';
        case SDLK_GREATER: return '>';
        case SDLK_QUESTION: return '?';
        case SDLK_AT: return '@';
        case SDLK_LEFTBRACKET: return '[';
        case SDLK_BACKSLASH: return '\\';
        case SDLK_RIGHTBRACKET: return ']';
        case SDLK_CARET: return '^';
        case SDLK_UNDERSCORE: return '_';
        case SDLK_BACKQUOTE: return '`';
        case SDLK_a: return 'a';
        case SDLK_b: return 'b';
        case SDLK_c: return 'c';
        case SDLK_d: return 'd';
        case SDLK_e: return 'e';
        case SDLK_f: return 'f';
        case SDLK_g: return 'g';
        case SDLK_h: return 'h';
        case SDLK_i: return 'i';
        case SDLK_j: return 'j';
        case SDLK_k: return 'k';
        case SDLK_l: return 'l';
        case SDLK_m: return 'm';
        case SDLK_n: return 'n';
        case SDLK_o: return 'o';
        case SDLK_p: return 'p';
        case SDLK_q: return 'q';
        case SDLK_r: return 'r';
        case SDLK_s: return 's';
        case SDLK_t: return 't';
        case SDLK_u: return 'u';
        case SDLK_v: return 'v';
        case SDLK_w: return 'w';
        case SDLK_x: return 'x';
        case SDLK_y: return 'y';
        case SDLK_z: return 'z';
        case SDLK_DELETE: return 127;
        case SDLK_WORLD_0:
            
        default: return 0;
        }
    }
 
