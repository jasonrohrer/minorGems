/*
 * Modification History
 *
 * 2003-March-16   Jason Rohrer
 * Created.
 */



#include <stdlib.h>
#include <stdio.h>


#include "minorGems/network/SocketServer.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"
#include "minorGems/network/HostAddress.h"

#include "minorGems/system/Thread.h"


#include "minorGems/io/InputStream.h"



/**
 * Prints usage message and exits.
 */  
void usage( char *inAppName );



/**
 * Thread that prints inbound data.
 *
 * @author Jason Rohrer.
 */
class ReceivedPrintThread : public Thread {

    public:

        /**
         * Constructs a thread.
         *
         * @param inStream the stream to read data from.
         *   Must be destroyed by caller after this thread terminates.
         */
        ReceivedPrintThread( InputStream *inStream );


        
        // implements the Thread interface
        void run();


        
    protected:
        InputStream *mStream;

        
    };



ReceivedPrintThread::ReceivedPrintThread( InputStream *inStream )
    : mStream( inStream ) {

    }


void ReceivedPrintThread::run() {
    unsigned char readChar;

    int numRead = mStream->readByte( &readChar );

    while( numRead == 1 ) {

        printf( "%c", readChar );

        numRead = mStream->readByte( &readChar );
        }
    }





int main( int inNumArgs, char **inArgs ) {

    if( inNumArgs != 2 ) {
        usage( inArgs[0] );
        }

    int port;

    int numRead = sscanf( inArgs[1], "%d", &port );

    if( numRead != 1 ) {
        usage( inArgs[0] );
        }

    printf( "Listening for a connection on port %d...\n", port );

    SocketServer *server = new SocketServer( port, 1 );
    
    
    Socket *sock = server->acceptConnection();

    if( sock != NULL ) {

        HostAddress *address = sock->getRemoteHostAddress();

        printf( "Received connection from %s:%d\n", address->mAddressString,
                address->mPort );

        SocketStream *stream = new SocketStream( sock );

        ReceivedPrintThread *receiveThread = new ReceivedPrintThread( stream );

        receiveThread->start();


        int readChar = getc( stdin );

        int numWritten = 1;
        while( readChar != EOF && numWritten == 1 ) {
            unsigned char charToWrite = (unsigned char)readChar;
            numWritten = stream->write( &charToWrite, 1 );
            
            readChar = getc( stdin );
            }

        printf( "\nConnection broken by remote host\n" );

        receiveThread->join();

        delete receiveThread;

        delete stream;
        
        delete sock;
        }

    delete server;

    return 0;
    }



void usage( char *inAppName ) {

    printf( "tenlet:  like telnet, but in reverse\n\n" );
    printf( "With telnet, you establish an IP connection to a host, and\n"
            "then your keystrokes (or entered lines) are forwarded to\n"
            "that host, and its responses are printed on your terminal.\n\n" );
    printf( "telnet is commonly used for remote logins, but it is also\n"
            "great for manually testing and exploring text-based protocols,\n"
            "like HTTP.  Of course, telnet is only useful for testing the\n"
            "*client* end of a protocol, since it can establish connections\n"
            "but cannot receive inbound connections.  That is where tenlet\n"
            "comes in.\n\n" );

    printf( "tenlet essentially implents an interactive telnet server:\n"
            "your keystrokes are sent back to whatever client connects.\n"
            "With telnet, you might point your client at a web server and\n"
            "manually issue an HTTP request.  With tenlet, you can start\n"
            "a server and then *connect to it* with your web browser.  You\n"
            "can then observer the HTTP request that your browser sends and\n"
            "manually generate a response to test how your browser reacts."
            "\n\n" );

    
    printf( "Usage:\n" );
    printf( "\t%s listen_port\n\n", inAppName );
    printf( "Example:\n" );

    printf( "\t%s 5013\n", inAppName );

    exit( 1 );
    }
