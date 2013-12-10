// interface for accessing crypto-secure random number generation features
// on various platforms


// outBytes allocated by caller
// returns true on success, or false if random bytes couldn't be acquired
char getCryptoRandomBytes( unsigned char *outBytes, int inNumBytes );
