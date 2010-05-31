/*
 * Modification History
 *
 * 2002-May-25   Jason Rohrer
 * Created.
 *
 * 2003-September-21   Jason Rohrer
 * Added test of long string.
 */



#include "sha1.h"



#include <stdio.h>
#include <string.h>



/**
 * All parameters must be destroyed by caller.
 */
void checkHash( char *inString, char *inTestName, char *inCorrectHash );



int main() {


    /*
     * Test vectors:
     *
     *  "abc"
     *  A999 3E36 4706 816A BA3E  2571 7850 C26C 9CD0 D89D
     *
     *  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
     *  8498 3E44 1C3B D26E BAAE  4AA1 F951 29E5 E546 70F1
     *
     *  A million repetitions of "a"
     *  34AA 973C D4C4 DAA4 F61E  EB2B DBAD 2731 6534 016F
     */

    char *abc = "abc";
    char *correctABCHash = "A9993E364706816ABA3E25717850C26C9CD0D89D";
    char *mixedAlpha =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    char *correctMixedAlphaHash = "84983E441C3BD26EBAAE4AA1F95129E5E54670F1";
    

    char *correctMillionHash = "34AA973CD4C4DAA4F61EEB2BDBAD27316534016F";

    int oneMillion = 1000000;
    char *millionAs = new char[ oneMillion + 1 ];
    for( int i=0; i<oneMillion; i++ ) {
        millionAs[i] = 'a';
        }
    millionAs[ oneMillion ] = '\0';


    
    checkHash( abc, abc, correctABCHash );
    checkHash( mixedAlpha, mixedAlpha, correctMixedAlphaHash );
    
    checkHash( millionAs, "A million repetions of \'a\'",
               correctMillionHash );
    

    
    delete [] millionAs;

    char *testString = strdup( "this is a very long chat message, so long in fact that we are going to put this program through the mill even longer: this is a very long chat message, so long in fact that we are going to put this program through the mill" );
    printf( "Hashing string: %s\n", testString );

    unsigned char *testHash = computeRawSHA1Digest( testString );

    printf( "String after hashing: %s\n", testString );

    delete [] testString;
    delete [] testHash;
    
    return 0;
    }



void checkHash( char *inString, char *inTestName, char *inCorrectHash ) {

    printf( "Hashing   %s\n", inTestName );
    printf( "    Output:\n" );

    char *stringHash = computeSHA1Digest( inString );
    printf( "        %s\n", stringHash );

    printf( "    Should Be:\n" );

    printf( "        %s\n", inCorrectHash );


    if( strcmp( inCorrectHash, stringHash ) == 0 ) {
        printf( "    Hash is correct.\n" );
        }
    else {
        printf( "    Hash is NOT correct.\n" );
        }

    delete [] stringHash;
    }
