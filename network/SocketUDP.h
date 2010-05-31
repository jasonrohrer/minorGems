/*
 * Modification History
 *
 * 2004-November-3   Jason Rohrer
 * Created.
 *
 * 2004-November-4   Jason Rohrer
 * Added default receive timeout (infinite).
 *
 * 2004-November-9   Jason Rohrer
 * Added functions for comparing and copying UDPAddresses.
 */


#ifndef SOCKET_UDP_INCLUDED
#define SOCKET_UDP_INCLUDED



/**
 * Structure representing a UDP endpoint.
 */
struct UDPAddress {
        // binary internet address in network byte order
        unsigned long mIPAddress;
        // port number in network byte order
        unsigned short mPort;
    };




/**
 * Network socket that can be used as an endpoint for sending and receiving
 * UDP packets (unreliable datagrams).
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the unix/ and win32/ subdirectories).
 *
 * Socket::initSocketFramework() must be called once before this class
 * is used.
 * 
 * @author Jason Rohrer
 */
class SocketUDP {


    public:
        
        /**
         * Constructs a UDP socket and starts listening for incoming datagrams.
         *
         * @param inReceivePort the port to listen on, in platform-dependent
         *   byte order.
         */
        SocketUDP( unsigned short inReceivePort );

        
        ~SocketUDP();

        

        /**
         * Makes a UDPAddress structure.
         *
         * @param inAddress the IP address in ascii numbers-and-dots notation.
         *   Must be destroyed by caller if non-const.
         * @param inPort the port number in platform-specific byte order.
         *
         * @return an address structure, or NULL if converting the address
         *   fails.
         *   Must be destroyed by caller if non-NULL.
         */
        static struct UDPAddress *makeAddress( const char *inAddress,
                                               unsigned short inPort );


        /**
         * Extracts address elements from a UDPAddress structure.
         *
         * @param inAddress the address structure.  Must be destroyed by
         *   caller.
         * @param outPort pointer to where the port number, in
         *   platform-specific byte order, should be returned.
         *
         * @return the IP address in ascci numbers-and-dots notation.
         *   Must be destroyed by caller.
         */
        static char *extractAddress( struct UDPAddress *inAddress,
                                     unsigned short *outPort );

        

        /**
         * Compares two UDP addresses.
         *
         * @param inFirst the first address.
         * @param inSecond the second address.
         *
         * @return true if the addresses are equal, or false if they are
         *   different.
         */
        static char compare( struct UDPAddress *inFirst,
                             struct UDPAddress *inSecond );



        /**
         * Makes a copy of a UDP address.
         *
         * @param inAddress the address to copy.
         *
         * @return a copy of the address.  Must be destroyed by caller.
         */
        static struct UDPAddress *copy( struct UDPAddress *inAddress );

        
        
        /**
         * Sends a datagram through this socket.
         *
         * Note:  the recommended maximum data length is 512 bytes
         *        to ensure that the datagram can be routed without
         *        fragmentation through all spec-compliant routers.
         *        Most routers support larger datagrams, however.
         *
         * @param inAddress the address to send data through.  Must be
         *   destroyed by caller.
         * @param inData the data bytes to send.
         * @param inNumBytes the number of bytes to send.
         *
         * @return the number of bytes sent successfully,
         *   or -1 for a socket error.
         */
        int send( struct UDPAddress *inAddress,
                  unsigned char *inData, unsigned long inNumBytes );

        
        
        /**
         * Receives a datagram from this socket.
         *
         * @param outAddress pointer to where the address of the remote
         *   host (the datagram sender) should be returned.
         *   Will be set to NULL on socket error or timeout.
         *   Must be destroyed by caller if non-NULL.
         * @param outData pointer to where the received data should be
         *   returned.  Will be set to NULL on socket error or timeout.
         *   Must be destroyed by caller if non-NULL.
         * @param inTimeout the timeout for this receive operation in
         *   milliseconds.  Set to -1 for an infinite timeout.
         *   -2 is returned from this call in the event of a timeout.
         *   Defaults to -1.
         *
         * @return the number of bytes received successfully,
         *   -1 for a socket error, or -2 for a timeout.
         */
        int receive( struct UDPAddress **outAddress,
                     unsigned char **outData,                     
                     long inTimeout = -1 );

        
        
        /**
         * Used by platform-specific implementations.
         */        
        void *mNativeObjectPointer;

        
        
    };



inline char SocketUDP::compare( struct UDPAddress *inFirst,
                                struct UDPAddress *inSecond ) {
    if( inFirst->mIPAddress == inSecond->mIPAddress &&
        inFirst->mPort == inSecond->mPort ) {
        return true;
        }
    else {
        return false;
        }
    }



inline struct UDPAddress *SocketUDP::copy( struct UDPAddress *inAddress ) {
    struct UDPAddress *returnAddress = new struct UDPAddress;

    returnAddress->mIPAddress = inAddress->mIPAddress;
    returnAddress->mPort = inAddress->mPort;

    return returnAddress;
    }



#endif
