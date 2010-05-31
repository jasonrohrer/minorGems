/*
 * Modification History
 *
 * 2001-January-27		Jason Rohrer
 * Created.
 *
 * 2001-March-4		Jason Rohrer
 * Replaced include of <winbase.h> and <windef.h> with <windows.h> 
 * to fix compile bugs encountered with newer windows compilers. 
 *
 * 2003-August-26   Jason Rohrer
 * Added support for timeouts on wait.
 */
 
#include "minorGems/system/BinarySemaphore.h"

#include <windows.h>


/**
 * Win32-specific implementation of the BinarySemaphore class member functions.
 */


/**
 * Native object pointer A is the semaphore handle.
 * Pointer B is not used.
 */


BinarySemaphore::BinarySemaphore() :
	mSemaphoreValue( 0 ) {

	// allocate a handle on the heap
	mNativeObjectPointerA = (void *)( new HANDLE[1] );
	
	// retrieve handle from the heap
	HANDLE *semaphorePointer = (HANDLE *)mNativeObjectPointerA;
	
	
	semaphorePointer[0] = CreateSemaphore(  
		(LPSECURITY_ATTRIBUTES) NULL,	// no attributes
		0,		// initial count
		1,		// maximum count
		(LPCTSTR) NULL );		// no name
	
	}

BinarySemaphore::~BinarySemaphore() {
	
	// retrieve handle from the heap
	HANDLE *semaphorePointer = (HANDLE *)mNativeObjectPointerA;
	
	// destroy the semaphore	
	CloseHandle( semaphorePointer[0] );
	
	// de-allocate the handle from the heap
	delete [] semaphorePointer;
	}



int BinarySemaphore::wait( int inTimeoutInMilliseconds ) {
	
	// retrieve handle from the heap
	HANDLE *semaphorePointer = (HANDLE *)mNativeObjectPointerA;

    if( inTimeoutInMilliseconds == -1 ) {
        WaitForSingleObject( semaphorePointer[0], INFINITE );
        return 1;
        }
    else {
        // timeout
        int result = WaitForSingleObject( semaphorePointer[0],
                                          inTimeoutInMilliseconds );

        if( result == WAIT_TIMEOUT ) {
            return 0;
            }
        else {
            return 1;
            }
        }
	}



void BinarySemaphore::signal() {
	
	// retrieve handle from the heap
	HANDLE *semaphorePointer = (HANDLE *)mNativeObjectPointerA;
	
	ReleaseSemaphore( semaphorePointer[0], 1, (LPLONG) NULL );
	}
