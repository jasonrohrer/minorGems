/*
 * Modification History
 *
 * 2001-February-13		Jason Rohrer
 * Created.
 *
 * 2001-February-16		Jason Rohrer
 * Finished implementation.   
 */
 
 
#ifndef STEREO_THREAD_INCLUDED
#define STEREO_THREAD_INCLUDED 

#include "minorGems/system/Thread.h"

/**
 * Thread that runs a stereo computation.
 *
 * @author Jason Rohrer
 */
class StereoThread : public Thread {
	
	public:
		
		/**
		 * Constructs a stereo thread.
		 *
		 * Note that all parameters must be destroyed by the caller
		 * and that they are not copied by this constructor.
		 *
		 * @param inStereo the stereo object to use.  Must be 
		 *   thread-safe if other threads will be using it simultaneously.
		 * @param inLeft the left image.
		 * @param inRight the right image.
		 * @param outResult pointer to location where the result
		 *   pointer will be returned.
		 */
		StereoThread( Stereo* inStereo, Image *inLeft, Image *inRight,
			Image **outResult );
		
		
		// implements the Thread interface
		void run();
	
	private:
		Stereo *mStereo;
		Image *mLeft;
		Image *mRight;
		Image **mOutResult;
	};



inline StereoThread::StereoThread( Stereo* inStereo, 
	Image *inLeft, Image *inRight, Image **outResult )
	: mStereo( inStereo ), mLeft( inLeft ), mRight( inRight ),
	mOutResult( outResult ) {
	
	}
	
	
	
inline void StereoThread::run() {
	*mOutResult = mStereo->computeDepthMap( mLeft, mRight );
	}

	
	
#endif
