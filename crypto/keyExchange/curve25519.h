


// Implements Eliptic Curve Diffie Hellman key exchange on Curve 25519.


// Generate a public 32-byte public key from a 32-byte secret key.
// Any 32 bytes work as a secret key. 
void curve25519_genPublicKey( unsigned char outPublicKey[32],
                              unsigned char inSecretKey[32] );


// Generate a shared secret key from our secret key and our communication
// partner's public key.
void curve25519_genSharedSecretKey( 
    unsigned char outSharedSecretKey[32],
    unsigned char inSecretKey[32],
    unsigned char inOtherPublicKey[32] );



