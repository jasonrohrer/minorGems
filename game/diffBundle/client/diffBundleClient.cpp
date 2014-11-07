#include "diffBundleClient.h"

#include "minorGems/game/game.h"


#define PLATFORM_CODE ""

#if defined(__mac__)
    #define PLATFORM_CODE "mac"
#elif defined(WIN_32)
    #define PLATFORM_CODE "win"
#endif



char isUpdatePlatformSupported() {
    if( PLATFORM_CODE == "" ) {
        return false;
        }
    else {
        return true;
        }
    }



static int webHandle;
static int updateSize = -1;

static char *updateServerURL = NULL;
static int oldVersionNumber;


// returns handle
void startUpdate( char *inUpdateServerURL, int inOldVersionNumber ) {
    char *fullURL = autoSprintf( "%s?action=is_update_available"
                                 "&platform=%s&old_version=%d",
                                 inUpdateServerURL, PLATFORM_CODE,
                                 inOldVersionNumber );
    
    webHandle = startWebRequest( "GET", fullURL, NULL );
    
    delete [] fullURL;
    
    if( updateServerURL != NULL ) {
        delete [] updateServerURL;
        }

    updateSize = -1;
    
    updateServerURL = inUpdateServerURL;
    oldVersionNumber = inOldVersionNumber;

    /*
    char *fullURL = autoSprintf( "%s?action=get_update"
                                 "&platform=%s&old_version=%d",
                                 inUpdateServerURL, PLATFORM_CODE,
                                 inOldVersionNumber );
    
    int handle = startWebRequest( "GET", fullURL, NULL );
    
    delete [] fullURL;
    return handle;
    */
    }



// take another non-blocking step
// return 1 if request complete
// return -1 if request hit an error
// return 0 if request still in-progress
int stepUpdate( int inHandle ) {
    int result = stepWebRequest( inHandle );

    if( result == 1 ) {
        if( updateSize == -1 ) {
            char *result = getWebResult( webHandle );
            
            int numRead = sscanf( result, "%d", &updateSize );
            
            delete [] result;
            if( numRead != 1 ) {
                updateSize = 0;
                return -1;
                }
            
            if( updateSize <= 0 ) {
                // no update available for this platform
                return -1;
                }
            
            // have an update size
            
            // start next request for update itself
            clearWebRequest( webHandle );

            char *fullURL = autoSprintf( "%s?action=get_update"
                                         "&platform=%s&old_version=%d",
                                         updateServerURL, PLATFORM_CODE,
                                         oldVersionNumber );
    
            webHandle = startWebRequest( "GET", fullURL, NULL );
    
            delete [] fullURL;
            
            return 0;
            }
        else {
            // have update itself
            
            // FIXME
            // process it, unzip, apply file changes, etc.
            
            }
        }
    else {
        return result;
        }
    }



float getUpdateProgress() {
    if( updateSize > 0 ) {
        return getWebProgressSize( webHandle ) / (float)updateSize;
        }
    else {
        return 0;
        }
    }




void clearUpdate() {
    clearWebRequest( webHandle );

    if( updateServerURL != NULL ) {
        delete [] updateServerURL;
        }
    
    updateServerURL = NULL;
    }

