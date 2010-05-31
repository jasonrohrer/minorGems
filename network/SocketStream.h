/*
 * Modification History
 *
 * 2001-January-10		Jason Rohrer
 * Created.
 *
 * 2001-January-15		Jason Rohrer
 * Changed so that underlying socket is not destroyed when the stream
 * is destroyed.  Updated to match new Stream interface, which uses longs
 * instead of ints.
 *
 * 2001-February-21		Jason Rohrer
 * Changed to set stream error messages on socket errors.
 *
 * 2002-July-31   Jason Rohrer
 * Added fix for partial sends.
 *
 * 2003-August-12   Jason Rohrer
 * Added support for read timeouts.
 *
 * 2004-January-27   Jason Rohrer
 * Made functions virtual to support subclassing.
 */



#include "minorGems/common.h"


#ifndef SOCKET_STREAM_CLASS_INCLUDED
#define SOCKET_STREAM_CLASS_INCLUDED

#include "Socket.h"

#include "minorGems/io/InputStream.h"
#include "minorGems/io/OutputStream.h"


/**
 * A input and output stream interface for a network socket.
 *
 * @author Jason Rohrer
 */ 
class SocketStream : public InputStream, public OutputStream {

	public:
		
		
		/**
		 * Constructs a SocketStream.
		 *
		 * @param inSocket the newtork socket wrapped by this stream.
		 *   inSocket is NOT destroyed when the stream is destroyed.
		 */
		SocketStream( Socket *inSocket );

        // a virtual destructor to ensure that subclass destructors are called
        virtual ~SocketStream();

        

        /**
         * Sets the timeout for reads on this socket.
         *
         * The timeout defaults to -1 (no timeout).
         *
         * @param inMilliseconds the timeout in milliseconds,
         *   or -1 to specify no timeout.
         */
        void setReadTimeout( long inMilliseconds );



        /**
         * Gets the timeout for reads on this socket.
         *
         * @return the timeout in milliseconds,
         *   or -1 to indicate  no timeout.
         */
        long getReadTimeout();

        
		
		// implements the InputStream interface.
        // virtual to allow subclasses to override
        
        // in addition, -2 is returned if the read times out.
		virtual long read( unsigned char *inBuffer, long inNumBytes );
		
		
		// implements the OutputStream interface
		virtual long write( unsigned char *inBuffer, long inNumBytes );
		
		
	protected:
		Socket *mSocket;	
		long mReadTimeout;
	};		



inline SocketStream::SocketStream( Socket *inSocket ) 
	: mSocket( inSocket ),
      mReadTimeout( -1 ) {

	}



inline SocketStream::~SocketStream() {
    // does nothing
    // exists only to allow for subclass destructors
    }



inline void SocketStream::setReadTimeout( long inMilliseconds ) {
    mReadTimeout = inMilliseconds;
    }



inline long SocketStream::getReadTimeout() {
    return mReadTimeout;
    }



inline long SocketStream::read( unsigned char *inBuffer, long inNumBytes ) {
	int numReceived = mSocket->receive( inBuffer, inNumBytes, mReadTimeout );
	if( numReceived == -1 ) {
		// socket error
		InputStream::setNewLastErrorConst(
            "Network socket error on receive." );
		}
	return numReceived;	
	}
		
		

inline long SocketStream::write( unsigned char *inBuffer, long inNumBytes ) {
    long numTotalSent = 0;

    while( numTotalSent < inNumBytes ) {

        unsigned char *partialBuffer = &( inBuffer[numTotalSent] );

        int numRemaining = inNumBytes - numTotalSent;
        
        int numSent = mSocket->send( partialBuffer, numRemaining );

        if( numSent == -1 || numSent == 0 ) {
            // socket error
            OutputStream::setNewLastErrorConst(
                "Network socket error on send." );
            return -1;
            }
        
        numTotalSent += numSent;
        }

    
	return numTotalSent;	
	}
	
	
	
#endif
