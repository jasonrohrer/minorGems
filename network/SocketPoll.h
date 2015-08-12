
#include "Socket.h"
#include "SocketServer.h"


typedef struct SocketOrServer {
        // if false, then is server
        char isSocket;
        
        // unused pointer is NULL
        Socket *sock;
        SocketServer *server;

        void *otherData;
        
    } SocketOrServer;




// watches a bunch of open sockets and socket servers for activity, and
// returns ones that need attention
class SocketPoll {
        

    public:
        
        SocketPoll();
        
        ~SocketPoll();


        
        
        // inOtherData is pointer to other stuff you want returned 
        // along with the socket or server when it needs attention


        // watch for data ready to be read
        //
        // returns true on success, false on failure
        char addSocket( Socket *inSock, 
                        void *inOtherData = NULL );
        
        // watch for incomming connections ready to be accepted
        //
        // returns true on success, false on failure
        char addSocketServer( SocketServer *inServer, 
                              void *inOtherData = NULL );


        void removeSocket( Socket *inSock );
        void removeSocketServer( SocketServer *inServer );

        
        // waits for next event, and returns socket or server that
        // needs attention, along with its original inOtherData
        //
        // returns NULL on timeout
        //
        // -1 for no timeout
        SocketOrServer *wait( int inTimeoutMS = -1 );
        
        
        // used by platform-specific implementations
        void *mNativeObjectPointer;
        
    protected:
        
        SimpleVector<SocketOrServer*> mWatchedList;
        

        // used internally by some implementations to track queue of
        // ready sockets/servers (unused by other implementations)
        // any elements in mReadyList are also in mWatchedList
        SimpleVector<SocketOrServer*> mReadyList;
        
        // used by some implementations to do round-robin selects
        int mNextSocketOrServer;
        
    };


