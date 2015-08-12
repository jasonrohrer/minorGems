
#include "minorGems/network/SocketPoll.h"

#include <sys/epoll.h>
#include <unistd.h>


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
            struct epoll_event ev;
            
            epoll_ctl( epollHandle, EPOLL_CTL_DEL, socketID, &ev );

            delete [] s;
            mWatchedList.deleteElement( i );
            return;
            }
        }
    }


void SocketPoll::removeSocketServer( SocketServer *inServer ) {

    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        if( s->server == inServer ) {
            struct epoll_event ev;
            
            epoll_ctl( epollHandle, EPOLL_CTL_DEL, socketID, &ev );

            delete [] s;
            mWatchedList.deleteElement( i );
            return;
            }
        }
    }




SocketOrServer *SocketPoll::wait( int inTimeoutMS ) {

    if( mReadyList.size() > 0 ) {
        SocketOrServer *result = mReadyList.getElementDirect( 0 );
        mReadyList.deleteElement( 0 );
        
        return result;
        }
    


    int nextSocketOrServer = 0;
    

    // select on batches of at most FD_SETSIZE, and add any that
    // are ready to our ready list
    while( nextSocketOrServer < mWatchedList.size() ) {

        SimpleVector< SocketOrServer *s> checkList;
        SimpleVector<int> checkIDList;

        fd_set fdr;

        FD_ZERO( &fsr );

        int maxSocketID = 0;

        for( int i=0; i<FD_SETSIZE && nextSocketOrServer < mWatchedList.size();
             i++ ) {
            
            SocketOrServer *s = 
                mWatchedList.getElementDirect( nextSocketOrServer );
            
            
            checkList.push_back( s );
            
            int *socketIDptr = (int *)( inSock->mNativeObjectPointer );
	
            if( s->isSocket ) {
                *socketIDptr = (int *)( s->sock->mNativeObjectPointer );
                }
            else {
                *socketIDptr = (int *)( s->server->mNativeObjectPointer );
                }
            
            int socketID = socketIDptr[0];

            checkIDList.push_back( socketID );

            FD_SET( socketID, &fsr );
            
            if( socketID > maxSocketID ) {
                maxSocketID = socketID;
                }

            nextSocketOrServer++;
            }
        
        
        // poll, no block
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    

        int ret = select( maxSocketID + 1, &fsr, NULL, NULL, NULL, &tv );

        if( ret > 0 ) {
            
            // some are ready

            int numChecked = checkIDList.size();
            
            for( int i=0; i<numChecked; i++ ) {
                
                if( FD_ISSET( checkIDList.getElementDirect( i ), &fsr ) ) {
                    
                    mReadyList.push_back( checkList.getElementDirect( i ) );
                    }
                }
            }
        }
    


    if( mReadyList.size() > 0 ) {
        SocketOrServer *result = mReadyList.getElementDirect( 0 );
        mReadyList.deleteElement( 0 );
        
        return result;
        }
    else {
        return NULL;
        }
    }

