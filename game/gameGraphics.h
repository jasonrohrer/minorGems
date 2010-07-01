

// platform-independent wrapper for graphics primitives


void setDrawColor( float inR, float inG, float inB, float inA );


// draw using last set color
// four vertices per quad
void drawQuads( int inNumQuads, double inVertices[] );


// four r,g,b,a values per quad vertex
void drawQuads( int inNumQuads, double inVertices[], float inVertexColors[] );


// draw using last set color
// three vertices per triangle
void drawTriangles( int inNumTriangles, double inVertices[], 
                    char inStrip=false );


// four r,g,b,a values per triangle vertex
void drawTriangles( int inNumTriangles, double inVertices[], 
                    float inVertexColors[],
                    char inStrip=false );


// switch into stencil-buffer drawing mode
// all primitives drawn add to the area that is inside the stencil
void startAddingToStencil( char inDrawColorToo );

// switch into mode where further drawing operations are passed through
// stenciled area
void startDrawingThroughStencil();

// stop stenciling and clear stencil buffer
void stopStencil();

