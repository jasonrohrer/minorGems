#include "Font.h"

#include "minorGems/graphics/RGBAImage.h"

#include <string.h>


typedef union rgbaColor {
        struct comp { 
                unsigned char r;
                unsigned char g;
                unsigned char b;
                unsigned char a;
            } comp;
        
        // access those bytes as an array
        unsigned char bytes[4];
        
        // reinterpret those bytes as an unsigned int
        unsigned int rgbaInt; 
    } rgbaColor;



Font::Font( const char *inFileName, int inCharSpacing, int inSpaceWidth,
            char inFixedWidth, double inScaleFactor, int inFixedCharWidth )
        : mScaleFactor( inScaleFactor ),
          mCharSpacing( inCharSpacing ), mSpaceWidth( inSpaceWidth ),
          mFixedWidth( inFixedWidth ) {


    Image *spriteImage = readTGAFile( inFileName );
    
    if( spriteImage != NULL ) {
        
        int width = spriteImage->getWidth();
        
        int height = spriteImage->getHeight();
        
        int numPixels = width * height;
        
        rgbaColor *spriteRGBA = new rgbaColor[ numPixels ];
        
        
        unsigned char *spriteBytes = 
            RGBAImage::getRGBABytes( spriteImage );
        
        delete spriteImage;

        for( int i=0; i<numPixels; i++ ) {
            
            for( int b=0; b<4; b++ ) {
                
                spriteRGBA[i].bytes[b] = spriteBytes[ i*4 + b ];
                }
            }
        
        delete [] spriteBytes;
        
        

        // use corner color as transparency
        // copy red channel into other channels
        // (ignore other channels, even for transparency)
        
        spriteRGBA[0].comp.a = 0;
        unsigned char tr;
        tr = spriteRGBA[0].comp.r;

        for( int i=0; i<numPixels; i++ ) {
            unsigned char r = spriteRGBA[i].comp.r;
            
            if( r == tr ) {
                // matches corner r
                spriteRGBA[i].comp.a = 0;
                }
                
            spriteRGBA[i].comp.g = r;
            spriteRGBA[i].comp.b = r;
            }
            
                        
                
        mSpriteWidth = width / 16;
        mSpriteHeight = height / 16;
        
        if( inFixedCharWidth == 0 ) {
            mCharBlockWidth = mSpriteWidth;
            }
        else {
            mCharBlockWidth = inFixedCharWidth;
            }


        int pixelsPerChar = mSpriteWidth * mSpriteHeight;
            
        for( int i=0; i<256; i++ ) {
            int yOffset = ( i / 16 ) * mSpriteHeight;
            int xOffset = ( i % 16 ) * mSpriteWidth;
            
            rgbaColor *charRGBA = new rgbaColor[ pixelsPerChar ];
            
            for( int y=0; y<mSpriteHeight; y++ ) {
                for( int x=0; x<mSpriteWidth; x++ ) {
                    
                    int imageIndex = (y + yOffset) * width
                        + x + xOffset;
                    int charIndex = y * mSpriteWidth + x;
                    
                    charRGBA[ charIndex ] = spriteRGBA[ imageIndex ];
                    }
                }
                
            // don't bother consuming texture ram for blank sprites
            char allTransparent = true;
            
            for( int p=0; p<pixelsPerChar && allTransparent; p++ ) {
                if( charRGBA[ p ].comp.a != 0 ) {
                    allTransparent = false;
                    }
                }
                
            if( !allTransparent ) {
                
                // convert into an image
                Image *charImage = new Image( mSpriteWidth, mSpriteHeight,
                                              4, false );
                
                for( int c=0; c<4; c++ ) {
                    double *chan = charImage->getChannel(c);
                    
                    for( int p=0; p<pixelsPerChar; p++ ) {
                        
                        chan[p] = charRGBA[p].bytes[c] / 255.0;
                        }
                    }
                

                mSpriteMap[i] = 
                    fillSprite( charImage );
                delete charImage;
                }
            else {
                mSpriteMap[i] = NULL;
                }
            

            if( mFixedWidth ) {
                mCharLeftEdgeOffset[i] = 0;
                mCharWidth[i] = mCharBlockWidth;
                }
            else if( allTransparent ) {
                mCharLeftEdgeOffset[i] = 0;
                mCharWidth[i] = mSpriteWidth;
                }
            else {
                // implement pseudo-kerning
                
                int farthestLeft = mSpriteWidth;
                int farthestRight = 0;
                
                char someInk = false;
                
                for( int y=0; y<mSpriteHeight; y++ ) {
                    for( int x=0; x<mSpriteWidth; x++ ) {
                        
                        unsigned char r = 
                            charRGBA[ y * mSpriteWidth + x ].comp.r;
                        
                        if( r > 0 ) {
                            someInk = true;
                            
                            if( x < farthestLeft ) {
                                farthestLeft = x;
                                }
                            if( x > farthestRight ) {
                                farthestRight = x;
                                }
                            }
                        }
                    }
                
                if( ! someInk  ) {
                    mCharLeftEdgeOffset[i] = 0;
                    mCharWidth[i] = mSpriteWidth;
                    }
                else {
                    mCharLeftEdgeOffset[i] = farthestLeft;
                    mCharWidth[i] = farthestRight - farthestLeft + 1;
                    }
                }
                

            delete [] charRGBA;
            }
                        
        delete [] spriteRGBA;
        }
    }



Font::~Font() {
    for( int i=0; i<256; i++ ) {
        if( mSpriteMap[i] != NULL ) {
            freeSprite( mSpriteMap[i] );
            }
        }
    }

// double pixel size
static double scaleFactor = 1.0 / 16;
//static double scaleFactor = 1.0 / 8;



double Font::drawString( const char *inString, doublePair inPosition,
                         TextAlignment inAlign ) {
    double scale = scaleFactor * mScaleFactor;
    
    unsigned int numChars = strlen( inString );
    
    double x = inPosition.x;
    
    // compensate for extra headspace in accent-equipped font files
    double y = inPosition.y + scale * mSpriteHeight / 4;
    
    double stringWidth = 0;
    
    if( inAlign != alignLeft ) {
        stringWidth = measureString( inString );
        }
    
    switch( inAlign ) {
        case alignCenter:
            x -= stringWidth / 2;
            break;
        case alignRight:
            x -= stringWidth;
            break;
        default:
            // left?  do nothing
            break;            
        }
    
    // character sprites are drawn on their centers, so the alignment
    // adjustments above aren't quite right.
    x += scale * mSpriteWidth / 2;
    

    for( unsigned int i=0; i<numChars; i++ ) {
        doublePair charPos = { x, y };
        
        double charWidth = drawCharacter( (unsigned char)( inString[i] ), 
                                          charPos );
        x += charWidth + mCharSpacing * scale;
        }
    // no spacing after last character
    x -= mCharSpacing * scale;

    return x;
    }





double Font::drawCharacter( unsigned char inC, doublePair inPosition ) {
    double scale = scaleFactor * mScaleFactor;

    if( inC == ' ' ) {
        return mSpaceWidth * scale;
        }

    if( !mFixedWidth ) {
        inPosition.x -= mCharLeftEdgeOffset[ inC ] * scale;
        }
    
    SpriteHandle spriteID = mSpriteMap[ inC ];
    
    if( spriteID != NULL ) {
        drawSprite( mSpriteMap[ inC ], inPosition, scale );
        }
    
    if( mFixedWidth ) {
        return mCharBlockWidth * scale;
        }
    else {
        return mCharWidth[ inC ] * scale;
        }
    }



double Font::measureString( const char *inString ) {
    double scale = scaleFactor * mScaleFactor;

    unsigned int numChars = strlen( inString );

    double width = 0;
    
    for( unsigned int i=0; i<numChars; i++ ) {
        unsigned char c = inString[i];
        
        if( c == ' ' ) {
            width += mSpaceWidth * scale;
            }
        else if( mFixedWidth ) {
            width += mCharBlockWidth * scale;
            }
        else {
            width += mCharWidth[ c ] * scale;
            }
    
        width += mCharSpacing * scale;
        }

    if( numChars > 0 ) {    
        // no extra space at end
        // (added in last step of loop)
        width -= mCharSpacing * scale;
        }
    
    return width;
    }



double Font::getFontHeight() {
    return scaleFactor * mScaleFactor * mSpriteHeight / 2;
    }

