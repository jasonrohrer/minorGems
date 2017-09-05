/*
 * Modification History
 *
 * 2001-October-13	Jason Rohrer
 * Created.
 * Changed to support alpha-blended fonts, even for 3-channel font textures.
 * Fixed a bug in the font coloring.
 *
 * 2001-November-3	Jason Rohrer
 * Fixed some missing includes.
 *
 * 2006-December-26   Jason Rohrer
 * Added support for variable-width characers.
 *
 * 2007-March-4   Jason Rohrer
 * Added support for character z coordinates.
 *
 * 2007-March-15   Jason Rohrer
 * Added support for pixelated characters.  Fixed font spacing issue.
 *
 * 2008-September-29   Jason Rohrer
 * Added support for font images with non-power-of-2 dimensions.
 *
 * 2010-January-29   Jason Rohrer
 * Fixed text measurement.
 *
 * 2010-May-7   Jason Rohrer
 * Extended ASCII.
 *
 * 2010-May-12   Jason Rohrer
 * Function to measure text height for Extended ASCII accents.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */



#ifndef TEXT_GL_INCLUDED
#define TEXT_GL_INCLUDED

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "minorGems/graphics/openGL/glInclude.h"

#include "minorGems/graphics/Color.h"
#include "minorGems/graphics/Image.h"

#include "minorGems/graphics/openGL/SingleTextureGL.h"

/**
 * A class for drawing text strings to an OpenGL context.
 * Fonts are texture maps.
 *
 * @author Jason Rohrer
 */
class TextGL {


		
	public:



		/**
		 * Constructs a gl text object.
		 *
		 * Note that this function can only be called
		 * after an OpenGL drawing context has been created.
		 *
		 * @param inImage the image to use as a font texture.
         *   If image is not square and height is greater than width, 
         *   extra height in each character is assumed to be the accent region
         *   where characters can extend above normal vertical space.
		 *   Should have dimensions that are a 
         *   power of 2.
         *   If dimensions are not a power of 2, then class will compensate
         *   by padding image.
		 *   Fonts in the image are assumed to be in row-major ASCII order,
		 *   with 16 characters per row.
		 *   Must be destroyed by caller.  Can be destroyed as soon as
		 *   this constructor returns.
		 * @param inUseAlpha true iff alpha blending should
		 *   be used when rendering the font textures.  If alpha
		 *   blending is enabled and the image has only 3 channels,
		 *   only the red channel (channel 0)
		 *   of the image is used:  it is interpreted as the alpha
		 *   channel for a white fontset.
         * @param inFixedWidth true iff font should be displayed in fixed
         *   width mode.  If false, a width will be measured for each
         *   character from the alpha channel.  If inUseAlpha is false,
         *   the font is always fixed width.  Defaults to true.
         * @param inCharacterSpacingFraction the extra width-fraction of
         *   spacing around each character.  A fraction of a full character
         *   width.  Space is added on *both* sides of character.
         *   Defaults to 0.1 
         * @param inSpaceWidthFraction the fraction of a full character width
         *   occupied by a space.  Defaults to 1.0 (a full width).
		 */
		TextGL( Image *inImage, char inUseAlpha = false,
                char inFixedWidth = true,
                double inCharacterSpacingFraction = 0.1,
                double inSpaceWidthFraction = 1.0 );



		virtual ~TextGL();



		/**
		 * Sets the main color of the font.
		 *
		 * If the gradient color is non-NULL, this color will be at the
		 * top of the fonts.  Otherwise, the entire extent
		 * of the font will be this color.
		 *
		 * @param inColor the main color of the font.
		 *   Will be destroyed when this class is destroyed.
		 */
		void setFontColor( Color *inColor );



		/**
		 * Gets the main color of the font.
		 *
		 * If the gradient color is non-NULL, this color will be at the
		 * top of the fonts. Otherwise, the entire extent
		 * of the font will be this color.
		 *
		 * @return the main color of the font.
		 *   Will be destroyed when this class is destroyed.
		 */
		Color *getFontColor();

		
		
		/**
		 * Sets the gradient color of the font.
		 *
		 * If non-NULL, this color will be at the
		 * bottom of the fonts.
		 *
		 * @param inColor the gradient color of the font, or
		 *   NULL to disable the gradient.
		 *   Will be destroyed when this class is destroyed.
		 */
		void setGradientColor( Color *inColor );



		/**
		 * Gets the gradient color of the font.
		 *
		 * If non-NULL, this color will be at the
		 * bottom of the fonts.
		 *
		 * @return the gradient color of the font, or
		 *   NULL if disable gradient disabled.
		 *   Will be destroyed when this class is destroyed.
		 */
		Color *getGradientColor();


        
        /**
         * Sets the z coordinate for font drawing.
         *
         * Defaults to 0.
         *
         * @param inZ the z coordinate.
         */
        void setFontZ( double inZ );

        

        /**
         * Sets smothing.
         *
         * @param inSmooth true to enable smoothing of font texture (linear
         *   interpolation) or false to show pure texture pixels (nearest
         *   neighbor).
         */
        void setSmoothing( char inSmooth );


        
		/**
		 * Draws a text string into a specified region of the
		 * context.  Text is squeezed to fit into the region.
		 *
		 * Uses the currently active OpenGL projection settings.
		 *
		 * @param inString the \0-delimited string to draw.
		 *   Must be destroyed by caller if not const.
		 * @param inX the x coordinate of the region's lower-left
		 *   corner.
		 * @param inY the y coordinate of the region's lower-left
		 *   corner.
		 * @param inWidth the width of the region.
		 * @param inHeight the height of the region.
		 */
		void drawText( const char *inString, double inX, double inY,
					   double inWidth, double inHeight );

        

        /**
         * Measures the width of a string of text.
         *
         * @inString the string to measure.  Destroyed by caller.
         *
         * @return the width of the display text in character height
         *  units.  A return value of 8.5 means the string would be as
         *  wide as 8.5 characters are high (if the string was displayed
         *  with a 1:1 aspect ratio).
         */
        double measureTextWidth( const char *inString );

        /**
         * Measures the height of a string of text
         *
         * @inString the string to measure.  Destroyed by caller.
         *
         * @return the height of the display text in character height
         *  units (basic block character height, equal to block character 
         *  width, without considering special
         *  accent space).  A return value of 1.5 means the string extends
         *  above the basic block height by 0.5.
         *  This is the true height that a call to drawText would use
         *  (measureTextHeight * inHeight).
         */
        double measureTextHeight( const char *inString );
        

	protected:
        
        double mXImageFractionUsed;
        double mYImageFractionUsed;

        // height/width of used area of whole font texture
        double mHWRatio;
        
        

		Color *mFontColor;
		Color *mGradientColor;

        double mZ;
        char mSmooth;
        
		SingleTextureGL *mFontTexture;

		char mUseAlpha;

        double *mStartWidthFractionMetrics;
        double *mEndWidthFractionMetrics;
        
        double mCharacterSpacingFraction;
        
        double *mHeightFractionMetrics;
        

        
		/**
		 * Draws a character into a specified region of the
		 * context.  The character is squeezed to fit into the region.
		 *
		 * Uses the currently active OpenGL projection settings.
		 *
		 * @param inCharacter the character to draw.  Unsigned to support
         *   extended ASCII.
		 * @param inX the x coordinate of the region's lower-left
		 *   corner.
		 * @param inY the y coordinate of the region's lower-left
		 *   corner.
		 * @param inWidth the width of the region.
		 * @param inHeight the height of the region.
         *   Note that character accents may extend above inHeight.
         * @param outUsedWidth pointer to where the actual width used
         *   by this character should be returned.
		 */
		void drawCharacter( unsigned char inCharacter, double inX, double inY,
							double inWidth, double inHeight,
                            double *outUsedWidth );

        

        /**
         * Generates internal character width metrics from a font image.
         *
         * @param inFontImage the font image.  Destroyed by caller.
         * @param inFixedWidth, inSpaceWidthFraction,
         *   inCharacterSpacingFractionsame as parameters for the TextGL
         *   constructor.
         */
        void generateCharacterWidths( Image *inFontImage,
                                      char inFixedWidth,
                                      double inCharacterSpacingFraction,
                                      double inSpaceWidthFraction );

        
        
   	};



inline TextGL::TextGL( Image *inImage, char inUseAlpha,
                       char inFixedWidth,
                       double inCharacterSpacingFraction,
                       double inSpaceWidthFraction )
    
	: mXImageFractionUsed( 1.0 ),
      mYImageFractionUsed( 1.0 ),
      mFontColor( new Color( 1.0f, 1.0f, 1.0f, 1.0f ) ),
	  mGradientColor( NULL ),
      mZ( 0 ),
      mSmooth( false ),
      mUseAlpha( inUseAlpha ),
      mStartWidthFractionMetrics( new double[ 256 ] ),
      mEndWidthFractionMetrics( new double[ 256 ] ),
      mCharacterSpacingFraction( inCharacterSpacingFraction ),
      mHeightFractionMetrics( new double[ 256 ] ) {


    // pad image to next power of 2 in each dimension

    int w = inImage->getWidth();
    int h = inImage->getHeight();
    int paddedW = w;
    int paddedH = h;
    

    double log2w = log( w ) / log( 2 );
    double log2h = log( h ) / log( 2 );
    

    int next2PowerW = (int)( ceil( log2w ) );
    int next2PowerH = (int)( ceil( log2h ) );
    
    if( next2PowerW != log2w ) {
        paddedW = (int)( pow( 2, next2PowerW ) );
        
        mXImageFractionUsed = (double)w / (double)paddedW;
        }

    if( next2PowerH != log2h ) {
        paddedH = (int)( pow( 2, next2PowerH ) );
        
        mYImageFractionUsed = (double)h / (double)paddedH;
        }
    
    mHWRatio = h / (double)w;


    // pad the image
    int numChannels = inImage->getNumChannels();
    
    Image paddedImage( paddedW, paddedH, numChannels, true );
    
    for( int c=0; c<numChannels; c++ ) {
        double *destChannel = paddedImage.getChannel( c );
        double *sourceChannel = inImage->getChannel( c );
        
        for( int y=0; y<h; y++ ) {
            // copy row
            memcpy( &( destChannel[ y * paddedW ] ),
                    &( sourceChannel[ y * w ] ),
                    sizeof( double ) * w );
            }
        }
    



	if( inUseAlpha && paddedImage.getNumChannels() != 4 ) {
	
		Image *fontImage = new Image( paddedImage.getWidth(),
									  paddedImage.getHeight(),
									  4 );

		int numPixels = paddedImage.getWidth() * paddedImage.getHeight();

	
		// copy the red channel of paddedImage into our alpha
		memcpy( fontImage->getChannel(3), paddedImage.getChannel(0),
				numPixels * sizeof( double ) );

		// set other channels to white
		for( int c=0; c<3; c++ ) {
			double *channel = fontImage->getChannel( c );
			for( int p=0; p<numPixels; p++ ) {
				channel[p] = 1.0;
				}
			}
	
	
		mFontTexture = new SingleTextureGL( fontImage );

        generateCharacterWidths( fontImage, inFixedWidth,
                                 inCharacterSpacingFraction,
                                 inSpaceWidthFraction );
        
		delete fontImage;
		}
	else {
		// use the texture directly
		mFontTexture = new SingleTextureGL( &paddedImage );

        generateCharacterWidths( &paddedImage, inFixedWidth,
                                 inCharacterSpacingFraction,
                                 inSpaceWidthFraction );
        }
	}



inline TextGL::~TextGL() {
	delete mFontTexture;
	if( mFontColor != NULL ) {
		delete mFontColor;
		}
	if( mGradientColor != NULL ) {
		delete mGradientColor;
		}

    delete [] mStartWidthFractionMetrics;
    delete [] mEndWidthFractionMetrics;
	delete [] mHeightFractionMetrics;
    }



inline void TextGL::setFontColor( Color *inColor ) {
	if( mFontColor != NULL && mFontColor != inColor ) {
		delete mFontColor;
		}
	mFontColor = inColor;
	}



inline void TextGL::setGradientColor( Color *inColor ) {
	if( mGradientColor != NULL && mGradientColor != inColor ) {
		delete mGradientColor;
		}
	mGradientColor = inColor;
	}



inline Color *TextGL::getFontColor() {
	return mFontColor;
	}



inline Color *TextGL::getGradientColor() {
	return mGradientColor;
	}



inline void TextGL::setFontZ( double inZ ) {
    mZ = inZ;
    }



inline void TextGL::setSmoothing( char inSmooth ) {
    mSmooth = inSmooth;
    }



inline void TextGL::drawText( const char *inString, double inX, double inY,
							  double inWidth, double inHeight ) {
    
	int numChars = strlen( inString );

	double charWidth = inWidth / numChars;

	double currentCharX = inX;

	char oldBlendState = glIsEnabled( GL_BLEND );

	if( mUseAlpha ) {
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	
	for( int i=0; i<numChars; i++ ) {
		unsigned char c = inString[i];

        double usedWidth;
		drawCharacter( c, currentCharX, inY, charWidth, inHeight,
                       &usedWidth );
		
		currentCharX += usedWidth;
		}

	if( mUseAlpha ) {
		// restore blend state if necessary
		if( !oldBlendState ) {
			glDisable( GL_BLEND );
			}
		}
	
	}



inline void TextGL::drawCharacter( unsigned char inCharacter, 
                                   double inX, double inY,
								   double inWidth, double inHeight,
                                   double *outUsedWidth ) {
    *outUsedWidth =
        inWidth *
        ( mEndWidthFractionMetrics[inCharacter] -
          mStartWidthFractionMetrics[inCharacter] );
    

    
	double charStartX = inX;
    // scoot to the left based on width metric
    charStartX -= ( mStartWidthFractionMetrics[inCharacter] * inWidth );
    
	double charStartY = inY;

	double charEndX = charStartX + inWidth;
    // let height extend above inHeight if accent region present
	double charEndY = inY + inHeight * mHWRatio;

	// y start and end in texture flipped
	double textureStartX = 
        mXImageFractionUsed * ( inCharacter % 16  ) / 16.0;
	double textureStartY = 
        mYImageFractionUsed *( inCharacter / 16  ) / 16.0 
        + mYImageFractionUsed / 16;

	double textureEndX = textureStartX + mXImageFractionUsed / 16;
	double textureEndY = textureStartY - mYImageFractionUsed / 16;

	
	mFontTexture->enable();

    if( mSmooth ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        }
    
	glBegin( GL_QUADS ); {
		if( mGradientColor != NULL ) {
			glColor4f( mGradientColor->r, mGradientColor->g,
					   mGradientColor->b, mGradientColor->a );
			}
		else {
			glColor4f( mFontColor->r, mFontColor->g,
					   mFontColor->b, mFontColor->a );
			}
				 
		glTexCoord2f( textureStartX, textureStartY );
		glVertex3d( charStartX, charStartY, mZ );
		
		glTexCoord2f( textureEndX, textureStartY );
		glVertex3d( charEndX, charStartY, mZ ); 

		
		glColor4f( mFontColor->r, mFontColor->g,
				   mFontColor->b, mFontColor->a );

		
		glTexCoord2f( textureEndX, textureEndY );
		glVertex3d( charEndX, charEndY, mZ );
		
		glTexCoord2f( textureStartX, textureEndY );
		glVertex3d( charStartX, charEndY, mZ );
		}
	glEnd();
	}



inline void TextGL::generateCharacterWidths( Image *inFontImage,
                                             char inFixedWidth,
                                             double inCharacterSpacingFraction,
                                             double inSpaceWidthFraction ) {
    if( mUseAlpha && ! inFixedWidth ) {

        double *alphaChannel = inFontImage->getChannel( 3 );
        
        int imageWidth = inFontImage->getWidth();
        int imageHeight = inFontImage->getHeight();
                
        int pixelsPerCharacterX = 
            (int)( mXImageFractionUsed * imageWidth ) / 16;

        int pixelsPerCharacterY = 
            (int)( mYImageFractionUsed * imageHeight ) / 16;
        
        // look at each row and column in texture image
        for( int r=0; r<16; r++ ) {

            int charStartY = pixelsPerCharacterY * r;
            int charEndY = charStartY + pixelsPerCharacterY;
            
            for( int c=0; c<16; c++ ) {

                int charStartX = pixelsPerCharacterX * c;
                int charEndX = charStartX + pixelsPerCharacterX;

                // by "ink", I mean part of the image that has a non-zero
                // alpha

                // we want to find how far this character stretches to the
                // left and right within it's fixed-width region
                
                int smallestXWithInk = charEndX;
                int largestXWithInk = charStartX;

                // also how high (but don't care about how low)
                // also note that y coordinates inverted.
                int smallestYWithInk = charEndY;

                for( int x=charStartX; x<charEndX; x++ ) {

                    for( int y=charStartY; y<charEndY; y++ ) {

                        if( alphaChannel[ y * imageWidth + x ] > 0 ) {
                            // hit some ink

                            if( x < smallestXWithInk ) {
                                smallestXWithInk = x;
                                }
                            if( x > largestXWithInk ) {
                                largestXWithInk = x;
                                }
                            if( y < smallestYWithInk ) {
                                smallestYWithInk = y;
                                }
                            }
                        
                        }
                    }
                
                int i = r * 16 + c;

                if( smallestXWithInk <= largestXWithInk ) {
                    mStartWidthFractionMetrics[ i ] =
                        (double)( smallestXWithInk - charStartX )
                        / (double)pixelsPerCharacterX;

                    mEndWidthFractionMetrics[ i ] =
                        (double)( largestXWithInk - charStartX )
                        / (double)pixelsPerCharacterX;

                    mStartWidthFractionMetrics[ i ] -=
                        inCharacterSpacingFraction;

                    mEndWidthFractionMetrics[ i ] +=
                        inCharacterSpacingFraction;
                    

                    // note:  base this on pixelsPerCharacterX,
                    // because height of 1.0 is basic block size, not
                    // extended (accent) size
                    mHeightFractionMetrics[ i ] =
                        (double)( charEndY - smallestYWithInk )
                        / (double)pixelsPerCharacterX;
                    }
                else {
                    // empty character image
                    // treat as a space
                    mStartWidthFractionMetrics[ i ] = 0;
                    mEndWidthFractionMetrics[ i ] = inSpaceWidthFraction;
                    mHeightFractionMetrics[ i ] = 0;
                    }
                }
            }
        }
    else {
        // not using alpha (or fixed width requested)

        // fix width
        for( int i=0; i<256; i++ ) {
            mStartWidthFractionMetrics[ i ] = 0 - inCharacterSpacingFraction;
            mEndWidthFractionMetrics[ i ] = 1 + inCharacterSpacingFraction;
            
            // assume it consumes full vertical space in each block
            mHeightFractionMetrics[ i ] = mHWRatio;
            }

        // special case for space
        mStartWidthFractionMetrics[ 32 ] = 0;
        mEndWidthFractionMetrics[ 32 ] = inSpaceWidthFraction;
        
        }
    }



inline double TextGL::measureTextWidth( const char *inString ) {

    double width = 0;

    int numChars = strlen( inString );

    for( int i=0; i<numChars; i++ ) {
        unsigned char character = inString[i];
        
        double charWidth =
            mEndWidthFractionMetrics[character] -
            mStartWidthFractionMetrics[character]; 

        width += charWidth;
        }
    // a bit too long (extra, empty space at end)
    // actually, too many other pieces of code depend on how it used to work.
    //width -= 2 * mCharacterSpacingFraction;
    
    
    return width;
    }


inline double TextGL::measureTextHeight( const char *inString ) {

    double height = 0;

    int numChars = strlen( inString );

    for( int i=0; i<numChars; i++ ) {
        unsigned char character = inString[i];
        
        double charHeight = mHeightFractionMetrics[ character ];
        
        if( charHeight > height ) {
            height = charHeight;
            }
        }
    
    return height;
    }




#endif
