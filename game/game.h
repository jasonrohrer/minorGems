
// interface between a platform-independent game engine 
// and the underlying platform




// these are called by SDL or iPhone (or other platform) app wrapper


const char *getWindowTitle();


// get data that should be saved in the header of the game that is 
// being recorded
// Must be encoded as a single string with no whitespace.
// Result destroyed by caller
char *getCustomRecordedGameData();

// get secret salt data for secure hashing
// Result destroyed by caller
char *getHashSalt();



// name of custom font TGA file to find in "graphics" folder
const char *getFontTGAFileName();

// called by platform to draw status messages on top of game image
// game can pick where these should be displayed (though platform ensures
// that it is drawn after frame is drawn)
// String might contain multiple lines separated by '\n'
void drawString( const char *inString );


// should demo code be obtained from user and checked against server?
char isDemoMode();

// secret shared with demo server (so that we can detect that it's a real
// demo server)
const char *getDemoCodeSharedSecret();

// url of demo code server PHP script
const char *getDemoCodeServerURL();



// recorded game data will be null if no game is playing back
void initFrameDrawer( int inWidth, int inHeight, int inTargetFrameRate,
                      const char *inCustomRecordedGameData,
                      char inPlayingBack );


// called at application termination
// good time to save state for next launch
void freeFrameDrawer();


// draw scene into frame using GL function calls
// inUpdate is true if game logic should be updated
void drawFrame( char inUpdate );



// these are all in world coordinates based on what is set
// in setViewCenterPosition and setViewSize below

// pointer movement unpressed
void pointerMove( float inX, float inY );

// start of pointer press
void pointerDown( float inX, float inY );

// movement with pointer pressed
void pointerDrag( float inX, float inY );

// end of pointer press
void pointerUp( float inX, float inY );


void keyDown( unsigned char inASCII );

void keyUp( unsigned char inASCII );


// this GL file is actually platform-independent and has nothing to do 
// with OpenGL
// Defines key codes
#include "minorGems/graphics/openGL/KeyboardHandlerGL.h"

void specialKeyDown( int inKeyCode );

void specialKeyUp( int inKeyCode );




#include <stdint.h>
typedef int16_t Sint16;
typedef uint8_t Uint8;


// should sound be initialized?
char getUsesSound();


// gets the next buffer-full of sound samples from the game engine
// inBuffer should be filled with stereo Sint16 samples, little endian,
//    left-right left-right ....
// NOTE:  may not be called by the same thread that calls drawFrame,
//        depending on platform implementation
void getSoundSamples( Uint8 *inBuffer, int inLengthToFillInBytes );










// These are called BY game engine (implemented by supporting platform)


unsigned int getRandSeed();


// when inFromKey is pressed, an event for inToKey will be generated
// (and no event for inFromKey will be generated)
void mapKey( unsigned char inFromKey, unsigned char inToKey );



// sample rate shared by game engine and sound rendering platform
//#define gameSoundSampleRate 22050
//#define gameSoundSampleRate 44100
// #define gameSoundSampleRate 11025


// each rendering platform sets a sample rate
int getSampleRate();


// true to start or resume playing
// false to pause
// Audio starts off paused
void setSoundPlaying( char inPlaying );


// for platforms where audio runs in separate thread
// Lock when manipulating data that is touched by getSoundSamples
void lockAudio();
void unlockAudio();


// returns true if sound started up and is running
char isSoundRunning();



// for moving view around
void setViewCenterPosition( float inX, float inY );

// 1:1 aspect ratio (truncated on shorter screen dimension)
void setViewSize( float inSize );


// default visible
void setCursorVisible( char inIsVisible );


// confines mouse pointer to window and prevents window manager
// from intercepting key presses
// default off
void grabInput( char inGrabOn );


// sets how mouse events are reported
// true to use game world coordinates
// false to use screen coordinates
//
// defaults to game world coordinates
void setMouseReportingMode( char inWorldCoordinates );


// warps mouse to center of screen
// does not generate pointer event callback (and dumps all pending pointer
// motion events that happened before warp)
// returns new mouse position
void warpMouseToCenter( int *outNewMouseX, int *outNewMouseY );



// returns translation of key using current language
const char *translate( const char *inTranslationKey );



// pause and resume the game
void pauseGame();

char isPaused();


// pause mode eventually runs in reduced frame rate to reduce CPU usage
// call this to temporarily increase framerate back to normal
void wakeUpPauseFrameRate();




// save a screenshot to the "screenShots" folder
// screenshot may be delayed until the end of the next redraw.
void saveScreenShot( const char *inPrefix );
