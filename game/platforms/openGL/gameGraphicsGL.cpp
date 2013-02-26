#include "minorGems/game/gameGraphics.h"

#include "SpriteGL.h"

#include "minorGems/graphics/openGL/glInclude.h"


static float lastR, lastG, lastB;


void setDrawColor( float inR, float inG, float inB, float inA ) {
    lastR = inR;
    lastG = inG;
    lastB = inB;

    glColor4f( inR, inG, inB, inA );
    }


void setDrawFade( float inA ) {    
    glColor4f( lastR, lastG, lastB, inA );
    }



void toggleAdditiveBlend( char inAdditive ) {
    if( inAdditive ) {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE );
        }
    else {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
    }


static char linearTextureFilterOn = false;

void toggleLinearMagFilter( char inLinearFilterOn ) {
    linearTextureFilterOn = inLinearFilterOn;
    }





void drawQuads( int inNumQuads, double inVertices[] ) {
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_DOUBLE, 0, inVertices );
        
    glDrawArrays( GL_QUADS, 0, inNumQuads * 4 );
    
    glDisableClientState( GL_VERTEX_ARRAY );
    }



void drawQuads( int inNumQuads, double inVertices[], 
                float inVertexColors[] ) {

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



void startAddingToStencil( char inDrawColorToo, char inAdd ) {
    if( !inDrawColorToo ) {
        
        // stop updating color
        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

        // skip fully-transparent areas
        glEnable( GL_ALPHA_TEST );
        glAlphaFunc( GL_GREATER, 0 );
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



SpriteHandle fillSprite( Image *inImage, 
                         char inTransparentLowerLeftCorner ) {
    return new SpriteGL( inImage, inTransparentLowerLeftCorner );
    }



SpriteHandle fillSprite( unsigned char *inRGBA, 
                         unsigned int inWidth, unsigned int inHeight ) {
    return new SpriteGL( inRGBA, inWidth, inHeight );
    }



SpriteHandle fillSpriteAlphaOnly( unsigned char *inA, 
                                  unsigned int inWidth, 
                                  unsigned int inHeight ) {
    
    return new SpriteGL( true, inA, inWidth, inHeight );
    }





void freeSprite( SpriteHandle inSprite ) {
    delete ( (SpriteGL *)inSprite );
    }




// draw with current draw color
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 double inZoom ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    
    Vector3D pos( inCenter.x, inCenter.y, 0 );

    sprite->draw( 0,
                  0, 
                  &pos,
                  inZoom,
                  linearTextureFilterOn );
    }


void drawSpriteAlphaOnly( SpriteHandle inSprite, doublePair inCenter, 
                          double inZoom ) {

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


    drawSprite( inSprite, inCenter, inZoom );

    // restore texture mode
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    
