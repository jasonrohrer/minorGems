/*
 * Modification History
 *
 * 2001-January-11		Jason Rohrer
 * Created.
 *
 * 2001-January-11		Jason Rohrer
 * Added a willBlock() function. 
 *
 * 2001-February-24		Jason Rohrer
 * Fixed incorrect delete usage.
 *
 * 2002-February-11		Jason Rohrer
 * Fixed a mistake in the signal() comment.
 *
 * 2003-August-26   Jason Rohrer
 * Added support for timeouts on wait.
 *
 * 2003-December-28   Jason Rohrer
 * Fixed a bug in semaphore value when we timeout on wait.
 *
 * 2004-January-9   Jason Rohrer
 * Fixed a preprocessor error.
 */

#include "minorGems/common.h" 



#ifndef SEMAPHORE_CLASS_INCLUDED
#define SEMAPHORE_CLASS_INCLUDED

#include "MutexLock.h"
#include "BinarySemaphore.h"


/**
 * General semaphore with an unbounded value.
 *
 * This class uses BinarySemaphores to implement general semaphores,
 * so it relies on platform-specific BinarySemaphore implementations, 
 * but this class itself is platform-independent.
 *
 * @author Jason Rohrer
 */
class Semaphore {

	public:
		
		/**
		 * Constructs a semaphore.
		 *
		 * @param inStartingValue the starting value for this semaphore.
		 *   Defaults to 0 if unspecified.
		 */
		Semaphore( int inStartingValue = 0 );
		
		~Semaphore();
		
		
		/**
		 * If this semaphore's current value is 0, then this call blocks 
		 * on this semaphore until signal() is called by another thread.
		 * If this semaphore's value is >0, then it is decremented by this
         * call.
         *
         * @param inTimeoutInMilliseconds the maximum time to wait in
         *   milliseconds, or -1 to wait forever.  Defaults to -1.
         *
         * @return 1 if the semaphore was signaled, or 0 if it timed out.
		 */
		int wait( int inTimeoutInMilliseconds = -1 );
        
		
		
		/**
		 * If a thread is waiting on this semaphore, then the thread
		 * becomes unblocked.
		 * If no thread is waiting, then the semaphore is incremented.
		 */
		void signal();

		
		/**
		 * Returns true if a call to wait would have blocked.
		 */
		char willBlock();


	private:
		
		// starts at 0
		int mSemaphoreValue;
		
		// mutex semaphore starts at 1
		BinarySemaphore *mMutexSemaphore;
		// blocking semaphore starts at 0
		BinarySemaphore *mBlockingSemaphore;
	};



inline Semaphore::Semaphore( int inStartingValue )
	: mSemaphoreValue( inStartingValue ), 
	mMutexSemaphore( new BinarySemaphore() ),
	mBlockingSemaphore(  new BinarySemaphore() ) {
	
	// increment the mutex semaphore to 1
	mMutexSemaphore->signal();
	}
	
	
	
inline Semaphore::~Semaphore() {
	delete mMutexSemaphore;
	delete mBlockingSemaphore;
	}
	
	
	
inline int Semaphore::wait( int inTimeoutInMilliseconds ) {
    int returnValue;
    // this implementation copied from _Operating System Concepts_, p. 172
	
	// lock the mutex
	mMutexSemaphore->wait();
	// decrement the semaphore
	mSemaphoreValue--;
	if( mSemaphoreValue < 0 ) {
		// we should block
		
		// release the mutex
		mMutexSemaphore->signal();
		
		// block
		returnValue = mBlockingSemaphore->wait( inTimeoutInMilliseconds );

        if( returnValue != 1 ) {
            // timed out

            // increment the semaphore, since we never got signaled
            // lock the mutex
            mMutexSemaphore->wait();
            mSemaphoreValue++;

            // we will unlock the mutex below
            }
		}
    else {
        returnValue = 1;
        }
	
	// release the mutex
	// ( if we were signaled, then the signaller left the mutex locked )
    // ( if we timed out, then we re-locked the mutex above )
	mMutexSemaphore->signal();

    return returnValue;
	}
	
	
	
inline char Semaphore::willBlock() {
	char returnValue = false;
	
	// lock the mutex
	mMutexSemaphore->wait();

	// check if we will block
	if( mSemaphoreValue <= 0 ) {
		returnValue = true;
		}
	
	// release the mutex
	mMutexSemaphore->signal();
	
	return returnValue;
	}
	
	
	
inline void Semaphore::signal() {
	// lock the mutex
	mMutexSemaphore->wait();
	// increment the semaphore
	mSemaphoreValue++;
	if( mSemaphoreValue <= 0 ) {
		// we need to wake up a waiting thread
		mBlockingSemaphore->signal();
		// let the waiting thread unlock the mutex
		}
	else {
		// no threads are waiting, so we need to unlock the mutex
		mMutexSemaphore->signal();
		}
	}
	
	
	

#endif
