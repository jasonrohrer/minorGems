/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2001-January-27		Jason Rohrer
 * Switched to a ThreadSafePrinter in attempt to get it to work on Win32.
 * Changed print call to printf. 
 *
 * 2002-November-14		Jason Rohrer
 * Added missing destructor. 
 */

#include "minorGems/common.h"



#ifndef TEST_THREAD_CLASS_INCLUDED
#define TEST_THREAD_CLASS_INCLUDED

#include "Thread.h"

#include "ThreadSafePrinter.h"

#include <stdio.h>

/**
 * Test subclass of Thread class.  Useful for testing if platform-specific
 * thread implmentations are working.
 *
 * @author Jason Rohrer
 */
class TestThread : public Thread {
	
	public:
		/**
		 * Constructs a test thread and tells it how high to count to.
		 *
		 * @param inID id number thread will print along with count.
		 * @param inNumToCount thread will count from 0 to this number.
		 */
		TestThread( int inID, int inNumToCount );
		
		~TestThread();
		
		// override the run method from PThread
		void run();
	
	private:
		int mID;
		int mNumToCount;
	
	};



inline TestThread::TestThread( int inID, int inNumToCount ) 
	: mID( inID ), mNumToCount( inNumToCount ) {
	
	}



inline TestThread::~TestThread() {
	
	}



inline void TestThread::run() {
	for( int i=0; i<=mNumToCount; i++ ) {
		ThreadSafePrinter::printf( "Thread %d counting %d.\n", mID, i );
		}
	}	
	
#endif
