#ifndef FONT_INCLUDED
#define FONT_INCLUDED


#include "minorGems/game/gameGraphics.h"


enum TextAlignment {
    alignCenter = 0,
    alignRight,
    alignLeft 
    };


class Font {
        
    public:
        
        // file contains TGA with 16x16 ascii table
        Font( const char *inFileName, int inCharSpacing, int inSpaceWidth,
              char inFixedWidth,
              double inScaleFactor = 1.0 );
        
        ~Font();
        
        
        // draws a string on the current screen using set draw color
        // returns x coordinate of string end
        double drawString( const char *inString, doublePair inPosition,
                           TextAlignment inAlign = alignCenter );


        double measureString( const char *inString );

    private:
        
        // returns x coordinate to right of drawn character
        double drawCharacter( unsigned char inC, doublePair inPosition );
        
        
        double mScaleFactor;
        
        
        int mCharSpacing;
        int mSpaceWidth;
        
        char mFixedWidth;
        
        int mSpriteWidth;
        int mSpriteHeight;
        
        // maps ascii chars to sprite IDs
        SpriteHandle mSpriteMap[ 256 ];
        
        // for kerning (ignored if fixed width flag on)
        int mCharLeftEdgeOffset[ 256 ];
        int mCharWidth[ 256 ];
        
        
    };


#endif
