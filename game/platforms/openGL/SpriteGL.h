#ifndef SPRITE_INCLUDED
#define SPRITE_INCLUDED


#include "minorGems/graphics/openGL/SingleTextureGL.h"
#include "minorGems/game/gameGraphics.h"


#include "minorGems/math/geometry/Vector3D.h"

#include <stdlib.h>



class SpriteGL{
    public:
        
        // toggles for subsequent constructor calls
        static void toggleMipMapGeneration( char inGenerateMipMaps ) {
            sGenerateMipMaps = inGenerateMipMaps;
            }
            
        

        // transparent color for RGB images can be taken from lower-left
        // corner pixel
        // lower-left corner ignored for RGBA images
        // image split vertically into inNumFrames of equal size
        SpriteGL( Image *inImage,
                char inTransparentLowerLeftCorner = false,
                int inNumFrames = 1,
                int inNumPages = 1 );
        
        
        SpriteGL( unsigned char *inRGBA, 
                  unsigned int inWidth, unsigned int inHeight,
                  int inNumFrames = 1,
                  int inNumPages = 1 );

        // one-channel, alpha-only, with other channels black
        // extra parameter just to differentiate function calls
        SpriteGL( char inAlphaOnly,
                  unsigned char *inA, 
                  unsigned int inWidth, unsigned int inHeight,
                  int inNumFrames = 1,
                  int inNumPages = 1 );


        ~SpriteGL();
        

        void draw( int inFrame,
                   Vector3D *inPosition,
                   double inScale = 1,
                   char inLinearMagFilter = false,
                   char inMipMapFilter = false,
                   // rotation where 0.25 = 90 degrees clockwise, 
                   // and 1.0 is a full  rotation
                   double inRotation = 0,
                   char inFlipH = false );
        
        // draw sprite with separate colors set for each corner
        // corners in BL, BR, TR, TL order
        void draw( int inFrame,
                   Vector3D *inPosition,
                   FloatColor inCornerColors[4],
                   double inScale = 1,
                   char inLinearMagFilter = false,
                   char inMipMapFilter = false,
                   double inRotation = 0,
                   char inFlipH = false );
        


        int getNumFrames() {
            return mNumFrames;
            }
        


        void setFlipHorizontal( char inFlip ) {
            mFlipHorizontal = inFlip;
            }
        
        // used to adjust horizontal center point, in pixels
        void setHorizontalOffset( double inOffset ) {
            mHorizontalOffset = inOffset;
            }
        

        void setPage( int inPage ) {
            mCurrentPage = inPage;
            }


        int getPage() {
            return mCurrentPage;
            }
        
        

        double getBaseScaleX() {
            return mBaseScaleX;
            }
        
        double getBaseScaleY() {
            return mBaseScaleY;
            }
        

    protected:

        static char sGenerateMipMaps;
        
        SingleTextureGL *mTexture;
        
        int mNumFrames;
        int mNumPages;
        
        double mBaseScaleX;
        double mBaseScaleY;

        char mFlipHorizontal;
        double mHorizontalOffset;
        
        int mCurrentPage;
        

        void initTexture( Image *inImage,
                          char inTransparentLowerLeftCorner = false,
                          int inNumFrames = 1,
                          int inNumPages = 1 );
        
        

        // common code for two different drawing functions
        void prepareDraw( int inFrame, 
                          Vector3D *inPosition, 
                          double inScale,
                          char inLinearMagFilter,
                          char inMipMapFilter,
                          double inRotation,
                          char inFlipH );
        
    };



#endif
