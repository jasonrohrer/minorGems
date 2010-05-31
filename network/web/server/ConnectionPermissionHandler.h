/*
 * Modification History
 *
 * 2002-August-2   Jason Rohrer
 * Created.
 *
 * 2002-September-17   Jason Rohrer
 * Changed to use the SettingsManager.
 *
 * 2002-November-9   Jason Rohrer
 * Added support for matching address patterns.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */


 
#ifndef CONNECTION_PERMISSION_HANDLER_INCLUDED
#define CONNECTION_PERMISSION_HANDLER_INCLUDED 



#include "minorGems/network/HostAddress.h"

#include "minorGems/util/SimpleVector.h"


#include <string.h>
#include <stdio.h>



/**
 * A class that handles permissions for received connections.
 *
 * @author Jason Rohrer.
 */
class ConnectionPermissionHandler {



    public:



        /**
         * Constructs a handler.
         */
        ConnectionPermissionHandler();


        ~ConnectionPermissionHandler();
        


        /**
         * Gets whether a connection is permitted.
         *
         * @param inAddress the address of the host connecting.
         *   Must be destroyed by caller.
         *
         * @return true iff a connection is allowed.
         */
        char isPermitted( HostAddress *inAddress );


        
    private:

        
        SimpleVector<HostAddress *> *mPermittedAddresses;

        SimpleVector<char *> *mPermittedPatterns;

        
    };



#endif

