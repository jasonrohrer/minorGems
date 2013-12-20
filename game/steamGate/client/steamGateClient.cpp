

#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/network/web/WebClient.h"
#include "minorGems/formats/encodingUtils.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/log/FileLog.h"
#include "minorGems/system/Thread.h"
#include "minorGems/system/Time.h"

#include "minorGems/crypto/cryptoRandom.h"
#include "minorGems/crypto/keyExchange/curve25519.h"

#define STEAM_API_NON_VERSIONED_INTERFACES 1

#include "steam/steam_api.h"
//#include "openSteamworks/Steamclient.h"


/////////////////////
// settings:
static const char *steamGateServerURL = 
"http://thecastledoctrine.net/sg/server.php";

#define linuxLaunchTarget "./CastleDoctrineApp"
#define macLaunchTarget "CastleDoctrine.app"
#define winLaunchTarget "CastleDoctrine.exe"

// end settings
/////////////////////



#ifdef __mac__

#include <unistd.h>
#include <stdarg.h>

static void launchGame() {
    AppLog::info( "Launching game." );
    int forkValue = fork();

    if( forkValue == 0 ) {
        // we're in child process, so exec command
        char *arguments[3] = { "open", (char*)macLaunchTarget, NULL };
                
        execvp( "open", arguments );

        // we'll never return from this call
        }
    }


static void showMessage( const char *inTitle, const char *inMessage,
                         char inError = false ) {
    const char *iconName = "note";
    if( inError ) {
        iconName = "stop";
        }

    const char *commandFormat =
        "osascript -e 'tell app \"System Events\" to activate' "
        "-e 'tell app \"System Events\" to display dialog \"%s\" "
        "with title \"%s\" buttons \"Ok\" "
        "with icon %s default button \"Ok\"' "
        "-e 'tell app \"System Events\" to quit' ";
    
    char *command = autoSprintf( commandFormat, inMessage, inTitle, 
                                 iconName );
    
    FILE *osascriptPipe = popen( command, "r" );
    
    delete [] command;
    
    if( osascriptPipe == NULL ) {
        AppLog::error( 
            "Failed to open pipe to osascript for displaying GUI messages." );
        }
    else {
        pclose( osascriptPipe );
        }
    }



#elif defined(LINUX)

#include <unistd.h>
#include <stdarg.h>

static void launchGame() {
    AppLog::info( "Launching game" );
    int forkValue = fork();

    if( forkValue == 0 ) {
        // we're in child process, so exec command
        char *arguments[2] = { (char*)linuxLaunchTarget, NULL };
                
        execvp( linuxLaunchTarget, arguments );

        // we'll never return from this call
        }
    }



static void showMessage( const char *inTitle, const char *inMessage,
                         char inError = false ) {
    const char *zenCommand = "--info";
    if( inError ) {
        zenCommand = "--error";
        }

    char *command = autoSprintf( "zenity %s --text='%s' --title='%s'",
                                 zenCommand, inMessage, inTitle );

    FILE *zentityPipe = popen( command, "r" );
    
    delete [] command;

    if( zentityPipe == NULL ) {
        AppLog::error( 
            "Failed to open pipe to zenity for displaying GUI messages." );
        }
    else {
        pclose( zentityPipe );
        }
    }


#elif defined(WIN_32)

#include <windows.h>
#include <process.h>

static void launchGame() {
    AppLog::info( "Launching game" );
    char *arguments[2] = { (char*)winLaunchTarget, NULL };
    
    _spawnvp( _P_NOWAIT, winLaunchTarget, arguments );
    }


#include <stdlib.h>

static void showMessage( const char *inTitle, const char *inMessage,
                         char inError = false ) {
    UINT uType = MB_OK;
    
    if( inError ) {
        uType |= MB_ICONERROR;
        }
    else {
        uType |= MB_ICONINFORMATION;    
        }
    
    wchar_t *wideTitle = new wchar_t[ strlen( inTitle ) * 2 + 2 ];
    mbstowcs( wideTitle, inTitle, strlen( inTitle ) + 1 );
    
    wchar_t *wideMessage = new wchar_t[ strlen( inMessage ) * 2 + 2 ];
    mbstowcs( wideMessage, inMessage, strlen( inMessage ) + 1 );
    
    MessageBox( NULL, wideMessage, wideTitle, uType );

    delete [] wideTitle;
    delete [] wideMessage;
    }


int main();


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCMD ) {
    
    return main();
    }

#endif





static char authTicketCallbackCalled = false;
static char authTicketCallbackError = false;



// a class that does nothing other than register itself as a Steam
// callback listener when it is constructed
class AuthTicketListener {
        
    public:
        
        AuthTicketListener() 
                // initializing this member object is what actually
                // registers our callback
                : mAuthSessionTicketResponse( 
                    this, 
                    &AuthTicketListener::OnAuthSessionTicketResponse ) {
            AppLog::info( "AuthTicketListener instantiated" );
            }

        
        // callback when our AuthSessionTicket is ready
        // this macro declares our callback function for us AND
        // defines our member object
        STEAM_CALLBACK( 
            // class that the callback function is in
            AuthTicketListener, 
            // name of callback function
            OnAuthSessionTicketResponse,
            // type of callback
            GetAuthSessionTicketResponse_t, 
            // name of the member object       
            mAuthSessionTicketResponse );


    };



void AuthTicketListener::OnAuthSessionTicketResponse( 
    GetAuthSessionTicketResponse_t *inCallback ) {
    
    AppLog::info( "AuthTicketListener callback called" );

    authTicketCallbackCalled = true;
            
    if ( inCallback->m_eResult != k_EResultOK ) {
        authTicketCallbackError = true;
        }
    }        




static void showTicketCreationError() {
    showMessage( "The Castle Doctrine:  Error",
                 "Could not create an account for you on the game server.",
                 true );
    }





int main() {

    AppLog::setLog( new FileLog( "log_steamGate.txt" ) );
    AppLog::setLoggingLevel( Log::DETAIL_LEVEL );

    char *code = SettingsManager::getStringSetting( "downloadCode" );
    char *email = SettingsManager::getStringSetting( "email" );


    if( code != NULL && email != NULL ) {

        delete [] code;
        delete [] email;
        
        AppLog::info( "We already have saved login info.  Exiting." );
        launchGame();
        return 0;
        }

    
    if( code != NULL ) {
        delete [] code;
        }
     
    if( email != NULL ) {
        delete [] email;
        }
    
    showMessage( "The Castle Doctrine:  First Launch",
                 "The game will try to create a server-side account for"
                 " you through Steam.\n\nThis may take a few moments." );
    
    AppLog::info( "No login info found.  "
                  "Executing first-login protocol with server." );

    
    if( ! SteamAPI_Init() ) {
        showMessage( "The Castle Doctrine:  Error",
                     "Failed to connect to Steam.",
                     true );
        AppLog::error( "Could not init Steam API." );
        return 0;
        }
    

    CSteamID id = SteamUser()->GetSteamID();
    
    uint64 rawID = id.ConvertToUint64();

    
    
    AppLog::infoF( "Steam ID %lli", rawID );


    unsigned int appID = SteamUtils()->GetAppID();

    AppLog::infoF( "App ID %d", appID );
    

    char loggedOn = SteamUser()->BLoggedOn();
    
    AppLog::infoF( "Logged onto steam: %d", loggedOn );

    char behindNAT = SteamUser()->BIsBehindNAT();
    
    AppLog::infoF( "Behind NAT: %d", behindNAT );

    /*
    EUserHasLicenseForAppResult doesNotOwnApp =
        SteamUser()->UserHasLicenseForApp( id, appID );
    
    AppLog::infoF( "doesNotOwnApp = %d", doesNotOwnApp );
    */

    // construct a listener to add it to Steam's listener pool
    AuthTicketListener *listener = new AuthTicketListener();

    unsigned char authTicketData[2048];
    uint32 authTicketSize = 0;
    HAuthTicket ticketHandle =
        SteamUser()->GetAuthSessionTicket( 
            authTicketData, sizeof(authTicketData), 
            &authTicketSize );
        

    AppLog::infoF( "GetAuthSessionTicket returned %d bytes, handle %d",
                   authTicketSize, ticketHandle );

    if( authTicketSize == 0 ) {
        
        showMessage( "The Castle Doctrine:  Error",
                     "Could not get an Authentication Ticket from Steam.",
                     true );
        AppLog::error( "GetAuthSessionTicket returned 0 length." );
        
        SteamAPI_Shutdown();
        return 0;
        }
    

    AppLog::info( "Waiting for Steam GetAuthSessionTicket callback..." );
    
    double startTime = Time::getCurrentTime();
    double maxTime = 20;
    
    while( ! authTicketCallbackCalled ) {
        if( Time::getCurrentTime() - startTime > maxTime ) {
            showMessage( "The Castle Doctrine:  Error",
                         "Timed out waiting for "
                         "Authentication Ticket validation from Steam.",
                         true );
            AppLog::error( "Timed out." );
            SteamAPI_Shutdown();
            return 0;
            }
        
        Thread::staticSleep( 100 );
        SteamAPI_RunCallbacks();
        }

    AppLog::info( "...got callback." );

    // de-register listener
    delete listener;
    
    if( authTicketCallbackError ) {
        showMessage( "The Castle Doctrine:  Error",
                     "Could not validate the Steam Authentication Ticket.",
                     true );
        AppLog::error( "GetAuthSessionTicket callback returned error." );
        SteamAPI_Shutdown();
        return 0;
        }
    

    char *authTicketHex = hexEncode( authTicketData, authTicketSize );
    
    AppLog::infoF( "Auth ticket data:  %s", authTicketHex );


    


    
    unsigned char ourPubKey[32];
    unsigned char ourSecretKey[32];
    
    char gotSecret = 
        getCryptoRandomBytes( ourSecretKey, 32 );
    
    if( ! gotSecret ) {
        showMessage( "The Castle Doctrine:  Error",
                     "Could not get random data to generate an "
                     "encryption key.",
                     true );
        AppLog::error( "Failed to get secure random bytes for "
                       "key generation." );
        SteamAPI_Shutdown();
        return 0;
        }
    
    
    curve25519_genPublicKey( ourPubKey, ourSecretKey );
    

    char *ourPubKeyHex = hexEncode( ourPubKey, 32 );
    
    char *webRequest = 
        autoSprintf( 
            "%s?action=get_account"
            "&auth_session_ticket=%s"
            "&client_public_key=%s",
            steamGateServerURL,
            authTicketHex,
            ourPubKeyHex );
            
    delete [] ourPubKeyHex;
    delete [] authTicketHex;

    AppLog::infoF( "Web request to URL: %s", webRequest );
    
    //printf( "Waiting....\n" );
    //int read;
    //scanf( "%d", &read );
    
    int resultLength;
    char *webResult = WebClient::getWebPage( webRequest, &resultLength );
         
    delete [] webRequest;
    

    if( webResult == NULL ) {
        showTicketCreationError();
        AppLog::error( "Failed to get response from server." );
        
        SteamAPI_Shutdown();
        return 0;
        }
    

    SimpleVector<char *> *tokens = tokenizeString( webResult );
    
    
    
    if( tokens->size() != 3 || 
        strlen( *( tokens->getElement( 0 ) ) ) != 64 ) {
        showTicketCreationError();
        AppLog::errorF( "Unexpected server response:  %s",
                        webResult );
        
        delete [] webResult;
        for( int i=0; i<tokens->size(); i++ ) {
            delete [] *( tokens->getElement(i) );
            }
        delete tokens;
        
        SteamAPI_Shutdown();
        return 0;
        }


    char *serverPublicKeyHex = *( tokens->getElement( 0 ) );
    email = *( tokens->getElement( 1 ) );
    char *encryptedTicketHex = *( tokens->getElement( 2 ) );


    unsigned char *serverPublicKey = hexDecode( serverPublicKeyHex );
    delete [] serverPublicKeyHex;
    
    if( serverPublicKey == NULL ) {
        showTicketCreationError();
        AppLog::errorF( "Unexpected server response:  %s",
                        webResult );
        
        delete [] email;
        delete [] encryptedTicketHex;
        delete [] webResult;
        delete tokens;
        
        SteamAPI_Shutdown();
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
        showTicketCreationError();
        AppLog::errorF( "Unexpected server response:  %s",
                        webResult );
    
        delete [] email;
        delete [] webResult;
        delete tokens;
        
        SteamAPI_Shutdown();
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
    
    SteamAPI_Shutdown();

    showMessage( "The Castle Doctrine:  First Launch",
                 "Your server account is set up.\n\n"
                 "The game will launch now." );

    launchGame();
    
    return 0;
    }
