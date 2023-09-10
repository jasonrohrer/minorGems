#include "Font.h"

#include "minorGems/graphics/RGBAImage.h"

#include <string.h>
#include <iostream>
#include "minorGems/util/SettingsManager.h"
#include "minorGems/graphics/openGL/SingleTextureGL.h"


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

unicode utf8ToCodepoint(const unsigned char *&p)
{
    unicode codepoint = 0;
    if ((*p & 0x80) == 0)
    {
        codepoint = *p;
        p++;
    }
    else if ((*p & 0xE0) == 0xC0)
    {
        codepoint = (*p & 0x1F) << 6;
        p++;
        codepoint |= (*p & 0x3F);
        p++;
    }
    else if ((*p & 0xF0) == 0xE0)
    {
        codepoint = (*p & 0x0F) << 12;
        p++;
        codepoint |= (*p & 0x3F) << 6;
        p++;
        codepoint |= (*p & 0x3F);
        p++;
    }
    else if ((*p & 0xF8) == 0xF0)
    {
        codepoint = (*p & 0x07) << 18;
        p++;
        codepoint |= (*p & 0x3F) << 12;
        p++;
        codepoint |= (*p & 0x3F) << 6;
        p++;
        codepoint |= (*p & 0x3F);
        p++;
    }
    else
    {
        std::cout << "Not UTF-8: " << (char)*p << ',' << (int)*p << ',' << std::endl;
        p++;
    }
    return codepoint;
}

// output should have the size of utf8String
void utf8ToUnicode(const char *utf8String, unicode* const output)
{
    const unsigned char *p = (const unsigned char *)utf8String;
    int len = 0;

    while (*p)
    {
        output[len] = utf8ToCodepoint(p);
        ++len;
    }

    output[len] = 0;
}

size_t strlen(const unicode *u)
{
    int i = 0;
    while (u[i])
        ++i;
    return i;
}

static int unicodeWide = 22;
static double unicodeScale = 1.4;
static int unicodeOffset = -5;
static xCharTexture g_TexID[65536];

 
void xFreeTypeLib::load(const char* font_file , int _w , int _h)  
{  
    FT_Library library;  
    if (FT_Init_FreeType( &library) )
    {
        std::cout << "Font library init error" << std::endl;
        exit(0);
    }
    // load a font file, get the default face, should be Regualer  
    if (FT_New_Face( library, font_file, 0, &mFTFace ))   
    {
        std::cout << "Can't read " << font_file << std::endl;
        exit(0);
    }
    // select charmap  
    FT_Select_Charmap(mFTFace, FT_ENCODING_UNICODE);  
    // set font width and height 
    FT_Set_Pixel_Sizes(mFTFace, _w, _h);  
}  
  
char xFreeTypeLib::loadChar(unicode ch)  
{  
    if(g_TexID[ch].mTexID)  
        return true;  
    // load char image(will replace old char image)
    if(FT_Load_Char(mFTFace, ch, FT_LOAD_DEFAULT))
    {  
        return false;  
    }
  
    xCharTexture& charTex = g_TexID[ch];  
   
    FT_Glyph glyph;  
    // copy from glyph slot to glyph, this function return error code and set glyph
    if(FT_Get_Glyph( mFTFace->glyph, &glyph ))  
        return false;  
  
    // change to bitmap
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );  
  
    // get bitmap  
    FT_Bitmap& bitmap = ((FT_BitmapGlyph)glyph)->bitmap;  
  
    int width = bitmap.width;  
    int height = bitmap.rows;  
   
    charTex.mWidth = width;
    charTex.mHeight = height;  
  
    glGenTextures(1, &charTex.mTexID);  
    SingleTextureGL::sLastBoundTextureID = charTex.mTexID;
    glBindTexture(GL_TEXTURE_2D, charTex.mTexID);  
    char* pBuf = new char[width * height * 4];  
    for(int j=0; j < height; j++)  
    {  
        for(int i=0; i < width; i++)  
        {  
            unsigned char _vl = bitmap.buffer[i + bitmap.width*j];  
            pBuf[(4*i + (height - j - 1) * width * 4)  ] = 0xFF;  
            pBuf[(4*i + (height - j - 1) * width * 4)+1] = 0xFF;  
            pBuf[(4*i + (height - j - 1) * width * 4)+2] = 0xFF;  
            pBuf[(4*i + (height - j - 1) * width * 4)+3] = _vl;  
        }  
     }  
  
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuf);  // map a 2D texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    delete[] pBuf;  
    FT_Done_Glyph(glyph);
    return true;  
}  


xFreeTypeLib g_FreeTypeLib;  


xCharTexture* getTextChar(unicode ch)  
{  
    char result = g_FreeTypeLib.loadChar(ch);  
    return result ? &g_TexID[ch] : NULL;  
}
  
void init(int size)  
{  
    unicodeScale = SettingsManager::getFloatSetting( "unicodeScale", 1.4 );
    unicodeWide = SettingsManager::getIntSetting( "unicodeWide", 22 );
    unicodeOffset = SettingsManager::getIntSetting( "unicodeOffset", -5 );

    size *= unicodeScale;
    g_FreeTypeLib.load("graphics/font.ttf", size, size);  
}  
  
static char isInit = false;

Font::Font( const char *inFileName, int inCharSpacing, int inSpaceWidth,
            char inFixedWidth, double inScaleFactor, int inFixedCharWidth )
        : mScaleFactor( inScaleFactor ),
          mCharSpacing( inCharSpacing ), mSpaceWidth( inSpaceWidth ),
          mFixedWidth( inFixedWidth ), mEnableKerning( true ),
          mMinimumPositionPrecision( 0 ) {

    if(strcmp(inFileName, "font_pencil_erased_32_32.tga") == 0)
        isErased = true;
    
    if(!isInit) {
        init((int)inScaleFactor);
        isInit = true;
    }

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
        
        if( mSpriteHeight == mSpriteWidth ) {
            mAccentsPresent = false;
            }
        else {
            mAccentsPresent = true;
            }

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
        if( !mFixedWidth ) {
            
            // first, compute left and right extremes for each pixel
            // row of each character
            int *rightExtremes[256];
            int *leftExtremes[256];
            
            for( int i=0; i<256; i++ ) {
                rightExtremes[i] = new int[ mSpriteHeight ];
                leftExtremes[i] = new int[ mSpriteHeight ];
                
                if( savedCharacterRGBA[i] != NULL ) {
                    for( int y=0; y<mSpriteHeight; y++ ) {
                        
                        int rightExtreme = 0;
                        int leftExtreme = mSpriteWidth;
                            
                        for( int x=0; x<mSpriteWidth; x++ ) {
                            int p = y * mSpriteWidth + x;
                                
                            if( savedCharacterRGBA[i][p].comp.a > inkA ) {
                                rightExtreme = x;
                                }
                            if( x < leftExtreme &&
                                savedCharacterRGBA[i][p].comp.a > inkA ) {
                                
                                leftExtreme = x;
                                }
                            // also check pixel rows above and below
                            // for left character, to look for
                            // diagonal collisions (perfect nesting
                            // with no vertical gap)
                            if( y > 0 && x < leftExtreme ) {
                                int pp = (y-1) * mSpriteWidth + x;
                                if( savedCharacterRGBA[i][pp].comp.a 
                                    > inkA ) {
                                    
                                    leftExtreme = x;
                                    }
                                }
                            if( y < mSpriteHeight - 1 
                                && x < leftExtreme ) {
                                
                                int pp = (y+1) * mSpriteWidth + x;
                                if( savedCharacterRGBA[i][pp].comp.a 
                                    > inkA ) {
                                    
                                    leftExtreme = x;
                                    }
                                }
                            }
                        
                        rightExtremes[i][y] = rightExtreme;
                        leftExtremes[i][y] = leftExtreme;
                        }
                    }
                }
            


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
                            
                                int rightExtreme = rightExtremes[i][y];
                                int leftExtreme = leftExtremes[j][y];
                            
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
                            // based on character width, so take that into 
                            // account
                            // 
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
                                // for characters that don't collide
                                // horizontally at all
                                && minDistance < mCharWidth[i] ) {
                            
                                mKerningTable[i]->offset[j] = - minDistance;
                                }
                            }
                        }
                
                    }
                }

            for( int i=0; i<256; i++ ) {
                delete [] rightExtremes[i];
                delete [] leftExtremes[i];
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



void Font::copySpacing( Font *inOtherFont ) {
    memcpy( mCharLeftEdgeOffset, inOtherFont->mCharLeftEdgeOffset,
            256 * sizeof( int ) );

    memcpy( mCharWidth, inOtherFont->mCharWidth,
            256 * sizeof( int ) );
    

    for( int i=0; i<256; i++ ) {
        if( mKerningTable[i] != NULL ) {
            delete mKerningTable[i];
            mKerningTable[i] = NULL;
            }

        if( inOtherFont->mKerningTable[i] != NULL ) {
            mKerningTable[i] = new KerningTable;
            memcpy( mKerningTable[i]->offset,
                    inOtherFont->mKerningTable[i]->offset,
                    256 * sizeof( short ) );
            }
        }

    mScaleFactor = inOtherFont->mScaleFactor;
        
    
    mCharSpacing = inOtherFont->mCharSpacing;
    mSpaceWidth = inOtherFont->mSpaceWidth;
        
    mFixedWidth = inOtherFont->mFixedWidth;
        
    mSpriteWidth = inOtherFont->mSpriteWidth;
    mSpriteHeight = inOtherFont->mSpriteHeight;
    
    mAccentsPresent = inOtherFont->mAccentsPresent;
        

    mCharBlockWidth = inOtherFont->mCharBlockWidth;
    }



// double pixel size
static double scaleFactor = 1.0 / 16;
//static double scaleFactor = 1.0 / 8;


void Font::drawChar(unicode c, doublePair inCenter) {
    double scale = scaleFactor * mScaleFactor;
    if(c < 128) {
        if(mSpriteMap[c] != NULL)
            drawSprite( mSpriteMap[c], inCenter, scale );
        return;
    }

    xCharTexture* pCharTex = getTextChar(c);  
    if(pCharTex == NULL)
        return;
    float alpha = getDrawColor().a;
    if(isErased)
        setDrawFade(alpha * 0.1);
    
    SingleTextureGL::sLastBoundTextureID = pCharTex->mTexID;
    glBindTexture(GL_TEXTURE_2D, pCharTex->mTexID); // bind to target texture
    int w = pCharTex->mWidth;  
    int h = pCharTex->mHeight;
    int ch_x = inCenter.x - w / 2 + unicodeOffset;
    int ch_y = inCenter.y - h / 2;
    glBegin ( GL_QUADS );
    {  
        glTexCoord2f(0.0f, 1.0f); glVertex2f(ch_x, ch_y + h);  
        glTexCoord2f(1.0f, 1.0f); glVertex2f(ch_x + w, ch_y + h);  
        glTexCoord2f(1.0f, 0.0f); glVertex2f(ch_x + w, ch_y);  
        glTexCoord2f(0.0f, 0.0f); glVertex2f(ch_x, ch_y);  
    }  
    glEnd();  

    if(isErased)
        setDrawFade(alpha);
}

double Font::getCharSpacing() {
    double scale = scaleFactor * mScaleFactor;
    
    return mCharSpacing * scale;
    }


double Font::getCharPos( SimpleVector<doublePair> *outPositions,
                         const char *inString, doublePair inPosition,
                         TextAlignment inAlign ) {
    unicode unicodeString[strlen(inString)];
    utf8ToUnicode(inString, unicodeString);
    return getCharPos(outPositions, unicodeString, inPosition, inAlign);
}
double Font::getCharPos( SimpleVector<doublePair> *outPositions,
                         const unicode *inString, doublePair inPosition,
                         TextAlignment inAlign ) {

    double scale = scaleFactor * mScaleFactor;
    
    unsigned int numChars = strlen( inString );
    
    double x = inPosition.x;
    
    
    double y = inPosition.y;

    // compensate for extra headspace in accent-equipped font files
    if( mAccentsPresent ) { 
        y += scale * mSpriteHeight / 4;
        }

    
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


    if( mMinimumPositionPrecision > 0 ) {
        x /= mMinimumPositionPrecision;
        
        x = lrint( floor( x ) );
        
        x *= mMinimumPositionPrecision;
        }
    

    for( unsigned int i=0; i<numChars; i++ ) {
        doublePair charPos = { x, y };
        
        doublePair drawPos;
        
        double charWidth = positionCharacter( inString[i], 
                                              charPos, &drawPos );
        outPositions->push_back( drawPos );
        
        x += charWidth + mCharSpacing * scale;
        
        if( !mFixedWidth && mEnableKerning 
            && i < numChars - 1 
            && inString[i] < 128
            && inString[i+1] < 128
            && mKerningTable[ inString[i] ] != NULL ) {
            // there's another character after this
            // apply true kerning adjustment to the pair
            int offset = mKerningTable[ inString[i] ]->
                offset[ inString[i+1] ];
            x += offset * scale;
            }
        }
    // no spacing after last character
    x -= mCharSpacing * scale;

    return x;
    }


double Font::drawString( const char *inString, doublePair inPosition,
                         TextAlignment inAlign ) {
    if( !SingleTextureGL::sTexturingEnabled ) {    
        glEnable( GL_TEXTURE_2D );
        SingleTextureGL::sTexturingEnabled = true;
        }

    unicode unicodeString[strlen(inString)];
    utf8ToUnicode(inString, unicodeString);
    SimpleVector<doublePair> pos( strlen( unicodeString ) );

    double returnVal = getCharPos( &pos, unicodeString, inPosition, inAlign );
    
    for( int i=0; i<pos.size(); i++ ) {
        drawChar(unicodeString[i], pos.getElementDirect(i));
        }
    
    return returnVal;
    }




double Font::positionCharacter( unicode inC, doublePair inTargetPos,
                                doublePair *outActualPos ) {
    *outActualPos = inTargetPos;
    
    double scale = scaleFactor * mScaleFactor;

    if( inC == ' ' ) {
        return mSpaceWidth * scale;
        }

    if( !mFixedWidth ) {
        outActualPos->x -= ( inC < 128 ? mCharLeftEdgeOffset[ inC ] : 0 ) * scale;
        }
    
    if( mFixedWidth ) {
        return mCharBlockWidth * scale;
        }
    else {
        return ( inC < 128 ? mCharWidth[ inC ] : unicodeWide ) * scale;
        }
    }

    


double Font::drawCharacter( unicode inC, doublePair inPosition ) {
    
    doublePair drawPos;
    double returnVal = positionCharacter( inC, inPosition, &drawPos );

    if( inC == ' ' ) {
        return returnVal;
        }

    drawChar(inC, drawPos);
    
    return returnVal;
    }



void Font::drawCharacterSprite( unicode inC, doublePair inPosition ) {
    drawChar(inC, inPosition);
    }

double Font::measureString( const char *inString, int inCharLimit ) {
    unicode unicodeString[strlen(inString)];
    utf8ToUnicode(inString, unicodeString);
    return measureString(unicodeString, inCharLimit);
}
double Font::measureString( const unicode *inString, int inCharLimit ) {
    double scale = scaleFactor * mScaleFactor;

    int numChars = inCharLimit;

    if( numChars == -1 ) {
        // no limit, measure whole string
        numChars = strlen( inString );
        }
    
    double width = 0;
    
    for( int i=0; i<numChars; i++ ) {
        unicode c = inString[i];
        
        if( c == ' ' ) {
            width += mSpaceWidth * scale;
            }
        else if( mFixedWidth ) {
            width += mCharBlockWidth * scale;
            }
        else {
            width += ( c < 128 ? mCharWidth[ c ] : unicodeWide ) * scale;

            if( mEnableKerning
                && i < numChars - 1
                && inString[i] < 128
                && inString[i+1] < 128
                && mKerningTable[ inString[i] ] != NULL ) {
                // there's another character after this
                // apply true kerning adjustment to the pair
                int offset = mKerningTable[ inString[i] ]->
                    offset[ inString[i+1] ];
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
    double accentFactor = 1.0f;
    
    if( mAccentsPresent ) {
        accentFactor = 0.5f;
        }
    
    return scaleFactor * mScaleFactor * mSpriteHeight * accentFactor;
    }



void Font::enableKerning( char inKerningOn ) {
    mEnableKerning = inKerningOn;
    }



void Font::setMinimumPositionPrecision( double inMinimum ) {
    mMinimumPositionPrecision = inMinimum;
    }



