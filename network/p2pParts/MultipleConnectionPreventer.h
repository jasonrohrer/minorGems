/*
 * Modification History
 *
 * 2003-June-20   Jason Rohrer
 * Created.
 */



#ifndef MULTIPLE_CONNECTION_PREVENTER_INCLUDED_H
#define MULTIPLE_CONNECTION_PREVENTER_INCLUDED_H



#include "minorGems/system/MutexLock.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/network/HostAddress.h"



/**
 * Class that tracks current connections of a p2p node and prevents
 * multiple connections to the same address.
 *
 * @author Jason Rohrer
 */
class MultipleConnectionPreventer {


        
    public:
        
        MultipleConnectionPreventer();

        ~MultipleConnectionPreventer();

        

        /**
         * Adds a connection to a host, and checks if the connection
         * is permitted.
         *
         * The connection will not be permitted if another connection
         * to the address already exists.
         *
         * Thread safe.
         *
         * @param inAddress the address of the connection.
         *   Must be destroyed by caller.
         *
         * @return true if the connection is permitted, or false otherwise.
         */
        char addConnection( HostAddress *inAddress );



        /**
         * Reports that a connection has been broken.
         *
         * Thread safe.
         *
         * @param inAddress the address of the broken connection.
         *   Must be destroyed by caller.
         */
        void connectionBroken( HostAddress *inAddress );

        
        
    protected:

        MutexLock *mLock;
        SimpleVector<HostAddress *> *mConnections;
        


    };



#endif
