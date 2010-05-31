/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2002-October-18    Jason Rohrer
 * Moved common include out of header and into platform-specific cpp files,
 * since MemoryTrack uses a mutex lock.
 * Changed to use malloc instead of new internally to work with debugMemory.
 * Made use of mNativeObjectPointer a bit cleaner.
 */

#include "minorGems/common.h"



#include <minorGems/system/MutexLock.h>
#include <pthread.h>
#include <stdlib.h>

/**
 * Linux-specific implementation of the MutexLock class member functions.
 *
 * May also be compatible with other POSIX-like systems.
 *
 * To compile:
 * g++ -lpthread
 */


MutexLock::MutexLock() {
	// allocate a mutex structure on the heap
	mNativeObjectPointer = malloc( sizeof( pthread_mutex_t ) );
	
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointer;
	
	// init the mutex
	pthread_mutex_init( mutexPointer, NULL );
	}



MutexLock::~MutexLock() {
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointer;
	
	// destroy the mutex	
	pthread_mutex_destroy( mutexPointer );
	
	// de-allocate the mutex structure from the heap
	free( mNativeObjectPointer );
	}



void MutexLock::lock() {
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointer;
	
	pthread_mutex_lock( mutexPointer );
	}



void MutexLock::unlock() {
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointer;
	
	pthread_mutex_unlock( mutexPointer );
	}
