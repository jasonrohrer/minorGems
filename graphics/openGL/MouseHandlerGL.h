/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created. 
 *
 * 2010-April-6 	Jason Rohrer
 * Blocked event passing to handlers that were added by event.
 */
 
 
#ifndef MOUSE_HANDLER_GL_INCLUDED
#define MOUSE_HANDLER_GL_INCLUDED 


/**
 * Interface for an object that can field OpenGL mouse events.
 *
 * @author Jason Rohrer 
 */
class MouseHandlerGL { 
	
	public:

		virtual ~MouseHandlerGL() {
            }
        
        

		/**
		 * Callback function for when mouse moves.
		 *
		 * @param inX x position of mouse.
		 * @param inY y position of mouse.
		 */
		virtual void mouseMoved( int inX, int inY ) = 0;
		
		
		/**
		 * Callback function for when mouse moves with button depressed.
		 *
		 * @param inX x position of mouse.
		 * @param inY y position of mouse.
		 */
		virtual void mouseDragged( int inX, int inY ) = 0;
		
		
		/**
		 * Callback function for when the mouse button is depressed.
		 *
		 * @param inX x position of mouse.
		 * @param inY y position of mouse.
		 */
		virtual void mousePressed( int inX, int inY ) = 0;
		
		
		/**
		 * Callback function for when the mouse button is released.
		 *
		 * @param inX x position of mouse.
		 * @param inY y position of mouse.
		 */
		virtual void mouseReleased( int inX, int inY ) = 0;

        
        char mHandlerFlagged;
			
    protected:
        
        MouseHandlerGL()
                : mHandlerFlagged( false ) {
            }
        
	};



#endif
