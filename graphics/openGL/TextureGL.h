/*
 * Modification History
 *
 * 2000-December-19		Jason Rohrer
 * Created.
 *
 * 2001-January-19		Jason Rohrer
 * Added support for multi-texturing.  Now a TextureGL no longer has 
 * an overall width and height (each layer can have a different size).
 * Also, a TextureGL no longer has an overall ID.
 * Got rid of an old, commented-out interface in the process.
 * Changed how multi-textures are blended.  Settled on using MODULATE
 * for all layers of the texture, since nothing else produces results
 * that are compatible with vertex lighting.
 *
 * 2001-January-30		Jason Rohrer
 * Changed to check for GL version 1.2 before using 1.2 features.
 * Now we assume multi-texture is defined (because of the availability
 * of glext), but we check that it is supported at runtime.
 * Moved enable() below ARB function name definitions.
 *
 * 2001-January-31		Jason Rohrer
 * Fixed even more compile bugs.  There's no need to do a multitexture
 * bind when setting texture data.
 * Removed extra checks for unneeded extensions, and fixed more compile bugs. 
 * Fixed a bug in initMultitexture().
 * Fixed a bug in disable().
 */
 
 
#ifndef TEXTURE_GL_INCLUDED
#define TEXTURE_GL_INCLUDED 

#include <GL/gl.h>
#include <stdio.h>
#include <string.h>

// try and include extensions if multitexture isn't
// built-in to gl.h
#ifndef GL_ARB_multitexture
#include <GL/glext.h>
#endif

 
/**
 * OpenGL 32-bit multi-layered texture map.
 *
 * @author Jason Rohrer 
 */
class TextureGL { 
	
	public:
		
		/**
		 * Constructs a texture map.
		 *
		 * @param inNumLayers the number of multi-texture layers
		 *   in this texture.  Currently supports a value in range [1:8].
		 */
		TextureGL( int inNumLayers );
		
		/**
		 * The OpenGL texture is deleted when the TextureGL object is
		 * destroyed.
		 */
		~TextureGL();
		
		
		/**
		 * Sets the data for a layer of this texture.
		 *
		 * Blocks of 4 characters specify each pixel, and each 
		 * block must be ordered as RGBA.
		 *
		 * Note that width and height must each be powers of 2 and
		 * not larger than 256 (for 3DFX compatability).  Additionally,
		 * for 3DFX compatability, the aspect ratio should not exceed 8:1.
		 *
		 * @param inLayer the multi-texture layer to set the data to.
		 * @param inData a character array containing the color and alpha
		 *   components for this texture.  All data is copied internally.
		 * @param inWide width of the map in 4-byte blocks.
		 * @param inHigh height of the map.
		 */
		void setTextureData( int inLayer, unsigned char *inData,
			int inWide, int inHigh );
		
		
		/**
		 * Gets the number of layers in this multi-texture.
		 *
		 * @return the number of layers in this multi-texture.
		 */
		int getNumLayers();
		
		
		/**
		 * Enables this texture and all of its layers.
		 *
		 * Note that this function only makes sense when
		 * isMultitexturingSupported() returns true.  
		 * Otherwise, it is equivalent
		 * to calling enable( 0 ).
		 */
		void enable();
		
		
		/**
		 * Enables a particular texture layer.
		 *
		 * @param inLayerNumber the layer to enable.
		 */
		void enable( int inLayerNumber );
		
		
		/**
		 * Disables this texture and all of its layers.
		 */
		void disable();		
		
		
		/**
		 * Gets whether multitexturing is supported by the loaded
		 * GL implementation.
		 *
		 * @return true if multitexturing is supported.
		 */
		static char isMultiTexturingSupported();
		
		
		static GLenum sMultiTextureEnum[8];
		
	private:
		
		static int sNextFreeTextureID;
		
		int mNumLayers;
		unsigned int *mTextureID;
		
		static const int MAX_NUM_LAYERS = 8;
		
		
		static char sMultiTexturingSupported;
	
		// a static initializer function
		static void staticInit();
		
		static char sBeenInited;
	};



// initialize static members
int TextureGL::sNextFreeTextureID = 13;
char TextureGL::sBeenInited = false;
char TextureGL::sMultiTexturingSupported = false;

GLenum TextureGL::sMultiTextureEnum[MAX_NUM_LAYERS] = { 
	GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB, GL_TEXTURE3_ARB,
	GL_TEXTURE4_ARB, GL_TEXTURE5_ARB, GL_TEXTURE6_ARB, GL_TEXTURE7_ARB };



inline TextureGL::TextureGL( int inNumLayers ) 
	: mNumLayers( inNumLayers ), 
	mTextureID( new unsigned int[inNumLayers] ) {
	
	if( !sBeenInited ) {
		// call static initializer function
		TextureGL::staticInit();
		}
	
	if( inNumLayers > MAX_NUM_LAYERS ) {
		printf( "TextureGL only supports 8 multi-texture layers.\n" );
		printf( "The following number of layers is out of range: %d\n",
			inNumLayers );
		}
		
	for( int i=0; i<mNumLayers; i++ ) {
		mTextureID[i] = sNextFreeTextureID++;
		}
	
	}



inline TextureGL::~TextureGL() {
	for( int i=0; i<mNumLayers; i++ ) {
		glDeleteTextures( 1, &( mTextureID[i] ) );
		}
	delete [] mTextureID;	
	}



inline int TextureGL::getNumLayers() {
	return mNumLayers;
	}



inline char TextureGL::isMultiTexturingSupported() {
	return sMultiTexturingSupported;
	}



inline void TextureGL::enable( int inLayerNumber ) {
	glBindTexture( GL_TEXTURE_2D, mTextureID[inLayerNumber] ); 
	glEnable( GL_TEXTURE_2D );
	int error = glGetError();
	if( error != GL_NO_ERROR ) {		// error
		printf( "Error binding texture, %d\n", error );
		}
	}



inline void TextureGL::setTextureData( int inLayer, unsigned char *inData,
	int inWide, int inHigh  ) {
	
	GLenum texFormat = GL_RGBA;
	
	glBindTexture( GL_TEXTURE_2D, mTextureID[inLayer] );
    
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
	// for now, use the modulate function for all layers and see 
	// how it looks.
	//if( inLayer == 0 ) {
		// use modulation function for the first layer
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	//	}
	//else {
		// use decal function for the remaining layers
	//	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	//	}

	glTexImage2D( GL_TEXTURE_2D, 0, 
		texFormat, inWide, inHigh, 0, 
		texFormat, GL_UNSIGNED_BYTE, inData );
		
	}



#ifdef GL_VERSION_1_2

// if we're not using multitexture as an extension,
// our static initializer doesn't need to do anything
inline void TextureGL::staticInit() {
	// assume it's supported if we're in GL 1.2
	sMultiTexturingSupported = true;
	sBeenInited = true;
	}
	
#endif



// if we're using multitexture as an extension
#ifndef GL_VERSION_1_2

// copied from the following source
// last seen at htt://nehe.gamedev.net

/*		This Code Was Created by Jens Schneider (WizardSoft) 2000 
 *		Lesson22 to the series of OpenGL tutorials by NeHe-Production
 */

PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB	= NULL;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	= NULL;
PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB	= NULL;
PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB= NULL;

// Always Check For Extension-Availability During Run-Time!
// Here We Go!
inline char isInString(char *string, const char *search) {
	int pos=0;
	int maxpos=strlen(search)-1;
	int len=strlen(string);	
	char *other;
	for (int i=0; i<len; i++) {
		if ((i==0) || ((i>1) && string[i-1]=='\n')) {				// New Extension Begins Here!
			other=&string[i];			
			pos=0;													// Begin New Search
			while (string[i]!='\n') {								// Search Whole Extension-String
				if (string[i]==search[pos]) pos++;					// Next Position
				if ((pos>maxpos) && string[i+1]=='\n') return true; // We Have A Winner!
				i++;
			}			
		}
	}	
	return false;													// Sorry, Not Found!
	}
	
// isMultitextureSupported() Checks At Run-Time If Multitexturing Is Supported
inline char initMultitexture(void) {
	
	char *extString = (char *)glGetString( GL_EXTENSIONS );	// Fetch Extension String
	int len = strlen( extString );
	
	// allow for end of string character
	char *extensions = new char[ len + 1 ];
	
	strcpy( extensions, extString );
	
	for (int i=0; i<len; i++)										// Separate It By Newline Instead Of Blank
		if (extensions[i]==' ') extensions[i]='\n';

	if (isInString(extensions,"GL_ARB_multitexture") )				// Is Multitexturing Supported?
	{	
		//glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);
		glMultiTexCoord1fARB	= (PFNGLMULTITEXCOORD1FARBPROC)		wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB	= (PFNGLMULTITEXCOORD3FARBPROC)		wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB	= (PFNGLMULTITEXCOORD4FARBPROC)		wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");		

		return true;
		}
	return false;
	}

// end copied code


inline void TextureGL::staticInit() {
	// our static init should find out if multi-texture is really 
	// available
	sMultiTexturingSupported = initMultitexture();
	sBeenInited = true;
	}

#endif



inline void TextureGL::enable() {
	if( sMultiTexturingSupported ) {
		for( int i=0; i<mNumLayers; i++ ) {
			glActiveTextureARB( sMultiTextureEnum[i] ); 
			glBindTexture( GL_TEXTURE_2D, mTextureID[i] ); 
			glEnable( GL_TEXTURE_2D ); 
			}

		int error = glGetError();
		if( error != GL_NO_ERROR ) {		// error
			printf( "Error binding texture, %d\n", error );
			}
		}
	else {
		// default behavior if multitexturing not supported
		enable( 0 );
		}
	
	}



inline void TextureGL::disable() {
	if( sMultiTexturingSupported ) {
		for( int i=0; i<mNumLayers; i++ ) {
			glActiveTextureARB( sMultiTextureEnum[i] );  
			glDisable( GL_TEXTURE_2D ); 
			}
		}
	else {
		glDisable( GL_TEXTURE_2D );
		}
	}
	
	
	
#endif
