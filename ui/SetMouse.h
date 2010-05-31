// Jason Rohrer
// SetMouse.h

/**
*
*	Interface for force-setting mouse cursor position
*
*	Created 1-14-2000
*	Mods:
*		Jason Rohrer	1-18-2000	Added capture and release mouse functions to help Win32
*/

void SetMouse( int x, int y);
void CaptureMouse();
void ReleaseMouse();