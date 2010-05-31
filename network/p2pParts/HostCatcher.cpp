/*
 * Modification History
 *
 * 2003-June-22    Jason Rohrer
 * Copied from the konspire2b project and modified.
 *
 * 2003-July-27    Jason Rohrer
 * Fixed a bug when catcher is empty.
 *
 * 2003-December-21    Jason Rohrer
 * Fixed a memory leak when catcher is full.
 *
 * 2004-January-11   Jason Rohrer
 * Made include paths explicit to help certain compilers.
 *
 * 2004-December-20   Jason Rohrer
 * Changed to convert to numerical form before comparing against host list.
 * Changed getHost to return hosts in random order.
 * Added a getOrderedHost function that returns hosts in linear order.
 */



#include "minorGems/network/p2pParts/HostCatcher.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/random/StdRandomSource.h"



HostCatcher::HostCatcher( int inMaxListSize )
    : mMaxListSize( inMaxListSize ),
      mHostVector( new SimpleVector<HostAddress *>() ),
      mLock( new MutexLock() ),
      mRandSource( new StdRandomSource() ) {
    
    }


        
HostCatcher::~HostCatcher() {
    mLock->lock();
    
    int numHosts = mHostVector->size();

    for( int i=0; i<numHosts; i++ ) {
        delete *( mHostVector->getElement( i ) );
        }

    delete mHostVector;
    
    mLock->unlock();

    delete mLock;
    delete mRandSource;
    }

    

void HostCatcher::addHost( HostAddress * inHost ) {
    

    // convert to numerical form once and for all here
    // (to avoid converting over and over in equals checks below)
    HostAddress *numericalAddress = inHost->getNumericalAddress();

    if( numericalAddress != NULL ) {

        mLock->lock();
        
        // make sure this host doesn't already exist in our list
        char exists = false;
    
        int numHosts = mHostVector->size();
    
        for( int i=0; i<numHosts; i++ ) {
            HostAddress *otherHost =  *( mHostVector->getElement( i ) );

            if( otherHost->equals( numericalAddress ) ) {
                exists = true;
                // jump out of loop
                i = numHosts;
                }
            }
    
    
    
        if( !exists ) {
            mHostVector->push_back( numericalAddress->copy() );
            }
        
    
        while( mHostVector->size() > mMaxListSize ) {
            // remove first host from queue
            HostAddress *host = *( mHostVector->getElement( 0 ) );
            mHostVector->deleteElement( 0 );
            delete host;
            }
    
        mLock->unlock();

        delete numericalAddress;
        }
    }



HostAddress * HostCatcher::getHostOrdered(  ) {

    mLock->lock();
    
    int numHosts = mHostVector->size();

    if( numHosts == 0 ) {
        mLock->unlock();
        return NULL;
        }

    // remove first host from queue
    HostAddress *host = *( mHostVector->getElement( 0 ) );
    mHostVector->deleteElement( 0 );

    // add host to end of queue
    mHostVector->push_back( host );

    HostAddress *hostCopy = host->copy();

    
    mLock->unlock();
    
    return hostCopy;   
    }



HostAddress * HostCatcher::getHost(  ) {

    mLock->lock();
    
    int numHosts = mHostVector->size();

    if( numHosts == 0 ) {
        mLock->unlock();
        return NULL;
        }

    // remove random host from queue
    int index = mRandSource->getRandomBoundedInt( 0, numHosts - 1 ); 
    HostAddress *host = *( mHostVector->getElement( index ) );
    mHostVector->deleteElement( index );

    // add host to end of queue
    mHostVector->push_back( host );

    HostAddress *hostCopy = host->copy();

    
    mLock->unlock();
    
    return hostCopy;   
    }



SimpleVector<HostAddress *> *HostCatcher::getHostList(
    int inMaxHostCount,
    HostAddress *inSkipHost ) {

    HostAddress *hostToSkip;

    if( inSkipHost != NULL ) {
        hostToSkip = inSkipHost->copy();
        }
    else {
        // don't skip any host
        // create a dummy host that won't match any other valid hosts
        // make sure dummy is in numerical form to avoid DNS lookups
        hostToSkip = new HostAddress( stringDuplicate( "1.1.1.1" ), 1 );
        }
             
    
    SimpleVector<HostAddress *> *collectedHosts =
        new SimpleVector<HostAddress *>();

    char repeat = false;
    int numCollected = 0;

    // This function assumes that getHostOrdered() draws
    // hosts in order with no repetition except when we have
    // exhausted the host supply.

    // Note that this will not be true when other threads
    // have getHostOrdered() (or getHost) calls interleaved with ours, but this
    // should be a rare case.  It will simply result
    // in a smaller host list being returned.

    HostAddress *firstHost = getHostOrdered();

    if( firstHost == NULL ) {
        // the catcher is empty

        delete hostToSkip;

        // an empty host list
        return collectedHosts;       
        }
    

    if( ! hostToSkip->equals( firstHost ) ) {
        collectedHosts->push_back( firstHost );
        numCollected++;
        }

    
    while( numCollected < inMaxHostCount && !repeat ) {

        HostAddress *nextHost = getHostOrdered();

        if( nextHost->equals( firstHost ) ) {
            delete nextHost;
            repeat = true;
            }
        else {
            if( ! hostToSkip->equals( nextHost ) ) {
                collectedHosts->push_back( nextHost );
                numCollected++;
                }
            else {
                delete nextHost;
                }
            }
        
        }


    if( hostToSkip->equals( firstHost ) ) {
        // we didn't include firstHost in our collectedHosts, so
        // we must delete it.
        delete firstHost;
        }

    
    delete hostToSkip;

    return collectedHosts;
    }



void HostCatcher::addHostList( SimpleVector<HostAddress *> * inHostList ) {
    int numToAdd = inHostList->size();

    for( int i=0; i<numToAdd; i++ ) {
        addHost( *( inHostList->getElement( i ) ) );
        }
    }



void HostCatcher::noteHostBad( HostAddress * inHost ) {
    mLock->lock();
    
    // make sure this host already exists in our list
    char exists = false;

    HostAddress *foundHost = NULL;
    
    int numHosts = mHostVector->size();

    for( int i=0; i<numHosts; i++ ) {
        HostAddress *otherHost =  *( mHostVector->getElement( i ) );

        if( otherHost->equals( inHost ) ) {
            exists = true;

            // delete the host that we've found
            mHostVector->deleteElement( i );

            foundHost = otherHost;
            
            // jump out of loop
            i = numHosts;
            }
        }

    
    if( exists ) {
        delete foundHost;
        //mHostVector->push_back( foundHost );
        }

    mLock->unlock();
    }
    



