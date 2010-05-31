/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2002-November-14		Jason Rohrer
 * Added more verbose printouts. 
 *
 * 2004-March-31    Jason Rohrer
 * Added test of detached threads. 
 */

#include "TestThread.h"

int numToCount = 1000;


/**
 * Main method that spawns two TestThreads.
 *
 * @author Jason Rohrer
 */
int main() {
	
	TestThread *thread1 = new TestThread( 1, numToCount );
	TestThread *thread2 = new TestThread( 2, numToCount );
    TestThread *thread3 = new TestThread( 3, numToCount );
	
	
    ThreadSafePrinter::printf( "Starting thread 1\n" );
	thread1->start();
    ThreadSafePrinter::printf( "Starting thread 2\n" );
	thread2->start();
    ThreadSafePrinter::printf( "Starting thread 3 in detached mode\n" );
	thread3->start( true );

    Thread::sleep( 5000 );
    
    ThreadSafePrinter::printf( "Joining thread 1\n" );
	thread1->join();
    ThreadSafePrinter::printf( "Joining thread 2\n" );
	thread2->join();

    ThreadSafePrinter::printf( "Destroying thread 1\n" );
	delete thread1;
    ThreadSafePrinter::printf( "Destroying thread 2\n" );
	delete thread2;

    ThreadSafePrinter::printf(
        "Thread 3 should handle its own destruction.\n" );
    
	return 0;
	}
