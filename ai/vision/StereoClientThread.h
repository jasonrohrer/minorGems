/*
 * Modification History
 *
 * 2001-February-20		Jason Rohrer
 * Created.
 *
 * 2001-April-12   Jason Rohrer
 * Changed to take ImageSources instead of taking input streams.
 */


#ifndef STEREO_CLIENT_THREAD_INCLUDED
#define STEREO_CLIENT_THREAD_INCLUDED

#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"

#include "Stereo.h"
#include "ImageSource.h"

#include "minorGems/util/CircularBuffer.h"

#include "minorGems/io/InputStream.h"

/**
 * A thread used internally by StereoClient.
 *
 * @author Jason Rohrer
 */
class StereoClientThread : public Thread {
	
	public:
		/**
		 * Constructs a StereoClientThread.
		 *
		 * None of the parameters are destroyed when 
		 * this thread is destroyed.
		 *
		 * @param inSource the source read images from.
		 * @param inBuffer the buffer to put received images into.
		 */
		StereoClientThread( ImageSource *inSource,
							CircularBuffer *inBuffer );

		~StereoClientThread();

		/**
		 * Signals this thread to stop.
		 *
		 * Note that if the thread is blocked on the buffer, it 
		 * will write one more image into the buffer before stopping
		 * (and it will wait for the buffer to unblock before doing so).
		 *
		 * Thus, to stop the thread, the proceedure order must be as follows:
		 * 1. Check if the buffer has readable elements (canRead()).
		 * 2. Call stop().
		 * 3. Read one readable element (depends on step 1).
		 * 4. Call join().
		 *
		 * After this call, this proceedure, the buffer may contain
		 * elements that need to be deleted.
		 */
		void stop();
		
		// implements the Thread interface
		void run();	
	
	private:
		ImageSource *mImageSource;
		CircularBuffer *mBuffer;
		
		MutexLock *mStopFlagLock;
		char mStopFlag;
	};



inline StereoClientThread::StereoClientThread( 
	ImageSource *inSource, CircularBuffer *inBuffer )
	: mImageSource( inSource ), mBuffer( inBuffer ),
	mStopFlagLock( new MutexLock() ), mStopFlag( false ) {
	
	}
	
	
	
inline StereoClientThread::~StereoClientThread() {
	delete mStopFlagLock;
	}	
	
	
	
inline void StereoClientThread::stop() {
	mStopFlagLock->lock();
	mStopFlag = true;
	mStopFlagLock->unlock();
	}
		

inline void StereoClientThread::run() {
	char running = true;
	
	while( running ) {
		
		// get the next image
		Image *receivedImage = mImageSource->getNextImage();
		
		// write the image to our buffer
		mBuffer->writeObject( (void *)receivedImage );
		
		// check the stop flag
		mStopFlagLock->lock();
		running = !mStopFlag;
		mStopFlagLock->unlock();
		}
	}
	
#endif	
