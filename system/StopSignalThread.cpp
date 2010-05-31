/*
 * Modification History
 *
 * 2002-April-4    Jason Rohrer
 * Created.
 * Changed to reflect the fact that the base class
 * destructor is called *after* the derived class destructor.
 *
 * 2002-August-5   Jason Rohrer
 * Fixed member initialization order to match declaration order.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 *
 * 2005-January-9   Jason Rohrer
 * Changed to sleep on a semaphore to make sleep interruptable by stop.
 */



#include "StopSignalThread.h"



StopSignalThread::StopSignalThread()
    : mStopLock( new MutexLock() ), mStopped( false ),
      mSleepSemaphore( new BinarySemaphore() ) {

    }



StopSignalThread::~StopSignalThread() {

    delete mStopLock;
    delete mSleepSemaphore;
    }



void StopSignalThread::sleep( unsigned long inTimeInMilliseconds ) {
    mSleepSemaphore->wait( inTimeInMilliseconds );
    }



char StopSignalThread::isStopped() {
    mStopLock->lock();
    char stoped = mStopped;
    mStopLock->unlock();

    return stoped;
    }



void StopSignalThread::stop() {
    mStopLock->lock();
    mStopped = true;
    mStopLock->unlock();

    // signal the semaphore to wake up the thread, if it is sleeping
    mSleepSemaphore->signal();
    }

