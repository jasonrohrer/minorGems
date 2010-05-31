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
 * 2006-June-26   Jason Rohrer
 * Added function to get events that are waiting in the queue.
 */
 
#ifndef KEYBOARD_INCLUDED
#define KEYBOARD_INCLUDED



/**
 * Interface for accessing keyboard input.
 *
 * Note:
 * Certain implementations may require a ScreenGraphics object to 
 * be constructed before accessing the keyboard (i.e., to give the
 * keyboard input a context).
 */
class Keyboard {
	
	public:
		
		/*
		 * Key descriptions:
		 *
		 * For the standard ascii characters, pass in the string containing
		 * the lower case character, for example, "a" for the character A.
		 *
		 * For other keys, the descriptors have not been defined yet.
		 */
		
		
		/**
		 * Gets whether a key is down.
		 *
		 * @param inKeyDescription string describing the queried key.
		 * 
		 * @return true if key represented by given key code is down.
		 */
		//char getKeyDown( int vKeyCode );
		char getKeyDown( const char *inKeyDescription );


		/**
		 * Gets whether a key is up.
		 *
		 * @param inKeyDescription string describing the queried key. 
		 *
		 * @return true if key represented by given key code is up.
		 */
		//char getKeyUp( int vKeyCode );	
		char getKeyUp( const char *inKeyDescription );


        
        /**
		 * Gets the next keyboard event.
         *
		 * @return the ASCII encoding of the pressed key, or -1 if no
         *   keyboard events are waiting.
		 */
        int getKeyPressedEvent();

        
	};

			
#endif
