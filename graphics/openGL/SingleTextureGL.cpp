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
 *
 * 2011-January-17   Jason Rohrer
 * Support for single-channel textures for efficiency.
 *
 * 2011-January-23   Jason Rohrer
 * Changed internal format of single-channel texture to RGBA for compatibility.
 */



#include "SingleTextureGL.h"


SimpleVector<SingleTextureGL *> SingleTextureGL::sAllLoadedTextures;

char SingleTextureGL::sTexturingEnabled = false;

GLuint SingleTextureGL::sLastBoundTextureID = 0;



void SingleTextureGL::contextChanged() {
    // tell all textures to reload

    sLastBoundTextureID = 0;
    
    int numTextures = sAllLoadedTextures.size();
    
    printf( "Reloading %d textures due to OpenGL context change\n",
            numTextures );
    
    for( int i=0; i<numTextures; i++ ) {
        SingleTextureGL *t = *( sAllLoadedTextures.getElement( i ) );
        
        t->reloadFromBackup();
        }
    }





void SingleTextureGL::disableTexturing() {    
    glDisable( GL_TEXTURE_2D );
	sTexturingEnabled = false;
    sLastBoundTextureID = 0;
    }




void SingleTextureGL::reloadFromBackup() {
    
    if( mBackupBytes != NULL ) {
        
        glGenTextures( 1, &mTextureID );
        
        int error = glGetError();
        if( error != GL_NO_ERROR ) {		// error
            printf( "Error generating new texture ID, error = %d, \"%s\"\n",
                    error, glGetString( error ) );
            }
        
        
        setTextureData( mBackupBytes, mAlphaOnly, 
                        mWidthBackup, mHeightBackup, 
                        // backup already has edges expanded
                        false );
        }
    }
    



SingleTextureGL::SingleTextureGL( Image *inImage, char inRepeat, 
                                  char inMipMap )
    : mRepeat( inRepeat ), 
      mMipMap( inMipMap ),
      mAlphaOnly( false ),
      mBackupBytes( NULL ) {

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
                                  char inRepeat, char inMipMap )
    : mRepeat( inRepeat ),
      mMipMap( inMipMap ),
      mAlphaOnly( false ),
      mBackupBytes( NULL ) {

    glGenTextures( 1, &mTextureID );
    
    int error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error generating new texture ID, error = %d, \"%s\"\n",
                error, glGetString( error ) );
        }


	setTextureData( inRGBA, mAlphaOnly, inWidth, inHeight, true );
    
    sAllLoadedTextures.push_back( this );
	}



SingleTextureGL::SingleTextureGL( char inAlphaOnly,
                                  unsigned char *inA, 
                                  unsigned int inWidth, unsigned int inHeight,
                                  char inRepeat, char inMipMap )
    : mRepeat( inRepeat ),
      mMipMap( inMipMap ),
      mAlphaOnly( true ),
      mBackupBytes( NULL ) {

    glGenTextures( 1, &mTextureID );
    
    int error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error generating new texture ID, error = %d, \"%s\"\n",
                error, glGetString( error ) );
        }


	setTextureData( inA, mAlphaOnly, inWidth, inHeight, 
                    // don't expand edge for alpha-only
                    false );
    
    sAllLoadedTextures.push_back( this );
	}
    




SingleTextureGL::~SingleTextureGL() {
	sAllLoadedTextures.deleteElementEqualTo( this );
    
    glDeleteTextures( 1, &mTextureID );
	
    if( mBackupBytes != NULL ) {
        delete [] mBackupBytes;
        mBackupBytes = NULL;
        }
    
    }







void SingleTextureGL::setTextureData( Image *inImage ) {
	
	// extract the rgba data
	unsigned char *textureData = RGBAImage::getRGBABytes( inImage );

    setTextureData( textureData, 
                    false, inImage->getWidth(), inImage->getHeight(),
                    true );
    
    delete [] textureData;    
    }



void SingleTextureGL::replaceBackupData( unsigned char *inBytes,
                                         char inAlphaOnly,
                                         unsigned int inWidth, 
                                         unsigned int inHeight ) {
    if( inBytes != mBackupBytes ) {
        
        // clear old backup
        if( mBackupBytes != NULL ) {
            delete [] mBackupBytes;
            mBackupBytes = NULL;
            }

        int numBytesPerPixel = 4;

        if( inAlphaOnly ) {
            numBytesPerPixel = 1;
            }        

        mBackupBytes = 
            new unsigned char[ inWidth * inHeight * numBytesPerPixel ];
        
        memcpy( mBackupBytes, inBytes, inWidth * inHeight * numBytesPerPixel );
        
        mWidthBackup = inWidth;
        mHeightBackup = inHeight;
        }    
    }



void SingleTextureGL::setTextureData( unsigned char *inBytes,
                                      char inAlphaOnly,
                                      unsigned int inWidth, 
                                      unsigned int inHeight,
                                      char inExpandEdge ) {
    
    if( inExpandEdge && !inAlphaOnly ) {
        
        unsigned int maxY = 0;
        unsigned int minY = inHeight - 1;
        
        unsigned int maxX = 0;
        unsigned int minX = inWidth - 1;
        
        int aIndex = 3;
        for( unsigned int y=0; y<inHeight; y++ ) {
            for( unsigned int x=0; x<inWidth; x++ ) {
                
                if( inBytes[ aIndex ] > 0 ) {    
                    if( x > maxX ) {
                        maxX = x;
                        }
                    if( x < minX ) {
                        minX = x;
                        }
                    if( y > maxY ) {
                        maxY = y;
                        }
                    if( y < minY ) {
                        minY = y;
                        }
                    }

                aIndex += 4;
                }
            }

        if( minY < maxY &&
            minX < maxX &&
            minY > 0 &&
            maxY < inHeight - 1 &&  
            minX > 0 &&
            maxX < inWidth - 1 ) {

            // found edges away from image edge

            // duplicate them
            
            // row edges

            int rowBytes = inWidth * 4;

            int rowStart = minY * rowBytes;
            int rowDestStart = rowStart - rowBytes;

            // don't duplicate row unless it has some fully-opaque
            // pixels in it (it's something of a hard edge)
            // thus, we don't accidentally expand the soft edges
            // of feathered sprites, fonts, etc
            char solidPresent = false;
            
            for( int i=rowStart + 3; i<rowStart + rowBytes; i+=4 ) {
                if( inBytes[i] == 255 ) {
                    solidPresent = true;
                    break;
                    }
                }

            if( solidPresent ) {
                memcpy( &( inBytes[ rowDestStart ] ), 
                        &( inBytes[ rowStart ] ), 
                        inWidth * 4 );
                }
            
            rowStart = maxY * inWidth * 4;
            rowDestStart = rowStart + inWidth * 4;

            solidPresent = false;

            for( int i=rowStart + 3; i<rowStart + rowBytes; i+=4 ) {
                if( inBytes[i] == 255 ) {
                    solidPresent = true;
                    break;
                    }
                }

            if( solidPresent ) {
                memcpy( &( inBytes[ rowDestStart ] ), 
                        &( inBytes[ rowStart ] ), 
                        inWidth * 4 );
                }
            

            // now column edges

            char solidPresentLeft = false;
            char solidPresentRight = false;
            
            for( unsigned int y=minY; y<=maxY; y++ ) {

                int iL = (y * inWidth + minX) * 4;

                if( inBytes[ iL + 3 ] == 255 ) {
                    solidPresentLeft = true;
                    break;
                    }
                }
            
            for( unsigned int y=minY; y<=maxY; y++ ) {

                int iR = (y * inWidth + maxX) * 4;

                if( inBytes[ iR + 3 ] == 255 ) {
                    solidPresentRight = true;
                    break;
                    }
                }
            

            if( solidPresentLeft ) {    
                for( unsigned int y=minY; y<=maxY; y++ ) {
                    int iL = (y * inWidth + minX) * 4;
                    
                    inBytes[iL - 4] = inBytes[ iL ];
                    inBytes[iL - 3] = inBytes[ iL + 1 ];
                    inBytes[iL - 2] = inBytes[ iL + 2 ];
                    inBytes[iL - 1] = inBytes[ iL + 3 ];
                    }
                }
            
                

            if( solidPresentRight ) {
                for( unsigned int y=minY; y<=maxY; y++ ) {
                    int iR = (y * inWidth + maxX) * 4;
                    inBytes[iR + 4] = inBytes[ iR ];
                    inBytes[iR + 5] = inBytes[ iR + 1 ];
                    inBytes[iR + 6] = inBytes[ iR + 2 ];
                    inBytes[iR + 7] = inBytes[ iR + 3 ];
                    }
                }
            
            }
        
        }
    

    replaceBackupData( inBytes, inAlphaOnly, inWidth, inHeight );
    
    

    int error;
    
	GLenum internalTexFormat = GL_RGBA;
	GLenum texDataFormat = GL_RGBA;

    if( inAlphaOnly ) {
        internalTexFormat = GL_RGBA;
        texDataFormat = GL_ALPHA;
        }
    

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
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        }
    
    if( mMipMap ) {
        // GL_GENERATE_MIPMAP not available on some platforms,
        // like mingw
        // use gluBuild2DMipmaps in that case
       #ifdef GL_GENERATE_MIPMAP
            glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
            
            glTexImage2D( GL_TEXTURE_2D, 0,
                          internalTexFormat, inWidth,
                          inHeight, 0,
                          texDataFormat, GL_UNSIGNED_BYTE, inBytes );
        #else
            gluBuild2DMipmaps( GL_TEXTURE_2D,
                               internalTexFormat, inWidth,
                               inHeight,
                               texDataFormat, GL_UNSIGNED_BYTE, inBytes );
        #endif
        }
    else {    
        glTexImage2D( GL_TEXTURE_2D, 0,
                      internalTexFormat, inWidth,
                      inHeight, 0,
                      texDataFormat, GL_UNSIGNED_BYTE, inBytes );
        }
    
	error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error setting texture data for id %d, error = %d, \"%s\"\n",
                (int)mTextureID, error, glGetString( error ) );
        printf( "Perhaps texture image width or height is not a power of 2\n"
                "Width = %u, Height = %u\n",
                inWidth, inHeight );
		}
	}


void SingleTextureGL::replaceTextureData( unsigned char *inBytes,
                                          char inAlphaOnly,
                                          unsigned int inWidth, 
                                          unsigned int inHeight ) {

    replaceBackupData( inBytes, inAlphaOnly, inWidth, inHeight );
    


    int error;


	GLenum texDataFormat = GL_RGBA;

    if( inAlphaOnly ) {
        texDataFormat = GL_ALPHA;
        }


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
                     texDataFormat,
                     GL_UNSIGNED_BYTE, inBytes );

	error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error replacing texture data for id %d, error = %d, \"%s\"\n",
                (int)mTextureID, error, glGetString( error ) );
        printf( "Perhaps texture image width or height is not a power of 2\n"
                "Width = %u, Height = %u\n",
                inWidth, inHeight );
		}


    }

        
