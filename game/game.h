
// interface between a platform-independent game engine 
// and the underlying platform




// these are called by SDL or iPhone (or other platform) app wrapper


const char *getWindowTitle();


void initFrameDrawer( int inWidth, int inHeight );


// called at application termination
// good time to save state for next launch
void freeFrameDrawer();


// draw scene into frame using GL function calls
void drawFrame();



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


// sample rate shared by game engine and sound rendering platform
//#define gameSoundSampleRate 22050
//#define gameSoundSampleRate 44100
// #define gameSoundSampleRate 11025


// called by rendering platform when setting sample rate
int getSampleRate();




// gets the next buffer-full of sound samples from the game engine
// inBuffer should be filled with stereo Sint16 samples, little endian,
//    left-right left-right ....
// NOTE:  may not be called by the same thread that calls drawFrame,
//        depending on platform implementation
void getSoundSamples( Uint8 *inBuffer, int inLengthToFillInBytes );










// These are called BY game engine (implemented by supporting platform)



// true to start or resume playing
// false to pause
void setSoundPlaying( char inPlaying );



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
