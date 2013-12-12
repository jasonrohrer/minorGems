

#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/network/web/WebClient.h"
#include "minorGems/formats/encodingUtils.h"

#include "minorGems/crypto/cryptoRandom.h"
#include "minorGems/crypto/keyExchange/curve25519.h"


static const char *steamGateServerURL = 
"http://localhost/jcr13/steamGate/server.php";


int main() {

    char *code = SettingsManager::getStringSetting( "downloadCode" );
    char *email = SettingsManager::getStringSetting( "email" );


    if( code != NULL && email != NULL ) {

        delete [] code;
        delete [] email;
        
        printf( "steamGate:  We already have saved login info.  Exiting.\n" );
        
        return 0;
        }

    
    if( code != NULL ) {
        delete [] code;
        }
     
    if( email != NULL ) {
        delete [] email;
        }
     
    printf( "steamGate:  No login info found.\n"
            "steamGate:  Executing first-login protocol with server.\n" );
    
    unsigned char ourPubKey[32];
    unsigned char ourSecretKey[32];
    
    char gotSecret = 
        getCryptoRandomBytes( ourSecretKey, 32 );
    
    if( ! gotSecret ) {
        printf( "steamGate:  Failed to get secure random bytes for "
                "key generation.\n" );
        return 0;
        }
    
    
    curve25519_genPublicKey( ourPubKey, ourSecretKey );
    

    char *ourPubKeyHex = hexEncode( ourPubKey, 32 );

    const char *dummyAuthTicket = "blahblahblah";
    
    char *webRequest = 
        autoSprintf( 
            "%s?action=get_account"
            "&auth_session_ticket=%s"
            "&client_public_key=%s",
            steamGateServerURL,
            dummyAuthTicket,
            ourPubKeyHex );
            
    delete [] ourPubKeyHex;

    printf( "Web request to URL: %s\n", webRequest );
    
    int resultLength;
    char *webResult = WebClient::getWebPage( webRequest, &resultLength );
         
    delete [] webRequest;
    

    if( webResult == NULL ) {
        printf( "steamGate:  Failed to get response from server.\n" );
        return 0;
        }
    
    /*
Returns:
server_public_key
email
encrypted_ticket_id
    */

    SimpleVector<char *> *tokens = tokenizeString( webResult );
    
    
    
    if( tokens->size() != 3 || 
        strlen( *( tokens->getElement( 0 ) ) ) != 64 ) {
        printf( "steamGate:  Unexpected server response:\n%s\n",
                webResult );
        
        delete [] webResult;
        for( int i=0; i<tokens->size(); i++ ) {
            delete [] *( tokens->getElement(i) );
            }
        delete tokens;
        return 0;
        }


    char *serverPublicKeyHex = *( tokens->getElement( 0 ) );
    email = *( tokens->getElement( 1 ) );
    char *encryptedTicketHex = *( tokens->getElement( 2 ) );


    unsigned char *serverPublicKey = hexDecode( serverPublicKeyHex );
    delete [] serverPublicKeyHex;
    
    if( serverPublicKey == NULL ) {
        printf( "steamGate:  Unexpected server response:\n%s\n",
                webResult );
        
        delete [] email;
        delete [] encryptedTicketHex;
        delete [] webResult;
        delete tokens;
        return 0;
        }
    
    
    unsigned char sharedSecretKey[32];

    curve25519_genSharedSecretKey( sharedSecretKey, ourSecretKey, 
                                   serverPublicKey );
    delete [] serverPublicKey;
    

    int numTicketBytes = strlen( encryptedTicketHex ) / 2;
    unsigned char *encryptedTicket = hexDecode( encryptedTicketHex );
    delete [] encryptedTicketHex;
    
    if( encryptedTicket == NULL ) {
        printf( "steamGate:  Unexpected server response:\n%s\n",
                webResult );
    
        delete [] email;
        delete [] webResult;
        delete tokens;
        return 0;
        }
    
    char *plaintextTicket = new char[ numTicketBytes + 1 ];
    
    for( int i=0; i<numTicketBytes; i++ ) {
        plaintextTicket[i] = encryptedTicket[i] ^ sharedSecretKey[i];
        }
    
    plaintextTicket[ numTicketBytes ] = '\0';
    
    printf( "Decrypted ticket as:  %s\n", plaintextTicket );
    
    
    delete [] plaintextTicket;
    delete [] email;
    delete [] webResult;
    delete tokens;
    

    return 0;
    }
