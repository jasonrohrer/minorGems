/*
 * Modification History
 *
 * 2001-February-24	Jason Rohrer
 * Created    
 */
 
 
#ifndef SCREEN_STEREO_PROCESSOR_INCLUDED
#define SCREEN_STEREO_PROCESSOR_INCLUDED 

#include "StereoProcessor.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/ScreenGraphics.h"

/**
 * Implementation of StereoProcessor that displays images to 
 * the screen.  The images are displayed on the screen left above depth map.
 * the right image is not displayed.
 *
 * @author Jason Rohrer
 */
class ScreenStereoProcessor : public StereoProcessor {
	
	public:
		
		/**
		 * Constructs a processor and displays its screen.
		 *
		 * @param inWidth the width of the screen.
		 * @param inHeight the height of the screen.
		 */
		ScreenStereoProcessor( int inWidth, int inHeight );
		
		~ScreenStereoProcessor();
		
		
		// implements StereoProcessor interface
		// images must be the same width as the screen,
		// and the twice the image heights must equal the screen height
		virtual void addData( Image *inLeftImage, Image *inRightImage, 
			Image *inStereoData );	
	
	private:
		int mWidth, mHeight;
		
		ScreenGraphics *mScreen;
		GraphicBuffer *mGraphicBuffer;
		
		unsigned long *mBuffer;
		
		
	};



inline ScreenStereoProcessor::
	ScreenStereoProcessor( int inWidth, int inHeight )
	: mWidth( inWidth ), mHeight( inHeight ),
	mScreen( new ScreenGraphics( inWidth, inHeight ) ) {
	
	mBuffer = new unsigned long[ mWidth * mHeight ];
	mGraphicBuffer = new GraphicBuffer( mBuffer, mWidth, mHeight );
	
	}



inline ScreenStereoProcessor::
	~ScreenStereoProcessor() {
	
	delete mScreen;
	delete [] mBuffer;
	delete mGraphicBuffer;
	}



inline void ScreenStereoProcessor::addData( Image *inLeftImage, 
	Image *inRightImage, Image *inStereoData ) {
	
	
	double *imagePixelsR = inLeftImage->getChannel( 0 );
	double *imagePixelsG;
	double *imagePixelsB;
	
	
	// check the number of input channels
	if( inLeftImage->getNumChannels() == 3 ) {
		imagePixelsG = inLeftImage->getChannel( 1 );
		imagePixelsB = inLeftImage->getChannel( 2 );
		}
	else {
		// all channels the same
		imagePixelsG = inLeftImage->getChannel( 0 );
		imagePixelsB = inLeftImage->getChannel( 0 );
		}
		
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

	// stereo data is always only one channel
	imagePixelsR = inStereoData->getChannel( 0 );
	imagePixelsG = inStereoData->getChannel( 0 );
	imagePixelsB = inStereoData->getChannel( 0 );
	
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
	
	
	// delete the data
	delete inLeftImage;
	delete inRightImage;
	delete inStereoData;
	}
	
	
	
#endif
