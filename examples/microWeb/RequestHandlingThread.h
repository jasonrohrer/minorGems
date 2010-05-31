/*
 * Modification History
 *
 * 2001-May-11   Jason Rohrer
 * Created.  
 */
 
 
#ifndef REQUEST_HANDLING_THREAD_INCLUDED
#define REQUEST_HANDLING_THREAD_INCLUDED 

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#define REQUEST_HANDLING_THREAD_BUFFER_SIZE 4096

/**
 * Request handler for the microWeb server.
 *
 * @author Jason Rohrer.
 */
class RequestHandlingThread : public Thread {

	public:

		/**
		 * Construct a handler.
		 *
		 * @param inSocket the socket to send the requested
		 *   file trough.  Is destroyed before this thread terminates.
		 * @param inMimeString the mime type to send with a file.
		 *   Is not destroyed by this class.
		 * @param inRootPath the root file path to look in for
		 *   requested files.  Is not destroyed by this class.
		 */
		RequestHandlingThread( Socket *inSocket, char *inMimeString,
							   char *inRootPath );

		~RequestHandlingThread();

		
		/**
		 * Returns true if this handler is done and ready to be destroyed.
		 *
		 * Synchronized, so may block.
		 *
		 * @return true if this handler is done and ready to be destroyed.
		 */
		char isDone();

		
		/**
		 * Gets a string representation of the current time.
		 *
		 * @return a timestamp string.  Must be destroyed by caller.
		 */
		static char* getTimestamp();
		
		
		// implements the Thread interface
		virtual void run();
		
	private:
		Socket *mSocket;

		MutexLock *mDoneLock;
		char mDone;

		char *mMimeString;
		
		char *mRootPathString;

		/**
		 * Sends an HTTP "not found" message with a "not found" web page.
		 *
		 * @param inStream the stream to send the not found page to.
		 * @param inFileName the name of the requested file,
		 *   or NULL.
		 */
		void sendNotFoundPage( SocketStream *inStream,
							   char *inFileName );

		
		/**
		 * Sends a "bad request" web page.
		 *
		 * @param inStream the stream to send the page to.
		 */
		void sendBadRequest( SocketStream *inStream );

		
	};



inline RequestHandlingThread::RequestHandlingThread(
	Socket *inSocket, char *inMimeString,
	char *inRootPath )
	: mSocket( inSocket ),
	  mMimeString( inMimeString  ), mRootPathString( inRootPath ),
	  mDoneLock( new MutexLock() ), mDone( false ) {

	}



inline RequestHandlingThread::~RequestHandlingThread() {
	delete mDoneLock;
	}



inline char RequestHandlingThread::isDone() {
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


inline void RequestHandlingThread::run() {

	// first, receive the request and parse it
	

	SocketStream *sockStream = new SocketStream( mSocket );

	int requestBufferLength = 200;
	char *requestBuffer = new char[requestBufferLength];
	int requestBufferIndex = 0;
		
	// read until we see two \r\n 's in a row
	unsigned char *charRead = new unsigned char[1];
	charRead[0] = 0;
	char requestDone = false;

	// _we_ actually only care about the first line of
	// the request, but we need to read the entire request
	// to make the other host happy
	char firstLineDone = false;

	while( !requestDone ) {
		while( charRead[0] != 13 ) {
			sockStream->read( charRead, 1 );
			if( !firstLineDone ) {
				// read data into our buffer
				if( requestBufferIndex < requestBufferLength ) {
					requestBuffer[ requestBufferIndex ] =
						charRead[0];
					requestBufferIndex++;
					}
				}
			}
		// look for rest of double \r\n
		sockStream->read( charRead, 1 );
		if( charRead[0] == 10 ) {
			sockStream->read( charRead, 1 );
			if( charRead[0] == 13 ) {
				sockStream->read( charRead, 1 );
				if( charRead[0] == 10 ) {
					requestDone = true;
					}
				}
			}
		}

	// at this point, we have received the entire
	// request, and stored the most important part in
	// requestBuffer

		// \0 terminate the request buffer
	requestBuffer[ requestBufferLength - 1 ] = '\0';

	char error = false;
		
		// the second string scanned from the buffer should
		// be the file path requested
		
	char *filePathBuffer = new char[200];
	int numRead = sscanf( requestBuffer, "%s", filePathBuffer );

	if( numRead != 1 || strcmp( filePathBuffer, "GET" ) ) {
		// an invalid request
		error = true;
		sendBadRequest( sockStream );
		}
	else {
		// a proper GET request

		// skip the GET and read the file name
		numRead = sscanf( &( requestBuffer[3] ),
						  "%s", filePathBuffer );
		
		if( numRead != 1 ) {
			error = true;
			sendBadRequest( sockStream );
			}
		else {
			// make sure file path doesn't escape
			// from the subdirectory
			if( strstr( filePathBuffer, ".." ) != NULL ) {
				error = true;
				sendNotFoundPage( sockStream, filePathBuffer );
				}
			}
		}
		
	delete [] requestBuffer;
	delete [] charRead;

	File *file;
	
	if( !error ) {
		char *fileName = new char[400];
		sprintf( fileName, "%s%s", mRootPathString, filePathBuffer );

		file = new File( NULL, fileName, strlen( fileName ) );
				
		if( file->exists() ) {
			// print a log message to stdio
			char *timestamp = getTimestamp();
			printf( "%s:  serving file:  %s\n",
					timestamp,
					fileName );
			delete [] timestamp;
			
			
			char *responseBuffer = new char[400];
			sprintf( responseBuffer,
					 "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n",
					 mMimeString );
			sockStream->write( (unsigned char *)responseBuffer,
							   strlen( responseBuffer ) );

			delete responseBuffer;

			// now we have sent the response, so the client
			// is waiting for the data
			}
		else {
			sendNotFoundPage( sockStream, filePathBuffer );
			error = true;
			delete file;
			}

		delete [] fileName;
		}

		
	delete [] filePathBuffer;
   


	// now send the requested file
	if( !error ) {
		
		// open our streams
		FileInputStream *fileStream = new FileInputStream( file );

		int fileLength = file->getLength();

		int bufferSize = REQUEST_HANDLING_THREAD_BUFFER_SIZE;
		unsigned char *buffer = new unsigned char[ bufferSize ];


		int bytesRemaining = fileLength;
	
		while( bytesRemaining > 0 && !error ) {

			// deal with the last partial buffer in the same loop
			int bytesToSend;
			if( bytesRemaining > bufferSize ) {
				bytesToSend = bufferSize;
				}
			else {
				bytesToSend = bytesRemaining;
				}

		
			int bytesRead = fileStream->read( buffer, bytesToSend );

			if( bytesRead == bytesToSend ) {
				int bytesWritten = sockStream->write( buffer, bytesToSend );
			
				if( bytesWritten != bytesToSend ) {
					error = true;
					}
				else {
					bytesRemaining -= bytesToSend;
					}
				}
			else {
				error = true;
				}
			}

		if( error ) {
			// there was an error... print for now
			printf( "error while sending file\n" );
			}

		delete fileStream;
		delete file;

		delete [] buffer;
		}
	
	delete sockStream;
	delete mSocket;

	
	// flag that we're done
	mDoneLock->lock();
	mDone = true;
	mDoneLock->unlock();
	}



inline void RequestHandlingThread::sendNotFoundPage(
	SocketStream *inStream,
	char *inFileName ) {

	// example "not found" response
	/*
	  HTTP/1.0 404 Not Found
	  Date: Fri, 11 May 2001 18:26:16 GMT
	  Server: GWS/1.10
/	  Connection: close
	  Set-Cookie: PREF=ID=3300d4623bf73a57:TM=989605576:LM=989605576; domain=.google.com; path=/; expires=Sun, 17-Jan-2038 19:14:07 GMT
	  Content-Length: 142
	  Content-Type: text/html

	  <HTML><HEAD><TITLE>Not Found</TITLE></HEAD>
	  <BODY><H1>404 Not Found</H1>
	  The requested URL /fjfj was not found on this server.
	  </BODY></HTML>
	*/
	char *buffer = new char[500];

	if( inFileName != NULL ) {
		sprintf( buffer, "HTTP/1.0 404 Not Found\r\n\r\n<HTML><BODY><H1>404 Not Found</H1>The requested file <b>%s</b> was not found</BODY></HTML>\r\n",
				 inFileName );
		}
	else {
		sprintf( buffer, "HTTP/1.0 404 Not Found\r\n\r\n<HTML><BODY><H1>404 Not Found</H1>The requested file was not found</BODY></HTML>\r\n" );
		}

	inStream->write( (unsigned char *)buffer, strlen( buffer ) );

	delete [] buffer;
	}



inline void RequestHandlingThread::sendBadRequest(
	SocketStream *inStream ) {

	// exampl "bad request" response
	/*
	  <HTML><HEAD><TITLE>Bad Request</TITLE></HEAD>
	  <BODY><H1>400 Bad Request</H1>
	  Your client has issued a malformed or illegal request.
	  </BODY></HTML>
	*/

	char *buffer = new char[500];

	sprintf( buffer, "<HTML><BODY><H1>400 Bad Request</H1>Your client has issued a malformed or illegal request.</BODY></HTML>\r\n" );

	inStream->write( (unsigned char *)buffer, strlen( buffer ) );

	delete [] buffer;
	}



inline char* RequestHandlingThread::getTimestamp() {
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



#endif
