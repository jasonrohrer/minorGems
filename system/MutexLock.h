/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-October-18    Jason Rohrer
 * Moved common include out of header and into platform-specific cpp files,
 * since MemoryTrack uses a mutex lock.
 */



#ifndef MUTEX_LOCK_CLASS_INCLUDED
#define MUTEX_LOCK_CLASS_INCLUDED



#ifdef FORTIFY
#include "minorGems/util/development/fortify/fortify.h"
#endif



/**
 * Mutex lock class.
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the mac/ linux/ and win32/ 
 *   subdirectories).
 *
 * @author Jason Rohrer
 */
class MutexLock {

	public:
		
		/**
		 * Constructs a mutex lock;
		 */
		MutexLock();
		
		~MutexLock();
		
		
		/**
		 * Locks the mutex.  Blocks until mutex available if it's
		 * already locked by another thread.
		 */
		void lock();
		
		/**
		 * Unlocks the mutex.
		 */
		void unlock();


	private:
	
		/**
		 * Used by platform-specific implementations.
		 */		
		void *mNativeObjectPointer;

	};


#endif
