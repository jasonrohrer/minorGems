/*
 * Modification History
 *
 * 2003-October-9   Jason Rohrer
 * Created.
 *
 * 2003-October-12   Jason Rohrer
 * Switched to a floating point limit.
 *
 * 2004-January-2   Jason Rohrer
 * Added seprate mutex for transmission function to prevent UI freeze.
 *
 * 2005-April-15   Jason Rohrer
 * Changed to use updated Thread interface.
 */



#include "MessagePerSecondLimiter.h"


#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"



MessagePerSecondLimiter::MessagePerSecondLimiter( double inLimitPerSecond )
    : mLock( new MutexLock() ),
      mTransmitLock( new MutexLock() ),
      mLimitPerSecond( inLimitPerSecond ) {

    if( mLimitPerSecond != -1 ) {
        mMillisecondsBetweenMessages = (int)( 1000 / mLimitPerSecond );
        }
    else {
        mMillisecondsBetweenMessages = 0;
        }
    
    Time::getCurrentTime( &mSecondTimeOfLastMessage,
                          &mMillisecondTimeOfLastMessage );
    
    }


        
MessagePerSecondLimiter::~MessagePerSecondLimiter() {
    delete mLock;
    delete mTransmitLock;
    }


        
void MessagePerSecondLimiter::setLimit( double inLimitPerSecond ) {
    mLock->lock();
    mLimitPerSecond = inLimitPerSecond;

    if( mLimitPerSecond != -1 ) {
        mMillisecondsBetweenMessages = (int)( 1000 / mLimitPerSecond );
        }
    else {
        mMillisecondsBetweenMessages = 0;
        }


    mLock->unlock();
    }


        
double MessagePerSecondLimiter::getLimit() {
    mLock->lock();
    double limit = mLimitPerSecond;
    mLock->unlock();

    return limit;
    }

        

void MessagePerSecondLimiter::messageTransmitted() {
    // allow only one transmitter to report at a time
    mTransmitLock->lock();


    // protect our variables (make sure settings functions are not
    // called while we touch the variables)
    mLock->lock();


    unsigned long millisecondsSinceLastMessage =
        Time::getMillisecondsSince( mSecondTimeOfLastMessage,
                                    mMillisecondTimeOfLastMessage );

    
    if( millisecondsSinceLastMessage < mMillisecondsBetweenMessages ) {
        // this message is coming too soon after last message
        
        // sleep
        unsigned long sleepTime =
            mMillisecondsBetweenMessages -
            millisecondsSinceLastMessage;

        // unlock main lock befor sleeping so that settings can be changed
        mLock->unlock();
        
        Thread::staticSleep( sleepTime );

        // relock
        mLock->lock();
        }


    Time::getCurrentTime( &mSecondTimeOfLastMessage,
                          &mMillisecondTimeOfLastMessage );
    mLock->unlock();

    
    mTransmitLock->unlock();
    }










