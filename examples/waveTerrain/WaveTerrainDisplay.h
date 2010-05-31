/*
 * Modification History
 *
 * 2001-April-21   Jason Rohrer
 * Created.  
 */
 
 
#ifndef WAVE_TERRAIN_DISPLAY_INCLUDED
#define WAVE_TERRAIN_DISPLAY_INCLUDED 

#include "TerrainPath.h"
#include "minorGems/graphics/ScreenGraphics.h"
#include "minorGems/system/Thread.h"

// for memcpy
#include <string.h>

/**
 * Class displays a wave terrain and path with periodic updates
 * of the path.
 *
 * @author Jason Rohrer.
 */
class WaveTerrainDisplay : public Thread {
	public:

		/**
		 * Constructs a display
		 *
		 * All passed in parameters must be destroyed by caller
		 * after this class is stopped, joined,  and destroyed.
		 *
		 * @param inTerrain the terrain under the path.
		 * @param inPath the path to draw.
		 * @param inWidth the width of the display, in pixels.
		 * @param inHeight the height of the display, in pixels.
		 */
		WaveTerrainDisplay( WaveTerrain *inTerrain,
							TerrainPath *inPath,
							int inWidth,
							int inHeight );

		~WaveTerrainDisplay();


		/**
		 * Sends this display thread a stop signal.
		 *
		 * Blocks until the display has stopped.
		 */
		void stop();

		// implements the Thread interface
		virtual void run();


		
	private:

		ScreenGraphics *mScreen;
		WaveTerrain *mTerrain;
		TerrainPath *mPath;
		
		int mWidth;
		int mHeight;

		GraphicBuffer *mBuffer;
		unsigned long *mPixels;

		// pixels for the underlying terrain
		unsigned long *mTerrainPixels;
		
		char mStopFlag;

		int mNumPathSamples;
	};



inline WaveTerrainDisplay::WaveTerrainDisplay(  WaveTerrain *inTerrain,
												TerrainPath *inPath,
												int inWidth,
												int inHeight )
	: mTerrain( inTerrain ), mPath( inPath ),
	  mWidth( inWidth ), mHeight( inHeight ),
	  mScreen( new ScreenGraphics( inWidth, inHeight ) ),
	  mStopFlag( false ),
	  mNumPathSamples( 250 ) {
	
	mPixels = new unsigned long[ mWidth * mHeight ];	  
	mBuffer = new GraphicBuffer( mPixels, mWidth, mHeight );  

	mTerrainPixels = new unsigned long[ mWidth * mHeight ];

	double invHeight = 1.0 / mHeight;
	double invWidth = 1.0 / mWidth;
	
	// setup the terrain pixels
	for( int y=0; y<mHeight; y++ ) {
		for( int x=0; x<mWidth; x++ ) {
			double sample =
				mTerrain->getSample( x * invWidth, y * invHeight );

			int green = 
				(int)( 255 * sample );

			
			// stick the green value into the buffer
			mTerrainPixels[ y * mWidth + x ] = green << 8;
			}
		}
	
	start();
	}



inline WaveTerrainDisplay::~WaveTerrainDisplay() {
	delete mScreen;
	delete mBuffer;
	
	delete [] mPixels;
	delete [] mTerrainPixels;
	}



inline void WaveTerrainDisplay::stop() {
	// don't worry about synchronization for now.
	mStopFlag = true;
	join();
	}



inline void WaveTerrainDisplay::run() {

	double invNumPathSamples = 1.0 / mNumPathSamples;
	
	while( !mStopFlag ) {
		// copy the terrain pixels into the buffer
		memcpy( mPixels, mTerrainPixels,
				mWidth * mHeight * sizeof( long ) );

		// now draw the path
		for( int i=0; i<mNumPathSamples; i++ ) {
			double x;
			double y;
			mPath->getPosition( i * invNumPathSamples, &x, &y );

			int intX = (int)( x * mWidth );
			int intY = (int)( y * mHeight );

			// set the pixel to yellow
			mPixels[ intY * mWidth + intX ] = 0x00FFFF00;
			}
		
		mScreen->swapBuffers( mBuffer );

		// the graphics buffer may have changed during the swap,
		// so we need to get a new set of pixels
		mPixels = mBuffer->getBuffer();

		
		sleep( 200 );
		}
	}



#endif
