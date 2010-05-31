/*
 * Modification History
 *
 * 2001-February-20		Jason Rohrer
 * Created. 
 *
 * 2001-February-21	Jason Rohrer
 * Fixed a bug in the image indexing.    
 */
 
 
#ifndef SCREEN_IMAGE_PAIR_SIDE_PROCESSOR_INCLUDED
#define SCREEN_IMAGE_PAIR_SIDE_PROCESSOR_INCLUDED 


#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ScreenGraphics.h"

/**
 * Implementation of ImagePairSideProcessor that displays images to 
 * the screen.  The images are displayed on the screen left above right.
 *
 * @author Jason Rohrer
 */
class ScreenImagePairSideProcessor : public ImagePairSideProcessor {
	
	public:
		
		/**
		 * Constructs a processor and displays its screen.
		 *
		 * @param inWidth the width of the screen.
		 * @param inHeight the height of the screen.
		 */
		ScreenImagePairSideProcessor( int inWidth, int inHeight );
		
		~ScreenImagePairSideProcessor();
		
		
		// implements ImagePairSideProcessor interface
		// images must be the same width as the screen,
		// and the sum of the image heights must equal the screen height
		virtual void process( Image *inLeft, Image *inRight );	
	
	private:
		int mWidth, mHeight;
		
		ScreenGraphics *mScreen;
		GraphicBuffer *mGraphicBuffer;
		
		unsigned long *mBuffer;
		
		
	};



inline ScreenImagePairSideProcessor::
	ScreenImagePairSideProcessor( int inWidth, int inHeight )
	: mWidth( inWidth ), mHeight( inHeight ),
	mScreen( new ScreenGraphics( inWidth, inHeight ) ) {
	
	mBuffer = new unsigned long[ mWidth * mHeight ];
	mGraphicBuffer = new GraphicBuffer( mBuffer, mWidth, mHeight );
	
	}



inline ScreenImagePairSideProcessor::
	~ScreenImagePairSideProcessor() {
	
	delete mScreen;
	delete [] mBuffer;
	delete mGraphicBuffer;
	}



inline void ScreenImagePairSideProcessor::process( 
	Image *inLeft, Image *inRight ) {
	
	double *imagePixelsR = inLeft->getChannel( 0 );
	double *imagePixelsG = inLeft->getChannel( 1 );
	double *imagePixelsB = inLeft->getChannel( 2 );
	
	int y, x;
	
	// top half = left image
	for( y=0; y<( mHeight / 2 ); y++ ) {
		for( x=0; x<mWidth; x++ ) {
			int i = y * mWidth + x;
			
			unsigned char r = (unsigned char)( 255 * imagePixelsR[i] );
			unsigned char g = (unsigned char)( 255 * imagePixelsG[i] );
			unsigned char b = (unsigned char)( 255 * imagePixelsB[i] );		
		
			mBuffer[i] = b | g << 8 | r << 16 | 0 << 24;
			
			}
		}


	imagePixelsR = inRight->getChannel( 0 );
	imagePixelsG = inRight->getChannel( 1 );
	imagePixelsB = inRight->getChannel( 2 );
	
	// bottom half = right image
	for( y=( mHeight / 2 ); y<mHeight; y++ ) {
		for( x=0; x<mWidth; x++ ) {
			int i = ( y - ( mHeight / 2 ) ) * mWidth + x;
			
			unsigned char r = (unsigned char)( 255 * imagePixelsR[i] );
			unsigned char g = (unsigned char)( 255 * imagePixelsG[i] );
			unsigned char b = (unsigned char)( 255 * imagePixelsB[i] );		
		
			mBuffer[y * mWidth + x] = b | g << 8 | r << 16 | 0 << 24;
			}
		}
	
	// swap to screen
	mScreen->swapBuffers( mGraphicBuffer );
	
	mBuffer = mGraphicBuffer->getBuffer();
	}
	
	
	
#endif
