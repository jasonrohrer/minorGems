/*
 * Modification History
 *
 * 2001-January-11		Jason Rohrer
 * Created.
 *
 * 2003-August-26   Jason Rohrer
 * Added support for timeouts on wait.
 *
 * 2006-February-28   Jason Rohrer
 * Fixed bug in sub-second timeout computation.
 */
 
#include "minorGems/system/BinarySemaphore.h"
#include "minorGems/system/Time.h"
#include <pthread.h>


/**
 * Linux-specific implementation of the BinarySemaphore class member functions.
 *
 * May also be compatible with other POSIX-like systems.
 *
 * To compile:
 * g++ -lpthread
 */

/**
 * Native object pointer A is the condition variable.
 * Pointer B is the mutex that must go along with it.
 */


BinarySemaphore::BinarySemaphore() :
	mSemaphoreValue( 0 ) {

	// allocate a condition variable structure on the heap
	mNativeObjectPointerA = (void *)( new pthread_cond_t[1] );
	
	// get a pointer to the cond
	pthread_cond_t *condPointer = 
		(pthread_cond_t *)mNativeObjectPointerA;
	
	// init the cond
	pthread_cond_init( &( condPointer[0] ), NULL );
	
	
	// allocate a mutex structure on the heap
	mNativeObjectPointerB = (void *)( new pthread_mutex_t[1] );
	
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointerB;
	
	// init the mutex
	pthread_mutex_init( &( mutexPointer[0] ), NULL );
	
	}

BinarySemaphore::~BinarySemaphore() {
	
	// get a pointer to the cond
	pthread_cond_t *condPointer = 
		(pthread_cond_t *)mNativeObjectPointerA;
	
	// destroy the cond
	pthread_cond_destroy( &( condPointer[0] ) );
	
	// de-allocate the cond structure from the heap
	delete [] condPointer;
	
	
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointerB;
	
	// destroy the mutex	
	pthread_mutex_destroy( &( mutexPointer[0] ) );
	
	// de-allocate the mutex structure from the heap
	delete [] mutexPointer;
	}



int BinarySemaphore::wait( int inTimeoutInMilliseconds ) {

    int returnValue = 1;
    
	// get a pointer to the cond
	pthread_cond_t *condPointer = 
		(pthread_cond_t *)mNativeObjectPointerA;
	
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointerB;
	
	
	// lock the mutex
	pthread_mutex_lock( &( mutexPointer[0] ) );
	
	if( mSemaphoreValue == 0 ) {
		// wait on condition variable, which automatically unlocks
		// the passed-in mutex

        if( inTimeoutInMilliseconds == -1 ) {
            // no timeout
            pthread_cond_wait( &( condPointer[0] ), &( mutexPointer[0] ) );
            }
        else {
            // use timeout version

            unsigned long nsecPerSecond = 1000000000;
            unsigned long nsecPerMillisecond = 1000000;


            unsigned long currentSec;
            unsigned long currentMS;

            Time::getCurrentTime( &currentSec, &currentMS );

            unsigned long currentNS = currentMS * nsecPerMillisecond;

            
            long timeoutSec = inTimeoutInMilliseconds / 1000;
            long extraMS = inTimeoutInMilliseconds % 1000;

            long extraNS = extraMS * nsecPerMillisecond;

            

            unsigned long absTimeoutSec = currentSec + timeoutSec;
            unsigned long absTimeoutNsec = currentNS + extraNS;

            // check for nsec overflow
            if( absTimeoutNsec > nsecPerSecond ) {
                absTimeoutSec += 1;
                absTimeoutNsec -= nsecPerSecond;
                }
            
            struct timespec abstime;
            abstime.tv_sec = absTimeoutSec;
            abstime.tv_nsec = absTimeoutNsec;

            int result = pthread_cond_timedwait( &( condPointer[0] ),
                                                 &( mutexPointer[0] ),
                                                 &abstime );

            if( result != 0 ) {
                // timed out
                returnValue = 0;
                }
            }
        
		// mutex is apparently re-locked when we return from cond_wait
		
		}
		
	// decrement the semaphore value
	mSemaphoreValue = 0;
	
	// unlock the mutex again
	pthread_mutex_unlock( &( mutexPointer[0] ) );

    return returnValue;
	}



void BinarySemaphore::signal() {
	
	// get a pointer to the cond
	pthread_cond_t *condPointer = 
		(pthread_cond_t *)mNativeObjectPointerA;
	
	// get a pointer to the mutex
	pthread_mutex_t *mutexPointer = 
		(pthread_mutex_t *)mNativeObjectPointerB;
	
	
	// lock the mutex
	pthread_mutex_lock( &( mutexPointer[0] ) );
	
	// increment the semaphore value
	mSemaphoreValue = 1;
	
	pthread_cond_signal( &( condPointer[0] ) );
		
		
	// unlock the mutex
	pthread_mutex_unlock( &( mutexPointer[0] ) );
	}
