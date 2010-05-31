/*
 * Modification History
 *
 * 2003-June-22    Jason Rohrer
 * Copied from the konspire2b project and modified.
 *
 * 2004-December-20   Jason Rohrer
 * Changed getHost to return hosts in random order.
 * Added a getOrderedHost function that returns hosts in linear order.
 */



#ifndef HOST_CATCHER_INCLUDED
#define HOST_CATCHER_INCLUDED



#include "minorGems/network/HostAddress.h"


#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/random/RandomSource.h"
#include "minorGems/system/MutexLock.h"



/**
 * Manages a collection of hosts.
 *
 * @author Jason Rohrer
 */
class HostCatcher {



    public:


        
        /**
         * Constructs a host catcher.
         *
         * @param inMaxListSize the maximum number of hosts to hold.
         */
        HostCatcher( int inMaxListSize );


        
        ~HostCatcher();

        
        
        /**
         * Adds a host to this catcher.
         *
         * Thread safe.
         *
         * @param inHost the host to add.
         *   Must be destroyed by caller.
         */
        void addHost( HostAddress * inHost );



        /**
         * Gets a "fresh" host from this catcher.
         *
         * The returned host is "fresh" in that it has not
         * been returned by this call in a while.
         *
         * Thread safe.
         *
         * @return a host, or NULL if there are no hosts.
         *   Must be destroyed by caller if non-NULL.
         */
        HostAddress * getHost(  );



        /**
         * Gets a "fresh" host list from this catcher.
         * The result of this function call is similar to that obtained
         * by calling getHost inMaxHostCount times and constructing
         * a HostList from the result, except that this function
         * will produce a list with no repeated hosts.
         *
         * Thread safe.
         *
         * @para inMaxHostCount the maximum number of hosts to retrieve.
         * @param inSkipHost a host to skip when building the list,
         *   or NULL to not skip any host.  (Useful
         *   fo constructing a host list at the request of another host.)
         *   Defaults to NULL.  Must be destroyed by caller.
         *
         * @return a host list containing <= inMaxHostCount hosts.
         *   Vector and addresses must be destroyed by caller.
         */
        SimpleVector<HostAddress *> *getHostList(
            int inMaxHostCount,
            HostAddress *inSkipHost = NULL );


        
        /**
         * Adds an entire list of hosts to this catcher.
         *
         * Thread safe.
         *
         * @param inHostList the list to add.
         *   Vector and addresses must be destroyed by caller.
         */
        void addHostList( SimpleVector<HostAddress *> *inHostList );



        /**
         * Tells this catcher that a host is "bad"
         * (in other words, dead, dropping connections, etc.).
         *
         * Thread safe.
         *
         * @param inHost the bad host.
         *   Must be destroyed by caller.
         */
        void noteHostBad( HostAddress * inHost );



    protected:

        int mMaxListSize;
        
        SimpleVector<HostAddress *> *mHostVector;

        MutexLock *mLock;

        RandomSource *mRandSource;
        
        /**
         * Gets a "fresh" host from this catcher, walking through the host
         * list in order.
         *
         * The returned host is "fresh" in that it has not
         * been returned by this call in a while.
         *
         * Thread safe.
         *
         * @return a host, or NULL if there are no hosts.
         *   Must be destroyed by caller if non-NULL.
         */
        HostAddress *getHostOrdered();

        
        
    };



#endif
