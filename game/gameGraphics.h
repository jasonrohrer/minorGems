

// platform-independent wrapper for graphics primitives


void setDrawColor( float inR, float inG, float inB, float inA );


// draw using last set color
// four vertices per quad
void drawQuads( int inNumQuads, double inVertices[] );


// four r,g,b,a values per quad vertex
void drawQuads( int inNumQuads, double inVertices[], float inVertexColors[] );


// draw using last set color
// three vertices per triangle
void drawTriangles( int inNumTriangles, double inVertices[] );


// four r,g,b,a values per quad vertex
void drawTriangles( int inNumTriangles, double inVertices[], 
                    float inVertexColors[] );


