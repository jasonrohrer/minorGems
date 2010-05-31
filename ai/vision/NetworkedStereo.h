/*
 * Modification History
 *
 * 2001-February-26		Jason Rohrer
 * Created.     
 */
 
 
#ifndef NETWORKED_STEREO_INCLUDED
#define NETWORKED_STEREO_INCLUDED 

#include "Stereo.h"
#include "NetworkedPartialStereo.h"
#include "StereoThread.h"

#include "minorGems/network/SocketStream.h"

#include <float.h>
#include <stdio.h>

/**
 * Stereo implementation that runs several non-overlapping networked partial
 * stereos in separate threads.
 *
 * @author Jason Rohrer
 */
class NetworkedStereo : public Stereo {
	
	public:
		
		public:
		
		/**
		 * Constructs a multi-threaded, networked stereo object.
		 *
		 * @param inStreams the streams to use to send/receive data for
		 *   the distributed computation of each part.
		 *   Will be destroyed when this class is destroyed.
		 * @param inNumParts the number of sections to split
		 *   the image into (one section per socket stream connection).
		 */
		NetworkedStereo( SocketStream **inStreams, int inNumParts );
	
		~NetworkedStereo();
		
		// implements the stereo interface
		virtual Image *computeDepthMap( Image *inLeft, Image *inRight );
		
		// implementation provided, but actually invoking copy()
		// does not make sense for this class.
		// Currently returns NULL.
		virtual Stereo *copy();
	
	private:
		int mNumParts;
		SocketStream **mStreams;
		NetworkedPartialStereo **mStereoComputers;	
	};



inline NetworkedStereo::NetworkedStereo( 
	SocketStream **inStreams, int inNumParts )
	: Stereo( 1 ), mStreams( inStreams ), mNumParts( inNumParts ),
	mStereoComputers( new NetworkedPartialStereo*[ mNumParts ] ) {
	
	// note that we pass a "dummy" inMaxDisparity value into the Stereo
	// constructor, since disparity is not used on this end of the
	// streams.
	
	// setup stereo computers
	for( int i=0; i<mNumParts; i++ ) {
		mStereoComputers[i] = new NetworkedPartialStereo( mStreams[i] );
		
		double yStart = i / (double)mNumParts;
		double yEnd =  (i+1) / (double)mNumParts;
		mStereoComputers[i]->setRange( 0.0, 1.0, yStart, yEnd );
		}
		
	}



inline NetworkedStereo::~NetworkedStereo() {
	for( int i=0; i<mNumParts; i++ ) {
		delete mStreams[i];
		delete mStereoComputers[i];
		}
	delete [] mStreams;
	delete [] mStereoComputers;
	}



inline Stereo *NetworkedStereo::copy() {

	return NULL;
	}



inline Image *NetworkedStereo::computeDepthMap( 
	Image *inLeft, Image *inRight ) {
	
	
	// where threads will put pointers to their output images
	Image **outImages = new Image*[ mNumParts ];
	
	StereoThread **threads = new StereoThread*[ mNumParts ];
	
	int i;
	
	// for each part
	for( i=0; i<mNumParts; i++ ) {
		// spawn a thread to wait on each stereo computation
	
		
		// create the thread
		// don't copy input images since stereo object shouldn't
		// modify them (so they are accessed in a thread-safe manner)
		threads[i] = new StereoThread( mStereoComputers[i], 
			inLeft, inRight, &( outImages[i] ) );
		
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
		}
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
