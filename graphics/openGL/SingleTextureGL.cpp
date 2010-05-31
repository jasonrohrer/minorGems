/*
 * Modification History
 *
 * 2005-February-21   Jason Rohrer
 * Created.
 *
 * 2005-August-24	  Jason Rohrer
 * Added control over texture wrapping.
 *
 * 2009-September-9  Jason Rohrer
 * Added direct RGBA construction.
 *
 * 2009-November-5  Jason Rohrer
 * Added texture data replacement.
 *
 * 2010-April-20   	Jason Rohrer
 * Reload all SingleTextures after GL context change.
 */



#include "SingleTextureGL.h"


SimpleVector<SingleTextureGL *> SingleTextureGL::sAllLoadedTextures;



void SingleTextureGL::contextChanged() {
    // tell all textures to reload

    int numTextures = sAllLoadedTextures.size();
    
    printf( "Reloading %d textures due to OpenGL context change\n",
            numTextures );
    
    for( int i=0; i<numTextures; i++ ) {
        SingleTextureGL *t = *( sAllLoadedTextures.getElement( i ) );
        
        t->reloadFromBackup();
        }
    }



void SingleTextureGL::reloadFromBackup() {
    
    if( mRGBABackup != NULL ) {
        
        glGenTextures( 1, &mTextureID );
        
        int error = glGetError();
        if( error != GL_NO_ERROR ) {		// error
            printf( "Error generating new texture ID, error = %d, \"%s\"\n",
                    error, glGetString( error ) );
            }
        
        
        setTextureData( mRGBABackup, mWidthBackup, mHeightBackup );
        }
    }
    



SingleTextureGL::SingleTextureGL( Image *inImage, char inRepeat )
    : mRepeat( inRepeat ),
      mRGBABackup( NULL ) {

    glGenTextures( 1, &mTextureID );
    
    int error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error generating new texture ID, error = %d, \"%s\"\n",
                error, glGetString( error ) );
        }


	setTextureData( inImage );

    sAllLoadedTextures.push_back( this );
	}



SingleTextureGL::SingleTextureGL( unsigned char *inRGBA, 
                                  unsigned int inWidth, 
                                  unsigned int inHeight,
                                  char inRepeat )
    : mRepeat( inRepeat ), 
      mRGBABackup( NULL ) {

    glGenTextures( 1, &mTextureID );
    
    int error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error generating new texture ID, error = %d, \"%s\"\n",
                error, glGetString( error ) );
        }


	setTextureData( inRGBA, inWidth, inHeight );
    
    sAllLoadedTextures.push_back( this );
	}



SingleTextureGL::~SingleTextureGL() {
	sAllLoadedTextures.deleteElementEqualTo( this );
    
    glDeleteTextures( 1, &mTextureID );
	
    if( mRGBABackup != NULL ) {
        delete [] mRGBABackup;
        mRGBABackup = NULL;
        }
    
    }







void SingleTextureGL::setTextureData( Image *inImage ) {
	
	// first, convert our image to an RGBAImage
	RGBAImage *rgbaImage = new RGBAImage( inImage );

    

	if( inImage->getNumChannels() < 4 ) {
		// we should fill in 1.0 for the alpha channel
		// since the input image doesn't have an alpha channel
		double *channel = rgbaImage->getChannel( 3 );
        
        int numPixels = inImage->getWidth() * inImage->getHeight();
        
		for( int i=0; i<numPixels; i++ ) {
			channel[i] = 1.0;
			}
		}
    
	// extract the rgba data
	unsigned char *textureData = rgbaImage->getRGBABytes();

    setTextureData( textureData, 
                    rgbaImage->getWidth(), rgbaImage->getHeight() );
    
    
    delete rgbaImage;
	delete [] textureData;    
    }


void SingleTextureGL::setTextureData( unsigned char *inRGBA,
                                      unsigned int inWidth, 
                                      unsigned int inHeight ) {
    
    if( inRGBA != mRGBABackup ) {
        
        // clear old backup
        if( mRGBABackup != NULL ) {
            delete [] mRGBABackup;
            mRGBABackup = NULL;
            }
        mRGBABackup = new unsigned char[ inWidth * inHeight * 4 ];
        memcpy( mRGBABackup, inRGBA, inWidth * inHeight * 4 );
        mWidthBackup = inWidth;
        mHeightBackup = inHeight;
        }
    

    int error;
    
	GLenum texFormat = GL_RGBA;
	glBindTexture( GL_TEXTURE_2D, mTextureID );

    error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error binding to texture id %d, error = %d\n",
                (int)mTextureID,
                error );
		}
    
    
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    if( mRepeat ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        }
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glTexImage2D( GL_TEXTURE_2D, 0,
				  texFormat, inWidth,
				  inHeight, 0,
				  texFormat, GL_UNSIGNED_BYTE, inRGBA );

	error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error setting texture data for id %d, error = %d, \"%s\"\n",
                (int)mTextureID, error, glGetString( error ) );
        printf( "Perhaps texture image width or height is not a power of 2\n"
                "Width = %u, Height = %u\n",
                inWidth, inHeight );
		}
	}


void SingleTextureGL::replaceTextureData( unsigned char *inRGBA,
                                          unsigned int inWidth, 
                                          unsigned int inHeight ) {
    if( inRGBA != mRGBABackup ) {
        // clear old backup
        if( mRGBABackup != NULL ) {
            delete [] mRGBABackup;
            mRGBABackup = NULL;
            }
        mRGBABackup = new unsigned char[ inWidth * inHeight * 4];
        memcpy( mRGBABackup, inRGBA, inWidth * inHeight * 4 );
        mWidthBackup = inWidth;
        mHeightBackup = inHeight;
        }
    


    int error;

    GLenum texFormat = GL_RGBA;
	glBindTexture( GL_TEXTURE_2D, mTextureID );
    
    error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error binding to texture id %d, error = %d\n",
                (int)mTextureID,
                error );
		}

    
    glTexSubImage2D( GL_TEXTURE_2D, 0,
                     0, 0, // offset
                     inWidth, inHeight, 
                     texFormat,
                     GL_UNSIGNED_BYTE, inRGBA );

	error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error replacing texture data for id %d, error = %d, \"%s\"\n",
                (int)mTextureID, error, glGetString( error ) );
        printf( "Perhaps texture image width or height is not a power of 2\n"
                "Width = %u, Height = %u\n",
                inWidth, inHeight );
		}


    }

        
