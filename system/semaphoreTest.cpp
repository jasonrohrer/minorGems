/*
 * Modification History
 *
 * 2001-January-11		Jason Rohrer
 * Created.
 *
 * 2001-January-27		Jason Rohrer
 * Made printing in threads thread-safe. 
 */
 
#include "BinarySemaphore.h"
#include "Semaphore.h"
#include "Thread.h"
#include "ThreadSafePrinter.h"

#include <stdio.h>

/**
 * Thread that waits on a semaphore.
 *
 * @author Jason Rohrer
 */
class WaitingThread : public Thread {
	
	public:

		WaitingThread( int inID, Semaphore *inSemaphore );
		
		// override the run method from PThread
		void run();
	
	private:
		Semaphore *mSemaphore;
		int mID;
	};


inline WaitingThread::WaitingThread( int inID, Semaphore *inSemaphore ) 
	: mID( inID ), mSemaphore( inSemaphore ) {
	
	}


inline void WaitingThread::run() {
	for( int i=0; i<10; i++ ) {
		ThreadSafePrinter::printf( "%d waiting for signal %d...\n", mID, i );
		mSemaphore->wait();
		ThreadSafePrinter::printf( "%d received signal %d.\n", mID, i );
		}
	}
 
 
/**
 * Thread that signals on a semaphore.
 *
 * @author Jason Rohrer
 */
class SignalingThread : public Thread {
	
	public:

		SignalingThread( Semaphore *inSemaphore );
		
		// override the run method from PThread
		void run();
	
	private:
		Semaphore *mSemaphore;
	
	};


inline SignalingThread::SignalingThread( Semaphore *inSemaphore ) 
	: mSemaphore( inSemaphore ) {
	
	}


inline void SignalingThread::run() {
	for( int i=0; i<5; i++ ) {
		sleep( 5000 ); 
		ThreadSafePrinter::printf( "Signaling 20 times\n" );
		for( int j=0; j<20; j++ ) {
			mSemaphore->signal();
			}
		}
	} 
 
 
 
int main() {
	int i;
	Semaphore *semph = new Semaphore();

	SignalingThread *threadS = new SignalingThread( semph );
	
	WaitingThread **threadW = new WaitingThread*[10];
	for( i=0; i<10; i++ ) {
		threadW[i] = new WaitingThread( i, semph );
		threadW[i]->start();
		}

	threadS->start();
	
	for( i=0; i<10; i++ ) {
		threadW[i]->join();
		delete threadW[i];
		}
	threadS->join();

	delete semph;
	delete threadS;
	delete [] threadW;	
	return 0;
	} 
