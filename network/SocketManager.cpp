/*
 * Modification History
 *
 * 2004-December-13   Jason Rohrer
 * Created.
 */



#include "SocketManager.h"



// static initialization
SocketManagerDataWrapper SocketManager::mDataWrapper;



SocketManagerDataWrapper::SocketManagerDataWrapper()
    : mLock( new MutexLock() ),
      mSocketVector( new SimpleVector<Socket *>() ) {

    }



SocketManagerDataWrapper::~SocketManagerDataWrapper() {
    int numSockets = mSocketVector->size();

    for( int i=0; i<numSockets; i++ ) {
        delete *( mSocketVector->getElement( i ) );
        }
    delete mSocketVector;

    delete mLock;
    }



void SocketManager::addSocket( Socket *inSocket ) {

    MutexLock *lock = mDataWrapper.mLock;
    SimpleVector<Socket *> *socketVector = mDataWrapper.mSocketVector;

    
    lock->lock();

    socketVector->push_back( inSocket );
    
    lock->unlock();
    }



void SocketManager::breakConnection( Socket *inSocket ) {

    MutexLock *lock = mDataWrapper.mLock;
    SimpleVector<Socket *> *socketVector = mDataWrapper.mSocketVector;

    lock->lock();

    int numSockets = socketVector->size();
    char found = false;
    
    for( int i=0; i<numSockets && !found; i++ ) { 

        Socket *currentSocket = *( socketVector->getElement( i ) );

        if( currentSocket == inSocket ) {
            currentSocket->breakConnection();
            found = true;
            }
        }
    
    lock->unlock();
    }



void SocketManager::destroySocket( Socket *inSocket ) {

    MutexLock *lock = mDataWrapper.mLock;
    SimpleVector<Socket *> *socketVector = mDataWrapper.mSocketVector;

    lock->lock();

    int numSockets = socketVector->size();
    char found = false;
    
    for( int i=0; i<numSockets && !found; i++ ) { 

        Socket *currentSocket = *( socketVector->getElement( i ) );

        if( currentSocket == inSocket ) {
            delete currentSocket;
            socketVector->deleteElement( i );
            
            found = true;
            }
        }
    
    lock->unlock();
    }
