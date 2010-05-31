/*
 * Modification History
 *
 * 2004-May-20   Jason Rohrer
 * Created.
 */



#include "sha1.h"
#include "minorGems/formats/encodingUtils.h"


#include <stdio.h>
#include <stdlib.h>


/**
 * Prints usage message and exits.
 *
 * @param inAppName the name of the app.
 */
void usage( char *inAppName );



int main( int inNumArgs, char **inArgs ) {


    if( inNumArgs != 2 ) {
        usage( inArgs[0] );
        }


    FILE *file = fopen( inArgs[1], "rb" );

    if( file == NULL ) {
        printf( "Failed to open file %s for reading\n\n", inArgs[1] );

        usage( inArgs[0] );
        }

    
    SHA_CTX shaContext;

    SHA1_Init( &shaContext );

               
    int bufferSize = 5000;
    unsigned char *buffer = new unsigned char[ bufferSize ];

    int numRead = bufferSize;

    char error = false;
    
    // read bytes from file until we run out
    while( numRead == bufferSize && !error ) {

        numRead = fread( buffer, 1, bufferSize, file );

        if( numRead > 0 ) {
            SHA1_Update( &shaContext, buffer, numRead );
            }
        else{
            error = true;
            }
        }

    fclose( file ); 
    delete [] buffer;


    if( error ) {
        printf( "Error reading from file %s\n", inArgs[1] );
        }
    else {
        unsigned char *rawDigest = new unsigned char[ SHA1_DIGEST_LENGTH ];
        
        SHA1_Final( rawDigest, &shaContext );
        
        // else hash is correct
        char *digestHexString = hexEncode( rawDigest, SHA1_DIGEST_LENGTH );
        
        printf( "%s  %s\n", digestHexString, inArgs[1] );

        delete [] rawDigest;
        delete [] digestHexString;
        }
    
    return 0;
    }



void usage( char *inAppName ) {

    printf( "Usage:\n\n" );
    printf( "\t%s file_to_sum\n", inAppName );

    printf( "example:\n" );

    printf( "\t%s test.txt\n", inAppName );

    exit( 1 );
    }
