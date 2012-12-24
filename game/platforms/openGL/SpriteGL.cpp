#include "SpriteGL.h"


#include "minorGems/math/geometry/Angle3D.h"

#include "minorGems/util/log/AppLog.h"




SpriteGL::SpriteGL( Image *inImage,
                    char inTransparentLowerLeftCorner,
                    int inNumFrames,
                    int inNumPages ) {
    
    initTexture( inImage, inTransparentLowerLeftCorner, inNumFrames,
                 inNumPages );
    }
 


void SpriteGL::initTexture( Image *inImage,
                            char inTransparentLowerLeftCorner,
                            int inNumFrames,
                            int inNumPages ) {
    
    mNumFrames = inNumFrames;
    mNumPages = inNumPages;
    
    Image *spriteImage = inImage;
    
    Image *imageToDelete = NULL;
    
    if( spriteImage->getNumChannels() == 3 && inTransparentLowerLeftCorner ) {
        // use lower-left corner color as transparent color for alpha
        spriteImage = spriteImage->generateAlphaChannel();
        imageToDelete = spriteImage;
        }
    
                

    mTexture = new SingleTextureGL( spriteImage,
                                    // no wrap
                                    false );

    mBaseScaleX = spriteImage->getWidth() / mNumPages;
    mBaseScaleY = spriteImage->getHeight() / mNumFrames;
    
    if( imageToDelete != NULL ) {
        delete imageToDelete;
        }

    mFlipHorizontal = false;
    mHorizontalOffset = 0;
    mCurrentPage = 0;
    }



SpriteGL::SpriteGL( unsigned char *inRGBA, 
                    unsigned int inWidth, unsigned int inHeight,
                    int inNumFrames,
                    int inNumPages ) {
    
    mNumFrames = inNumFrames;
    mNumPages = inNumPages;

    mTexture = new SingleTextureGL( inRGBA, inWidth, inHeight,
                                    // no wrap
                                    false );

    mBaseScaleX = inWidth / mNumPages;
    mBaseScaleY = inHeight / mNumFrames;
    
    
    mFlipHorizontal = false;
    mHorizontalOffset = 0;
    mCurrentPage = 0;
    }



SpriteGL::SpriteGL( char inAlphaOnly,
                    unsigned char *inA, 
                    unsigned int inWidth, unsigned int inHeight, 
                    int inNumFrames,
                    int inNumPages ) {
    
    mNumFrames = inNumFrames;
    mNumPages = inNumPages;

    mTexture = new SingleTextureGL( inAlphaOnly,
                                    inA, inWidth, inHeight,
                                    // no wrap
                                    false );

    mBaseScaleX = inWidth / mNumPages;
    mBaseScaleY = inHeight / mNumFrames;
    
    
    mFlipHorizontal = false;
    mHorizontalOffset = 0;
    mCurrentPage = 0;
    }




SpriteGL::~SpriteGL() {
    delete mTexture;
    }



// opt (found with profiler)
// only construct these once, not every draw call
Vector3D corners[4];

        

void SpriteGL::draw( int inFrame, 
                     double inRotation, Vector3D *inPosition, 
                     double inScale,
                     char inLinearMagFilter,
                     double inFadeFactor,
                     Color *inColor ) {
    /*
    printf( "Drawing sprite %d, r%f, (%f,%f), s%f, f%f\n",
            (int)(this), inRotation, inPosition->mX, inPosition->mY, inScale,
            inFadeFactor );
    */
    
    // profiler opt:
    // this is expensive, and the game never needs it
    // (all frame references are specific and never auto-cycling)
    // inFrame = inFrame % mNumFrames;
    

    double xRadius = inScale * mBaseScaleX / 2;
    double yRadius = inScale * mBaseScaleY / 2;

    double xOffset = mHorizontalOffset * inScale;
    

    if( mFlipHorizontal ) {
        xRadius = -xRadius;
        }
    


    // first, set up corners relative to 0,0
    // loop is unrolled here, with all offsets added in
    // also, mZ ignored now, since rotation no longer done
    double posX = inPosition->mX + xOffset;
    double posY = inPosition->mY;
    
    corners[0].mX = posX - xRadius;
    corners[0].mY = posY - yRadius;
    //corners[0].mZ = 0;

    corners[1].mX = posX + xRadius;
    corners[1].mY = posY - yRadius;
    //corners[1].mZ = 0;

    corners[2].mX = posX + xRadius;
    corners[2].mY = posY + yRadius;
    //corners[2].mZ = 0;

    corners[3].mX = posX - xRadius;
    corners[3].mY = posY + yRadius;
    //corners[3].mZ = 0;

    // int i;
    

    // now rotate around center
    // then add inPosition so that center is at inPosition
    
    // Found with profiler:  ignore rotation, since game doesn't use it anyway.
    // Angle3D rot( 0, 0, inRotation );
    
    // found with profiler:
    // unroll this loop
    /*
    for( i=0; i<4; i++ ) {
        corners[i].mX += xOffset;
        
        // corners[i].rotate( &rot );
        corners[i].add( inPosition );
        }
    */

    /*
    if( inColor != NULL  ) {
        glColor4f( inColor->r, inColor->g, inColor->b, inFadeFactor );
        }
    else {
        glColor4f( 1, 1, 1, inFadeFactor );
        }
    */          
    mTexture->enable();
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    if( inLinearMagFilter ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        }
    
    double textXA = (1.0 / mNumPages) * mCurrentPage;
    double textXB = textXA + (1.0 / mNumPages );


    double textYB = (1.0 / mNumFrames) * inFrame;
    double textYA = textYB + (1.0 / mNumFrames );
    
    
    
    glBegin( GL_QUADS ); {
        
        glTexCoord2f( textXA, textYA );
        glVertex2d( corners[0].mX, corners[0].mY );
        
        glTexCoord2f( textXB, textYA );
        glVertex2d( corners[1].mX, corners[1].mY );
        
        glTexCoord2f( textXB, textYB );
        glVertex2d( corners[2].mX, corners[2].mY );
        
        glTexCoord2f( textXA, textYB );
        glVertex2d( corners[3].mX, corners[3].mY );
        }
    glEnd();
    mTexture->disable();
    }


