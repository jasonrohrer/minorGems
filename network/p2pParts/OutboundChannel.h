/*
 * Modification History
 *
 * 2003-June-22    Jason Rohrer
 * Copied from the konspire2b project and modified.
 *
 * 2003-July-27    Jason Rohrer
 * Added a setHost function.
 *
 * 2003-September-23    Jason Rohrer
 * Replaced sleep-waiting with semaphores.  This results in a major speedup.
 *
 * 2003-October-9   Jason Rohrer
 * Added support for message limiters.
 *
 * 2004-December-12   Jason Rohrer
 * Added a queue size parameter.
 */



#ifndef OUTBOUND_CHANNEL_INCLUDED
#define OUTBOUND_CHANNEL_INCLUDED



#include "minorGems/network/HostAddress.h"


#include "minorGems/io/OutputStream.h"

#include "minorGems/system/MutexLock.h"
#include "minorGems/system/Semaphore.h"

#include "minorGems/system/Thread.h"

#include "minorGems/util/SimpleVector.h"

#include "minorGems/network/p2pParts/MessagePerSecondLimiter.h"



/**
 * A channel that can send messages to a receiving host.
 *
 * NOTE:
 * None of the member functions are safe to call if this class has been
 * destroyed.  Since the application-specific channel manager class can
 * destroy a given instance of this class at any time, these member functions
 * should NEVER be called directly.
 * Instead, the appropriate channel manager functions should be called.
 *
 * @author Jason Rohrer
 */
class OutboundChannel : public Thread {



    public:

        /**
         * Constructs an OutboundChannel and starts the sending thread.
         *
         * @param inOutputStream the stream wrapped by this channel.
         *   Must be destroyed by caller.
         * @param inHost the host on the other end of this channel.
         *   Will be destroyed when this class is destroyed.
         * @param inLimiter the limiter for outbound messages.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inQueueSize the size of the send queue.  Defaults to 50.
         */
        OutboundChannel( OutputStream *inOutputStream, HostAddress *inHost,
                         MessagePerSecondLimiter *inLimiter,
                         unsigned long inQueueSize = 50 );



        /**
         * Stops the sending thread and destroys this channel.
         */
        ~OutboundChannel();

        
        
        /**
         * Sends a message to this channel's receiver.
         *
         * Thread safe.
         *
         * This call queues the message to be sent, so it returns before
         * the send is complete.
         *
         * @param inMessage the message to send.
         *   Must be destroyed by caller if non-const.
         * @param inPriority the priority of this message.
         *   Values less than or equal to 0 indicate default priority,
         *   while positive values suggest higher priority.
         *   Granularity of prioritization is implementation dependent.
         *   Defaults to 0.
         *
         * @return true if the channel is still functioning properly,
         *   or false if the channel has been broken.
         */
        char sendMessage( char *inMessage, int inPriority = 0 );



        /**
         * Gets the host receiving from this channel.
         *
         * @return this channel's receiving host.
         *   Must be destroyed by caller.
         */
        HostAddress * getHost(  );



        /**
         * Sets the address of the remotely connected host.
         *
         * @param inHost the host on the other end of this channel.
         *   Will be destroyed when this class is destroyed.
         */
        void setHost( HostAddress *inHost );

        

        /**
         * Gets the number of outbound messages that have been sent on
         * this channel.
         *
         * Thread safe.
         *
         * @return the number of sent messages.
         */
        int getSentMessageCount();



        /**
         * Gets the number of outbound messages that are currently
         * queued for this channel.
         *
         * Thread safe.
         *
         * @return the number of queued messages.
         */
        int getQueuedMessageCount();



        /**
         * Gets the number of outbound messages that have been dropped
         * by this channel.
         *
         * Thread safe.
         *
         * @return the number of dropped messages.
         */
        int getDroppedMessageCount();

        

        // implements the Thread interface
        void run();

        
        
    protected:

        MutexLock *mLock;
        Semaphore *mMessageReadySemaphore;
        
        OutputStream *mStream;
        
        HostAddress *mHost;

        MessagePerSecondLimiter *mLimiter;
        
        
        char mConnectionBroken;

        char mThreadStopped;


        SimpleVector<char*> *mMessageQueue;
        SimpleVector<char*> *mHighPriorityMessageQueue;


        int mMaxQueueSize;
        
        int mDroppedMessageCount;

        int mSentMessageCount;
        
    };



#endif
