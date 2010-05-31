/*
 * Modification History
 *
 * 2004-January-1   Jason Rohrer
 * Created.
 */



#ifndef NETWORK_FUNCTION_LOCKS_INCLUDED
#define NETWORK_FUNCTION_LOCKS_INCLUDED



#include "minorGems/system/MutexLock.h"



/**
 * Collection of mutex locks for protecting various network functions.
 *
 * These locks are necessary because basic networking functions are not
 * thread-safe on many platforms.
 *
 * @author Jason Rohrer
 */
class NetworkFunctionLocks {


        
    public:


        
        /**
         * Lock to use around calls to gethostbyname().
         *
         * Since gethostbyname returns data in a statically allocated buffer,
         * this lock must remain locked until the data returned from
         * gethostbyname is copied.  Making a shallow copy of the returned
         * hostent is not good enough.
         */
        static MutexLock mGetHostByNameLock;



        /**
         * Lock to use around calls to inet_ntoa().
         */
        static MutexLock mInet_ntoaLock;


        
    };



#endif

