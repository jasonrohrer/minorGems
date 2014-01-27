#include "cryptoRandom.h"




#ifdef WIN_32
// special case for Windows which has no /dev/urandom

#include <windows.h>
#include <wincrypt.h>

char getCryptoRandomBytes( unsigned char *outBytes, int inNumBytes ) {
    
    HCRYPTPROV hCryptProv;
    
    char result =
        CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, 
                             CRYPT_VERIFYCONTEXT );
    
    if( !result ) {
        return false;
        }
    

    result = CryptGenRandom( hCryptProv, inNumBytes, outBytes );


    CryptReleaseContext( hCryptProv, 0 );

    return result;
    }



#else
// general case:  most unix-like systems, including GNU/Linux and MacOSX,
// provide /dev/urandom

#include <stdio.h>

char getCryptoRandomBytes( unsigned char *outBytes, int inNumBytes ) {

    FILE *urandomFile = fopen( "/dev/urandom", "rb" );
    
    if( urandomFile == NULL ) {
        return false;
        }
    
    int numRead = fread( outBytes, 1, inNumBytes, urandomFile );

    fclose( urandomFile );
    

    return (numRead == inNumBytes );
    }


#endif
