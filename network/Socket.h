/*
 * Modification History
 *
 * 2001-January-9		Jason Rohrer
 * Created.
 *
 * 2001-January-28		Jason Rohrer
 * Added a static framework init function.
 *
 * 2001-November-13		Jason Rohrer
 * Changed timeout parameter to signed, since -1 is a possible argument.
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-August-2    Jason Rohrer
 * Added functon for getting remote host address.
 *
 * 2003-February-3   Jason Rohrer
 * Added a function for getting the local host address from a socket.
 *
 * 2003-August-12   Jason Rohrer
 * Added more verbose comment about receive timeout parameter.
 * Added a function for flushing socket sends.
 *
 * 2003-November-20   Jason Rohrer
 * Made flush function robust against bogus receive return values.
 *
 * 2004-December13   Jason Rohrer
 * Added a breakConnection function.
 *
 * 2005-July-5   Jason Rohrer
 * Added port number when getting address of remote host.
 *
 * 2006-May-28   Jason Rohrer
 * Changed timeout behavior slightly to support emulation of non-blocking mode.
 * Added support for non-blocking sends.
 *
 * 2008-September-30   Jason Rohrer
 * Added support for non-blocking connect.
 *
 * 2010-January-26  Jason Rohrer
 * Added support for disabling Nagle algorithm.
 *
 * 2011-March-9    Jason Rohrer
 * Removed Fortify inclusion.
 */



#include "minorGems/common.h"


#ifndef SOCKET_CLASS_INCLUDED
#define SOCKET_CLASS_INCLUDED



#include "minorGems/network/HostAddress.h"
#include "minorGems/system/Time.h"






/**
 * Network socket.  Does not contain an interface for listening to connections
 * (see SocketServer for these interfaces) or for establishing connections
 * (see SocketClient for these interfaces).
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the mac/ linux/ and win32/ 
 *   subdirectories).
 *
 * @author Jason Rohrer
 */ 
class Socket {

	public:

        

        /**
         * Constructor for a socket.
         *
         * Should not be called directly.  Use SocketClient or SocketServer
         * to obtain an outbound or inbound socket connection.
         */
        Socket();

        
		// destroying a Socket closes the connection
		~Socket();
		
		
		/**
		 * Initializes the socket framework.  Must be called
		 * once by program before sockets are used.  Note
		 * that SocketClient and SocketServer both should call
		 * init automatically when they are first used.
		 *
		 * @return 0 on success, or -1 on failure.
		 */ 
		static int initSocketFramework();
		
		
		/**
		 * Gets whether the socket framework has been initialized.
		 *
		 * @return true if the framework has been initialized.
		 */
		static char isFrameworkInitialized();

		

        /**
         * For outbound sockets that were opened in non-blocking mode:
         *
         * Get whether socket is connected yet.
         *
         * @return 1 on success, 0 on still waiting, -1 on error.
         */
        int isConnected();
        

		
		/**
		 * Sends bytes through this socket.
		 *
		 * @param inBuffer the buffer of bytes to send.
		 * @param inNumBytes the number of bytes to send.
		 * @param inAllowedToBlock set to false to prevent blocking.
         *   Defaults to true.
         * @param inAllowDelay set to false to disable Nagle algorithm,
         *   where a buffer should be sent NOW without waiting for
         *   more data accumulation.
         *   Defaults to true.
         *
		 * @return the number of bytes sent successfully,
		 *   or -1 for a socket error.
         *   Returns -2 if not allowed to block and the operation
         *   would block.
		 */
		int send( unsigned char *inBuffer, int inNumBytes,
                  char inAllowedToBlock = true,
                  char inAllowDelay = true );
		
		
		/**
		 * Receives bytes from this socket.
		 *
		 * @param inBuffer the buffer where received bytes will be put.
		 *   Must be pre-allocated memory space.
		 * @param inNumBytes the number of bytes to read from the socket.
		 * @param inTimeout the timeout for this receive operation in
         *   milliseconds.  Set to -1 for an infinite timeout.
		 *   -2 is returned from this call in the event of a timeout.
         *   If timeout is set, only available data is returned (up
         *   to, but not more than inNumBytes).  The socket does not
         *   wait for all inNumBytes to become available before returning.
         *   Thus, a timeout return value (-2) means no data was available
         *   before the timeout.  If some data is available, we never
         *   time out.  If no timeout is set (-1 for infinite), the socket
         *   waits for all inNumBytes to arrive before returning.
         *   Thus, setting a timeout of 0 effectively puts the socket
         *   into non-blocking mode.
		 *
		 * @return the number of bytes read successfully,
		 *   or -1, -2 for a socket error or timeout, respectively.
		 */
		int receive( unsigned char *inBuffer, int inNumBytes,
                     long inTimeout );


        /**
         * Flushes sent data through the socket.
         * 
         * Intended to be called before deleting the socket to ensure
         * that data goes through.  The mechanism used by this function
         * does not guarantee that data goes through, but it works
         * better than simply closing the socket on many platforms.
         *
         * The method used by this call works best if the remote
         * host closes the connection.  For example, if we send the
         * remote host a "connection closing" indicator, then
         * call writeFlushBeforeClose, and the remote host closes
         * the connection upon receiving the indicator, all of our
         * sent data will be received by the remote host after
         * writeFlushBeforeClose returns (assuming that the maximum
         * time is long enough for the remote host to actually close
         * the connection).
         *
         * Good maximum times should be in the several-second range,
         * though the Apache system uses 30 seconds.  For slow connections,
         * this might be necessary.
         *
         * This call will cause any received data to be discarded.
         *
         * This call DOES NOT close the socket.  The socket must be deleted
         * as usual after this call returns.
         *
         * @param inMaxTimeInMilliseconds the maximum time to wait
         *   in milliseconds.  This call may block slightly longer than
         *   this if the remote host is still sending data.
         */
        void sendFlushBeforeClose( int inMaxTimeInMilliseconds );


        
        /**
         * Forces this socket's connection to break, causing any blocked
         * send or receive operations to fail.
         *
         * Note that this operation is inherently not thread-safe, so
         * some external mechanism must be used to ensure that the socket
         * is not destroyed by another thread before breakConnection is called.
         * The SocketManager class is one such external mechanism.
         */         
        void breakConnection();
        
        

        /**
         * Gets the host connected to the other end of this socket.
         *
         * @return the address of the remote host, or NULL if obtaining
         *   the address fails.  The port of the returned address
         *   will always be set to the port that the host is connecting from.
         *   Must be destroyed by caller if non-NULL.
         */
        HostAddress *getRemoteHostAddress();


        
        /**
         * Gets the local address attached to this socket.
         *
         * Getting the local address from a socket is more
         * accurate than non-connected methods (for example, the methods
         * used in HostAddress.h implementations.
         *
         * @return the address of the local host, or NULL if obtaining
         *   the address fails.  The port of the returned address
         *   will always be set to 0.
         *   Must be destroyed by caller if non-NULL.
         */
        HostAddress *getLocalHostAddress();


        
		/**
		 * Used by platform-specific implementations.
		 */		
		void *mNativeObjectPointer;

		
        // called by socket client to set connected status
        void setConnected( char inConnected ) {
            mConnected = inConnected;
            }
        

	private:
		
		static char sInitialized;

        char mConnected;
        
        char mIsConnectionBroken;
        

        // toggle Nagle algorithm (inValue=1 turns it off)
        void setNoDelay( int inValue );
        
        
	};			



inline Socket::Socket()
    : mConnected( true ), mIsConnectionBroken( false ) {

    }



inline char Socket::isFrameworkInitialized() {

	return sInitialized;
	}



inline void Socket::sendFlushBeforeClose( int inMaxTimeInMilliseconds ) {
    unsigned char *tempBuffer = new unsigned char[1];

    int numRead = -2;

    int timeout = 1000;
    if( timeout > inMaxTimeInMilliseconds ) {
        timeout = inMaxTimeInMilliseconds;
        }        
    
    long totalTimeout = 0;

    timeSec_t startSec;
    unsigned long startMsec;

    Time::getCurrentTime( &startSec, &startMsec );
    
    // keep reading data from socket until we get an error
    // or wait too long (pass our max timeout)
    while( numRead != -1 && totalTimeout < inMaxTimeInMilliseconds ) {
        numRead =
            this->receive( tempBuffer, 1, timeout );


        // track total time whether the receive timed out or not
        totalTimeout =
            (long)( Time::getMillisecondsSince( startSec, startMsec ) );
        }

    delete [] tempBuffer;
    }


	
#endif
