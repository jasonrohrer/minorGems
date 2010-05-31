/*
 * Modification History
 *
 * 2004-November-9    Jason Rohrer
 * Created.
 * Modified from MUTE's ChannelReceivingThreadManager.
 *
 * 2005-January-9    Jason Rohrer
 * Changed to sleep on a semaphore to allow sleep to be interrupted.
 */



#include "minorGems/system/FinishedSignalThreadManager.h"



FinishedSignalThreadManager::FinishedSignalThreadManager()
    : mLock( new MutexLock() ),
      mThreadVector( new SimpleVector<FinishedSignalThread *>() ),
      mStopSignal( false ),
      mSleepSemaphore( new BinarySemaphore() ) {

    this->start();
    }



FinishedSignalThreadManager::~FinishedSignalThreadManager() {
    mLock->lock();
    mStopSignal = true;
    mLock->unlock();

    // signal the sleeping semaphore to wake up the thread
    mSleepSemaphore->signal();
    
    this->join();

    mLock->lock();

    // destroy all remaining threads
    int numThreads = mThreadVector->size();

    for( int i=0; i<numThreads; i++ ) {
        delete *( mThreadVector->getElement( i ) );
        }
    delete mThreadVector;

    
    mLock->unlock();
    delete mLock;

    delete mSleepSemaphore;
    }



void FinishedSignalThreadManager::addThread(
    FinishedSignalThread *inThread ) {

    mLock->lock();

    mThreadVector->push_back( inThread );

    mLock->unlock();
    }



void FinishedSignalThreadManager::run() {
    
    char stopped;

    mLock->lock();
    stopped = mStopSignal;
    mLock->unlock();


    while( !stopped ) {
        // wait for 10 seconds
        int wasSignaled = mSleepSemaphore->wait( 10000 );

        if( wasSignaled == 1 ) {
            // signaled... we should stop
            return;
            }
        
        char foundFinished = true;

        while( foundFinished ) {
            foundFinished = false;
            
            mLock->lock();
            
            int numThreads = mThreadVector->size();

            for( int i=0; i<numThreads && !foundFinished; i++ ) {
                
                FinishedSignalThread *currentThread =
                    *( mThreadVector->getElement( i ) );

                if( currentThread->isFinished() ) {
                    delete currentThread;
                    mThreadVector->deleteElement( i );
                    foundFinished = true;
                    }
                }
            mLock->unlock();
            }

        mLock->lock();
        stopped = mStopSignal;
        mLock->unlock();
        }
    }

