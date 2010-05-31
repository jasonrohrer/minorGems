/** 32 bit DrawSprockets pluggable framework
*
* Jason Rohrer, 4-28-99
*
*	Mods:	
*		Jason Rohrer	11-8-99		Changed to use GraphicBuffer object as screen buffer
*		Jason Rohrer	12-15-99	Added support for keyboard querying
*		Jason Rohrer	3-21-2000	Added support for mouse querying
*
*/


#include <DrawSprocket.h>
#include <QuickDraw.h>
#include <Events.h>		// for keyboard events

#include <stdlib.h>		
#include <string.h>	 
#include "swapBuffers.h"	// interface for swapping buffers
#include "getKey.h"		// interface for getting key states
#include "getMouse.h"	// interface for getting mouse state

//#include "ALifeGuiRunner.h"
#include "GoGUIRunner.h"
#include "GraphicBuffer.h"

// Constants
#define BallWidth		20
#define BallHeight		20
#define BobSize			8		/* Size of text in each ball */

//MT Tutorial
#define rWindow			128		// resource ID number for window


// Globals
Rect	windRect;
WindowPtr w;
DSpContextReference theContext;
unsigned long        *double_buffer = NULL;     // the double buffer
unsigned long		 *screen_buffer = NULL;

GraphicBuffer *graphicDoubleBuffer;


short bufferHigh = 480;
short bufferWide = 640;


// prototypes
void Initialize(void);
void graphixPicker(void);		// our mother function, picks effect order, etc.
void initVideo(DSpContextReference *theContext);
void MyInitAttributes(DSpContextAttributes*);
void CleanUp(DSpContextReference theContext);



int main() {

	Initialize();		// setup a window
	MaxApplZone();
	
	initVideo( &theContext );		// set up and switch video
	
	graphixPicker();
	
	CleanUp( theContext);
	
	}


void graphixPicker() {		// our "mother function"  
							// selects which effect to run next
	
	// graphix plug-in format:
		// myGraphix( bufferPtr, bufferHigh, bufferWide, colorBits, numFrames)
		// function can cll "swapBuffer32" internally whenever it needs the back buffer
		//		sent to the screen.
		
			
	
	//	jcrTorus( double_buffer, 480, 640, 32, 100);
	//	jcrVoxels( double_buffer, 480, 640, 32, 230);
	//	jcrTorus( double_buffer, 480, 640, 32, 50);
		
		
		//jcrBloom( double_buffer, 480, 640, 32, 100);
		
		// GraphicBuffer passed in contains all needed info about screen
		// pass in number of frames to run for					
		//ALifeGuiRunner( *graphicDoubleBuffer, 1000 );

		GoGUIRunner( *graphicDoubleBuffer, 0 );
	}


// Initialize the draw sprockets
// set up the video, fade out display, and switch video modes
void initVideo(DSpContextReference *theContext) {	
	
	CGrafPtr backBuffer;
	PixMapHandle bufferMap;
	
	DSpContextAttributes theDesiredAttributes;
	OSStatus theError;
	

	
	MyInitAttributes(&theDesiredAttributes);
	theDesiredAttributes.displayWidth = 640;
	theDesiredAttributes.displayHeight = 480;
	theDesiredAttributes.colorNeeds = kDSpColorNeeds_Require;
	theDesiredAttributes.backBufferDepthMask = kDSpDepthMask_32;
	theDesiredAttributes.displayDepthMask = kDSpDepthMask_32;
	theDesiredAttributes.backBufferBestDepth = 32;
	theDesiredAttributes.displayBestDepth = 32;
	theError = DSpFindBestContext(&theDesiredAttributes, theContext);
	
	// add page flipping by video card to request
	theDesiredAttributes.contextOptions |= kDSpContextOption_PageFlip;
	//theDesiredAttributes.contextOptions |= kDSpContextOption_DontSyncVBL;
	
	// Reserver a display
	theError = DSpContext_Reserve(*theContext, &theDesiredAttributes);
	
	// fade out
	theError = DSpContext_FadeGammaOut(NULL, NULL);

	theError = DSpContext_SetState(*theContext, kDSpContextState_Active);
	ShowCursor();
	HideCursor();


	// setup global pointers to screen and back buffer
	
	theError = DSpContext_GetBackBuffer(*theContext, kDSpBufferKind_Normal, &backBuffer);
	bufferMap = (PixMapHandle)GetGWorldPixMap(backBuffer);
	double_buffer = (unsigned long*)(**bufferMap).baseAddr;
	
	// create GraphicBuffer object
	graphicDoubleBuffer = new GraphicBuffer( double_buffer, 640, 480 );

	theError = DSpContext_GetFrontBuffer(*theContext, &backBuffer);
	bufferMap = (PixMapHandle)GetGWorldPixMap(backBuffer);
	screen_buffer = (unsigned long*)(**bufferMap).baseAddr;

	theError = DSpContext_FadeGammaIn(NULL, NULL);

	}


// initialize a set of Display attributes to zero
void MyInitAttributes (DSpContextAttributes *inAttributes)			
{
   if (NULL == inAttributes)
      DebugStr("\pStimpy! You Idiot!");
   
   inAttributes->frequency = 0;
   inAttributes->displayWidth = 0;
   inAttributes->displayHeight = 0;
   inAttributes->reserved1 = 0;
   inAttributes->reserved2 = 0;
   inAttributes->colorNeeds = 0;
   inAttributes->colorTable = NULL;
   inAttributes->contextOptions = 0;
   inAttributes->backBufferDepthMask = 0;
   inAttributes->displayDepthMask = 0;
   inAttributes->backBufferBestDepth = 0;
   inAttributes->displayBestDepth = 0;
   inAttributes->pageCount = 0;
   inAttributes->gameMustConfirmSwitch = false;
   inAttributes->reserved3[0] = 0;
   inAttributes->reserved3[1] = 0;
   inAttributes->reserved3[2] = 0;
   inAttributes->reserved3[3] = 0;
}

void CleanUp(DSpContextReference theContext) {
	OSStatus theError;
	
	theError = DSpContext_FadeGammaOut(NULL, NULL);

	/* put the context into the inactive state */
	theError = DSpContext_SetState(theContext, kDSpContextState_Inactive);

	/* fade back in */
	theError = DSpContext_FadeGammaIn(NULL, NULL);

	/* release the context */
	theError = DSpContext_Release(theContext);
	
	ShowCursor();
	}





// swap bufferB to the screen (32 bit version)
void swapBuffers32( GraphicBuffer &bufferB ) {
	OSStatus theError;
	
	// swap buffers	
	theError = DSpContext_SwapBuffers(theContext, NULL, 0);
	
	
	// get pointer to new back buffer and return it
	CGrafPtr backBuffer;
	PixMapHandle bufferMap;
	theError = DSpContext_GetBackBuffer(theContext, kDSpBufferKind_Normal, &backBuffer);
	bufferMap = (PixMapHandle)GetGWorldPixMap(backBuffer);
	
	// replace the buffer in bufferB with the new double buffer
	bufferB.setBuffer( (unsigned long*)(**bufferMap).baseAddr );

//	memcpy(screen_buffer, bufferPtrB, 4*bufferWide*bufferHigh);

	

	}		// end of swapBuffers



// returns true if key represented by given key code is down
char getKeyDown( int vKeyCode ) { 
	
	unsigned char keyArray [16];
	GetKeys( (long *)keyArray );
	
	return ((keyArray[vKeyCode>>3] >> (vKeyCode & 7)) & 1);
	}

// returns true if key is up
char getKeyUp( int vKeyCode ) {
	
	unsigned char keyArray [16];
	GetKeys( (long *)keyArray );
	
	return !((keyArray[vKeyCode>>3] >> (vKeyCode & 7)) & 1);
	}


void getMouse( int *x, int *y ) {
	Point mousePoint;
	DSpGetMouse( &mousePoint );
	
	*x = mousePoint.h;
	*y = mousePoint.v;
	}



void Initialize(void)
{
	WindowPtr	mainPtr;
	OSErr		error;
	SysEnvRec	theWorld;
		
	//
	//	Test the computer to be sure we can do color.  
	//	If not we would crash, which would be bad.  
	//	If we can¹t run, just beep and exit.
	//

	error = SysEnvirons(1, &theWorld);
	if (theWorld.hasColorQD == false) {
//		SysBeep(50);
		ExitToShell();					// If no color QD, we must leave. 
	}
	
	// Initialize all the needed managers.
	InitGraf(&qd.thePort);
//	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();

	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don¹t for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	GetDateTime((unsigned long*) &qd.randSeed);

	//
	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	//
	mainPtr = GetNewCWindow(rWindow, nil, (WindowPtr) -1);	// MW Tutorial
	windRect = mainPtr->portRect;
		
	SetPort(mainPtr);						// set window to current graf port 
	TextSize(BobSize);						// smaller font for drawing. 
}





