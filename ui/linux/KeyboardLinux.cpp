/*
 * Modification History
 *
 * 2000-December-7		Jason Rohrer
 * Created.
 *
 * 2000-December-8		Jason Rohrer
 * Changed so that key state functions take a string instead of
 * an integer vkey code.
 *
 * 2001-May-2   Jason Rohrer
 * Changed to use more standard SDL include location.
 *
 * 2006-June-26   Jason Rohrer
 * Added function to get events that are waiting in the queue.
 */
 
#include "minorGems/ui/Keyboard.h"

#include <SDL/SDL.h>

#include <string.h>

/**
 * Note:  Linux implementation:
 * Requires that a ScreenGraphics be constructed before accessing the keyboard.
 */

// prototypes:

/**
 * Maps an ascii string description of a key, such as "a", to an SDL keycode.
 *
 * @param inKeyDescription an ascii description of a key.
 *
 * @return the SDL keycode.
 */
int getKeyCode( const char *inKeyDescription );



/**
 * Maps a keycode to an ascii character.
 *
 * @param inSDLKeycode the keycode.
 *
 * @return the ascii character, or -1 if the keycode is not mappable to ascii.
 */
int getKeyASCII( int inSDLKeycode );




#define M_KEY 	SDLK_m
#define N_KEY	SDLK_n

#define S_KEY	SDLK_s

#define Q_KEY	SDLK_q

#define L_KEY	SDLK_l

#define R_KEY	SDLK_r

#define T_KEY	SDLK_t



//char Keyboard::getKeyDown( int vKeyCode ) {
char Keyboard::getKeyDown( const char *inKeyDescription ) {
	SDL_PumpEvents();
	Uint8 *keys;
	keys = SDL_GetKeyState( NULL );
	return keys[ getKeyCode( inKeyDescription ) ] == SDL_PRESSED;
	}



//char Keyboard::getKeyUp( int vKeyCode ) {
char Keyboard::getKeyUp( const char *inKeyDescription ) {
	SDL_PumpEvents();
	Uint8 *keys;
	keys = SDL_GetKeyState( NULL );
	return keys[ getKeyCode( inKeyDescription ) ] == SDL_RELEASED;
	}



int Keyboard::getKeyPressedEvent() {

    SDL_Event event;

    if( SDL_PollEvent( &event ) ) {
        switch( event.type ) {
            case SDL_KEYDOWN:
                return getKeyASCII( event.key.keysym.sym );
                break;
            }
        }
    else {
        return -1;
        }
    }



int getKeyCode( const char *inKeyDescription ) {
	
	// note that strcmp functions return 0 if strings match
	
	if( !strcasecmp( inKeyDescription, "m" ) ) {
		return SDLK_m;
		}
	else if( !strcasecmp( inKeyDescription, "n" ) ) {
		return SDLK_n;
		}
	else if( !strcasecmp( inKeyDescription, "s" ) ) {
		return SDLK_s;
		}	
	else if( !strcasecmp( inKeyDescription, "q" ) ) {
		return SDLK_q;
		}	
	else if( !strcasecmp( inKeyDescription, "l" ) ) {
		return SDLK_l;
		}
	else if( !strcasecmp( inKeyDescription, "r" ) ) {
		return SDLK_r;
		}
	else if( !strcasecmp( inKeyDescription, "t" ) ) {
		return SDLK_t;
		}
		 
	}



int getKeyASCII( int inSDLKeycode ) {
    switch( inSDLKeycode ) {
        case SDLK_m:
            return 'm';
            break;
        case SDLK_n:
            return 'n';
            break;
        case SDLK_s:
            return 's';
            break;
        case SDLK_q:
            return 'a';
            break;
        case SDLK_l:
            return 'l';
            break;
        case SDLK_r:
            return 'r';
            break;
        case SDLK_t:
            return 't';
            break;
        default:
            return -1;
            break;
        }
    }


/*	
#define M_KEY 	SDLK_m
#define N_KEY	SDLK_n

#define S_KEY	SDLK_s

#define Q_KEY	SDLK_q

#define L_KEY	SDLK_l

#define R_KEY	SDLK_r

#define T_KEY	SDLK_t
*/
