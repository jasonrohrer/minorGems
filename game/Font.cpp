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


// what alpha level counts as "ink" when measuring character width
// and doing kerning
// values at or below this level will not count as ink
// this improves kerning and font spacing, because dim "tips" of pointed
// glyphs don't cause the glyph to be logically wider than it looks visually 
const unsigned char inkA = 127;



Font::Font( const char *inFileName, int inCharSpacing, int inSpaceWidth,
            char inFixedWidth, double inScaleFactor, int inFixedCharWidth )
        : mScaleFactor( inScaleFactor ),
          mCharSpacing( inCharSpacing ), mSpaceWidth( inSpaceWidth ),
          mFixedWidth( inFixedWidth ), mEnableKerning( true ) {

    for( int i=0; i<256; i++ ) {
        mSpriteMap[i] = NULL;
        mKerningTable[i] = NULL;
        }
    


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
        
        

        // use red channel intensity as transparency
        // make entire image solid white and use transparency to 
        // mask it

        for( int i=0; i<numPixels; i++ ) {
            spriteRGBA[i].comp.a = spriteRGBA[i].comp.r;
            
            spriteRGBA[i].comp.r = 255;
            spriteRGBA[i].comp.g = 255;
            spriteRGBA[i].comp.b = 255;
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
            
        // hold onto these for true kerning after
        // we've read this data for all characters
        rgbaColor *savedCharacterRGBA[256];
        

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
                        
                        unsigned char a = 
                            charRGBA[ y * mSpriteWidth + x ].comp.a;
                        
                        if( a > inkA ) {
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
                

            if( !allTransparent && ! mFixedWidth ) {
                savedCharacterRGBA[i] = charRGBA;
                }
            else {
                savedCharacterRGBA[i] = NULL;
                delete [] charRGBA;
                }
            }
        

        // now that we've read in all characters, we can do real kerning
        if( !mFixedWidth )
        for( int i=0; i<256; i++ ) {
            if( savedCharacterRGBA[i] != NULL ) {
                
                mKerningTable[i] = new KerningTable;


                // for each character that could come after this character
                for( int j=0; j<256; j++ ) {

                    mKerningTable[i]->offset[j] = 0;

                    // not a blank character
                    if( savedCharacterRGBA[j] != NULL ) {
                        
                        short minDistance = 2 * mSpriteWidth;

                        // for each pixel row, find distance
                        // between the right extreme of the first character
                        // and the left extreme of the second
                        for( int y=0; y<mSpriteHeight; y++ ) {
                            
                            int rightExtreme = 0;
                            int leftExtreme = mSpriteWidth;
                            
                            for( int x=0; x<mSpriteWidth; x++ ) {
                                int p = y * mSpriteWidth + x;
                                
                                if( savedCharacterRGBA[i][p].comp.a > inkA ) {
                                    rightExtreme = x;
                                    }
                                if( x < leftExtreme &&
                                    savedCharacterRGBA[j][p].comp.a > inkA ) {
                                    
                                    leftExtreme = x;
                                    }
                                // also check pixel rows above and below
                                // for left character, to look for
                                // diagonal collisions (perfect nesting
                                // with no vertical gap)
                                if( y > 0 && x < leftExtreme ) {
                                    int pp = (y-1) * mSpriteWidth + x;
                                    if( savedCharacterRGBA[j][pp].comp.a 
                                        > inkA ) {
                                    
                                        leftExtreme = x;
                                        }
                                    }
                                if( y < mSpriteHeight - 1 
                                    && x < leftExtreme ) {
                                    
                                    int pp = (y+1) * mSpriteWidth + x;
                                    if( savedCharacterRGBA[j][pp].comp.a 
                                        > inkA ) {
                                    
                                        leftExtreme = x;
                                        }
                                    }
                                }
                            
                            int rowDistance =
                                ( mSpriteWidth - rightExtreme - 1 ) 
                                + leftExtreme;

                            if( rowDistance < minDistance ) {
                                minDistance = rowDistance;
                                }
                            }
                        
                        // have min distance across all rows for 
                        // this character pair

                        // of course, we've already done pseudo-kerning
                        // based on character width, so take that into account
                        // true kerning is a tweak to that
                        
                        // pseudo-kerning already accounts for
                        // gap to left of second character
                        minDistance -= mCharLeftEdgeOffset[j];
                        // pseudo-kerning already accounts for gap to right
                        // of first character
                        minDistance -= 
                            mSpriteWidth - 
                            ( mCharLeftEdgeOffset[i] + mCharWidth[i] );
                        
                        if( minDistance > 0 
                            // make sure we don't have a full overhang
                            // for characters that don't collide horizontally
                            // at all
                            && minDistance < mCharWidth[i] ) {
                            
                            mKerningTable[i]->offset[j] = - minDistance;
                            }
                        }
                    }
                
                }
            }
        

        for( int i=0; i<256; i++ ) {
            if( savedCharacterRGBA[i] != NULL ) {
                delete [] savedCharacterRGBA[i];
                }
            }
        

        delete [] spriteRGBA;
        }
    }



Font::~Font() {
    for( int i=0; i<256; i++ ) {
        if( mSpriteMap[i] != NULL ) {
            freeSprite( mSpriteMap[i] );
            }
        if( mKerningTable[i] != NULL ) {
            delete mKerningTable[i];
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
        
        if( !mFixedWidth && mEnableKerning 
            && i < numChars - 1 
            && mKerningTable[(unsigned char)( inString[i] )] != NULL ) {
            // there's another character after this
            // apply true kerning adjustment to the pair
            int offset = mKerningTable[ (unsigned char)( inString[i] ) ]->
                offset[ (unsigned char)( inString[i+1] ) ];
            x += offset * scale;
            }
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

            if( mEnableKerning
                && i < numChars - 1 
                && mKerningTable[(unsigned char)( inString[i] )] != NULL ) {
                // there's another character after this
                // apply true kerning adjustment to the pair
                int offset = mKerningTable[ (unsigned char)( inString[i] ) ]->
                    offset[ (unsigned char)( inString[i+1] ) ];
                width += offset * scale;
                }
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



void Font::enableKerning( char inKerningOn ) {
    mEnableKerning = inKerningOn;
    }


