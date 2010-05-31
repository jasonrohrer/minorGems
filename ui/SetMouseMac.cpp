// Jason Rohrer
// SetMouseMac.cpp

/**
*
*	implementation of SetMouse on Mac
*	This uses a hack that may not be supported in later OSs (e.g., OSX)
*
*	Created 1-14-2000
*	Mods:
*/



#include <MacTypes.h>
#include "SetMouse.h"

void SetMouse (int x, int y) {
	Point base;
	// This routine donated to MacMAME by John Stiles
	// Picked up for RadiosGL from the Mac GLQuake site
    Point *RawMouse   = (Point*) 0x82C;
    Point *MTemp      = (Point*) 0x828;
    Ptr    CrsrNew    = (Ptr)    0x8CE;
    Ptr    CrsrCouple = (Ptr)    0x8CF;
    
	base.v = y;
	base.h = x;
	LocalToGlobal(&base);
	
    *RawMouse = base;
    *MTemp    = base;
    *CrsrNew  = *CrsrCouple;
	}
	
// do nothing, these are needed in windows only	
void CaptureMouse() {
	}
void ReleaseMouse() {
	}