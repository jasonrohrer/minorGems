#ifndef GAME_GRAPHICS_INCLUDED
#define GAME_GRAPHICS_INCLUDED


// platform-independent wrapper for graphics primitives


#include "doublePair.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/RawRGBAImage.h"


typedef struct FloatColor {
        float r, g, b, a;
    } FloatColor;




void setDrawColor( float inR, float inG, float inB, float inA );

void setDrawColor( FloatColor inColor );

FloatColor getDrawColor();

void setDrawFade( float inA );


// adds a global fade that is multiplied into the draw fade
// multiple fades can be stacked, and they will multiply together
// Only affects fade of subsequent calls to setDrawColor or setDrawFade
// returns a handle
int addGlobalFade( float inA );

void removeGlobalFade( int inHandle );

float getTotalGlobalFade();



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

char getLinearMagFilterOn();



void toggleMipMapMinFilter( char inMipMapFilterOn );



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
// inMinAlpha sets alpha test cuttoff.  Only used if inDrawColorToo is false
//   only pixels with alpha value greater than inMinAlpha will be added to 
//   or subtracted from stencil.
void startAddingToStencil( char inDrawColorToo, char inAdd,
                           float inMinAlpha = 0.0f );

// switch into mode where further drawing operations are passed through
// stenciled area
void startDrawingThroughStencil( char inInvertStencil=false );

// stop stenciling and clear stencil buffer
void stopStencil();


// stop stenciling without clearing stencil buffer
// (more efficient in cases where the buffer doesn't need to be cleared)
void disableStencil();




typedef void * SpriteHandle;


// toggles mipmap generation for subsequent sprite loading/filling calls
void toggleMipMapGeneration( char inGenerateMipMaps );

// if off, entire sprite rectangle is drawn
// if on, sprite is cropped to remove fully-transparent rows and columns
// at the left, right, top, and bottom
// MUST be turned on before loading or filling sprites
// (sticks with the sprite for its lifetime after it is loaded)
void toggleTransparentCropping( char inCrop );


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

// fails and returns NULL if inRawImage doesn't have 4 channels
SpriteHandle fillSprite( RawRGBAImage *inRawImage );



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


RawRGBAImage *readTGAFileRaw( const char *inTGAFileName );

// same, but reads from the main directory
RawRGBAImage *readTGAFileRawBase( const char *inTGAFileName );


// reads from a memory buffer containing the full contents of a TGA file
// buffer destroyed by caller
RawRGBAImage *readTGAFileRawFromBuffer( unsigned char *inBuffer, 
                                        int inLength );



// write a TGA file into main directory
// Image destroyed by caller
void writeTGAFile( const char *inTGAFileName, Image *inImage );


void freeSprite( SpriteHandle inSprite );


int getSpriteWidth( SpriteHandle inSprite );

int getSpriteHeight( SpriteHandle inSprite );


// sets the sprite's center offset, in pixels, relative to it's center point
// defaults to 0,0
void setSpriteCenterOffset( SpriteHandle inSprite, doublePair inOffset );



void startCountingSpritePixelsDrawn();


// returns the number of pixels drawn since we started counting
unsigned int endCountingSpritePixelsDrawn();



void startCountingSpritesDrawn();

// returns the number of sprites drawn since we started counting
unsigned int endCountingSpritesDrawn();



// draw with current draw color
// mag filter defaults to off (nearest neighbor, big pixels)
// Rotation is in fractions of a full clockwise rotation (0.25 is 90 deg cw)
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 double inZoom = 1.0,
                 double inRotation = 0.0,
                 char inFlipH = false );

// draw sprite with separate colors set for each corner
// corners in BL, BR, TR, TL order
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 FloatColor inCornerColors[4],
                 double inZoom = 1.0,
                 double inRotation = 0.0, 
                 char inFlipH = false );


// draw sprite with specific corner positions
// Sprite is stretched so that its widest non-transparent pixels fill
// the quad.
// corners in BL, BR, TR, TL order
void drawSprite( SpriteHandle inSprite, doublePair inCornerPos[4], 
                 FloatColor inCornerColors[4] );


// draw with current draw color, but ignore sprite's colors and use
// only it's alpha.
void drawSpriteAlphaOnly( SpriteHandle inSprite, doublePair inCenter, 
                          double inZoom = 1.0,
                          double inRotation = 0.0,
                          char inFlipH = false );




// both cooridinates must be in the range (0,0) to (width-1, height-1)
// values are in view space
Image *getScreenRegion( double inX, double inY, 
                        double inWidth, double inHeight );

// this version uses coordinates in raw screen space, where 0,0 is 
// the upper left corner of the screen and the width and height are in 
// screen pixels
Image *getScreenRegionRaw( int inStartX, int inStartY, 
                           int inWidth, int inHeight );



char isPrintingSupported();

void printImage( Image *inImage, char inFullColor=false );


#endif
                 
