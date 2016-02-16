#include "SpriteGL.h"


#include "minorGems/math/geometry/Angle3D.h"

#include "minorGems/util/log/AppLog.h"



char SpriteGL::sGenerateMipMaps = false;



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
                                    false,
                                    sGenerateMipMaps );


    mWidth = spriteImage->getWidth();
    mHeight = spriteImage->getHeight();
    
    mBaseScaleX =  mWidth / mNumPages;
    mBaseScaleY = mHeight / mNumFrames;
    
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
                                    false,
                                    sGenerateMipMaps );
    
    mWidth = inWidth;
    mHeight = inHeight;
    
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
                                    false,
                                    sGenerateMipMaps );

    mWidth = inWidth;
    mHeight = inHeight;

    mBaseScaleX = inWidth / mNumPages;
    mBaseScaleY = inHeight / mNumFrames;
    
    
    mFlipHorizontal = false;
    mHorizontalOffset = 0;
    mCurrentPage = 0;
    }




SpriteGL::~SpriteGL() {
    delete mTexture;
    }




#ifdef GLES


// opt (found with profiler)
// only construct these once, not every draw call
GLfloat squareVertices[4*2];

GLubyte squareColors[4*4];

GLfloat squareTextureCoords[4*2];


void SpriteGL::prepareDraw( int inFrame, 
                            Vector3D *inPosition, 
                            double inScale,
                            char inLinearMagFilter,
                            char inMipMapFilter,
                            double inRotation, inFlipH ) {
    /*
    printf( "Drawing sprite %d, r%f, (%f,%f), s%f, f%f\n",
            (int)(this), inRotation, inPosition->mX, inPosition->mY, inScale,
            inFadeFactor );
    */
    
    // profiler opt:
    // this is expensive, and the game never needs it
    // (all frame references are specific and never auto-cycling)
    // inFrame = inFrame % mNumFrames;
    

    float xRadius = (float)( inScale * mBaseScaleX / 2 );
    float yRadius = (float)( inScale * mBaseScaleY / 2 );

    double xOffset = mHorizontalOffset * inScale;
    

    if( mFlipHorizontal ) {
        xRadius = -xRadius;
        }
    


    // first, set up corners relative to 0,0
    // loop is unrolled here, with all offsets added in
    // also, mZ ignored now, since rotation no longer done
    float posX = (float)( inPosition->mX + xOffset );
    float posY = (float)( inPosition->mY );
    
    squareVertices[0] = posX - xRadius;
    squareVertices[1] = posY - yRadius;
    
    squareVertices[2] = posX + xRadius; 
    squareVertices[3] = posY - yRadius;
    
    squareVertices[4] = posX - xRadius; 
    squareVertices[5] = posY + yRadius;
    
    squareVertices[6] = posX + xRadius; 
    squareVertices[7] = posY + yRadius;
    
    
    if( inRotation != 0 ) {
        double cosAngle = cos( - 2 * M_PI * inRotation );
        double sinAngle = sin( - 2 * M_PI * inRotation );
        
        int index = 0;
        for( int i=0; i<4; i++ ) {
            double x = squareVertices[ index ];
            double y = squareVertices[ index + 1 ];
            
            squareVertices[ index ] = x * cosAngle - y * sinAngle;
            squareVertices[ index + 1 ] = x * sinAngle + y * cosAngle;
            
            index += 2;
            }
        }
    


    mTexture->enable();
    
    if( inMipMapFilter ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                         GL_LINEAR_MIPMAP_LINEAR );
        }
    else {
        
        if( inLinearMagFilter ) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                             GL_LINEAR );
            }
        else {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                             GL_NEAREST );
            }
        }
    
    if( inLinearMagFilter ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        }


    float textXA = (1.0f / mNumPages) * mCurrentPage;
    float textXB = textXA + (1.0f / mNumPages );


    float textYB = (1.0f / mNumFrames) * inFrame;
    float textYA = textYB + (1.0f / mNumFrames );
    
    if( inFlipH ) {
        double temp = textXA;
        textXA = textXB;
        textXB = temp;
        }

    squareTextureCoords[0] = textXA;
    squareTextureCoords[1] = textYA;

    squareTextureCoords[2] = textXB;
    squareTextureCoords[3] = textYA;
    
    squareTextureCoords[4] = textXA;
    squareTextureCoords[5] = textYB;

    squareTextureCoords[6] = textXB;
    squareTextureCoords[7] = textYB;
    }


        

void SpriteGL::draw( int inFrame, 
                     Vector3D *inPosition, 
                     double inScale,
                     char inLinearMagFilter,
                     char inMipMapFilter,
                     double inRotation,
                     char inFlipH ) {
    
    
    prepareDraw( inFrame, inPosition, inScale, inLinearMagFilter, 
                 inMipMapFilter
                 inRotation, inFlipH );

    glVertexPointer( 2, GL_FLOAT, 0, squareVertices );
    glEnableClientState( GL_VERTEX_ARRAY );
    
    //glColorPointer( 4, GL_FLOAT, 0, squareColors );
    //glEnableClientState( GL_COLOR_ARRAY );
    
    glTexCoordPointer( 2, GL_FLOAT, 0, squareTextureCoords );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    mTexture->disable();
    }



void SpriteGL::draw( int inFrame,
                     Vector3D *inPosition,
                     FloatColor inCornerColors[4],
                     double inScale,
                     char inLinearMagFilter,
                     char inMipMapFilter,
                     double inRotation,
                     char inFlipH ) {

    prepareDraw( inFrame, inPosition, inScale, inLinearMagFilter,
                 inMipMapFilter,
                 inRotation, inFlipH );


    glVertexPointer( 2, GL_FLOAT, 0, squareVertices );
    glEnableClientState( GL_VERTEX_ARRAY );


    for( int c=0; c<4; c++ ) {
        
        int cDest = c;
        if( c == 2 ) {
            cDest = 3;
            }
        else if( c == 3 ) {
            cDest = 2;
            }

        int start = cDest * 4;
        squareColors[ start ] = inCornerColors[c].r;
        squareColors[ start + 1 ] = inCornerColors[c].g;
        squareColors[ start + 2 ] = inCornerColors[c].b;
        squareColors[ start + 3 ] = inCornerColors[c].a;
        }
    
    
    glColorPointer( 4, GL_FLOAT, 0, squareColors );
    glEnableClientState( GL_COLOR_ARRAY );
    
    glTexCoordPointer( 2, GL_FLOAT, 0, squareTextureCoords );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    mTexture->disable();
    }







#else

// opt (found with profiler)
// only construct these once, not every draw call
Vector3D corners[4];

double textXA, textXB, textYA, textYB;


void SpriteGL::prepareDraw( int inFrame, 
                            Vector3D *inPosition, 
                            double inScale,
                            char inLinearMagFilter,
                            char inMipMapFilter,
                            double inRotation,
                            char inFlipH ) {
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

    if( inRotation == 0 ) {
        
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
        }
    else {
        corners[0].mX = - xRadius;
        corners[0].mY = - yRadius;
        //corners[0].mZ = 0;
        
        corners[1].mX = xRadius;
        corners[1].mY = - yRadius;
        //corners[1].mZ = 0;
        
        corners[2].mX = xRadius;
        corners[2].mY = yRadius;
        //corners[2].mZ = 0;
        
        corners[3].mX = - xRadius;
        corners[3].mY = yRadius;

        double cosAngle = cos( - 2 * M_PI * inRotation );
        double sinAngle = sin( - 2 * M_PI * inRotation );
        
        for( int i=0; i<4; i++ ) {
            double x = corners[i].mX;
            double y = corners[i].mY;
            
            corners[i].mX = x * cosAngle - y * sinAngle;
            corners[i].mY = x * sinAngle + y * cosAngle;

            corners[i].mX += posX;
            corners[i].mY += posY;
            }
        }

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
    

    
    if( inMipMapFilter ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                         GL_LINEAR_MIPMAP_LINEAR );
        }
    else {
        
        if( inLinearMagFilter ) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                             GL_LINEAR );
            }
        else {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                             GL_NEAREST );
            }
        }
    
    if( inLinearMagFilter ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        }

    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    
    textXA = (1.0 / mNumPages) * mCurrentPage;
    textXB = textXA + (1.0 / mNumPages );


    textYB = (1.0 / mNumFrames) * inFrame;
    textYA = textYB + (1.0 / mNumFrames );

    
    if( inFlipH ) {
        double temp = textXA;
        textXA = textXB;
        textXB = temp;
        }
    
    }


        

void SpriteGL::draw( int inFrame, 
                     Vector3D *inPosition, 
                     double inScale,
                     char inLinearMagFilter,
                     char inMipMapFilter,
                     double inRotation,
                     char inFlipH ) {
    
    prepareDraw( inFrame, inPosition, inScale, inLinearMagFilter,
                 inMipMapFilter,
                 inRotation, inFlipH );
    
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



void SpriteGL::draw( int inFrame,
                     Vector3D *inPosition,
                     FloatColor inCornerColors[4],
                     double inScale,
                     char inLinearMagFilter,
                     char inMipMapFilter,
                     double inRotation,
                     char inFlipH ) {

    prepareDraw( inFrame, inPosition, inScale, inLinearMagFilter,
                 inMipMapFilter,
                 inRotation, inFlipH );
    
    glBegin( GL_QUADS ); {
        
        glColor4f( inCornerColors[0].r, inCornerColors[0].g, 
                   inCornerColors[0].b, inCornerColors[0].a );
        glTexCoord2f( textXA, textYA );
        glVertex2d( corners[0].mX, corners[0].mY );
        
        glColor4f( inCornerColors[1].r, inCornerColors[1].g, 
                   inCornerColors[1].b, inCornerColors[1].a );
        glTexCoord2f( textXB, textYA );
        glVertex2d( corners[1].mX, corners[1].mY );
        
        glColor4f( inCornerColors[2].r, inCornerColors[2].g, 
                   inCornerColors[2].b, inCornerColors[2].a );
        glTexCoord2f( textXB, textYB );
        glVertex2d( corners[2].mX, corners[2].mY );
        
        glColor4f( inCornerColors[3].r, inCornerColors[3].g, 
                   inCornerColors[3].b, inCornerColors[3].a );
        glTexCoord2f( textXA, textYB );
        glVertex2d( corners[3].mX, corners[3].mY );
        }
    glEnd();
    mTexture->disable();
    }



#endif


