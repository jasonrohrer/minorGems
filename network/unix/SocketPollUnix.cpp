
#include "minorGems/network/SocketPoll.h"
#include "minorGems/system/Time.h"


// implementation of SocketPoll for unix-like platforms
// that don't have epoll (non-linux)

// emulate epoll behavior with select for portability
// (at the cost of performance)

// overcome the limits of FD_SETSIZE with multiple, sequential select calls


// Note that this implementation is pretty much a hack.
// The assumption is that end users who are running servers for their
// friends will not need to multiplex thousands of client connections,
// so O(N) operations over N connections are not a big deal.
// Anyone who is running a serious, high performance server will be 
// doing it on Linux and will be able to take advantage of the better
// implementation in SocketPollLinux

// This implementation breaks select calls up into batches, selects one
// batch at a time, and returns as soon as an event in any batch is ready
// without processing further batches.
// To prevent starvation, later wait calls start on the next batch in
// line, round-robin, until each batch has been select-ed, before returning
// to select again on the first batch.

// If no events are ready on a given batch, the next batch is examined in
// the same call.  The timeout is only applied to the final batch in the
// round-robin sequence, assuming that no events happened on any of the
// previous n-1 batches.


// (I investigated IOCP and WSAEventSelect, but they seemed too complicated
// to figure out, given the target application of end users hosting small-time
// servers.)



#ifdef __mac__
    
#include <sys/select.h>

#elif defined(WIN_32)

#include <Winsock.h>

#endif


SocketPoll::SocketPoll() {
	mNativeObjectPointer = NULL;

    mNextSocketOrServer = 0;
    }



SocketPoll::~SocketPoll() {

    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        delete s;
        }
    }




char SocketPoll::addSocket( Socket *inSock, void *inOtherData ) {

    SocketOrServer *s = new SocketOrServer;
    
    s->isSocket = true;
    s->sock = inSock;
    s->server = NULL;
    s->otherData = inOtherData;
    
    mWatchedList.push_back( s );
    
    return true;
    }
        



char SocketPoll::addSocketServer( SocketServer *inServer, void *inOtherData ) {
    
    SocketOrServer *s = new SocketOrServer;
    
    s->isSocket = false;
    s->sock = NULL;
    s->server = inServer;
    s->otherData = inOtherData;
    
    mWatchedList.push_back( s );
    
    return true;
    }




void SocketPoll::removeSocket( Socket *inSock ) {

    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        if( s->sock == inSock ) {

            delete s;
            mWatchedList.deleteElement( i );
            return;
            }
        }
    }


void SocketPoll::removeSocketServer( SocketServer *inServer ) {

    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        if( s->server == inServer ) {
            
            delete s;
            mWatchedList.deleteElement( i );
            return;
            }
        }
    }




SocketOrServer *SocketPoll::wait( int inTimeoutMS ) {
    double startTime = Time::getCurrentTime();
    
    if( mReadyList.size() > 0 ) {
        SocketOrServer *result = mReadyList.getElementDirect( 0 );
        mReadyList.deleteElement( 0 );
        
        return result;
        }

    
    if( mNextSocketOrServer >= mWatchedList.size() ) {
        // start back over at beginning of queue

        mNextSocketOrServer = 0;
        }

    int endPoint = mNextSocketOrServer - 1;

    if( endPoint < 0 ) {
        endPoint = mWatchedList.size();
        }
    

    // select on batches of at most FD_SETSIZE, and add any that
    // are ready to our ready list
    while( mNextSocketOrServer != endPoint ) {

        SimpleVector<SocketOrServer *> checkList;
        SimpleVector<int> checkIDList;

        fd_set fdr;

        FD_ZERO( &fdr );

        int maxSocketID = 0;

        for( int i=0; i<FD_SETSIZE && mNextSocketOrServer != endPoint; i++ ) {
            
            SocketOrServer *s = 
                mWatchedList.getElementDirect( mNextSocketOrServer );
            
            
            checkList.push_back( s );
            
            int *socketIDptr;
	
            if( s->isSocket ) {
                socketIDptr = (int *)( s->sock->mNativeObjectPointer );
                }
            else {
                socketIDptr = (int *)( s->server->mNativeObjectPointer );
                }
            
            int socketID = socketIDptr[0];

            checkIDList.push_back( socketID );

            FD_SET( socketID, &fdr );
            
            if( socketID > maxSocketID ) {
                maxSocketID = socketID;
                }

            mNextSocketOrServer++;

            if( mNextSocketOrServer != endPoint ) {    
                // wrap around
                if( mNextSocketOrServer >= mWatchedList.size() ) {
                    mNextSocketOrServer = 0;
                    }
                }
            
            }
        
        
        struct timeval tv;
        
        struct timeval *tvPointer = NULL;
        
        if( mNextSocketOrServer == endPoint ) {

            if( inTimeoutMS != -1 ) {
                
                // at end of list, and this is our final batch to check
                // previous ones returned instantly with zero timeouts
                // or happened during previous calls
                
                // so, it's safe to block for our timeout on this last batch
                
                // BUT, account for time that has already elapsed
                
                double passedTime = Time::getCurrentTime() - startTime;
                
                int passedMS = (int)( passedTime * 1000 );
                
                inTimeoutMS -= passedMS;
                
                if( inTimeoutMS < 0 ) {
                    inTimeoutMS = 0;
                    }
                
                tv.tv_sec = inTimeoutMS / 1000;
                int remainder = inTimeoutMS % 1000;
                tv.tv_usec = remainder * 1000;
                
                tvPointer = &tv;
                }
            // else, -1 timeout supplied, which means we should
            // block forever on this last batch
            }
        else {
            // not our last batch, ignore caller's timeout for now

            // poll, no block
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            
            tvPointer = &tv;
            }
        

        int ret = select( maxSocketID + 1, &fdr, NULL, NULL, tvPointer );

        if( ret > 0 ) {
            
            // some are ready

            int numChecked = checkIDList.size();
            
            for( int i=0; i<numChecked; i++ ) {
                
                if( FD_ISSET( checkIDList.getElementDirect( i ), &fdr ) ) {
                    
                    mReadyList.push_back( checkList.getElementDirect( i ) );
                    }
                }

            if( mReadyList.size() > 0 ) {
                
                // return first one right away

                // don't bother selecting on later batches now

                // we will handle them on the next call, fairly, because
                // of mNextSocketOrServer round robin

                SocketOrServer *result = mReadyList.getElementDirect( 0 );
                mReadyList.deleteElement( 0 );
                
                return result;
                }

            }

        // else no events ready, go on to next select batch
        }
    
    
    // none ready in any batch, and reached endpoint in round robin
    return NULL;
    }

