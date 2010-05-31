/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created. 
 */
 
 
#ifndef SCENE_HANDLER_GL_INCLUDED
#define SCENE_HANDLER_GL_INCLUDED 


/**
 * Interface for an object that can draw a scene onto a ScreenGL.
 *
 * @author Jason Rohrer 
 */
class SceneHandlerGL { 
	
	public:
        
        virtual ~SceneHandlerGL() {
            }
        
        
		
		/**
		 * Draws a scene of objects into a ScreenGL.
		 */
		virtual void drawScene() = 0;

	};

#endif
