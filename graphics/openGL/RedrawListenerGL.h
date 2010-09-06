/*
 * Modification History
 *
 * 2001-February-4		Jason Rohrer
 * Created.  
 *
 * 2010-April-9 	Jason Rohrer
 * Added post-redraw events.
 *
 * 2010-September-6   Jason Rohrer
 * New frame timing behavior explained.
 */
 
 
#ifndef REDRAW_LISTENER_GL_INCLUDED
#define REDRAW_LISTENER_GL_INCLUDED 


/**
 * Interface for an object that will be called at the beginning of
 * every openGL redraw.
 *
 * @author Jason Rohrer 
 */
class RedrawListenerGL { 
	
	public:

		virtual ~RedrawListenerGL() {
            }

        
		/**
		 * Tells the redraw listener that a redraw is occuring.
		 *
		 * Note that the redraw operation waits for this call to return
		 * before proceeding.
         *
         * This is where sleeping to adjust frame timing should occur.
		 */
		virtual void fireRedraw() = 0;
		

        /**
         * Tells this redraw listener that all scenes have been drawn.
         *
         * Defaults to doing nothing.
         */
        virtual void postRedraw() {
            }
        
		
	};

#endif
