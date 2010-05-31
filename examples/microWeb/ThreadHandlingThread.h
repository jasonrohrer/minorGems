/*
 * Modification History
 *
 * 2001-May-11   Jason Rohrer
 * Created.  
 */
 
 
#ifndef THREAD_HANDLING_THREAD_INCLUDED
#define THREAD_HANDLING_THREAD_INCLUDED 

#include "RequestHandlingThread.h"

#include "minorGems/system/MutexLock.h"

#include "minorGems/util/SimpleVector.h"

#include <string.h>
#include <stdio.h>


/**
 * Thread handler for the microWeb server.  Runs periodically
 * and checks for finished threads that can be deleted.
 *
 * @author Jason Rohrer.
 */
class ThreadHandlingThread : public Thread {

	public:

		/**
		 * Construct a handler.
		 */
		ThreadHandlingThread();

		~ThreadHandlingThread();

		/**
		 * Adds a thread to the set managed by this handler.
		 *
		 * @param inThread the thread to add.
		 */
		void addThread( RequestHandlingThread *inThread );
		
		
		// implements the Thread interface
		virtual void run();
		
	private:

		SimpleVector<RequestHandlingThread*> *mThreadVector;
		
		MutexLock *mLock;
	};



inline ThreadHandlingThread::ThreadHandlingThread()
	: mLock( new MutexLock ),
	  mThreadVector( new SimpleVector<RequestHandlingThread*> ) {

	}



inline ThreadHandlingThread::~ThreadHandlingThread() {
	mLock->lock();

	int numThreads = mThreadVector->size();

	// join each thread and delete it
	for( int i=0; i<numThreads; i++ ) {
		RequestHandlingThread *thread =
			*( mThreadVector->getElement( i ) );

		thread->join();
		delete thread;
		}

	
	delete mThreadVector;

	mLock->unlock();

	
	delete mLock;
	}



inline void ThreadHandlingThread::addThread(
	RequestHandlingThread *inThread ) {
	mLock->lock();

	mThreadVector->push_back( inThread );
	
	mLock->unlock();
	}



inline void ThreadHandlingThread::run() {

	while( true ) {
		
		// sleep for 10 seconds 
		sleep( 10000 );

		// printf( "Thread handler looking for finished threads\n" );
		
		// look for threads that need to be deleted
		mLock->lock();

		

		char threadFound = true;
		
		// examine each thread
		while( threadFound ) {
			threadFound = false;

			int numThreads = mThreadVector->size();
			
			for( int i=0; i<numThreads; i++ ) {
				RequestHandlingThread *thread =
					*( mThreadVector->getElement( i ) );

				if( thread->isDone() ) {

					// join the thread before destroying it
					// to prevent memory leaks
					thread->join();
					
					// remove the thread from the vector and delete it
					// printf( "deleting a thread\n" );
					mThreadVector->deleteElement( i );
					delete thread;

					threadFound = true;
					
					// jump out of the for loop, since our
					// vector size has changed
					i = numThreads;
					}
				}

			}
		
		mLock->unlock();
		}
	
	}



#endif
