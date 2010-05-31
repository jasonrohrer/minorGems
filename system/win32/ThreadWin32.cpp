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
 * 2004-March-31   Jason Rohrer
 * Added missing call to CloseHandle in destructor.
 * Added support for detatched mode.
 *
 * 2004-April-1   Jason Rohrer
 * Fixed a bug in CloseHandle call pointed out by Mycroftxxx.
 *
 * 2005-January-22   Jason Rohrer
 * Added a static sleep function.
 */
 
#include "minorGems/system/Thread.h"

#include <windows.h>


/**
 * Win32-specific implementation of the Thread class member functions.
 *
 */


// prototype
/**
 * A wrapper for the run method, since windows thread (perhaps) won't take
 * a class's member function.  Takes a pointer to the Thread to run,
 * cast as a void*;
 */
DWORD WINAPI win32ThreadFunction( void * );



Thread::Thread() {
	// allocate a handle on the heap
	mNativeObjectPointer = (void *)( new HANDLE[1] );
	}



Thread::~Thread() {
    // get a pointer to the allocated handle
	HANDLE *threadPointer = (HANDLE *)mNativeObjectPointer;

    // close the handle to ensure that the thread resources are freed
    CloseHandle( threadPointer[0] );
    
    // de-allocate the thread handle from the heap
	delete [] threadPointer;
	}



void Thread::start( char inDetach ) {

    mIsDetached = inDetach;
    
	// get a pointer to the allocated handle
	HANDLE *threadPointer = (HANDLE *)mNativeObjectPointer;
	
	DWORD threadID;
	
	threadPointer[0] = CreateThread( 
						(LPSECURITY_ATTRIBUTES)NULL,	// no attributes 
						(DWORD)0, 		// default stack size
						win32ThreadFunction,	// function
						(LPVOID)this,			// function arg
						(DWORD)0,		// no creation flags (start thread immediately)
						&threadID );					 
	}    



void Thread::join() {
	
	HANDLE *threadPointer = (HANDLE *)mNativeObjectPointer;
	
	WaitForSingleObject( threadPointer[0], INFINITE );
	}


void Thread::staticSleep( unsigned long inTimeInMilliseconds ) {
	Sleep( inTimeInMilliseconds );
	}


// takes a pointer to a Thread object as the data value
DWORD WINAPI win32ThreadFunction( void *inPtrToThread ) {
	Thread *threadToRun = (Thread *)inPtrToThread;
	threadToRun->run();

    
    if( threadToRun->isDetatched() ) {
        // thread detached, so we must destroy it
        delete threadToRun;
        }

	return 0;
	}
