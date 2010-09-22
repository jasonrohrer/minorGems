#include "minorGems/game/gameGraphics.h"

#include "SpriteGL.h"

#include <GL/gl.h>



void setDrawColor( float inR, float inG, float inB, float inA ) {
    glColor4f( inR, inG, inB, inA );
    }


void toggleAdditiveBlend( char inAdditive ) {
    if( inAdditive ) {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE );
        }
    else {
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
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



void startDrawingThroughStencil() {
    // Re-enable update of color
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDisable( GL_ALPHA_TEST );
    
    // Now, only render where stencil is set to 1.
    glStencilFunc( GL_EQUAL, 1, 0xffffffff );  // draw if == 1
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    }



void stopStencil() {
    // Re-enable update of color (just in case stencil drawing was not started)
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDisable( GL_ALPHA_TEST );

    // back to unstenciled drawing
    glDisable( GL_STENCIL_TEST );
    glClear( GL_STENCIL_BUFFER_BIT );
    }



SpriteHandle fillSprite( Image *inImage ) {
    return new SpriteGL( inImage, true );
    }




void freeSprite( SpriteHandle inSprite ) {
    delete ( (SpriteGL *)inSprite );
    }




// draw with current draw color
void drawSprite( SpriteHandle inSprite, doublePair inCenter, 
                 double inZoom ) {
    SpriteGL *sprite = (SpriteGL *)inSprite;
    
    Vector3D pos( inCenter.x, inCenter.y, 0 );

    sprite->draw( 1,
                  0, 
                  &pos,
                  inZoom );
    }

