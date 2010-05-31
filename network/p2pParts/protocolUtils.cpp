/*
 * Modification History
 *
 * 2003-August-12   Jason Rohrer
 * Created.
 *
 * 2003-August-22   Jason Rohrer
 * Added function for getting a token after reading.
 */



#include "minorGems/network/p2pParts/protocolUtils.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/stringUtils.h"



char *readStreamUpToTag( InputStream *inInputStream,
                         char *inTag,
                         int inMaxCharsToRead ) {

    char *readCharBuffer = new char[ inMaxCharsToRead + 1 ];


    int numCharsRead = 0;
    char tagSeen = false;
    char readError = false;

    int tagLength = strlen( inTag );

    // start with empty string
    readCharBuffer[ numCharsRead ] = '\0';

    
    while( numCharsRead < inMaxCharsToRead && !tagSeen && !readError ) {
        long numRead = inInputStream->readByte(
            (unsigned char *)
            ( &( readCharBuffer[ numCharsRead ] ) ) );
        
        if( numRead != 1 ) {
            readError = true;
            readCharBuffer[ numCharsRead ] = '\0';
            }
        else {
            numCharsRead++;
                
            // terminate and check if buffer ends with our tag
            readCharBuffer[ numCharsRead ] = '\0';

            if( numCharsRead > tagLength ) {
                char *possibleBodyStart =
                    &( readCharBuffer[ numCharsRead - tagLength ] );
                
                if( strcmp( possibleBodyStart, inTag ) == 0 ) {
                    tagSeen = true;
                    }
                }
            }
        }
    

    
    if( !readError && tagSeen ) {
        char *returnString = stringDuplicate( readCharBuffer );

        delete [] readCharBuffer;

        return returnString;
        }
    else {
        char *message = autoSprintf(
            "Failed to find end tag \"%s\", read %d characters:\n%s\n",
            inTag, numCharsRead, readCharBuffer );
        
        AppLog::info( "readStreamUpToTag", message );

        delete [] message;
        
        delete [] readCharBuffer;

        return NULL;
        }
    }



char *readStreamUpToTagAndGetToken( InputStream *inInputStream,
                                    char *inTag, int inMaxCharsToRead,
                                    int inTokenNumber ) {

    // read the string
    char *readString = readStreamUpToTag( inInputStream,
                                          inTag,
                                          inMaxCharsToRead );

    if( readString == NULL ) {
        return NULL;
        }

    SimpleVector<char *> *readTokens =
        tokenizeString( readString );

    delete [] readString;

    
    // second token should be their key
    char *selectedToken = NULL;

    int numTokens = readTokens->size(); 
    if( numTokens > inTokenNumber ) {
        selectedToken =
            stringDuplicate( *( readTokens->getElement( inTokenNumber ) ) );
        }
    else {
        char *message = autoSprintf(
            "Looking for token %d, but only %d tokens available\n",
            inTokenNumber, numTokens );
        
        AppLog::error( "readStreamUpToTagAndGetToken", message );
        }

    
    for( int i=0; i<numTokens; i++ ) {
        delete [] *( readTokens->getElement( i ) );
        }
    delete readTokens;


    // will be NULL if not enough tokens read
    return selectedToken;
    }
