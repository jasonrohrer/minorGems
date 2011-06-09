/*
 * Modification History
 *
 * 2001-September-15	Jason Rohrer
 * Created.
 *
 * 2005-February-21	  Jason Rohrer
 * Added comments about channel ordering.
 * Created a cpp file for static init and longer function definitions.
 *
 * 2005-August-24	  Jason Rohrer
 * Added control over texture wrapping.
 *
 * 2006-September-20  Jason Rohrer
 * Changed type of mTextureID to avoid compile warnings on MacOSX.
 *
 * 2009-September-9  Jason Rohrer
 * Added direct RGBA construction.
 *
 * 2009-November-5  Jason Rohrer
 * Added texture data replacement.
 *
 * 2010-April-20   	Jason Rohrer
 * Reload all SingleTextures after GL context change.
 *
 * 2011-January-17   Jason Rohrer
 * Support for single-channel textures for efficiency.
 */
 
 
#ifndef SINGLE_TEXTURE_GL_INCLUDED
#define SINGLE_TEXTURE_GL_INCLUDED 

#include "glInclude.h"
#include <stdio.h>
#include <string.h>

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/RGBAImage.h"
#include "minorGems/util/SimpleVector.h"


 
/**
 * A single-layer OpenGL 32-bit texture map.
 *
 * This is essentially a simplified version of TextureGL.
 *
 * @author Jason Rohrer 
 */
class SingleTextureGL { 
	
	public:


		
		/**
		 * Constructs a texture map, specifying texture data.
		 *
		 * Note that this constructor can only be called
		 * after a GL context has been setup.
		 *
		 * @param inImage the image to use as a source
		 *   for texture data.  The image should have
		 *   4 channels.  Any extra channels are ignored,
		 *   and black is set into additional texture
		 *   channels if inImage has too few channels.
		 *   Must be destroyed by caller, and can be
		 *   destroyed as soon as this constructor returns.
		 *   Channels are ordered as RGBA.
		 *   Must be destroyed by caller.
         * @param inRepeat true to repeat (tile) texture.
         *   Defaults to true.
		 *
		 * Note that width and height must each be powers of 2 and
		 * not larger than 256 (for 3DFX compatability).  Additionally,
		 * for 3DFX compatability, the aspect ratio should not exceed 8:1.
		 */
		SingleTextureGL( Image *inImage, char inRepeat = true );


		/**
		 * Specifies texture data as rgba bytes.
		 */
		SingleTextureGL( unsigned char *inRGBA, 
                         unsigned int inWidth, unsigned int inHeight,
                         char inRepeat = true );

        
        /**
		 * Specifies single-channel texture data as alpha bytes.
         *
         * Extra parameter used only to differentiate constructor calls.
		 */
		SingleTextureGL( char inAlphaOnly,
                         unsigned char *inA, 
                         unsigned int inWidth, unsigned int inHeight,
                         char inRepeat = true );


		
		/**
		 * The OpenGL texture is deleted when the SingleTextureGL object is
		 * destroyed.
		 */
		~SingleTextureGL();
		


        /**
         * Replaces data in a texture that's already been set.
         * Dimensions must match original dimensions.
         *
         * Data can contain either RGBA or Alpha-only bytes.
         */
        void replaceTextureData( unsigned char *inBytes,
                                 char inAlphaOnly,
                                 unsigned int inWidth, 
                                 unsigned int inHeight );
        

		
		/**
		 * Sets the data for this texture.
		 *
		 * @param inImage the image to use as a source
		 *   for texture data.  The image should have
		 *   4 channels.  Any extra channels are ignored,
		 *   and black is set into additional texture
		 *   channels if inImage has too few channels.
		 *   Must be destroyed by caller, and can be
		 *   destroyed as soon as this constructor returns.
		 *   Channels are ordered as RGBA.
		 *   Must be destroyed by caller.
		 *
		 * Note that width and height must each be powers of 2 and
		 * not larger than 256 (for 3DFX compatability).  Additionally,
		 * for 3DFX compatability, the aspect ratio should not exceed 8:1.
		 */
		void setTextureData( Image *inImage );

		/**
		 * Sets texture data as rgba or alpha-only bytes.
		 */
        void setTextureData( unsigned char *inBytes,
                             char inAlphaOnly,
                             unsigned int inWidth, 
                             unsigned int inHeight );        

		
		/**
		 * Enables this texture.
		 */
		void enable();

		
		
		/**
		 * Disables this texture and all of its layers.
		 */
		void disable();		


        // tell all textures about a GL context change so they can reload
        // int texture memory
        static void contextChanged();
        
		
		
	private:
        char mRepeat;
        
		GLuint mTextureID;
        
        char mAlphaOnly;

        // in case of context switch where we need to reload our texture
        // backup bytes might contain either RGBA or A only bytes
        unsigned char *mBackupBytes;
        unsigned int mWidthBackup;
        unsigned int mHeightBackup;
        
        void reloadFromBackup();
        
        void replaceBackupData( unsigned char *inBytes,
                                char inAlphaOnly,
                                unsigned int inWidth, 
                                unsigned int inHeight );


        static SimpleVector<SingleTextureGL *> sAllLoadedTextures;
        
        
	};








inline void SingleTextureGL::enable() {	
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, mTextureID ); 

    int error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error binding texture id %d, error = %d\n",
                (int)mTextureID, error );
		}
	}



inline void SingleTextureGL::disable() {
	glDisable( GL_TEXTURE_2D );
	}
	
	
	
#endif


