
#include "minorGems/network/SocketPoll.h"

#include <sys/epoll.h>
#include <unistd.h>



SocketPoll::SocketPoll() {
    int *epollStorage = new int[1];

    // a non-zero starting size value that is ignored by newer kernels
	epollStorage[0] = epoll_create( 10 );

    
	mNativeObjectPointer = (void *)epollStorage;
    
    }



SocketPoll::~SocketPoll() {
    int *epollStorage = (int *)( mNativeObjectPointer );
	int epollHandle = epollStorage[0];

    if( epollHandle != -1 ) {
        close( epollHandle );
        }

    delete [] epollStorage;

    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        delete s;
        }
    }




char SocketPoll::addSocket( Socket *inSock, void *inOtherData ) {

    int *epollStorage = (int *)( mNativeObjectPointer );
	int epollHandle = epollStorage[0];

    if( epollHandle == -1 ) {
        return false;
        }
    

    int *socketIDptr = (int *)( inSock->mNativeObjectPointer );
	int socketID = socketIDptr[0];



    SocketOrServer *s = new SocketOrServer;
    
    s->isSocket = true;
    s->sock = inSock;
    s->server = NULL;
    s->otherData = inOtherData;
    
    mWatchedList.push_back( s );
    

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    // clear entire union to suppress valgrind uninit errors on platforms
    // with 32-bit pointers
    ev.data.u64 = 0;
    
    // store our pointer
    ev.data.ptr = s;

    int result = epoll_ctl( epollHandle, EPOLL_CTL_ADD, socketID, &ev );

    if( result == 0 ) {
        return true;
        }
    return false;
    }

        



char SocketPoll::addSocketServer( SocketServer *inServer, void *inOtherData ) {
    int *epollStorage = (int *)( mNativeObjectPointer );
    int epollHandle = epollStorage[0];

    if( epollHandle == -1 ) {
        return false;
        }
    

    int *socketIDptr = (int *)( inServer->mNativeObjectPointer );
	int socketID = socketIDptr[0];



    SocketOrServer *s = new SocketOrServer;
    
    s->isSocket = false;
    s->sock = NULL;
    s->server = inServer;
    s->otherData = inOtherData;
    
    mWatchedList.push_back( s );
    

    struct epoll_event ev;
    ev.events = EPOLLIN;
    // clear entire union to suppress valgrind uninit errors on platforms
    // with 32-bit pointers
    ev.data.u64 = 0;
    
    // store our pointer
    ev.data.ptr = s;

    int result = epoll_ctl( epollHandle, EPOLL_CTL_ADD, socketID, &ev );

    if( result == 0 ) {
        return true;
        }
    return false;    
    }



void SocketPoll::removeSocket( Socket *inSock ) {
    int *epollStorage = (int *)( mNativeObjectPointer );
    int epollHandle = epollStorage[0];

    if( epollHandle == -1 ) {
        return;
        }

    int *socketIDptr = (int *)( inSock->mNativeObjectPointer );
	int socketID = socketIDptr[0];



    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        if( s->sock == inSock ) {
            struct epoll_event ev;
            
            epoll_ctl( epollHandle, EPOLL_CTL_DEL, socketID, &ev );

            delete s;
            mWatchedList.deleteElement( i );
            return;
            }
        }
    }

void SocketPoll::removeSocketServer( SocketServer *inServer ) {
    int *epollStorage = (int *)( mNativeObjectPointer );
    int epollHandle = epollStorage[0];

    if( epollHandle == -1 ) {
        return;
        }

    int *socketIDptr = (int *)( inServer->mNativeObjectPointer );
	int socketID = socketIDptr[0];



    for( int i=0; i<mWatchedList.size(); i++ ) {
        SocketOrServer *s = *( mWatchedList.getElement( i ) );
        if( s->server == inServer ) {
            struct epoll_event ev;
            
            epoll_ctl( epollHandle, EPOLL_CTL_DEL, socketID, &ev );

            delete s;
            mWatchedList.deleteElement( i );
            return;
            }
        }
    }




SocketOrServer *SocketPoll::wait( int inTimeoutMS ) {
    int *epollStorage = (int *)( mNativeObjectPointer );
    int epollHandle = epollStorage[0];


    struct epoll_event returnedEvents[1];

    int numEvents = epoll_wait( epollHandle, returnedEvents, 1, inTimeoutMS );

    if( numEvents <= 0 ) {
        // timeout or error
        return NULL;
        }
    
    
    // else we have an event!
    
    return (SocketOrServer *)( returnedEvents[0].data.ptr );
    }

