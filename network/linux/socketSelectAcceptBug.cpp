/*
 * Demonstration of "accept after select" socket bug.
 *
 * This program works fine on:
 * LinuxPPC, FreeBSD, and MacOSX
 *
 * This program does not work on:
 * LinuxX86 Kernel 2.4
 * (accept fails after select returns)
 *
 * This bug has something to do with the size of the
 * variables on the stack...
 * Getting rid of the union (which is unnecessary) seems
 * to fix the problem.
 *
 *
 * Jason Rohrer
 * 2002-April-12
 * rohrer@cse.ucsc.edu
 */


#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>



union sock {
    struct sockaddr s;
    struct sockaddr_in i;
    } sock;


void usage( char *inAppName );


int acceptConnection( int inSocketID, long inTimeoutMS );



int port;

int main( int inNumArgs, char **inArgs ) {

    if( inNumArgs != 2 ) {
        usage( inArgs[0] );
        }


    int numRead = sscanf( inArgs[1], "%d", &port );


    // create a socket and start listening for connections

    printf( "Creating socket\n" );
    int socketID = socket( AF_INET, SOCK_STREAM, 0 );
	
    if( socketID == -1 ) {
        printf( "Socket creation failed\n" );
        exit( 1 );
        }    
    
    // bind socket to the port
    union sock sockAddress;
    
    sockAddress.i.sin_family = AF_INET;
    sockAddress.i.sin_port = htons( port );
    sockAddress.i.sin_addr.s_addr = INADDR_ANY;

    printf( "Binding to socket at port %d\n", port );
    int error = bind( socketID, &(sockAddress.s), sizeof( struct sockaddr ) );
    
    if( error == -1  ) {
        printf( "Bad socket bind, port %d\n", port );
        exit( 1 );
        }
    
    
    // start listening for connections, max queued = 100 connections
    printf( "Starting to listen to socket\n" );
    error = listen( socketID, 100 );
    if( error == -1 ) {
        printf( "Bad socket listen\n" );
        exit(1);
        }


    // no we're listening to this socket

    int acceptedID = acceptConnection( socketID, 50000 );

    if( acceptedID != -1 ) {
        // close the connection
        shutdown( acceptedID, SHUT_RDWR );
    
        close( acceptedID );
        }

    
    shutdown( socketID, SHUT_RD );

    
    return 1;
    }



int acceptConnection( int inSocketID, long inTimeoutMS ) {
        
    // use select to timeout if no connection received in 10 seconds
    
    struct timeval tv;
    

    
    socklen_t addressLength;
    union sock acceptedAddress;
    
    
    int acceptedID = accept( inSocketID, 
                             &( acceptedAddress.s ), &addressLength );
        
    if( acceptedID == -1 ) {
        printf( "Failed to accept the connection\n" );
        printf( "Error = %d\n", errno );

        return -1;
            }
    else {
        printf( "Successfully accepted the connection\n" );

        return acceptedID;                        
        }        
    
    
    }


    
void usage( char *inAppName ) {
    printf( "Usage:\n" );
    printf( "    %s port_number\n", inAppName );
    exit( 1 );
    }

