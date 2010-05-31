/*
 * Modification History
 *
 * 2002-April-12   Jason Rohrer
 * Created.
 */


#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketServer.h"

#include <stdlib.h>



void usage( char *inAppName );



int main( int inNumArgs, char **inArgs ) {

    if( inNumArgs != 2 ) {
        usage( inArgs[0] );
        }

    int port;

    int numRead = sscanf( inArgs[1], "%d", &port );

    if( numRead != 1 ) {
        usage( inArgs[0] );
        }

    SocketServer *server = new SocketServer( port, 100 );

    char timedOut;
    Socket *sock;

    printf( "waiting for connection on port %d\n", port );
    sock = server->acceptConnection( 5000, &timedOut );

    if( timedOut ) {

        printf( "timed out after 5 seconds, waiting again\n" );
        sock = server->acceptConnection( 5000, &timedOut );

        }

    if( !timedOut && sock != NULL ) {
        printf( "connection received\n" );
        
        delete sock;
        }
    else {
        printf( "timed out again\n" );
        }
    

    delete server;
    

    return 1;
    }


    
void usage( char *inAppName ) {
    printf( "Usage:\n" );
    printf( "    %s port_number\n", inAppName );
    exit( 1 );
    }
