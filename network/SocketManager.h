/*
 * Modification History
 *
 * 2004-December-13   Jason Rohrer
 * Created.
 */



#ifndef SOCKET_MANAGER_INCLUDED
#define SOCKET_MANAGER_INCLUDED



#include "minorGems/network/Socket.h"
#include "minorGems/system/MutexLock.h"
#include "minorGems/util/SimpleVector.h"



/**
 * Ensures proper destruction of static data items at program termination.
 */
class SocketManagerDataWrapper {

    public:

        SocketManagerDataWrapper();
        
        ~SocketManagerDataWrapper();


        MutexLock *mLock;
        SimpleVector<Socket *> *mSocketVector;

    };



/**
 * A class that ensures thread-safe socket shutdown and destruction.
 *
 * Useful if a thread needs to break a socket that another thread is using.
 */
class SocketManager {

    public:


        
        /**
         * Adds a socket to this manager.
         *
         * @param inSocket the socket to add.
         *   Will be destroyed by this manager.
         */
        static void addSocket( Socket *inSocket );

        

        /**
         * Breaks the connection (both directions) associated with a socket.
         *
         * This call is safe even if inSocket has already been destroyed.
         *
         * @param inSocket the socket to break.
         */
        static void breakConnection( Socket *inSocket );


        
        /**
         * Destroys a socket and removes it from this manager.
         *
         * @param inSocket the socket to destroy.
         */
        static void destroySocket( Socket *inSocket );


        
    private:

        // allocated statically to ensure destruction on program termination
        static SocketManagerDataWrapper mDataWrapper;

    };



#endif
