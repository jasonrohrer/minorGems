/*
 * Modification History
 *
 * 2001-February-25		Jason Rohrer
 * Created.
 *
 * 2001-February-26		Jason Rohrer
 * Finished and tested.  Seems to be working, but
 * performance is poor on linux machines with multiple processors.     
 */
 
 
#ifndef THREADED_PARTIAL_STEREO_INCLUDED
#define THREADED_PARTIAL_STEREO_INCLUDED 

#include "Stereo.h"
#include "PartialStereo.h"
#include "StereoThread.h"

#include "minorGems/util/random/RandomSource.h"

#include <float.h>
#include <stdio.h>

/**
 * Stereo implementation that runs several non-overlapping partial
 * stereos in separate threads.
 *
 * @author Jason Rohrer
 */
class ThreadedPartialStereo : public Stereo {
	
	public:
		
		public:
		
		/**
		 * Constructs a multi-threaded multi-channel stereo object.
		 *
		 * @param inStereo the stereo object to be used
		 *   on each channel individually.  For the sake of 
		 *   efficiency, inStereo should be an implementation
		 *   that only processes the channel specified by
		 *   a call to its getImageChannel() function.
		 *   Is destroyed when the class is destroyed.
		 * @param inNumParts the number of sections to split
		 *   the image into (one section per thread).
		 */
		ThreadedPartialStereo( PartialStereo *inStereo, int inNumParts );

		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		virtual Stereo *copy();
	
	private:
		int mNumParts;
		PartialStereo *mStereo;
				
	};



inline ThreadedPartialStereo::ThreadedPartialStereo( 
	PartialStereo *inStereo, int inNumParts  )
	: Stereo( inStereo->getMaxDisparity() ),
	mStereo( inStereo ), mNumParts( inNumParts ) {
	
	}



inline Stereo *ThreadedPartialStereo::copy() {

	ThreadedPartialStereo *returnValue =
		new ThreadedPartialStereo( 
			(PartialStereo *)( mStereo->copy() ), mNumParts );
	
	return returnValue;
	}



inline Image *ThreadedPartialStereo::computeDepthMap( 
	Image *inLeft, Image *inRight ) {
	
	
	// where threads will put pointers to their output images
	Image **outImages = new Image*[ mNumParts ];
	
	StereoThread **threads = new StereoThread*[ mNumParts ];
	
	// copies of stereo objects
	PartialStereo **stereoCopies = new PartialStereo*[ mNumParts ];
	
	int i;
	
	// for each part
	for( i=0; i<mNumParts; i++ ) {
		// copy stereo object and spawn a thread
		
		stereoCopies[i] = (PartialStereo *)( mStereo->copy() );
		
		// assign stereo object to process its specific image part
		// split image horizontally:
		double yStart = i / (double)mNumParts;
		double yEnd =  (i+1) / (double)mNumParts;
		stereoCopies[i]->setRange( 0.0, 1.0, yStart, yEnd );
		
		
		
		// create the thread
		// don't copy input images since stereo object shouldn't
		// modify them (so they are accessed in a thread-safe manner)
		threads[i] = new StereoThread( stereoCopies[i], inLeft, inRight,
			&( outImages[i] ) );
		
		// start the thread
		threads[i]->start();
		}
	
	// now join each thread to wait for them to finish
	for( i=0; i<mNumParts; i++ ) {
		threads[i]->join();
		}
	
	// delete the threads and the stereo copies
	for( i=0; i<mNumParts; i++ ) {
	
		delete threads[i];
		delete stereoCopies[i];
		}
	delete [] stereoCopies;
	delete [] threads;
	
	// now our out images should be filled.
	
	// combine their parts to make a final image
	
	// first, make sure they are not NULL
	char imagesOK = true;
	for( i=0; i<mNumParts; i++ ) {
	
		if( outImages[i] == NULL ) {
			imagesOK = false;
			}
		}
	if( !imagesOK ) {
		// delete any ok images and return NULL
		for( i=0; i<mNumParts; i++ ) {
			if( outImages[i] != NULL ) {
				delete outImages[i];
				}
			}
		
		delete [] outImages;
		
		return NULL;
		}
	else {
		// the images are ok...
		
		// average them together to make our output
		
		int w = outImages[0]->getWidth();
		int h = outImages[0]->getHeight();
		int numPixels = w * h;
		
		Image *finalImage = new Image( w, h, 1 );
		double *finalChannel = finalImage->getChannel( 0 );
		
		int p;
		
		// combine all of the output images into the final image
		for( i=0; i<mNumParts; i++ ) {
			// y range of this part
			double yStart = i / (double)mNumParts;
			double yEnd =  (i+1) / (double)mNumParts;
			int yIntStart = (int)( h * yStart );
			int yIntEnd = (int)( h * yEnd - 1 );
			
			double *channel = outImages[i]->getChannel( 0 );
			
			for( int y=yIntStart; y<=yIntEnd; y++ ) {
				for( int x=0; x<w; x++ ) {
					int index = y * w + x;
					finalChannel[ index ] = channel[ index ];
					}
				}
			}
		
		// now delete the temp images
		for( i=0; i<mNumParts; i++ ) {
			delete outImages[i];
			}
		
		delete [] outImages;
			
		return finalImage;			
		}
	}
	
		
		
#endif
