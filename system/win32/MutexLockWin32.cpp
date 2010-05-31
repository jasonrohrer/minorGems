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
 * 2002-October-18    Jason Rohrer
 * Moved common include out of header and into platform-specific cpp files,
 * since MemoryTrack uses a mutex lock.
 *
 * 2002-October-19    Jason Rohrer
 * Changed to use malloc instead of new internally to work with debugMemory.
 * Made use of mNativeObjectPointer a bit cleaner.
 * Fixed a few bugs with new use of mNativeObjectPointer.
 */

#include "minorGems/common.h"



#include "minorGems/system/MutexLock.h"

#include <windows.h>
#include <stdlib.h>



/**
 * Win32-specific implementation of the MutexLock class member functions.
 */


MutexLock::MutexLock() {
	// allocate a handle on the heap
	mNativeObjectPointer = malloc( sizeof( HANDLE ) );
	
	// retrieve handle from the heap
	HANDLE *mutexPointer = (HANDLE *)mNativeObjectPointer;
	
	// create the mutex
	*mutexPointer = CreateMutex(
		(LPSECURITY_ATTRIBUTES) NULL,	//  no attributes
  		(BOOL) false,			// not initially locked
  		(LPCTSTR) NULL );		// no name
	}



MutexLock::~MutexLock() {
	// retrieve handle from the heap
	HANDLE *mutexPointer = (HANDLE *)mNativeObjectPointer;
	  
	// destroy the mutex	
	CloseHandle( *mutexPointer );
	
	// de-allocate the mutex structure from the heap
	free( mutexPointer );
	}



void MutexLock::lock() {
	// retrieve handle from the heap
	HANDLE *mutexPointer = (HANDLE *)mNativeObjectPointer;
	
	WaitForSingleObject( *mutexPointer, INFINITE );
	}



void MutexLock::unlock() {
	// retrieve handle from the heap
	HANDLE *mutexPointer = (HANDLE *)mNativeObjectPointer;
	
	ReleaseMutex( *mutexPointer );
	}

