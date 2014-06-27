#include "minorGems/crypto/cryptoRandom.h"
#include "minorGems/crypto/keyExchange/curve25519.h"
#include "minorGems/formats/encodingUtils.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static void usage() {
    printf( "Generates a public key and a shared secret key from a\n"
            "communication partner's public key.\n\n" );
    
    printf( "Accepts and outputs 32-bit keys as hex-encoded,\n"
            "64-character strings.\n\n" );
    
    

    printf( "Usage:\n" );
    
    printf( "  curve25519GenKeys other_public_key\n\n" );


    printf( "Prints:\n" );

    printf( "  public_key\n" );
    printf( "  shared_secret_key\n\n" );
    

    printf( "Example:\n" );
    
    printf( 
     "  curve25519GenKeys "
     "B6B72BA25F09EA70DD2FC7C02CE3B953B8FA78C1A832E3ADFD4DA523A6E49115\n\n" );


    printf( "Prints:\n" );

    printf( "  CC64D3E200B1EB8AD0247B56FC17334BB4C6D2422DBFE430733B58D823F5210B\n" );
    printf( "  00E06A86A96572D4B56105C8640C9E476F79581F9FB0B49E356D92EDF727915A\n" );

    exit(0);
    }



int main( int inNumArgs, char **inArgs ) {
    if( inNumArgs != 2 ) {
        usage();
        }

    char *otherPublicKeyHex = inArgs[1];
    
    if( strlen( otherPublicKeyHex ) != 64 ) {
        usage();
        }
    

    unsigned char *otherPublicKey = hexDecode( otherPublicKeyHex );
    
    if( otherPublicKey == NULL ) {
        usage();
        }

    unsigned char secretKey[32];
    unsigned char ourPublicKey[32];
    unsigned char sharedSecretKey[32];
    
    char gotRandom = getCryptoRandomBytes( secretKey, 32 );
    
    if( !gotRandom ) {
        delete [] otherPublicKey;
        
        printf( "Failed to generate crypto-secure random bytes.\n" );
        return 1;
        }
    
        
    

    

    curve25519_genPublicKey( ourPublicKey, secretKey );
    
    curve25519_genSharedSecretKey( sharedSecretKey, 
                                   secretKey, otherPublicKey );
    

    char *ourPublicKeyHex = hexEncode( ourPublicKey, 32 );
    char *sharedSecretKeyHex = hexEncode( sharedSecretKey, 32 );
    
    printf( "%s\n%s\n", ourPublicKeyHex, sharedSecretKeyHex );
    

    delete [] otherPublicKey;
    delete [] ourPublicKeyHex;
    delete [] sharedSecretKeyHex;

    return 0;
    }

    

