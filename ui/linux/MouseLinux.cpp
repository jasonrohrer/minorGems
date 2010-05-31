/*
 * Modification History
 *
 * 2000-November-28		Jason Rohrer
 * Created.
 *
 * 2001-May-2   Jason Rohrer
 * Changed to use more standard SDL include location.
 */
 
#include "minorGems/ui/Mouse.h"

#include <SDL/SDL.h>

/**
 * Note:  Linux implementation:
 * Requires that a ScreenGraphics be constructed before accessing the mouse.
 */



void Mouse::getLocation( int *outX, int *outY ) {
	SDL_PumpEvents();
	SDL_GetMouseState( outX, outY );
	/*
	SDL_PumpEvents( void );
	
	int numEventsToGet = 99;
	
	SDL_Event *events = new SDL_Event[numEventsToGet];
	
	// get events from the queue
	int numEventsRetrieved = SDL_PeepEvents( events, numEventsToGet,
		SDL_GETEVENT, SDL_MOUSEMOTIONMASK ); 
	
	// for mouse motion, we only care about the last event
	SDL_Event lastEvent = events[ numEventsRetrieved - 1 ];
	
	delete [] events;
	*/
	}
	
	
char Mouse::isButtonDown( int inButtonNumber ) {
	SDL_PumpEvents();
	
	int x, y;
	
	Uint8 buttonState = SDL_GetMouseState( &x, &y );
	
	if( inButtonNumber == 0 ) {
		return( buttonState & SDL_BUTTON_LMASK );
		}
	
	if( mNumButtons >=3 ) {	
		// if we care about 3 buttons, then count the middle button
		// as button 1
		if( inButtonNumber == 1 ) {
			return( buttonState & SDL_BUTTON_MMASK );
			}
		if( inButtonNumber == 2 ) {
			return( buttonState & SDL_BUTTON_RMASK );
			}
		}
	else {
		// we care about 2 or fewer buttons 
		if( inButtonNumber == 1 ) {
			return( buttonState & SDL_BUTTON_RMASK );
			}
		}
		
	return false;
	}	
