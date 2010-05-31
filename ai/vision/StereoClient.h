/*
 * Modification History
 *
 * 2001-February-17		Jason Rohrer
 * Created. 
 *
 * 2001-February-20		Jason Rohrer
 * Changed to take an ImagePairSideProcessor also.
 * Moved StereoClientThread into a separate class.
 *
 * 2001-February-24		Jason Rohrer
 * Fixed a bug in deletion.  Note that delete must be invoked
 * separately (?) for each pointer.  In other words, the following
 * command does not work correctly:
 * delete pointerA, pointerB; 
 * Changed so that left and right images are passed into stereo processor.
 *
 * 2001-February-26		Jason Rohrer
 * Added grayscale conversion before processing stereo (to
 * improve noise robustness and to speed up stereo computation).
 *
 * 2001-April-12   Jason Rohrer
 * Changed to take ImageSources instead of taking
 * the addresses of networked image servers.
 *
 * 2001-May-2   Jason Rohrer
 * Removed use of image gathering threads because the paradiagm
 * has changed:  We need to wait for the last move to finish
 * before collecting images for the next move.
 */
 
 
#ifndef STEREO_CLIENT_INCLUDED
#define STEREO_CLIENT_INCLUDED 

#include <stdio.h>

#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"

#include "Stereo.h"
#include "StereoProcessor.h"
#include "ImagePairSideProcessor.h"
#include "StereoClientThread.h"
#include "ImageSource.h"


#include "minorGems/util/CircularBuffer.h"

#include "minorGems/graphics/ImageColorConverter.h"
	
/**
 * Client for networked stereo applications.  Connects
 * to two image servers (left and right), receives image pairs,
 * computes stereo, and then sends the disparity data to 
 * a stereo processor.
 *
 * Runs as a thread.  Call start() to start the client.
 *
 * @author Jason Rohrer
 */
class StereoClient : public Thread {
	
	public:
		
		/**
		 * Constructs a stereo client.
		 *
		 * All parameters are destroyed when this class is destroyed.
		 *
		 * @param inLeftImageSource the source for left
		 *   images for stereo pairs.
		 * @param inRightImageSource the source for right
		 *   images for stereo pairs.
		 * @param inStereo the class to compute disparity maps
		 *   from image pairs.
		 * @param inImageProcessor the processor for stereo pairs.
		 * @param inProcessor the stereo processor that disparity
		 *   maps will be sent to.
		 */
		StereoClient( ImageSource *inLeftImageSource,
					  ImageSource *inRightImageSource,
					  Stereo *inStereo,
					  StereoProcessor *inStereoProcessor,
					  ImagePairSideProcessor *inImageProcessor );
		
		~StereoClient();
		
		
		/**
		 * Signals the client to stop.  Should call join()
		 * afterward to wait for the client to stop.
		 */
		void stop();
		
		
		// implements the Thread interface
		void run();
		
	private:
		MutexLock *mStopFlagLock;
		char mStopFlag;
		
		
		Stereo *mStereo;
		
		StereoProcessor *mStereoProcessor;	
		ImagePairSideProcessor *mImageProcessor;
		
		
		ImageSource *mLeftImageSource;
	    ImageSource *mRightImageSource;
		
		// buffers for receive threads to put images into
		CircularBuffer *mLeftImageBuffer;
		CircularBuffer *mRightImageBuffer;	
		
		// threads for receiving images
		//StereoClientThread *mLeftThread;
		//StereoClientThread *mRightThread;
			
	};



inline StereoClient::StereoClient( ImageSource *inLeftImageSource,
								   ImageSource *inRightImageSource,
								   Stereo *inStereo,
								   StereoProcessor *inStereoProcessor,
								   ImagePairSideProcessor *inImageProcessor ) 
	
	: mLeftImageSource( inLeftImageSource ),
	mRightImageSource( inRightImageSource ),
	mStereo( inStereo ),
	mImageProcessor( inImageProcessor ),
	mStereoProcessor( inStereoProcessor ),
	mLeftImageBuffer( new CircularBuffer( 1 ) ),
	mRightImageBuffer( new CircularBuffer( 1 ) ),
	mStopFlag( false ), mStopFlagLock( new MutexLock() ) {

	
	//mLeftThread = new StereoClientThread( mLeftImageSource,
	//									  mLeftImageBuffer );
	//mRightThread = new StereoClientThread( mRightImageSource,
	//									   mRightImageBuffer );
		
	}



inline StereoClient::~StereoClient() {

	delete mLeftImageSource;
	delete mRightImageSource;
	//delete mLeftThread;
	//delete mRightThread;

	delete mStereo; 
	delete mStereoProcessor; 
	delete mImageProcessor; 
	delete mLeftImageBuffer; 
	delete mRightImageBuffer;
	
	delete mStopFlagLock;	
	}



inline void StereoClient::stop() {
	mStopFlagLock->lock();
	mStopFlag = true;
	mStopFlagLock->unlock();
	}



inline void StereoClient::run() {	
	
	//mLeftThread->start();
	//mRightThread->start();
	
	char running = true;
	
	int numRead = 0;
	while( running ) {
		
		//Image *leftImage = (Image *)( mLeftImageBuffer->readNextObject() );
		//Image *rightImage = (Image *)( mRightImageBuffer->readNextObject() );

		Image *leftImage = mLeftImageSource->getNextImage();
		Image *rightImage = mRightImageSource->getNextImage();
		
		// convert images to grayscale before computing stereo
		Image *leftGrayImage = 
			ImageColorConverter::RGBToGrayscale( leftImage );
		Image *rightGrayImage = 
			ImageColorConverter::RGBToGrayscale( rightImage );
		
		mImageProcessor->process( leftImage, rightImage );
		
		Image *depthMap = mStereo->computeDepthMap( leftGrayImage, 
			rightGrayImage );
		
		mStereoProcessor->addData( leftImage, rightImage, depthMap );
		
		delete leftGrayImage;
		delete rightGrayImage;
		
		// check for stop signal
		mStopFlagLock->lock();
		running = ! mStopFlag;
		mStopFlagLock->unlock();
		}
	
	// stop each thread, and empty out buffers
	
	
	/** 
	 * 1. Check if the buffer has readable elements (canRead()).
	 * 2. Call stop().
	 * 3. Read one readable element (depends on step 1).
	 * 4. Call join().
	 */
	
	// left
	/*
	char canRead = mLeftImageBuffer->canRead();
	mLeftThread->stop();
	if( canRead ) {
		// read and destroy the element that might be blocking the thread
		Image *image = (Image *)( mLeftImageBuffer->readNextObject() );
		delete image;
		}
	mLeftThread->join();	
	
	// look for last data in buffer
	canRead = mLeftImageBuffer->canRead();
	while ( canRead ) {
		// read and destroy last element
		Image *image = (Image *)( mLeftImageBuffer->readNextObject() );
		delete image;
		canRead = mLeftImageBuffer->canRead();
		}
		
	// right
	canRead = mRightImageBuffer->canRead();
	mRightThread->stop();
	if( canRead ) {
		// read and destroy the element that might be blocking the thread
		Image *image = (Image *)( mRightImageBuffer->readNextObject() );
		delete image;
		}
	mRightThread->join();	
	
	// look for last data in buffer
	canRead = mRightImageBuffer->canRead();
	while( canRead ) {
		// read and destroy last element
		Image *image = (Image *)( mRightImageBuffer->readNextObject() );
		delete image;
		canRead = mRightImageBuffer->canRead();
		}
		
	// both threads stopped, and buffers are emptied		
	*/
	}

#endif
