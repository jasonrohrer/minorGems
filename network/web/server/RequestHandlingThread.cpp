/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-April-20   Jason Rohrer
 * Fixed many violations of spec and included support for mime types.
 * Fixed a few bugs.
 *
 * 2002-April-21   Jason Rohrer
 * Fixed a memory leak.
 *
 * 2002-August-2   Jason Rohrer
 * Added use of ConnectionPermissionHandler.
 *
 * 2002-August-5   Jason Rohrer
 * Added another error message.
 * Changed so PageGenerator is responsible for detecting .. parent 
 * directory symbols.
 *
 * 2002-August-7   Jason Rohrer
 * Fixed possible buffer overflows.
 *
 * 2002-September-13   Jason Rohrer
 * Increased buffer size to 5000 bytes
 * (to accommodate long broadcast comments).
 *
 * 2002-October-20   Jason Rohrer
 * Fixed a memory leak of requestBuffer on error.
 *
 * 2003-April-2   Jason Rohrer
 * Changed to specify that the HTTP connection should not be kept alive.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */



#include "RequestHandlingThread.h"




RequestHandlingThread::RequestHandlingThread(
    Socket *inSocket, PageGenerator *inGenerator,
    ConnectionPermissionHandler *inConnectionPermissionHandler )
    : mSocket( inSocket ),
      mGenerator( inGenerator ),
      mConnectionPermissionHandler( inConnectionPermissionHandler ),
      mDoneLock( new MutexLock() ), mDone( false ) {

    }



RequestHandlingThread::~RequestHandlingThread() {
    delete mDoneLock;
    }



char RequestHandlingThread::isDone() {
    char tempDone;

    mDoneLock->lock();
    tempDone = mDone;
    mDoneLock->unlock();

    return mDone;
    }



    // example HTTP request and response
    /*
      GET /images/title_homepage4.gif HTTP/1.0

      HTTP/1.0 200 OK
      Date: Fri, 11 May 2001 18:05:08 GMT
      Server: GWS/1.10
      Connection: close
      Expires: Sun, 17 Jan 2038 19:14:07 GMT
      Content-Length: 7963
      Content-Type: image/gif
      Last-Modified: Tue, 21 Nov 2000 16:20:07 GMT
      
      GIF89a1s
    */


void RequestHandlingThread::run() {

    HostAddress *receivedAddress = mSocket->getRemoteHostAddress();

    if( receivedAddress == NULL ) {
        
        printf( "Failed to obtain host address, so "
                "refusing web connection.\n" );
        
        // refuse
        delete mSocket;

        
        // flag that we're done
        mDoneLock->lock();
        mDone = true;
        mDoneLock->unlock();

        return;
        }
    else if(
        ! mConnectionPermissionHandler->isPermitted( receivedAddress ) ) { 

        printf( "Refusing web connection from:  " );
        receivedAddress->print();
        printf( "\n" );

        
        // not permitted
        delete mSocket;

        delete receivedAddress;
        
        // flag that we're done
        mDoneLock->lock();
        mDone = true;
        mDoneLock->unlock();

        return;
        }
    
    // else permitted
    delete receivedAddress;
    

    int maxLength = 5000;
    
    
    // first, receive the request and parse it
    

    SocketStream *sockStream = new SocketStream( mSocket );

    int requestBufferLength = maxLength;
    char *requestBuffer = new char[requestBufferLength];
    int requestBufferIndex = 0;
        
    // read until we see two \r\n 's in a row
    unsigned char *charRead = new unsigned char[1];
    charRead[0] = 0;
    char requestDone = false;

    char error = false;
    
    // _we_ actually only care about the first line of
    // the request, but we need to read the entire request
    // to make the other host happy
    char firstLineDone = false;
    int numRead = 0;

    while( !requestDone && !error ) {
        while( charRead[0] != 13 && !error ) {
            numRead = sockStream->read( charRead, 1 );
                            
            if( !firstLineDone ) {
                if( numRead != 1 ) {
                    error = true;
                    sendBadRequest( sockStream );
                    }
                // read data into our buffer
                else if( requestBufferIndex < requestBufferLength ) {
                    requestBuffer[ requestBufferIndex ] =
                        charRead[0];
                    requestBufferIndex++;

                    if( charRead[0] == 13 ) {
                        firstLineDone = true;
                        }
                    }
                else {
                    error = true;
                    sendBadRequest( sockStream );
                    }
                }
            }
        if( !error ) {
            // look for rest of double \r\n
            // this will effectively skip other lines in the request,
            // since we don't care about them
            numRead = sockStream->read( charRead, 1 );

            if( charRead[0] == 10 && numRead == 1 ) {

                numRead = sockStream->read( charRead, 1 );
                if( charRead[0] == 13 && numRead == 1  ) {

                    numRead = sockStream->read( charRead, 1 );
                    if( charRead[0] == 10 && numRead == 1 ) {
                        requestDone = true;
                        }
                    }
                }

            if( numRead != 1 ) {
                error = true;
                sendBadRequest( sockStream );
                }
            }
        }

    // \0 terminate the request buffer
    if( requestBufferIndex < requestBufferLength ) {
        requestBuffer[ requestBufferIndex ] = '\0';
        }
    else {
        requestBuffer[ requestBufferLength - 1 ] = '\0';
        }
    
    if( !error ) {
        // at this point, we have received the entire
        // request, and stored the most important part in
        // requestBuffer


        // if maxLength = 500,
        // formatString = "%499s"
        // used to limit length of scanned string 
        char *formatString = new char[ 20 ];
        sprintf( formatString, "%%%ds", maxLength - 1 );
        
        
        // the second string scanned from the buffer should
        // be the file path requested
        
        char *filePathBuffer = new char[ maxLength ];
        int numRead = sscanf( requestBuffer, formatString, filePathBuffer );

        if( numRead != 1 || strcmp( filePathBuffer, "GET" ) != 0 ) {
            // an invalid request
            error = true;
            sendBadRequest( sockStream );
            }
        else {
            // a proper GET request

            // skip the GET and read the file name
            numRead = sscanf( &( requestBuffer[3] ),
                              formatString, filePathBuffer );
        
            if( numRead != 1 ) {
                error = true;
                sendBadRequest( sockStream );
                }
            }
        
        delete [] requestBuffer;
        delete [] charRead;


        delete [] formatString;
        
        if( !error ) {
            // now we have the requested file string
            sockStream->writeString(
                "HTTP/1.0 200 OK\r\n" );

            char *mimeType = mGenerator->getMimeType( filePathBuffer );

            sockStream->writeString( "Content-Type: " );
            sockStream->writeString( mimeType );
            sockStream->writeString( "\r\n" );
            
            delete [] mimeType;

            // even if the client requests a keep-alive, we force a close
            sockStream->writeString( "Connection: close" );
            
            // finish header
            sockStream->writeString( "\r\n\r\n" );
            
            // pass it to our page generator, which will send the content
            mGenerator->generatePage( filePathBuffer, sockStream );
            }

        delete [] filePathBuffer;  
        }
    else {
        delete [] requestBuffer;
        delete [] charRead;
        }

    
    delete sockStream;
    delete mSocket;

    
    // flag that we're done
    mDoneLock->lock();
    mDone = true;
    mDoneLock->unlock();
    }



void RequestHandlingThread::sendNotFoundPage(
    SocketStream *inStream,
    char *inFileName ) {

    // example "not found" response
    /*
      HTTP/1.0 404 Not Found
      Date: Fri, 11 May 2001 18:26:16 GMT
      Server: GWS/1.10
      Connection: close
      Set-Cookie: PREF=ID=3300d4623bf73a57:TM=989605576:LM=989605576;
          domain=.google.com; path=/; expires=Sun, 17-Jan-2038 19:14:07 GMT
      Content-Length: 142
      Content-Type: text/html

      <HTML><HEAD><TITLE>Not Found</TITLE></HEAD>
      <BODY><H1>404 Not Found</H1>
      The requested URL /fjfj was not found on this server.
      </BODY></HTML>
    */
    char *buffer = new char[500];

    if( inFileName != NULL ) {
        sprintf( buffer,
                 "HTTP/1.0 404 Not Found\r\n\r\n<HTML>"
                 "<BODY><H1>404 Not Found</H1>The requested file "
                 "<b>%s</b> was not found</BODY></HTML>\r\n",
                 inFileName );
        }
    else {
        sprintf( buffer,
                 "HTTP/1.0 404 Not Found\r\n\r\n<HTML>"
                 "<BODY><H1>404 Not Found</H1>The requested "
                 "file was not found</BODY></HTML>\r\n" );
        }

    inStream->write( (unsigned char *)buffer, strlen( buffer ) );

    delete [] buffer;
    }



void RequestHandlingThread::sendBadRequest(
    SocketStream *inStream ) {

    // exampl "bad request" response
    /*
      <HTML><HEAD><TITLE>Bad Request</TITLE></HEAD>
      <BODY><H1>400 Bad Request</H1>
      Your client has issued a malformed or illegal request.
      </BODY></HTML>
    */

    char *buffer = new char[500];

    sprintf( buffer,
             "<HTML><BODY><H1>400 Bad Request</H1>"
             "Your client has issued a malformed or illegal request."
             "</BODY></HTML>\r\n" );

    inStream->write( (unsigned char *)buffer, strlen( buffer ) );

    delete [] buffer;
    }



char* RequestHandlingThread::getTimestamp() {
    char *stampBuffer = new char[99];

    time_t t = time( NULL );
    
    char *asciiTime = ctime( &t );

    // this time string ends with a newline...
    // get rid of it
    asciiTime[ strlen(asciiTime) - 1 ] = '\0';
    
    
    sprintf( stampBuffer, "[%s]", asciiTime );

    // delete [] asciiTime;
    
    return stampBuffer;
    }


