/*
 * Modification History
 *
 * 2004-January-15   Jason Rohrer
 * Created.
 */

/**
 * A test program for snprintf behavior.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main() { 
    int numStrings = 3;
    
    // test strings of length 3, 4, and 5
    const char *testStrings[3] = { "tst", "test", "testt" };

    int result;
    
    // a buffer of length 4, which IS NOT large
    // enough to hold the last two testStrings
    char *buffer = (char*)( malloc( 4 ) );

    int i;
    
    for( i=0; i<numStrings; i++ ) {
        // clear buffer with 'a' characters
        memset( (void*)buffer, (int)( 'a' ), 4 );
    
        // print testStringA into buffer
        result = snprintf( buffer, 4, "%s", testStrings[i] );

        printf( "Printed string of length %d to buffer of "
                "size 4 with snprintf.\n"
                "Return value = %d\n",
                strlen( testStrings[i] ),
                result );

        if( buffer[3] == '\0' ) {
            printf( "Buffer was null-terminated by snprintf\n\n" );
            }
        else {
            printf( "Buffer was NOT null-terminated by snprintf\n\n" );
            }
        }

    free( buffer );
    
    return 0;
}

