/*
 * Modification History
 *
 * 2002-May-5   Jason Rohrer
 * Created.
 * Added a utility function for receiving data.
 *
 * 2002-May-12   Jason Rohrer
 * Added support for fetching the final (after redirects) URL.
 *
 * 2002-May-26   Jason Rohrer
 * Added support for fetching mime types and content length.
 * Added a function for fetching MIME types alone.
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

#include "minorGems/common.h"

 
#ifndef WEB_CLIENT_INCLUDED
#define WEB_CLIENT_INCLUDED 



#include "minorGems/network/HostAddress.h"
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketStream.h"


#include <string.h>
#include <stdio.h>


/**
 * A class that implements a basic web client.
 *
 * @author Jason Rohrer.
 */
class WebClient {



    public:


        
        /**
         * Gets a web page.
         *
         * @param inURL the URL to get as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         * @param outContentLength pointer to where the length of the content,
         *   in bytes, should be returned.
         *   Useful for binary content which cannot be reliably terminated
         *   by \0.
         * @param outFinalURL pointer to where the actual
         *   URL of the content (after following redirects) should be returned,
         *   or NULL to ignore the final URL.  Defaults to NULL.
         *   The string returned in this location must be destroyed
         *   by caller.
         * @param outMimeType pointer to where the MIME type
         *   of the content should be returned,
         *   or NULL to ignore the MIME type.  Defaults to NULL.
         *   The string returned in this location must be destroyed
         *   by caller.
         * @param inTimeoutInMilliseconds the timeout value when making
         *   the connection and again when reading data in milliseconds,
         *   or -1 for no timeout.
         *   Defaults to -1.
         *
         * @return the fetched web page as a \0-terminated string,
         *   or NULL if fetching the page fails.
         *   Must be destroyed by caller if non-NULL.
         */
        static char *getWebPage( char *inURL,
                                 int *outContentLength,
                                 char **outFinalURL = NULL,
                                 char **outMimeType = NULL,
                                 long inTimeoutInMilliseconds = -1 );
        


        /**
         * Fetches a web page via POST.
         *
         * @param inURL the URL to post to as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         * @param inPostBody the body of the post as \0-terminated string.
         *   Body must match MIME type   application/x-www-form-urlencoded
         *   Must be destroyed by caller if non-const. 
         * @param outContentLength pointer to where the length of the content,
         *   in bytes, should be returned.
         *   Useful for binary content which cannot be reliably terminated
         *   by \0.
         * @param outFinalURL pointer to where the actual
         *   URL of the content (after following redirects) should be returned,
         *   or NULL to ignore the final URL.  Defaults to NULL.
         *   The string returned in this location must be destroyed
         *   by caller.
         * @param outMimeType pointer to where the MIME type
         *   of the content should be returned,
         *   or NULL to ignore the MIME type.  Defaults to NULL.
         *   The string returned in this location must be destroyed
         *   by caller.
         * @param inTimeoutInMilliseconds the timeout value when making
         *   the connection and again when reading data in milliseconds,
         *   or -1 for no timeout.
         *   Defaults to -1.
         *
         * @return the fetched web page as a \0-terminated string,
         *   or NULL if fetching the page fails.
         *   Must be destroyed by caller if non-NULL.
         */
        static char *getWebPagePOST( char *inURL,
                                     char *inPostBody,
                                     int *outContentLength,
                                     char **outFinalURL = NULL,
                                     char **outMimeType = NULL,
                                     long inTimeoutInMilliseconds = -1 );

        

        /**
         * Gets the MIME type for a web page without fetching the content.
         *
         * @param inURL the URL to get as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         *
         * @return the fetched MIME type as a \0-terminated string,
         *   or NULL if fetching the page fails.
         *   Must be destroyed by caller if non-NULL.
         */
        static char *getMimeType( char *inURL );

        

    protected:


        
        /**
         * Receives data on a connection until the connection is closed.
         *
         * @param inSocketStream the stream to read from.
         *   Must be destroyed by caller.
         * @param outContentLength pointer to where the length of the content,
         *   in bytes, should be returned.
         *   Useful for binary content which cannot be reliably terminated
         *   by \0.
         *
         * @return the received data as a \0-terminated string.
         *   Must be destroyed by caller.
         */
        static char *receiveData( SocketStream *inSocketStream,
                                  int *outContentLength );



        /**
         * Executes a web method.
         *
         * @param inMethod the method to execute (for example, GET or HEAD).
         *   Must be destroyed by caller if non-const.
         * @param inURL the URL to get as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         * @param inBody the request body as a \0-terminated string, or NULL
         *   for bodyless requests (like GET or HEAD).
         *   Body must match MIME type   application/x-www-form-urlencoded
         *   Must be destroyed by caller if non-const and not NULL.
         * @param outContentLength pointer to where the length of the content,
         *   in bytes, should be returned.
         *   Useful for binary content which cannot be reliably terminated
         *   by \0.
         * @param outFinalURL pointer to where the actual
         *   URL of the content (after following redirects) should be returned,
         *   or NULL to ignore the final URL.  Defaults to NULL.
         *   The string returned in this location must be destroyed
         *   by caller.
         * @param outMimeType pointer to where the MIME type
         *   of the content should be returned,
         *   or NULL to ignore the MIME type.  Defaults to NULL.
         *   The string returned in this location must be destroyed
         *   by caller.
         * @param inTimeoutInMilliseconds the timeout value when making
         *   the connection and again when reading data in milliseconds,
         *   or -1 for no timeout.
         *   Defaults to -1.
         *
         * @return the fetched web page as a \0-terminated string,
         *   or NULL if fetching the page fails.
         *   Must be destroyed by caller if non-NULL.
         */
        static char *executeWebMethod( const char *inMethod, char *inURL,
                                       char *inBody,
                                       int *outContentLength,
                                       char **outFinalURL = NULL,
                                       char **outMimeType = NULL,
                                       long inTimeoutInMilliseconds = -1 );
        
        
    };




#endif
