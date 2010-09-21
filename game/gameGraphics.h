

// platform-independent wrapper for graphics primitives


#include "doublePair.h"


void setDrawColor( float inR, float inG, float inB, float inA );

// defaults to non-additive blending
void toggleAdditiveBlend( char inAdditive );



// draw using last set color
// four vertices per quad
void drawQuads( int inNumQuads, double inVertices[] );


// four r,g,b,a values per quad vertex
void drawQuads( int inNumQuads, double inVertices[], float inVertexColors[] );


// draw using last set color
// three vertices per triangle
void drawTriangles( int inNumTriangles, double inVertices[], 
                    char inStrip=false, char inFan=false );


// four r,g,b,a values per triangle vertex
void drawTrianglesColor( int inNumTriangles, double inVertices[], 
                         float inVertexColors[],
                         char inStrip=false, char inFan=false );


// switch into stencil-buffer drawing mode
// inAdd determines whether drawn primitives add to or subtract from
//   pass-through area of stencil
void startAddingToStencil( char inDrawColorToo, char inAdd );

// switch into mode where further drawing operations are passed through
// stenciled area
void startDrawingThroughStencil();

// stop stenciling and clear stencil buffer
void stopStencil();



typedef void * SpriteHandle;


// can be NULL on load failure
SpriteHandle loadSprite( const char *inTGAFileName );


// draw with current draw color
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 double inZoom = 1.0 );

                 
