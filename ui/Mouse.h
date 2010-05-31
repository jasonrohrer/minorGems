/*
 * Modification History
 *
 * 2000-November-28		Jason Rohrer
 * Created.
 */
 
#ifndef MOUSE_INCLUDED
#define MOUSE_INCLUDED



/**
 * Interface for accessing mouse input.
 *
 * Note:
 * Certain implementations may require a ScreenGraphics object to 
 * be constructed before accessing the mouse (i.e., to give the
 * mouse coordinates a context).
 */
class Mouse {
	
	public:
		
		
		/**
		 * Constructs a Mouse.
		 *
		 * @param inNumButtons the number of buttons on the mouse that should
		 *   be monitored.  Default = 1.
		 */
		Mouse( int inNumButtons );
		
		~Mouse();
		
		
		/**
		 * Gets the location of the mouse.
		 *
		 * @param outX pointer to location where x component will be returned.
		 * @param outY pointer to location where y component will be returned.
		 */
		void getLocation( int *outX, int *outY );
		
		
		/**
		 * Gets whether the main mouse button is down.
		 *
		 * @return true if the main mouse button is down.
		 */	
		char isButtonDown();
		
		
		/**
		 * Gets whether a specified mouse button is down.
		 *
		 * @param inButtonNumber the number of the button to check for
		 *   (0 is the main mouse button).
		 *
		 * @return true if the specified mouse button is down.
		 */
		char isButtonDown( int inButtonNumber );
		
			
	private:
		int mXLocation;
		int mYLocation;
		
		int mNumButtons;
		
		// array of booleans that represent the current
		//   (or last known) state of each button
		char *mButtonDown;
	};



inline Mouse::Mouse( int inNumButtons = 1 ) 
	: mNumButtons( inNumButtons ), 
	mButtonDown( new char[inNumButtons] ) {
	
	}
	
	
	
inline Mouse::~Mouse() {
	delete [] mButtonDown;
	}	
	


inline char Mouse::isButtonDown() {
	return isButtonDown( 0 );
	}
	
		
#endif
