

#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/network/web/WebClient.h"
#include "minorGems/formats/encodingUtils.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/log/FileLog.h"

#include "minorGems/crypto/cryptoRandom.h"
#include "minorGems/crypto/keyExchange/curve25519.h"

#define STEAM_API_NON_VERSIONED_INTERFACES 1

//#include "steam/steam_api.h"
#include "openSteamworks/Steamclient.h"


static const char *steamGateServerURL = 
"http://192.168.0.3/jcr13/steamGate/server.php";


int main() {

    AppLog::setLog( new FileLog( "log_steamGate.txt" ) );
    AppLog::setLoggingLevel( Log::DETAIL_LEVEL );

    char *code = SettingsManager::getStringSetting( "downloadCode" );
    char *email = SettingsManager::getStringSetting( "email" );


    if( code != NULL && email != NULL ) {

        delete [] code;
        delete [] email;
        
        AppLog::info( "We already have saved login info.  Exiting." );
        
        return 0;
        }

    
    if( code != NULL ) {
        delete [] code;
        }
     
    if( email != NULL ) {
        delete [] email;
        }
     
    AppLog::info( "No login info found.  "
                  "Executing first-login protocol with server." );

    
    if( ! SteamAPI_Init() ) {
        AppLog::error( "Could not init Steam API." );
        return 0;
        }
    

    CSteamID id = SteamUser()->GetSteamID();
    
    uint64 rawID = id.ConvertToUint64();
    
    AppLog::infoF( "Steam ID %lli", rawID );
    
    unsigned char ourPubKey[32];
    unsigned char ourSecretKey[32];
    
    char gotSecret = 
        getCryptoRandomBytes( ourSecretKey, 32 );
    
    if( ! gotSecret ) {
        AppLog::error( "Failed to get secure random bytes for "
                       "key generation." );
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

    AppLog::infoF( "Web request to URL: %s", webRequest );
    
    int resultLength;
    char *webResult = WebClient::getWebPage( webRequest, &resultLength );
         
    delete [] webRequest;
    

    if( webResult == NULL ) {
        AppLog::error( "Failed to get response from server." );
        return 0;
        }
    

    SimpleVector<char *> *tokens = tokenizeString( webResult );
    
    
    
    if( tokens->size() != 3 || 
        strlen( *( tokens->getElement( 0 ) ) ) != 64 ) {
        AppLog::errorF( "Unexpected server response:  %s",
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
        AppLog::errorF( "Unexpected server response:  %s",
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
        AppLog::errorF( "Unexpected server response:  %s",
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
    delete [] encryptedTicket;


    AppLog::infoF( "Decrypted ticket as:  %s", plaintextTicket );
    
    SettingsManager::setSetting( "email", email );
    SettingsManager::setSetting( "downloadCode", plaintextTicket );
    
    
    delete [] plaintextTicket;
    delete [] email;
    delete [] webResult;
    delete tokens;
    

    return 0;
    }
