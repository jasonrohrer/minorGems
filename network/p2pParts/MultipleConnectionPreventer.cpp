/*
 * Modification History
 *
 * 2003-June-20   Jason Rohrer
 * Created.
 */



#include "MultipleConnectionPreventer.h"



MultipleConnectionPreventer::MultipleConnectionPreventer()
    : mLock( new MutexLock() ),
      mConnections( new SimpleVector<HostAddress *>() ) {

    }



MultipleConnectionPreventer::~MultipleConnectionPreventer() {
    mLock->lock();

    int numConnections = mConnections->size();

    for( int i=0; i<numConnections; i++ ) {
        delete *( mConnections->getElement( i ) );
        }
    delete mConnections;

    mLock->unlock();

    delete mLock;
    }



char MultipleConnectionPreventer::addConnection( HostAddress *inAddress ) {
    mLock->lock();

    char connectionExists = false;
    
    int numConnections = mConnections->size();

    for( int i=0; i<numConnections && !connectionExists; i++ ) {
        HostAddress *otherConnection = *( mConnections->getElement( i ) ); 

        if( inAddress->equals( otherConnection ) ) {
            connectionExists = true;
            }
        }

    if( !connectionExists ) {
        mConnections->push_back( inAddress->copy() );
        }
  
    
    mLock->unlock();

    return !connectionExists;
    }



void MultipleConnectionPreventer::connectionBroken( HostAddress *inAddress ) {
    mLock->lock();

    char connectionFound = false;
    
    int numConnections = mConnections->size();

    for( int i=0; i<numConnections && !connectionFound; i++ ) {
        HostAddress *otherConnection = *( mConnections->getElement( i ) ); 

        if( inAddress->equals( otherConnection ) ) {
            connectionFound = true;

            delete otherConnection;

            mConnections->deleteElement( i );
            }
        }
    
    mLock->unlock();
    }


