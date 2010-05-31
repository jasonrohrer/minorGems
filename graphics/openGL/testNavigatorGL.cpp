/*
 * Modification History
 *
 * 2001-August-29	Jason Rohrer
 * Created.
 */
 

#include "TestSceneHandlerGL.h"
#include "SceneNavigatorDisplayGL.h"

#include "minorGems/util/random/StdRandomSource.h"

// simple test function
int main() {
	
	StdRandomSource *randSource = new StdRandomSource( 2 );
	
	TestSceneHandlerGL *handler = 
		new TestSceneHandlerGL(); 
	
	char *name = "test window";
	
	SceneNavigatorDisplayGL *screen = 
		new SceneNavigatorDisplayGL( 200, 200, false, name, handler );
	
	screen->start();

	return 0;
	}


	 
