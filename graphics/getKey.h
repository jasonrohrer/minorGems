// Jason Rohrer
// getKey.h

/**
*
*	general interface for getting current key depressed
*	Implemented by a graphix framework on a particular platform
*
*
*	Created 12-15-99
*	Mods:	
*
*/


// returns true if key represented by given key code is down
char getKeyDown( int vKeyCode );

// returns true if key is up
char getKeyUp( int vKeyCode );	





/**


	Sample PC implementation

#include <windows.h>

getKeyDown( int vKeyCode ) { 
	return ((GetAsyncKeyState(vKeyCode) & 0x8000) ? true : false);
	}
getKeyUp( int vKeyCode ) {
	return ((GetAsyncKeyState(vKeyCode) & 0x8000) ? false : true);
	}



Sample Mac implementation


#include <Events.h>

getKeyDown( int vKeyCode ) { 
	KeyMapByteArray keyArray;
	GetKeys( keyArray );
	
	int arrayInd = vKeyCode >> 3;	// divide by 8 to get start array index of key code
	
	unsigned char neededByte = keyArray[ arrayInd ];
	
	return (neededByte >> vKeyCode % 8) && 0x01;		// trim off bit needed
	}
	
getKeyUp( int vKeyCode ) {
	KeyMapByteArray keyArray;
	GetKeys( keyArray );
	
	int arrayInd = vKeyCode >> 3;	// divide by 8 to get start array index of key code
	
	unsigned char neededByte = keyArray[ arrayInd ];
	
	return !((neededByte >> vKeyCode % 8) && 0x01);		// trim off bit needed, and invert
	}
	
	
	
*/