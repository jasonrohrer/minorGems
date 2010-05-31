/*
 * Modification History
 *
 * 2001-January-11		Jason Rohrer
 * Created.
 *
 * 2001-January-27		Jason Rohrer
 * Fixed a bug in the precompiler directives.
 *
 * 2003-August-26   Jason Rohrer
 * Added support for timeouts on wait.
 */

#include "minorGems/common.h" 



#ifndef BINARY_SEMAPHORE_CLASS_INCLUDED
#define BINARY_SEMAPHORE_CLASS_INCLUDED

#include "MutexLock.h"

/**
 * Binary semaphore class.  Semaphore starts out with a value of 0.
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the mac/ linux/ and win32/ 
 *   subdirectories).
 *
 * @author Jason Rohrer
 */
class BinarySemaphore {

	public:
		
		/**
		 * Constructs a binary semaphore.
		 */
		BinarySemaphore();
		
		~BinarySemaphore();
		
		
		/**
		 * Blocks on this semaphore until signal() is called by another thread.
		 * Note that if signal() has already been called before wait() is
		 * called, then this call will return immediately, though the semaphore
		 * is reset to 0 by this call.
         *
         * @param inTimeoutInMilliseconds the maximum time to wait in
         *   milliseconds, or -1 to wait forever.  Defaults to -1.
         *
         * @return 1 if the semaphore was signaled, or 0 if it timed out.
		 */
		int wait( int inTimeoutInMilliseconds = -1 );
		
		
		/**
		 * Signals the semaphore, allowing a waiting thread to return from
		 * its call to wait(). (The semaphore is set to 1 by this call if
		 * no thread is waiting on the semaphore currently.)
		 */
		void signal();


	private:
		
		// starts at 0
		int mSemaphoreValue;
		
		/**
		 * Used by platform-specific implementations.
		 */		
		void *mNativeObjectPointerA;
		void *mNativeObjectPointerB;
	};


#endif
