

#include "curve25519.h"
#include "../cryptoRandom.h"

#include "stdio.h"

static void printKey( unsigned char inKey[32] ) {
    for( int i = 0; i < 32; i++ ) {
        printf( "%02x", inKey[i] );
        }
    printf("\n");
    }

 

int main() {

    unsigned char mySecret[32];
    unsigned char otherSecret[32];
    
    getCryptoRandomBytes( mySecret, 32 );
    getCryptoRandomBytes( otherSecret, 32 );
    


    unsigned char myPublic[32];
    unsigned char otherPublic[32];
    
    curve25519_genPublicKey( myPublic, mySecret );
    curve25519_genPublicKey( otherPublic, otherSecret );

    unsigned char myShared[32];
    unsigned char otherShared[32];

    curve25519_genSharedSecretKey( myShared, mySecret, otherPublic );
    curve25519_genSharedSecretKey( otherShared, otherSecret, myPublic );
    
    
    printf( "My secret = \n" );
    printKey( mySecret );
    
    printf( "Other secret = \n" );
    printKey( otherSecret );


    printf( "My public = \n" );
    printKey( myPublic );
    
    printf( "Other public = \n" );
    printKey( otherPublic );
    

    printf( "My shared = \n" );
    printKey( myShared );
    
    printf( "Other shared = \n" );
    printKey( otherShared );

    return 0;
    }
