// Jason Rohrer
// SetMouseWin32.cpp

/**
*
*	implementation of SetMouse on Win32
*	This uses an "official" os feature (unlike the hacked mac version
*
*	Created 1-16-2000
*	Mods:
*		Jason Rohrer	1-18-2000	Added conversion from client to screen coords.
*									GLUT tracks mouse motion relative to window
*									Windows can only set the cursor position using 
*									screen coordinates.
*/



#include <winuser.h>

#include "SetMouse.h"

char captured = false;

void SetMouse( int x, int y ) {
	
	POINT p;
	p.x = x;
	p.y = y;
	
	HWND window = GetActiveWindow();
	
	ClientToScreen( window, &p );
	
	SetCursorPos( p.x, p.y );
	
	//SetCursorPos( x, y );
	
	}
	

// send all mouse movements to our window, even those outside the border	
void CaptureMouse() {
	HWND window = GetActiveWindow();
	SetCapture( window );
	}
	
void ReleaseMouse() {
	ReleaseCapture();
	}