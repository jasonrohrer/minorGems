/*
 * Modification History
 *
 * 2001-April-27   Jason Rohrer
 * Created.   
 *
 * 2011-June-21   Jason Rohrer
 * Added flag for forcing input to read until end.  Some JPG files
 * contain more than one image and thus have FFD9 in the middle.   
 */
 
 
#ifndef JPEG_IMAGE_CONVERTER_INCLUDED
#define JPEG_IMAGE_CONVERTER_INCLUDED 


#include "minorGems/graphics/ImageConverter.h"


/**
 * JPEG implementation of the image conversion interface.
 *
 * Implementations are platform dependent.
 *
 * @author Jason Rohrer
 */
class JPEGImageConverter : public ImageConverter {
	
	public:

		/**
		 * Constructs a JPEGImageConverter.
		 *
		 * @param inQuality a quality value in [0,100] for compression.
		 *   100 specifies highest quality.
         * @param inReadInputToEnd should input be read all the way until end 
         *   when deformatting?
         *   If false (default), input reading will stop at first occurrence
         *   of FFD9 (JPEG end marker). 
		 */
		JPEGImageConverter( int inQuality, char inReadInputToEnd=false );

		
		/**
		 * Sets the compression quality.
		 *
		 * @param inQuality a quality value in [0,100].  100
		 *   specifies highest quality.
		 */
		void setQuality( int inQuality );
		

		/**
		 * Gets the compression quality.
		 *
		 * @return a quality value in [0,100].  100
		 *   indicates highest quality.
		 */
		int getQuality();

		
		// implement the ImageConverter interface
		virtual void formatImage( Image *inImage, 
			OutputStream *inStream );
			
		virtual Image *deformatImage( InputStream *inStream );		

		
	private:
		int mQuality;
        char mReadInputToEnd;
	};



inline JPEGImageConverter::JPEGImageConverter( int inQuality, 
                                               char inReadInputToEnd )
	: mQuality( inQuality ), mReadInputToEnd( inReadInputToEnd ) {

	if( mQuality > 100 || mQuality < 0 ) {
		printf( "JPEG quality must be in [0,100]\n" );
		mQuality = 50;
		}
	}



inline void JPEGImageConverter::setQuality( int inQuality ) {
	mQuality = inQuality;

	if( mQuality > 100 || mQuality < 0 ) {
		printf( "JPEG quality must be in [0,100]\n" );
		mQuality = 50;
		}
	}
		


inline int JPEGImageConverter::getQuality() {
	return mQuality;
	}



#endif
