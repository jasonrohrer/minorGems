#ifndef GAME_GRAPHICS_INCLUDED
#define GAME_GRAPHICS_INCLUDED


// platform-independent wrapper for graphics primitives


#include "doublePair.h"

#include "minorGems/graphics/Image.h"


typedef struct FloatColor {
        float r, g, b, a;
    } FloatColor;




void setDrawColor( float inR, float inG, float inB, float inA );

void setDrawColor( FloatColor inColor );


void setDrawFade( float inA );


// defaults to non-additive (normal) blending
// turning on replaces current blending mode
// turning off goes back to normal blending
void toggleAdditiveBlend( char inAdditive );

// mutiplicative blending (good for simulating transparent inks and paints)
// note that alpha values are ignored
// turning on replaces current blending mode
// turning off goes back to normal blending
void toggleMultiplicativeBlend( char inMultiplicative );


// distinct from additive fragment blending (above)
// turning on makes texture and vertex color combination additive
// turning off goes back to multiplicative combination of texture and vertex
// colors
void toggleAdditiveTextureColoring( char inAdditive );


// defautls to nearest-neighbor texture magnification
void toggleLinearMagFilter( char inLinearFilterOn );


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


// trims drawing operations to a rectangular region
// values in view space
void enableScissor( double inX, double inY, double inWidth, double inHeight );

void disableScissor();



// switch into stencil-buffer drawing mode
// inAdd determines whether drawn primitives add to or subtract from
//   pass-through area of stencil
void startAddingToStencil( char inDrawColorToo, char inAdd );

// switch into mode where further drawing operations are passed through
// stenciled area
void startDrawingThroughStencil( char inInvertStencil=false );

// stop stenciling and clear stencil buffer
void stopStencil();


// stop stenciling without clearing stencil buffer
// (more efficient in cases where the buffer doesn't need to be cleared)
void disableStencil();




typedef void * SpriteHandle;


// loads sprite from graphics directory
// can be NULL on load failure
SpriteHandle loadSprite( const char *inTGAFileName, 
                         char inTransparentLowerLeftCorner = true );

// same, but loads from the main directory
SpriteHandle loadSpriteBase( const char *inTGAFileName, 
                             char inTransparentLowerLeftCorner = true );

SpriteHandle fillSprite( Image *inImage, 
                         char inTransparentLowerLeftCorner = true );

SpriteHandle fillSprite( unsigned char *inRGBA, 
                         unsigned int inWidth, unsigned int inHeight );

// fill a one-channel (alpha-only) sprite
// other channels will be set to black.
// (more efficient than full RGBA for things like shadows)
SpriteHandle fillSpriteAlphaOnly( unsigned char *inA,
                                  unsigned int inWidth, 
                                  unsigned int inHeight );



// reads from the graphics directory
// Image destroyed by caller
Image *readTGAFile( const char *inTGAFileName );

// same, but reads from the main directory
Image *readTGAFileBase( const char *inTGAFileName );


// write a TGA file into main directory
// Image destroyed by caller
void writeTGAFile( const char *inTGAFileName, Image *inImage );


void freeSprite( SpriteHandle inSprite );


// draw with current draw color
// mag filter defaults to off (nearest neighbor, big pixels)
// Rotation is in fractions of a full clockwise rotation (0.25 is 90 deg cw)
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 double inZoom = 1.0,
                 double inRotation = 0.0 );

// draw sprite with separate colors set for each corner
// corners in BL, BR, TR, TL order
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 FloatColor inCornerColors[4],
                 double inZoom = 1.0,
                 double inRotation = 0.0 );


// draw with current draw color, but ignore sprite's colors and use
// only it's alpha.
void drawSpriteAlphaOnly( SpriteHandle inSprite, doublePair inCenter, 
                          double inZoom = 1.0,
                          double inRotation = 0.0 );




// both cooridinates must be in the range (0,0) to (width-1, height-1)
// values are in view space
Image *getScreenRegion( double inX, double inY, 
                        double inWidth, double inHeight );



#endif
                 
