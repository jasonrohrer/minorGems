#include "SpriteGL.h"


#include "minorGems/math/geometry/Angle3D.h"

#include "minorGems/util/log/AppLog.h"



char SpriteGL::sGenerateMipMaps = false;

char SpriteGL::sCountingPixels = false;
unsigned int SpriteGL::sPixelsDrawn = 0;



void SpriteGL::findColoredRadii( Image *inImage ) {
    double *alpha = inImage->getChannel( 3 );

    int w = inImage->getWidth();
    int h = inImage->getHeight();
    
    int minX = w;
    int maxX = 0;
    int minY = h;
    int maxY = 0;
    
    for( int y=0; y<h; y++ ) {
        for( int x=0; x<w; x++ ) {
            int index = y * w + x;
            
            if( alpha[index] > 0 ) {
                
                if( x < minX ) {
                    minX = x;
                    }
                if( x > maxX ) {
                    maxX = x;
                    }
                if( y < minY ) {
                    minY = y;
                    }
                if( y > maxY ) {
                    maxY = y;
                    }
                }    
            }
        }
    

    if( minX > 0 ) {
        mColoredRadiusLeftX = 0.5 - minX / (double)w;
        }
    if( maxX < w - 1 ) {
        mColoredRadiusRightX = ( maxX + 1 ) / (double)w - 0.5;
        }
    
    if( minY > 0 ) {
        mColoredRadiusTopY = 0.5 - minY / (double)h;
        }
    if( maxY < h - 1 ) {
        mColoredRadiusBottomY = ( maxY + 1 ) / (double)h - 0.5;
        }
    }





void SpriteGL::findColoredRadii( unsigned char *inRGBA, 
                                 int inWidth, int inHeight ) {
    
    int w = inWidth;
    int h = inHeight;
    
    int minX = w;
    int maxX = 0;
    int minY = h;
    int maxY = 0;
    
    for( int y=0; y<h; y++ ) {
        for( int x=0; x<w; x++ ) {
            int index = y * w + x;
            
            if( inRGBA[ index * 4 + 3 ] > 0 ) {
                
                if( x < minX ) {
                    minX = x;
                    }
                if( x > maxX ) {
                    maxX = x;
                    }
                if( y < minY ) {
                    minY = y;
                    }
                if( y > maxY ) {
                    maxY = y;
                    }
                }    
            }
        }


    if( minX > 0 ) {
        mColoredRadiusLeftX = 0.5 - minX / (double)w;
        }
    if( maxX < w - 1 ) {
        mColoredRadiusRightX = ( maxX + 1 ) / (double)w - 0.5;
        }
    
    if( minY > 0 ) {
        mColoredRadiusTopY = 0.5 - minY / (double)h;
        }
    if( maxY < h - 1 ) {
        mColoredRadiusBottomY = ( maxY + 1 ) / (double)h - 0.5;
        }
    }



void SpriteGL::findColoredRadiiAlpha( unsigned char *inA, 
                                      int inWidth, int inHeight ) {
    
    int w = inWidth;
    int h = inHeight;
    
    int minX = w;
    int maxX = 0;
    int minY = h;
    int maxY = 0;
    
    for( int y=0; y<h; y++ ) {
        for( int x=0; x<w; x++ ) {
            int index = y * w + x;
            
            if( inA[ index ] > 0 ) {
                
                if( x < minX ) {
                    minX = x;
                    }
                if( x > maxX ) {
                    maxX = x;
                    }
                if( y < minY ) {
                    minY = y;
                    }
                if( y > maxY ) {
                    maxY = y;
                    }
                }    
            }
        }


    if( minX > 0 ) {
        mColoredRadiusLeftX = 0.5 - minX / (double)w;
        }
    if( maxX < w - 1 ) {
        mColoredRadiusRightX = ( maxX + 1 ) / (double)w - 0.5;
        }
    
    if( minY > 0 ) {
        mColoredRadiusTopY = 0.5 - minY / (double)h;
        }
    if( maxY < h - 1 ) {
        mColoredRadiusBottomY = ( maxY + 1 ) / (double)h - 0.5;
        }
    }




SpriteGL::SpriteGL( Image *inImage,
                    char inTransparentLowerLeftCorner,
                    int inNumFrames,
                    int inNumPages, char inSetColoredRadii ) {
    
    initTexture( inImage, inTransparentLowerLeftCorner, inNumFrames,
                 inNumPages, inSetColoredRadii );
    }
 


void SpriteGL::initTexture( Image *inImage,
                            char inTransparentLowerLeftCorner,
                            int inNumFrames,
                            int inNumPages, char inSetColoredRadii ) {
    
    mColoredRadiusLeftX = 0.5;
    mColoredRadiusRightX = 0.5;
    mColoredRadiusTopY = 0.5;
    mColoredRadiusBottomY = 0.5;

    mNumFrames = inNumFrames;
    mNumPages = inNumPages;
    
    Image *spriteImage = inImage;
    
    Image *imageToDelete = NULL;
    
    if( inTransparentLowerLeftCorner ) {
        
        char generateAlpha = false;
        
        if( spriteImage->getNumChannels() < 4 ) {
            generateAlpha = true;
            }
        else {
            // check if existing alpha is all 1.0
            // if so, override it
            // (but if it contains some information, keep it and ignore
            //  transparency color from corner)
            
            generateAlpha = true;
            
            int numPixels = 
                spriteImage->getWidth() * spriteImage->getHeight();
            
            double *alpha = spriteImage->getChannel( 3 );
            
            for( int i=0; i<numPixels; i++ ) {
                if( alpha[i] != 1.0 ) {
                    generateAlpha = false;
                    break;
                    }
                }
            }
        

        if( generateAlpha ) {        
            // use lower-left corner color as transparent color for alpha
            spriteImage = spriteImage->generateAlphaChannel();
            imageToDelete = spriteImage;
            }
        }
    
    if( inSetColoredRadii ) {
        findColoredRadii( spriteImage );
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
    mCurrentPage = 0;
    
    mCenterOffset.x = 0;
    mCenterOffset.y = 0;
    }



SpriteGL::SpriteGL( unsigned char *inRGBA, 
                    unsigned int inWidth, unsigned int inHeight,
                    int inNumFrames,
                    int inNumPages,
                    char inSetColoredRadii ) {

    mColoredRadiusLeftX = 0.5;
    mColoredRadiusRightX = 0.5;
    mColoredRadiusTopY = 0.5;
    mColoredRadiusBottomY = 0.5;
    
    mNumFrames = inNumFrames;
    mNumPages = inNumPages;

    if( inSetColoredRadii ) {
        findColoredRadii( inRGBA, inWidth, inHeight );
        }
    
    mTexture = new SingleTextureGL( inRGBA, inWidth, inHeight,
                                    // no wrap
                                    false,
                                    sGenerateMipMaps );    

    mWidth = inWidth;
    mHeight = inHeight;
    
    mBaseScaleX = inWidth / mNumPages;
    mBaseScaleY = inHeight / mNumFrames;
    
    
    mFlipHorizontal = false;
    mCurrentPage = 0;

    mCenterOffset.x = 0;
    mCenterOffset.y = 0;
    }



SpriteGL::SpriteGL( char inAlphaOnly,
                    unsigned char *inA, 
                    unsigned int inWidth, unsigned int inHeight, 
                    int inNumFrames,
                    int inNumPages,
                    char inSetColoredRadii ) {

    mColoredRadiusLeftX = 0.5;
    mColoredRadiusRightX = 0.5;
    mColoredRadiusTopY = 0.5;
    mColoredRadiusBottomY = 0.5;
    
    mNumFrames = inNumFrames;
    mNumPages = inNumPages;

    if( inSetColoredRadii ) {
        findColoredRadiiAlpha( inA, inWidth, inHeight );
        }

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
    mCurrentPage = 0;

    mCenterOffset.x = 0;
    mCenterOffset.y = 0;
    }




SpriteGL::~SpriteGL() {
    delete mTexture;
    }




// NOTE
// the GLES version of this is experimental and broken in terms of 
// rotations and center offsets
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
    



    float xLeftRadius = (float)( inScale * mBaseScaleX * mColoredRadiusLeftX );
    float xRightRadius = 
        (float)( inScale * mBaseScaleX * mColoredRadiusRightX );
    
    float yTopRadius = (float)( inScale * mBaseScaleY * mColoredRadiusTopY );
    float yBottomRadius = 
        (float)( inScale * mBaseScaleY * mColoredRadiusBottomY );

    doublePair centerOffset = mult( mCenterOffset, inScale );


    if( !(mFlipHorizontal) != !(inFlipH) ) {
        // make sure flips don't override each other, xor
        
        xLeftRadius = -xLeftRadius;
        xRightRadius = -xRightRadius;
        centerOffset.x = - centerOffset.x;
        }
    


    // first, set up corners relative to 0,0
    // loop is unrolled here, with all offsets added in
    // also, mZ ignored now, since rotation no longer done
    float posX = (float)( inPosition->mX - centerOffset.x );
    float posY = (float)( inPosition->mY + centerOffset.y );
    
    squareVertices[0] = posX - xLeftRadius;
    squareVertices[1] = posY - yBottomRadius;
    
    squareVertices[2] = posX + xRightRadius; 
    squareVertices[3] = posY - yBottomRadius;
    
    squareVertices[4] = posX - xLeftRadius; 
    squareVertices[5] = posY + yTopRadius;
    
    squareVertices[6] = posX + xRightRadius; 
    squareVertices[7] = posY + yTopRadius;
    
    
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

    
    textXA += (float)( 0.5 - mColoredRadiusLeftX );
    textXB -= (float)( 0.5 - mColoredRadiusRightX );


    float textYB = (1.0f / mNumFrames) * inFrame;
    float textYA = textYB + (1.0f / mNumFrames );
    
    
    textYA += (float)( 0.5 - mColoredRadiusTopY );
    textYB -= (float)( 0.5 - mColoredRadiusBottomY );



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
    

    double xLeftRadius = inScale * mBaseScaleX * mColoredRadiusLeftX;
    double xRightRadius = inScale * mBaseScaleX * mColoredRadiusRightX;

    double yTopRadius = inScale * mBaseScaleY * mColoredRadiusTopY;
    double yBottomRadius = inScale * mBaseScaleY * mColoredRadiusBottomY;

    doublePair centerOffset = mult( mCenterOffset, inScale );


    if( !(mFlipHorizontal) != !(inFlipH) ) {
        // make sure flips don't override eachother, xor

        xLeftRadius = -xLeftRadius;
        xRightRadius = -xRightRadius;
        centerOffset.x = - centerOffset.x;
        }
    


    // first, set up corners relative to 0,0
    // loop is unrolled here, with all offsets added in
    // also, mZ ignored now, since rotation no longer done

    if( inRotation == 0 ) {
        double posX = inPosition->mX - centerOffset.x;
        double posY = inPosition->mY + centerOffset.y;
        
        corners[0].mX = posX - xLeftRadius;
        corners[0].mY = posY - yBottomRadius;
        //corners[0].mZ = 0;
        
        corners[1].mX = posX + xRightRadius;
        corners[1].mY = posY - yBottomRadius;
        //corners[1].mZ = 0;
        
        corners[2].mX = posX + xRightRadius;
        corners[2].mY = posY + yTopRadius;
        //corners[2].mZ = 0;
        
        corners[3].mX = posX - xLeftRadius;
        corners[3].mY = posY + yTopRadius;
        //corners[3].mZ = 0;
        }
    else {
        double posX = inPosition->mX;
        double posY = inPosition->mY;

        corners[0].mX = - xLeftRadius - centerOffset.x;
        corners[0].mY = - yBottomRadius + centerOffset.y;
        //corners[0].mZ = 0;
        
        corners[1].mX = xRightRadius - centerOffset.x;
        corners[1].mY = - yBottomRadius + centerOffset.y;
        //corners[1].mZ = 0;
        
        corners[2].mX = xRightRadius - centerOffset.x;
        corners[2].mY = yTopRadius + centerOffset.y;
        //corners[2].mZ = 0;
        
        corners[3].mX = - xLeftRadius - centerOffset.x;
        corners[3].mY = yTopRadius + centerOffset.y;

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
    
    textXA += 0.5 - mColoredRadiusLeftX;
    textXB -= 0.5 - mColoredRadiusRightX;
    

    textYB = (1.0 / mNumFrames) * inFrame;
    textYA = textYB + (1.0 / mNumFrames );

    textYB += 0.5 - mColoredRadiusTopY;
    textYA -= 0.5 - mColoredRadiusBottomY;
    }


        
extern int numPixelsDrawn;


void SpriteGL::draw( int inFrame, 
                     Vector3D *inPosition, 
                     double inScale,
                     char inLinearMagFilter,
                     char inMipMapFilter,
                     double inRotation,
                     char inFlipH ) {
    // numPixelsDrawn += 
    //    ( mColoredRadiusRightX + mColoredRadiusLeftX ) * mWidth *
    //    ( mColoredRadiusTopY + mColoredRadiusBottomY ) * mHeight;

        
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

    /*
      // for debugging
      // show red square around each sprite
    
    FloatColor oldColor = getDrawColor();
    setDrawColor( 1, 0, 0, 0.75 );
    glBegin( GL_LINE_LOOP ); {
        glVertex2d( corners[0].mX, corners[0].mY );
        
        glVertex2d( corners[1].mX, corners[1].mY );
        
        glVertex2d( corners[2].mX, corners[2].mY );
        
        glVertex2d( corners[3].mX, corners[3].mY );
        }
    glEnd();
    setDrawColor( oldColor );
    */
    if( sCountingPixels ) {
        double increment = 
            ( corners[2].mX - corners[0].mX ) *
            ( corners[2].mY - corners[0].mY );        
        sPixelsDrawn += increment;
        }   

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


