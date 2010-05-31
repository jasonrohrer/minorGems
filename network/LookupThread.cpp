/*
 * Modification History
 *
 * 2008-October-21   Jason Rohrer
 * Created.
 *
 * 2009-February-3   Jason Rohrer
 * Changed to subclass FinishedSignalThread.
 *
 * 2009-February-14   Jason Rohrer
 * Changed to copy inAddress internally.
 */


#include "LookupThread.h"


LookupThread::LookupThread( HostAddress *inAddress )
        : mAddress( inAddress->copy() ), mNumericalAddress( NULL ),
          mLookupDone( false ) {

    start();
    }


LookupThread::~LookupThread() {
    join();
    
    delete mAddress;
    
    if( mNumericalAddress != NULL ) {
        delete mNumericalAddress;
        }
    }


char LookupThread::isLookupDone() {
    mLock.lock();
    char done = mLookupDone;
    mLock.unlock();
    
    return done;
    }



HostAddress *LookupThread::getResult() {
    mLock.lock();
    HostAddress *result = NULL;
    
    if( mNumericalAddress != NULL ) {
        result = mNumericalAddress->copy();
        }
    
    mLock.unlock();
    

    return result;
    }



void LookupThread::run() {
    
    HostAddress *numAddress = mAddress->getNumericalAddress();
    

    mLock.lock();        
    mLookupDone = true;
    mNumericalAddress = numAddress;
    mLock.unlock();

    setFinished();
    }
