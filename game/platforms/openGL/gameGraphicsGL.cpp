#include "minorGems/game/gameGraphics.h"

#include "SpriteGL.h"

#include "minorGems/graphics/openGL/glInclude.h"

#include "minorGems/util/SimpleVector.h"


static float lastR, lastG, lastB, lastA;


static char additiveTextureColorMode = false;



typedef struct GlobalFade {
        int handle;
        float fade;
    } GlobalFade;


static int nextFadeHandle = 0;

static SimpleVector<GlobalFade> globalFades;

static float globalFadeTotal = 1.0f;


static void recalcGlobalFade() {
    globalFadeTotal = 1.0f;
    
    for( int i=0; i<globalFades.size(); i++ ) {
        globalFadeTotal *=
            globalFades.getElementDirect( i ).fade;
        }
    }



int addGlobalFade( float inA ) {
    int h = nextFadeHandle;
    
    GlobalFade f = { h, inA };
    
    globalFades.push_back( f );
    
    nextFadeHandle++;

    recalcGlobalFade();    
    
    return h;
    }

    

void removeGlobalFade( int inHandle ) {
    for( int i=0; i<globalFades.size(); i++ ) {
        if( globalFades.getElementDirect( i ).handle == inHandle ) {
            globalFades.deleteElement( i );
            break;
            }
        }
    
    recalcGlobalFade();
    }



float getTotalGlobalFade() {
    return globalFadeTotal;
    }





void setDrawColor( float inR, float inG, float inB, float inA ) {
    lastR = inR;
    lastG = inG;
    lastB = inB;
    lastA = inA;

    if( additiveTextureColorMode && 
        globalFadeTotal < 1.0f ) {

        // multiplicative blend means we embed fade in color values
        float rFade = 1.0f - inR;
        float gFade = 1.0f - inG;
        float bFade = 1.0f - inB;
        
        rFade *= globalFadeTotal;
        gFade *= globalFadeTotal;
        bFade *= globalFadeTotal;
        
        inR = 1.0f - rFade;
        inG = 1.0f - gFade;
        inB = 1.0f - bFade;
        }        
    else {
        inA *= globalFadeTotal;
        }
        
    glColor4f( inR, inG, inB, inA );
    }



void setDrawColor( FloatColor inColor ) {
    setDrawColor( inColor.r, inColor.g, inColor.b, inColor.a );
    }



FloatColor getDrawColor() {
    FloatColor c;
    c.r = lastR;
    c.g = lastG;
    c.b = lastB;
    c.a = lastA;

    return c;
    }


void setDrawFade( float inA ) {    
    lastA = inA;
    
    glColor4f( lastR, lastG, lastB, inA * globalFadeTotal );
    }




static void setNormalBlend() {
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }



void toggleAdditiveBlend( char inAdditive ) {
    if( inAdditive ) {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE );
        }
    else {
        setNormalBlend();
        }
    }



void toggleMultiplicativeBlend( char inMultiplicative ) {
    if( inMultiplicative ) {
        glBlendFunc( GL_DST_COLOR, GL_ZERO );
        }
    else {
        setNormalBlend();
        }
    }



void toggleAdditiveTextureColoring( char inAdditive ) {
    if( inAdditive ) {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD );
        }
    else {
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        }
    
    additiveTextureColorMode = inAdditive;
    }




static char linearTextureFilterOn = false;

void toggleLinearMagFilter( char inLinearFilterOn ) {
    linearTextureFilterOn = inLinearFilterOn;
    }



char getLinearMagFilterOn() {
    return linearTextureFilterOn;
    }



void toggleMipMapGeneration( char inGenerateMipMaps ) {
    SpriteGL::toggleMipMapGeneration( inGenerateMipMaps );
    }



static char mipMapTextureFilterOn = false;

void toggleMipMapMinFilter( char inMipMapFilterOn ) {
    mipMapTextureFilterOn = inMipMapFilterOn;
    }


static char transparentCroppingOn = false;

void toggleTransparentCropping( char inCrop ) {
    transparentCroppingOn = inCrop;
    }


#ifdef GLES
// GL ES versions of these functions


static double *quadVertsToTriangles( int inNumQuads, double inVertices[] ) {
    double *triangleVertices = new double[ 2 * inNumQuads * 6 ];
    
    for( int i=0; i<inNumQuads; i++ ) {
        
        int skipQ = 8 * i;
        
        int skipT = 12 * i;
        
        // first 0, 1, 2
        triangleVertices[ skipT++ ] = inVertices[skipQ];
        triangleVertices[ skipT++ ] = inVertices[skipQ + 1];

        triangleVertices[ skipT++ ] = inVertices[skipQ + 2];
        triangleVertices[ skipT++ ] = inVertices[skipQ + 3];

        triangleVertices[ skipT++ ] = inVertices[skipQ + 4];
        triangleVertices[ skipT++ ] = inVertices[skipQ + 5];
        
        // then 0, 2, 3
        triangleVertices[ skipT++ ] = inVertices[skipQ];
        triangleVertices[ skipT++ ] = inVertices[skipQ + 1];

        triangleVertices[ skipT++ ] = inVertices[skipQ + 4];
        triangleVertices[ skipT++ ] = inVertices[skipQ + 5];

        triangleVertices[ skipT++ ] = inVertices[skipQ + 6];
        triangleVertices[ skipT++ ] = inVertices[skipQ + 7];
        }
    
    return triangleVertices;
    }


void drawQuads( int inNumQuads, double inVertices[] ) {
    SpriteGL::setTexturingDisabled();
    
    double *triangleVertices = quadVertsToTriangles( inNumQuads, inVertices );
        
    drawTriangles( inNumQuads * 2, triangleVertices );
    delete [] triangleVertices;
    }



void drawQuads( int inNumQuads, double inVertices[], 
                float inVertexColors[] ) {

    SpriteGL::setTexturingDisabled();

    double *triangleVertices = quadVertsToTriangles( inNumQuads, inVertices );
    
    float *triangleVertexColors = new float[ 3 * inNumQuads * 6 ];
    
    for( int i=0; i<inNumQuads; i++ ) {
        
        int skipQC = 12 * i;
        
        int skipTC = 18 * i;
        
        // first 0, 1, 2
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 1];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 2];

        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 3];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 4];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 5];

        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 6];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 7];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 8];

        // then 0, 2, 3
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 1];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 2];

        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 6];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 7];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 8];

        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 9];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 10];
        triangleVertexColors[ skipTC++ ] = inVertexColors[skipQC + 11];
        }
    
    drawTrianglesColor( inNumQuads * 2, triangleVertices, 
                        triangleVertexColors );
    delete [] triangleVertices;
    delete [] triangleVertexColors;
    }



void drawTriangles( int inNumTriangles, double inVertices[], 
                    char inStrip, char inFan ) {
    
    SpriteGL::setTexturingDisabled();
    
    glEnableClientState( GL_VERTEX_ARRAY );

    int numVerts;
    if( inStrip ) {
        numVerts = inNumTriangles + 2;
        }
    else if( inFan ) {
        numVerts = inNumTriangles + 2;
        }
    else {
        numVerts = inNumTriangles * 3;
        }
    
    int numCoords = numVerts * 2;
    
    float *verts = new float[numCoords];
    for( int i=0; i<numCoords; i++ ) {
        verts[i] = (float)( inVertices[i] );
        }

    glVertexPointer( 2, GL_FLOAT, 0, verts );
        
    if( inStrip ) {
        glDrawArrays( GL_TRIANGLE_STRIP, 0, inNumTriangles + 2 );
        }
    else if( inFan ) {
        glDrawArrays( GL_TRIANGLE_FAN, 0, inNumTriangles + 2 );
        }
    else {
        glDrawArrays( GL_TRIANGLES, 0, inNumTriangles * 3 );
        }
    
    
    glDisableClientState( GL_VERTEX_ARRAY );

    delete [] verts;
    }



void drawTrianglesColor( int inNumTriangles, double inVertices[], 
                         float inVertexColors[], char inStrip, char inFan ) {

    SpriteGL::setTexturingDisabled();
    
    glEnableClientState( GL_VERTEX_ARRAY );
    
    int numVerts;
    if( inStrip ) {
        numVerts = inNumTriangles + 2;
        }
    else if( inFan ) {
        numVerts = inNumTriangles + 2;
        }
    else {
        numVerts = inNumTriangles * 3;
        }
    
    int numCoords = numVerts * 2;
    
    float *verts = new float[numCoords];
    for( int i=0; i<numCoords; i++ ) {
        verts[i] = (float)( inVertices[i] );
        }


    glVertexPointer( 2, GL_FLOAT, 0, verts );
    
    
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( 4, GL_FLOAT, 0, inVertexColors );
    

    if( inStrip ) {
        glDrawArrays( GL_TRIANGLE_STRIP, 0, inNumTriangles + 2 );
        }
    else if( inFan ) {
        glDrawArrays( GL_TRIANGLE_FAN, 0, inNumTriangles + 2 );
        }
    else {
        glDrawArrays( GL_TRIANGLES, 0, inNumTriangles * 3 );
        }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );

    delete [] verts;
    }


#else



void drawQuads( int inNumQuads, double inVertices[] ) {
    SpriteGL::setTexturingDisabled();
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_DOUBLE, 0, inVertices );
        
    glDrawArrays( GL_QUADS, 0, inNumQuads * 4 );
    
    glDisableClientState( GL_VERTEX_ARRAY );
    }



void drawQuads( int inNumQuads, double inVertices[], 
                float inVertexColors[] ) {

    SpriteGL::setTexturingDisabled();

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_DOUBLE, 0, inVertices );
    
    
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( 4, GL_FLOAT, 0, inVertexColors );
    
        
    glDrawArrays( GL_QUADS, 0, inNumQuads * 4 );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    }



void drawTriangles( int inNumTriangles, double inVertices[], 
                    char inStrip, char inFan ) {
    
    SpriteGL::setTexturingDisabled();
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_DOUBLE, 0, inVertices );
        
    if( inStrip ) {
        glDrawArrays( GL_TRIANGLE_STRIP, 0, inNumTriangles + 2 );
        }
    else if( inFan ) {
        glDrawArrays( GL_TRIANGLE_FAN, 0, inNumTriangles + 2 );
        }
    else {
        glDrawArrays( GL_TRIANGLES, 0, inNumTriangles * 3 );
        }
    
    
    glDisableClientState( GL_VERTEX_ARRAY );
    }



void drawTrianglesColor( int inNumTriangles, double inVertices[], 
                         float inVertexColors[], char inStrip, char inFan ) {

    SpriteGL::setTexturingDisabled();
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_DOUBLE, 0, inVertices );
    
    
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer( 4, GL_FLOAT, 0, inVertexColors );
    

    if( inStrip ) {
        glDrawArrays( GL_TRIANGLE_STRIP, 0, inNumTriangles + 2 );
        }
    else if( inFan ) {
        glDrawArrays( GL_TRIANGLE_FAN, 0, inNumTriangles + 2 );
        }
    else {
        glDrawArrays( GL_TRIANGLES, 0, inNumTriangles * 3 );
        }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    }

#endif


void enableScissor( double inX, double inY, double inWidth, double inHeight ) {
    
    double endX = inX + inWidth;
    double endY = inY + inHeight;
    
    // scissor rectangle specified in integer screen coordinates

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
 
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    GLdouble winStartX, winStartY, winStartZ;
    GLdouble winEndX, winEndY, winEndZ;

    gluProject( inX, inY, 0, 
                modelview, projection, viewport, 
                &winStartX, &winStartY, &winStartZ );

    gluProject( endX, endY, 0, 
                modelview, projection, viewport, 
                &winEndX, &winEndY, &winEndZ );


    glScissor( lrint( winStartX ), lrint( winStartY ), 
               lrint( winEndX - winStartX ), lrint( winEndY - winStartY ) );
    glEnable( GL_SCISSOR_TEST );
    }



void disableScissor() {
    glDisable( GL_SCISSOR_TEST );
    }



void startAddingToStencil( char inDrawColorToo, char inAdd,
                           float inMinAlpha ) {
    if( !inDrawColorToo ) {
        
        // stop updating color
        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

        // skip fully-transparent areas
        glEnable( GL_ALPHA_TEST );
        glAlphaFunc( GL_GREATER, inMinAlpha );
        }
    else {
        // Re-enable update of color (in case stencil drawing was already
        //  started)
        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
        glDisable( GL_ALPHA_TEST );
        }
    
    glEnable( GL_STENCIL_TEST );
    glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

    if( inAdd ) {
        // Draw 1 into the stencil buffer wherever a sprite is
        glStencilFunc( GL_ALWAYS, 1, 0xffffffff );
        }
    else {
        // draw 0
        glStencilFunc( GL_ALWAYS, 0, 0xffffffff );
        }
    }



void startDrawingThroughStencil( char inInvertStencil ) {
    // Re-enable update of color
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDisable( GL_ALPHA_TEST );
    
    // Now, only render where stencil is set to 1.
    // unless inverted

    if( inInvertStencil ) {
        glStencilFunc( GL_EQUAL, 0, 0xffffffff );  // draw if == 0
        }
    else {        
        glStencilFunc( GL_EQUAL, 1, 0xffffffff );  // draw if == 1
        }
    
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    }



void stopStencil() {
    disableStencil();
    
    glClear( GL_STENCIL_BUFFER_BIT );
    }



void disableStencil() {
    // Re-enable update of color (just in case stencil drawing was not started)
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDisable( GL_ALPHA_TEST );

    // back to unstenciled drawing
    glDisable( GL_STENCIL_TEST );
    }


int totalLoadedTextureBytes = 0;

SpriteHandle fillSprite( Image *inImage, 
                         char inTransparentLowerLeftCorner ) {
    totalLoadedTextureBytes += inImage->getWidth() * inImage->getHeight() * 4;
    return new SpriteGL( inImage, inTransparentLowerLeftCorner, 1, 1,
                         transparentCroppingOn );
    }



SpriteHandle fillSprite( unsigned char *inRGBA, 
                         unsigned int inWidth, unsigned int inHeight ) {
    totalLoadedTextureBytes += inWidth * inHeight * 4;
    return new SpriteGL( inRGBA, inWidth, inHeight, 1, 1,
                         transparentCroppingOn );
    }



SpriteHandle fillSpriteAlphaOnly( unsigned char *inA, 
                                  unsigned int inWidth, 
                                  unsigned int inHeight ) {
    
    return new SpriteGL( true, inA, inWidth, inHeight, 1, 1,
                         transparentCroppingOn );
    }





void freeSprite( SpriteHandle inSprite ) {
    SpriteGL *s = (SpriteGL *)inSprite;
    totalLoadedTextureBytes -= s->mWidth * s->mHeight * 4;
    delete ( s );
    }



int getSpriteWidth( SpriteHandle inSprite ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    return sprite->getWidth();
    }



int getSpriteHeight( SpriteHandle inSprite ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    return sprite->getHeight();
    }



void setSpriteCenterOffset( SpriteHandle inSprite, doublePair inOffset ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    sprite->setCenterOffset( inOffset );
    }





void startCountingSpritePixelsDrawn() {
    SpriteGL::startCountingPixelsDrawn();
    }



unsigned int endCountingSpritePixelsDrawn() {
    return SpriteGL::endCountingPixelsDrawn();
    }



// more efficient and simpler to always count
// instead of keeping an "are we counting" state
// just set this to zero whenever user asks to start counting
static unsigned int numSpritesDrawn = 0;


void startCountingSpritesDrawn() {
    numSpritesDrawn = 0;
    }



unsigned int endCountingSpritesDrawn() {
    return numSpritesDrawn;
    }



// profiler found constructor/deconstructor calls were using 1.8% of time
static Vector3D spritePos( 0, 0, 0 );

// draw with current draw color
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 double inZoom, double inRotation, char inFlipH ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    
    spritePos.mX = inCenter.x;
    spritePos.mY = inCenter.y;
    
    sprite->draw( 0,
                  &spritePos,
                  inZoom,
                  linearTextureFilterOn,
                  mipMapTextureFilterOn, inRotation, inFlipH );
    
    numSpritesDrawn++;
    }



void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 FloatColor inCornerColors[4],
                 double inZoom, double inRotation, char inFlipH ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    
    spritePos.mX = inCenter.x;
    spritePos.mY = inCenter.y;

    sprite->draw( 0,
                  &spritePos,
                  inCornerColors,
                  inZoom,
                  linearTextureFilterOn, mipMapTextureFilterOn,
                  inRotation, inFlipH );
    
    numSpritesDrawn++;
    }



void drawSprite( SpriteHandle inSprite, doublePair inCornerPos[4], 
                 FloatColor inCornerColors[4] ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    
    sprite->draw( 0,
                  inCornerPos,
                  inCornerColors,
                  linearTextureFilterOn, mipMapTextureFilterOn );
    
    numSpritesDrawn++;
    }



void drawSpriteAlphaOnly( SpriteHandle inSprite, doublePair inCenter, 
                          double inZoom, double inRotation, char inFlipH ) {

    // ignores texture colors, use alpha only

    // solution found here:
    // http://stackoverflow.com/questions/2485370/
    //      use-only-alpha-channel-of-texture-in-opengl

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);


    drawSprite( inSprite, inCenter, inZoom, inRotation, inFlipH );

    // restore texture mode
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    
