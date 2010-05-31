/*
 * Modification History
 *
 * 2001-April-23   Jason Rohrer
 * Created.
 *
 * 2003-January-16   Jason Rohrer
 * Improved handling of POST data.
 */


// a test server for HTTP to aid in learning the protocol
// goal is to find out what a standard web browser sends to a
// web server



#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"

#include <stdio.h>


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

	while( true ) {
		printf( "waiting for a connection on port %d\n", port );
		Socket *sock = server->acceptConnection();

		printf( "connection received\n" );
		
		SocketStream *stream = new SocketStream( sock );

        //stream->writeString( "HTTP/1.0 100 Continue\r\n\r\n" );

        
		unsigned char *buffer = new unsigned char[1];

        char done = false;
        while( !done ) {
			// read and print characters from the stream
			stream->read( buffer, 1 );
			printf( "%c", buffer[0] );
            
            if( buffer[0] == '\r' ) {
                stream->read( buffer, 1 );
                printf( "%c", buffer[0] );

                if( buffer[0] == '\n' ) {
                    stream->read( buffer, 1 );
                    printf( "%c", buffer[0] );

                    if( buffer[0] == '\r' ) {
                        stream->read( buffer, 1 );
                        printf( "%c", buffer[0] );

                        if( buffer[0] == '\n' ) {
                            done = true;
                            }
                        }
                    }
                }
			}

        printf( "    Got full request.\n" );
        printf( "    Post-request data:\n" );
        
        //stream->writeString( "HTTP/1.1 100 Continue\r\n\r\n" );

        done = false;
        while( !done ) {
			// read and print characters from the stream
			int numRead = stream->read( buffer, 1 );
            if( numRead == 1 ) {
                printf( "%c", buffer[0] );

                if( buffer[0] == '\r' ) {
                    stream->read( buffer, 1 );
                    printf( "%c", buffer[0] );

                    if( buffer[0] == '\n' ) {
                        stream->read( buffer, 1 );
                        printf( "%c", buffer[0] );
                        
                        if( buffer[0] == '\r' ) {
                            stream->read( buffer, 1 );
                            printf( "%c", buffer[0] );
                            
                            if( buffer[0] == '\n' ) {
                                done = true;
                                }
                            }
                        }
                    }
                }
            else {
                // connection has been closed
                printf( "    Connection closed by remote host.\n" );
                done = true;
                }
            }

        printf( "Done receiving request.\n" );
        
		delete [] buffer;

		delete sock;
		delete stream;
		}

	delete server;

	return 1;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s port_number\n", inAppName );

	printf( "Example:\n" );
	printf( "\t%s 5101\n", inAppName );

	exit( 1 );
	}
