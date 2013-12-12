/*
 * Modification History
 *
 * 2002-May-5   Jason Rohrer
 * Created.
 * Changed to default to http:// if no URL type specified.
 * Added support for the Host: header.
 *
 * 2002-May-7   Jason Rohrer
 * Added support for pages that have been moved permanently.
 *
 * 2002-May-12   Jason Rohrer
 * Added support for the Object Moved message type.
 * Added support for fetching the final (after redirects) URL.
 * Fixed a memory leak.
 *
 * 2002-May-26   Jason Rohrer
 * Added support for fetching mime types and content length.
 * Added a function for fetching MIME types alone.
 * Changed to use case-ignoring string comparison where appropriate.
 *
 * 2002-July-19   Jason Rohrer
 * Changed to deal with not found headers properly.
 *
 * 2002-August-5   Jason Rohrer
 * Fixed a typo.
 *
 * 2002-November-10   Jason Rohrer
 * Added a fix to deal with a slashdot bug.
 *
 * 2006-January-22  Jason Rohrer
 * Added support for timeouts.
 *
 * 2008-September-25  Jason Rohrer
 * Added POST.
 *
 * 2013-December-12  Jason Rohrer
 * Fixed POST function call name.  Fixed const char* warnings.
 */



#include "WebClient.h"

#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"



char *WebClient::getWebPage( char *inURL, int *outContentLength,
                             char **outFinalURL,
                             char **outMimeType,
                             long inTimeoutInMilliseconds ) {

    return executeWebMethod( "GET", inURL, NULL, outContentLength,
                             outFinalURL, outMimeType,
                             inTimeoutInMilliseconds );
    }



char *WebClient::getWebPagePOST( char *inURL, char *inPostBody,
                                 int *outContentLength,
                                 char **outFinalURL,
                                 char **outMimeType,
                                 long inTimeoutInMilliseconds ) {

    return executeWebMethod( "POST", inURL, inPostBody, outContentLength,
                             outFinalURL, outMimeType,
                             inTimeoutInMilliseconds );
    }



char *WebClient::getMimeType( char *inURL ) {

    int contentLength;

    char *mimeType;
    
    char *content = executeWebMethod( "HEAD", inURL, NULL, &contentLength,
                                      NULL, &mimeType );

        
    if( content != NULL ) {
        delete [] content;
        }
    
    return mimeType;
    }



char *WebClient::executeWebMethod( const char *inMethod,
                                   char *inURL, 
                                   char *inBody,
                                   int *outContentLength,
                                   char **outFinalURL,
                                   char **outMimeType,
                                   long inTimeoutInMilliseconds ) {

    char *returnString = NULL;
    
    
    const char *startString = "http://";

    char *urlCopy = stringDuplicate( inURL );

    
    char *urlStart = stringLocateIgnoreCase(  urlCopy, startString );

    char *serverStart;
    
    if( urlStart == NULL ) {
        // no http:// at start of URL
        serverStart = urlCopy;
        }
    else {
        serverStart = &( urlStart[ strlen( startString ) ] );
        }
    
    // find the first occurrence of "/", which is the end of the
    // server name

    char *serverNameCopy = stringDuplicate( serverStart );
        
    char *serverEnd = strstr( serverNameCopy, "/" );

    const char *getPath = strstr( serverStart, "/" );

        
    if( serverEnd == NULL ) {
        serverEnd = &( serverStart[ strlen( serverStart ) ] );
        getPath = "/";
        }
    // terminate the url here to extract the server name
    serverEnd[0] = '\0';

    int portNumber = 80;

        // look for a port number
    char *colon = strstr( serverNameCopy, ":" );
    if( colon != NULL ) {
        char *portNumberString = &( colon[1] );
                
        int numRead = sscanf( portNumberString, "%d",
                              & portNumber );
        if( numRead != 1 ) {
            portNumber = 80;
            }

        // terminate the name here so port isn't taken as part
        // of the address
        colon[0] = '\0';
        }

    HostAddress *host = new HostAddress(
        stringDuplicate( serverNameCopy ),
        portNumber );

    // will be set to true if we time out while connecting
    char timedOut;
    
    Socket *sock = SocketClient::connectToServer( host,
                                                  inTimeoutInMilliseconds,
                                                  &timedOut );

    char *finalURL = stringDuplicate( inURL );
    char *mimeType = NULL;

    int receivedLength = 0;

    
    if( sock != NULL ) {
        SocketStream *stream = new SocketStream( sock );

        // reuse the same timeout for read operations
        stream->setReadTimeout( inTimeoutInMilliseconds );
        
        // method and trailing space need to be sent in the same
        // buffer to work around a bug in certain web servers
        char *methodWithSpace = new char[ strlen( inMethod ) + 2 ];
        sprintf( methodWithSpace, "%s ", inMethod );
        
        // send the request
        stream->writeString( methodWithSpace );
        stream->writeString( getPath );
        stream->writeString( " HTTP/1.0\r\n" );
        stream->writeString( "Host: " );
        stream->writeString( serverNameCopy );
        stream->writeString( "\r\n" );
        
        if( inBody != NULL ) {
            char *lengthString = autoSprintf( "Content-Length: %d\r\n",
                                              strlen( inBody ) );
            stream->writeString( lengthString );
            delete [] lengthString;
            stream->writeString(
                "Content-Type: application/x-www-form-urlencoded\r\n\r\n" );
            
            stream->writeString( inBody );
            }
        else {
            stream->writeString( "\r\n" );
            }
        
        

        delete [] methodWithSpace;

        // the simplest thing to do is to read upto the
        // socket close first, then extract the content
        
        char *received = receiveData( stream, &receivedLength );

        char *content = NULL;

        char notFound = false;

        if( stringLocateIgnoreCase( received, "404 Not Found" ) != NULL ) {
            notFound = true;            
            }
        
        // watch for redirection headers
        if( stringLocateIgnoreCase( received, "302 Found" ) != NULL ||
            stringLocateIgnoreCase( received,
                                    "301 Moved Permanently" ) != NULL ||
            stringLocateIgnoreCase( received,
                                    "302 Object Moved" ) != NULL ) {

            // call ourself recursively to fetch the redirection
            const char *locationTag = "Location: ";
            char *locationTagStart =
                stringLocateIgnoreCase( received, locationTag );

            if( locationTagStart != NULL ) {

                char *locationStart =
                    &( locationTagStart[ strlen( locationTag ) ] );

                // replace next \r with \0
                char *nextChar = locationStart;
                while( nextChar[0] != '\r' && nextChar[0] != '\0' ) {
                    nextChar = &( nextChar[1] );
                    }
                nextChar[0] = '\0';

                char *newFinalURL;
                
                content = getWebPage( locationStart, &receivedLength,
                                      &newFinalURL,
                                      &mimeType );
                delete [] finalURL;
                finalURL = newFinalURL;

                if( content == NULL ) {
                    // not found recursively
                    notFound = true;
                    }
                }                        
            }

        const char *contentStartString = "\r\n\r\n";
        const char *contentTypeStartString = "Content-type:";

        if( notFound ) {
            returnString = NULL;
            }
        else {
            if( content == NULL ) {

                // scan for content type
                char *contentTypeStartMarker =
                    stringLocateIgnoreCase( received, contentTypeStartString );

                if( contentTypeStartMarker != NULL ) {
                    // skip marker
                    char *contentTypeStart =
                        &( contentTypeStartMarker[
                            strlen( contentTypeStartString ) ] );

                    // extract content type
                    // make sure the buffer is big enough
                    char *contentType =
                        new char[ strlen( contentTypeStartMarker ) ];

                    int numRead = sscanf( contentTypeStart, "%s", contentType );

                    if( numRead == 1 ) {
                        // trim
                        mimeType = stringDuplicate( contentType );
                        }
                    delete [] contentType;
                    }

                // extract the content from what we've received
                char *contentStart = strstr( received, contentStartString );
            
                if( contentStart != NULL ) {
                    content =
                        &( contentStart[ strlen( contentStartString ) ] );


                    receivedLength =
                        receivedLength
                        - strlen( contentStartString )
                        - ( (int)contentStart - (int)received );

                    returnString = new char[ receivedLength + 1 ];
                    returnString = (char*)memcpy( returnString,
                                                  content, receivedLength );

                    returnString[ receivedLength ] = '\0';
                    }
                }
            else {
                // we already obtained our content recursively
                returnString = new char[ receivedLength + 1 ];
                returnString = (char*)memcpy( returnString,
                                              content, receivedLength );
                
                returnString[ receivedLength ] = '\0';

                delete [] content;
                }
            }
        
        delete [] received;
                               
        delete stream;
        delete sock;
        }

    delete host;

        
    delete [] serverNameCopy;

    delete [] urlCopy;


    if( outFinalURL != NULL ) {
        *outFinalURL = finalURL;
        }
    else {
        delete [] finalURL;
        }

    if( outMimeType != NULL ) {
        *outMimeType = mimeType;
        }
    else {
        if( mimeType != NULL ) {
            delete [] mimeType;
            }
        }

    *outContentLength = receivedLength;
    
    return returnString;
    }



char *WebClient::receiveData( SocketStream *inSocketStream,
                              int *outContentLength ) {

    SimpleVector<char> *receivedVector =
            new SimpleVector<char>();

    char connectionBroken = false;
    long bufferLength = 5000;
    unsigned char *buffer = new unsigned char[ bufferLength ];

    while( !connectionBroken ) {

        int numRead = inSocketStream->read( buffer, bufferLength );

        if( numRead != bufferLength ) {
            connectionBroken = true;
            }

        if( numRead > 0 ) {
            for( int i=0; i<numRead; i++ ) {
                receivedVector->push_back( buffer[i] );
                }
            }
        }

    delete [] buffer;

    // copy our vector into an array
    int receivedSize = receivedVector->size();
    char *received = new char[ receivedSize + 1 ];
        
    for( int i=0; i<receivedSize; i++ ) {
        received[i] = *( receivedVector->getElement( i ) );
        }
    received[ receivedSize ] = '\0';
                
    delete receivedVector;

    *outContentLength = receivedSize;
    
    return received;
    }
