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
 * 2004-January-11   Jason Rohrer
 * Made include paths explicit to help certain compilers.
 *
 * 2004-December-12   Jason Rohrer
 * Added a queue size parameter.
 */



#include "minorGems/network/p2pParts/OutboundChannel.h"



OutboundChannel::OutboundChannel( OutputStream *inOutputStream,
                                  HostAddress *inHost,
                                  MessagePerSecondLimiter *inLimiter,
                                  unsigned long inQueueSize )
    : mLock( new MutexLock() ), mMessageReadySemaphore( new Semaphore() ),
      mStream( inOutputStream ),
      mHost( inHost ),
      mLimiter( inLimiter ),
      mConnectionBroken( false ), mThreadStopped( false ),
      mMessageQueue( new SimpleVector<char*>() ),
      mHighPriorityMessageQueue( new SimpleVector<char*>() ),
      mMaxQueueSize( inQueueSize ),
      mDroppedMessageCount( 0 ),
      mSentMessageCount( 0 ) {
    

    // start our thread
    start();
    }



OutboundChannel::~OutboundChannel() {
    mLock->lock();

    mThreadStopped = true;

    mLock->unlock();

    // wake the thread up if it is waiting
    mMessageReadySemaphore->signal();
    
    // wait for our thread to get the stop signal and finish
    join();


    mLock->lock();

    delete mMessageReadySemaphore;
    
    // clear the queues
    int numMessages = mMessageQueue->size();
    int i;
    for( i=0; i<numMessages; i++ ) {
        char *message = *( mMessageQueue->getElement( i ) );
        delete [] message;
        }
    delete mMessageQueue;

    numMessages = mHighPriorityMessageQueue->size();
    for( i=0; i<numMessages; i++ ) {
        char *message = *( mHighPriorityMessageQueue->getElement( i ) );
        delete [] message;
        }
    delete mHighPriorityMessageQueue;

    

    delete mHost;

    mLock->unlock();

    delete mLock;
    }
    


char OutboundChannel::sendMessage( char * inMessage, int inPriority ) {
    mLock->lock();


    char sent;
    
    if( !mConnectionBroken ) {
        // add it to the queue
        SimpleVector<char *> *queueToUse;

        if( inPriority <=0 ) {
            queueToUse = mMessageQueue;
            }
        else {
            queueToUse = mHighPriorityMessageQueue;
            }

        
        queueToUse->push_back( stringDuplicate( inMessage ) );
        sent = true;

        if( queueToUse->size() > mMaxQueueSize ) {
            // the queue is over-full
            // drop the oldest message

            char *message = *( queueToUse->getElement( 0 ) );
            queueToUse->deleteElement( 0 );
            delete [] message;

            mDroppedMessageCount++;
            }
        }
    else {
        // channel no longer working
        sent = false;
        }
        
    mLock->unlock();

    if( sent ) {
        mMessageReadySemaphore->signal();
        }
    
    return sent;
    }



HostAddress * OutboundChannel::getHost() {
    return mHost->copy();
    }



void OutboundChannel::setHost( HostAddress *inHost ) {
    delete mHost;
    mHost = inHost->copy();
    }



int OutboundChannel::getSentMessageCount() {
    mLock->lock();
    int count = mSentMessageCount;
    mLock->unlock();

    return count;
    }



int OutboundChannel::getQueuedMessageCount() {
    mLock->lock();
    int count = mMessageQueue->size() + mHighPriorityMessageQueue->size();
    mLock->unlock();

    return count;
    }



int OutboundChannel::getDroppedMessageCount() {
    mLock->lock();
    int count = mDroppedMessageCount;
    mLock->unlock();

    return count;
    }



void OutboundChannel::run() {
    mLock->lock();
    char stopped = mThreadStopped;
    mLock->unlock();

    while( !stopped ) {


        // get a message from the queue, checking high priority queue first
        char *message = NULL;

        mLock->lock();

        if( mHighPriorityMessageQueue->size() >= 1 ) {
            message = *( mHighPriorityMessageQueue->getElement( 0 ) );
            mHighPriorityMessageQueue->deleteElement( 0 );             
            }        
        else if( mMessageQueue->size() >= 1 ) {
            message = *( mMessageQueue->getElement( 0 ) );
            mMessageQueue->deleteElement( 0 );            
            }

        mLock->unlock();

        // note that we're unlocked during the send, so messages
        // can be freely added to the queue without blocking while we send
        // this message
        
        if( message != NULL ) {

            // obey the limit
            // we will block here if message rate is too high
            mLimiter->messageTransmitted();
                        
            int bytesSent = mStream->writeString( message );

            int messageLength = strlen( message );
    
            delete [] message;


            char sent;
            
            if( bytesSent == messageLength ) {
                sent = true;

                mLock->lock();
                mSentMessageCount++;
                mLock->unlock();
                }
            else {
                sent = false;
                }


            if( !sent ) {
                // connection is broken
                // stop this thread
                
                mLock->lock();

                mConnectionBroken = true;
                mThreadStopped = true;

                mLock->unlock();
                }
            }
        else {
            // no messages in the queue.
            // wait for more messages to be ready
            mMessageReadySemaphore->wait();
            }
        

        // check if we've been stopped
        mLock->lock();
        stopped = mThreadStopped;
        mLock->unlock();
        }
    
    }
