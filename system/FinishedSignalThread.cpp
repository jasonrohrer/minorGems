/*
 * Modification History
 *
 * 2002-March-9    Jason Rohrer
 * Created.
 *
 * 2002-March-11    Jason Rohrer
 * Changed so that destructor joins thread.
 *
 * 2002-April-4    Jason Rohrer
 * Changed name of lock to avoid confusion with subclass-provided locks.
 *
 * 2002-August-5   Jason Rohrer
 * Fixed member initialization order to match declaration order.
 *
 * 2004-April-1    Jason Rohrer
 * Moved from konspire2b into minorGems.
 * Changed so that destructor does not join the thread.
 */



#include "FinishedSignalThread.h"


#include <stdio.h>



FinishedSignalThread::FinishedSignalThread()
    : mFinishedLock( new MutexLock() ), mFinished( false )  {

    }



FinishedSignalThread::~FinishedSignalThread() {
    
    delete mFinishedLock;
    }



char FinishedSignalThread::isFinished() {
    mFinishedLock->lock();
    char finished = mFinished;
    mFinishedLock->unlock();

    return finished;
    }



void FinishedSignalThread::setFinished() {
    mFinishedLock->lock();
    mFinished = true;
    mFinishedLock->unlock();
    }

