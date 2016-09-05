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
        // lower-left corner ignored for RGBA images unless A channel 
        // is solid 1.0
        // image split vertically into inNumFrames of equal size
        //
        // NOTE:  colored radius computation does NOT work for sprites
        // with frames and pages, leave false if using frames and pages
        SpriteGL( Image *inImage,
                  char inTransparentLowerLeftCorner = false,
                  int inNumFrames = 1,
                  int inNumPages = 1,
                  char inSetColoredRadii = false );
        
        
        SpriteGL( unsigned char *inRGBA, 
                  unsigned int inWidth, unsigned int inHeight,
                  int inNumFrames = 1,
                  int inNumPages = 1,
                  char inSetColoredRadii = false );

        // one-channel, alpha-only, with other channels black
        // extra parameter just to differentiate function calls
        SpriteGL( char inAlphaOnly,
                  unsigned char *inA, 
                  unsigned int inWidth, unsigned int inHeight,
                  int inNumFrames = 1,
                  int inNumPages = 1,
                  char inSetColoredRadii = false );


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
        


        // these ignore pages and frames, returning
        // total image size
        int getWidth() {
            return mWidth;
            }

        
        int getHeight() {
            return mHeight;
            }
        
        


    protected:

        static char sGenerateMipMaps;
        
        SingleTextureGL *mTexture;
        
        int mNumFrames;
        int mNumPages;
        
        int mWidth, mHeight;

        double mBaseScaleX;
        double mBaseScaleY;

        // these are in range -0.5 to 0.5
        // radius from center of furthest pixel column with some non-trans
        // pixel in it
        double mColoredRadiusLeftX;
        double mColoredRadiusRightX;
        // same for non-trans rows
        double mColoredRadiusTopY;
        double mColoredRadiusBottomY;
        // for fully-opaque sprites, 
        // left= 0.5, right=0.5, top=0.5, and bottom =0.5;


        
        char mFlipHorizontal;
        double mHorizontalOffset;
        
        int mCurrentPage;
        

        void initTexture( Image *inImage,
                          char inTransparentLowerLeftCorner = false,
                          int inNumFrames = 1,
                          int inNumPages = 1,
                          char inSetColoredRadii = false );
        
        

        // common code for two different drawing functions
        void prepareDraw( int inFrame, 
                          Vector3D *inPosition, 
                          double inScale,
                          char inLinearMagFilter,
                          char inMipMapFilter,
                          double inRotation,
                          char inFlipH );
        



        void findColoredRadii( Image *inImage );
        
        void findColoredRadii( unsigned char *inRGBA, 
                               int inWidth, int inHeight );
        
        void findColoredRadiiAlpha( unsigned char *inA, 
                                    int inWidth, int inHeight );

    };



#endif
