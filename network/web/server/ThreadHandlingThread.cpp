/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-April-5    Jason Rohrer
 * Changed to extend StopSignalThread.
 *
 * 2002-August-6   Jason Rohrer
 * Changed member init order.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */



#include "ThreadHandlingThread.h"



ThreadHandlingThread::ThreadHandlingThread()
    : mThreadVector( new SimpleVector<RequestHandlingThread*>() ),
      mLock( new MutexLock() ) {

    this->start();
    }



ThreadHandlingThread::~ThreadHandlingThread() {
    stop();
    join();

    mLock->lock();

    int numThreads = mThreadVector->size();

    // join each thread and delete it
    for( int i=0; i<numThreads; i++ ) {
        RequestHandlingThread *thread =
            *( mThreadVector->getElement( i ) );

        thread->join();
        delete thread;
        }

    
    delete mThreadVector;

    mLock->unlock();

    
    delete mLock;
    }



void ThreadHandlingThread::addThread(
    RequestHandlingThread *inThread ) {
    mLock->lock();

    mThreadVector->push_back( inThread );
    
    mLock->unlock();
    }



void ThreadHandlingThread::run() {

    while( !isStopped() ) {
        
        // sleep for 5 seconds 
        sleep( 5000 );

        // printf( "Thread handler looking for finished threads\n" );
        
        // look for threads that need to be deleted
        mLock->lock();

        

        char threadFound = true;
        
        // examine each thread
        while( threadFound ) {
            threadFound = false;

            int numThreads = mThreadVector->size();
            
            for( int i=0; i<numThreads; i++ ) {
                RequestHandlingThread *thread =
                    *( mThreadVector->getElement( i ) );

                if( thread->isDone() ) {

                    // join the thread before destroying it
                    // to prevent memory leaks
                    thread->join();
                    
                    // remove the thread from the vector and delete it
                    // printf( "deleting a thread\n" );
                    mThreadVector->deleteElement( i );
                    delete thread;

                    threadFound = true;
                    
                    // jump out of the for loop, since our
                    // vector size has changed
                    i = numThreads;
                    }
                }

            }
        
        mLock->unlock();
        }
    
    }


