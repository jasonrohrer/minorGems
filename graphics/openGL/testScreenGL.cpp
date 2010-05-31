/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created.
 *
 * 2001-February-2		Jason Rohrer
 * Fixed the random seed for testing. 
 */
 

#include "TestHandlerGL.h"
#include "ScreenGL.h"

#include "minorGems/util/random/StdRandomSource.h"

// simple test function
int main() {
	
	StdRandomSource *randSource = new StdRandomSource( 2 );
	
	TestHandlerGL *handler = 
		new TestHandlerGL( randSource, 30 ); 
	
	char *name = "test window";
	
	ScreenGL *screen = 
		new ScreenGL( 200, 200, false, name, handler, handler, handler );
	
	handler->setupPrimitives();
	
	screen->start();
	}


	 
