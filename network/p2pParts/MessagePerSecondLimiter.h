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
 */



#ifndef MESSAGE_PER_SECOND_LIMITER_INCLUDED_H
#define MESSAGE_PER_SECOND_LIMITER_INCLUDED_H



#include "minorGems/system/MutexLock.h"



/**
 * Class that limits the number of messages transmitted per second.
 *
 * @author Jason Rohrer
 */
class MessagePerSecondLimiter {


        
    public:


        
        /**
         * Constructs a limiter.
         *
         * @param inLimitPerSecond the maximum number of messages
         *   transmitted per second, or -1 for no limit.
         *   Defaults to -1.
         */        
        MessagePerSecondLimiter( double inLimitPerSecond = -1 );


        
        ~MessagePerSecondLimiter();

        
        
        /**
         * Sets the limit.
         *
         * Thread safe.
         *
         * @param inLimitPerSecond the maximum number of messages
         *   transmitted per second, or -1 for no limit.
         */        
        void setLimit( double inLimitPerSecond );


        
        /**
         * Gets the limit.
         *
         * Thread safe.
         *
         * @return the maximum number of messages
         *   transmitted per second, or -1 if no limit set.
         */        
        double getLimit();

        

        /**
         * Called by a message transmitter to indicate that a message
         * is about to be transmitted.  Will block if messages
         * are being transmitted too frequently.
         *
         * Thread safe.
         */
        void messageTransmitted();
        

        
    protected:
        MutexLock *mLock;
        MutexLock *mTransmitLock;
        
        double mLimitPerSecond;
        unsigned long  mMillisecondsBetweenMessages;

        unsigned long mSecondTimeOfLastMessage;
        unsigned long mMillisecondTimeOfLastMessage;


        
    };



#endif
