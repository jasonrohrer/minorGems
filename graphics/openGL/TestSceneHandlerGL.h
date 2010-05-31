/*
 * Modification History
 *
 * 2001-August-29		Jason Rohrer
 * Created.
 *
 * 2001-August-30		Jason Rohrer
 * Added random triangels for testing. 
 */
 
 
#ifndef TEST_SCENE_HANDLER_GL_INCLUDED
#define TEST_SCENE_HANDLER_GL_INCLUDED 


#include <GL/gl.h>

#include "SceneHandlerGL.h"

#include "minorGems/util/random/StdRandomSource.h"


/**
 * A test scene implementation.
 *
 * @author Jason Rohrer 
 */
class TestSceneHandlerGL : public SceneHandlerGL { 
	
	public:
		
		// implements the SceneHandlerGL interface
		virtual void drawScene();

	};



void TestSceneHandlerGL::drawScene() {	
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
	
	StdRandomSource source( 0 );
	
	glBegin( GL_TRIANGLES ); {
		for( int i=0; i<10; i++ ) {
			glColor4f( source.getRandomFloat(),
					   source.getRandomFloat(),
					   source.getRandomFloat(),
					   1.0 );
			glVertex3f( source.getRandomFloat() * 100,
						source.getRandomFloat() * 100,
						source.getRandomFloat() * 100 );
			
			glColor4f( source.getRandomFloat(),
					   source.getRandomFloat(),
					   source.getRandomFloat(),
					   1.0 );
			glVertex3f( source.getRandomFloat() * 100,
						source.getRandomFloat() * 100,
						source.getRandomFloat() * 100 );
			
			glColor4f( source.getRandomFloat(),
					   source.getRandomFloat(),
					   source.getRandomFloat(),
					   1.0 );
			glVertex3f( source.getRandomFloat() * 100,
						source.getRandomFloat() * 100,
						source.getRandomFloat() * 100 );
			}
		}
	glEnd();

	}



#endif

