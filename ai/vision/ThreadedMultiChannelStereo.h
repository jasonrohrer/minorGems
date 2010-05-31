/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created.
 *
 * 2001-February-16		Jason Rohrer
 * Finished implementation.   
 *
 * 2001-February-25		Jason Rohrer
 * Added a missing copy() function and fixed an inheritance bug. 
 * Fixed a thread starting bug.  Fixed some image pointer bugs.  
 */
 
 
#ifndef THREADED_MULTI_CHANNEL_STEREO_INCLUDED
#define THREADED_MULTI_CHANNEL_STEREO_INCLUDED 


#include "MultiChannelStereo.h"
#include "StereoThread.h"

/**
 * MultiChannelStereo implementation that uses a separate thread
 * for each channel.
 *
 * @author Jason Rohrer
 */
class ThreadedMultiChannelStereo : public MultiChannelStereo {
	
	
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
		 */
		ThreadedMultiChannelStereo( Stereo *inStereo );

		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		virtual Stereo *copy();
				
	};



inline ThreadedMultiChannelStereo::ThreadedMultiChannelStereo( 
	Stereo *inStereo )
	: MultiChannelStereo( inStereo ) {
	
	}



inline Stereo *ThreadedMultiChannelStereo::copy() {

	ThreadedMultiChannelStereo *returnValue =
		new ThreadedMultiChannelStereo( 
			mStereo->copy() );
	
	return returnValue;
	}



inline Image *ThreadedMultiChannelStereo::computeDepthMap( 
	Image *inLeft, Image *inRight ) {
	
	int numChannels = inLeft->getNumChannels();
	
	// where threads will put pointers to their output images
	Image **outImages = new Image*[ numChannels ];
	
	StereoThread **threads = new StereoThread*[ numChannels ];
	
	// copies of stereo objects
	Stereo **stereoCopies = new Stereo*[ numChannels ];
	
	int i;
	
	// for each channel
	for( i=0; i<numChannels; i++ ) {
		// copy stereo object and spawn a thread
		
		stereoCopies[i] = mStereo->copy();
		
		// assign stereo object to process this channel
		stereoCopies[i]->setImageChannel( i );
		
		// create the thread
		// don't copy input images since stereo object shouldn't
		// modify them (so they are accessed in a thread-safe manner)
		threads[i] = new StereoThread( stereoCopies[i], inLeft, inRight,
			&( outImages[i] ) );
		
		// start the thread
		threads[i]->start();
		}
	
	// now join each thread to wait for them to finish
	for( i=0; i<numChannels; i++ ) {
		threads[i]->join();
		}
	
	// delete the threads and the stereo copies
	for( i=0; i<numChannels; i++ ) {
	
		delete threads[i];
		delete stereoCopies[i];
		}
	delete [] stereoCopies;
	delete [] threads;
	
	// now our out images should be filled.
	
	// average them together to make our final output image
	
	// first, make sure they are not NULL
	char imagesOK = true;
	for( i=0; i<numChannels; i++ ) {
	
		if( outImages[i] == NULL ) {
			imagesOK = false;
			}
		}
	if( !imagesOK ) {
		// delete any ok images and return NULL
		for( i=0; i<numChannels; i++ ) {
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
		
		// first, sum all of the output channels into the final image
		for( i=0; i<numChannels; i++ ) {
			double *channel = outImages[i]->getChannel( 0 );
			for( p=0; p<numPixels; p++ ) {
				finalChannel[p] += channel[p];
				}
			}
		
		// now divide by the number of channels to complete the average
		double invNumChannels = 1.0 / (double)numChannels;
		for( p=0; p<numPixels; p++ ) {
			finalChannel[p] = finalChannel[p] * invNumChannels;
			}
		
		// now delete the temp images
		for( i=0; i<numChannels; i++ ) {
			delete outImages[i];
			}
		
		delete [] outImages;
			
		return finalImage;			
		}
	}
	
		
		
#endif
