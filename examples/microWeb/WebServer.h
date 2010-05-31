/*
 * Modification History
 *
 * 2001-May-11   Jason Rohrer
 * Created.  
 */
 
 
#ifndef WEB_SERVER_INCLUDED
#define WEB_SERVER_INCLUDED 

#include "minorGems/io/file/Path.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketServer.h"
#include "minorGems/network/SocketStream.h"

#include "RequestHandlingThread.h"
#include "ThreadHandlingThread.h"

#include <string.h>
#include <stdio.h>


/**
 * Main class for the microWeb web server.
 *
 * @author Jason Rohrer.
 */
class WebServer {



	public:

		/**
		 * Constructs a server, specifying a configuration file.
		 *
		 * @param inConfigurationaFile the file to read configuration
		 *   information from.  Must be destroyed by caller.
		 */
		WebServer( File *inConfigurationFile );

		~WebServer();

		/**
		 * Starts this server running and never returns.
		 */
		void runServer();

		
	private:
		int mPortNumber;
		int mMaxQueuedConnections;

		char *mMimeString;

		char *mRootPathString;

		SocketServer *mServer;
		ThreadHandlingThread *mThreadHandler;

		
		/**
		 * Reads a class configuration value from a file.
		 *
		 * @param inFile the file to read the value from.
		 * @param inKey the key marking the value in the file.
		 *   Must be destroyed by caller.
		 */
		void readConfigurationValue( FILE *inFile, char *inKey );

	};



inline WebServer::WebServer( File *inConfigurationFile )
	: mPortNumber( -1 ), mMaxQueuedConnections( -1 ),
	  mMimeString( NULL ), mRootPathString( NULL ),
	  mThreadHandler( new ThreadHandlingThread() ) {

	
	
	if( inConfigurationFile->exists() ) {
		int fileNameLength;
		char *fileName =
			inConfigurationFile->getFullFileName( &fileNameLength );

		FILE *file = fopen( fileName, "r" );

		
		int numRead = 1;
		char *inKeyBuffer = new char[99];

		// read each key from the file, and parse its value
		while( numRead == 1 ) {
			numRead = fscanf( file, "%s", inKeyBuffer );

			if( numRead == 1 ) {
				readConfigurationValue( file, inKeyBuffer );
				}
			}

		// last key read failed, so done with file

		fclose( file );
		delete [] inKeyBuffer;
		}

	
	// check for uninitialized configuration values

	if( mPortNumber == -1 ) {
		mPortNumber = 80;
		printf( "using default:  port_number = %d\n", mPortNumber );
		}
	if( mMaxQueuedConnections == -1 ) {
		mMaxQueuedConnections = 100;
		printf( "using default:  max_queued_connections = %d\n",
				mMaxQueuedConnections );
		}
	if( mMimeString == NULL ) {
		mMimeString = new char[99];
		sprintf( mMimeString, "audio/mpeg" );

		printf( "using default:  mime_type = %s\n", mMimeString );
		}
	if( mRootPathString == NULL ) {
		mRootPathString = new char[99];
		sprintf( mRootPathString, "httpRoot" );

		printf( "using default:  root_path = %s\n", mRootPathString );
		}

	mServer = new SocketServer( mPortNumber, mMaxQueuedConnections );
	}



inline WebServer::~WebServer() {
	delete [] mMimeString;
	delete [] mRootPathString;

	delete mServer;
	delete mThreadHandler;
	}


class dummyThread : public Thread {

	public:
		virtual void run();

	};

inline void dummyThread::run() {

	}


inline void WebServer::runServer() {

	mThreadHandler->start();

	// print a log message to stdio
	char *timestamp = RequestHandlingThread::getTimestamp();
	printf( "%s:  listening for connections on port %d\n",
			timestamp, mPortNumber );
	delete [] timestamp;

	
	// main server loop
	while( true ) {
		// printf( "waiting for a connection\n" );
		Socket *sock = mServer->acceptConnection();
		// printf( "received a connection\n" );

		RequestHandlingThread *thread =
					new RequestHandlingThread( sock, mMimeString,
											   mRootPathString );
		thread->start();

		mThreadHandler->addThread( thread );
		}
	
	}



inline void WebServer::readConfigurationValue(
	FILE *inFile, char *inKey ) {

	int numRead = 0;
	if( !strcmp( inKey, "port_number" ) ) {

		numRead = fscanf( inFile, "%d", &mPortNumber );
		}
	else if( !strcmp( inKey, "max_queued_connections" ) ) {

		numRead = fscanf( inFile, "%d", &mMaxQueuedConnections );
		}
	else if( !strcmp( inKey, "mime_type" ) ) {

		mMimeString = new char[99];
		numRead = fscanf( inFile, "%s", mMimeString );
		}
	else if( !strcmp( inKey, "root_path" ) ) {

		mRootPathString = new char[99];
		numRead = fscanf( inFile, "%s", mRootPathString );
		}
	else {
		printf( "unknown key in configuration file:\n" );
		printf( "%s\n", inKey );
		return;
		}

	// check for value read failure
	if( numRead != 1 ) {
		printf( "failed to read key value from configuration file:\n" );
		printf( "%s\n", inKey );
		}
	}




#endif
