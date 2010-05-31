/*
 * Modification History
 *
 * 2002-April-6   Jason Rohrer
 * Created.
 *
 * 2002-April-7   Jason Rohrer
 * Fixed a busy-waiting bug in ThreadManager.
 * Replaced use of strdup.
 */



#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketServer.h"
#include "minorGems/network/HostAddress.h"

#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"


#include <stdio.h>
#include <string.h>



/**
 * Thread that handles piping received bytes from our forwarding socket back
 * through our listening socket.
 */
class ReceiveThread : public Thread {

    public:
        ReceiveThread( Socket *inListenSocket,
                       Socket *inForwardSocket,
                       int inID );

        void run();

    private:
        char mStopped;
        Socket *mListenSocket;
        Socket *mForwardSocket;
        int mID;
        
    };



ReceiveThread::ReceiveThread( Socket *inListenSocket,
                              Socket *inForwardSocket,
                              int inID )
    : mListenSocket( inListenSocket ), mForwardSocket( inForwardSocket ),
      mID( inID ) {
    
    }



void ReceiveThread::run() {
    unsigned char receivedByte;
    int numReceived = 1;
    int numSent = 1;
    
    int numTransmitted = 0;
    
    char *fileName = new char[99];
    sprintf( fileName, "r%d.txt", mID );
    
    FILE *logFile = fopen( fileName, "w" );
    delete [] fileName;

    while( numReceived == 1 && numSent == 1) {
        
        numReceived = mForwardSocket->receive( &receivedByte, 1, -1 );
        if( numReceived == 1 ) {
            numSent = mListenSocket->send( &receivedByte, 1 );
            numTransmitted++;

            fputc( receivedByte, logFile );
            fflush( logFile );
            }
        else {
            numSent = 0;
            }                
        }

    fclose( logFile );
    
    printf( "Connection broken, %d total bytes received.\n",
            numTransmitted );
    
    }



/**
 * Class that handles piping bytes from our listening
 * socket out through the forwarding socket.
 */
class SendThread : public Thread {

    public:
        SendThread( Socket *inListenSocket,
                    char *inForwardAddress,
                    int inForwardPort,
                    int inID );

        void run();

    private:
        Socket *mListenSocket;
        char *mForwardAddress;
        int mForwardPort;
        int mID;
        

    };



SendThread::SendThread( Socket *inListenSocket,
                        char *inForwardAddress,
                        int inForwardPort,
                        int inID )
    : mListenSocket( inListenSocket ),
      mForwardAddress( inForwardAddress ), mForwardPort( inForwardPort ),
      mID( inID ) {
    
    }



void SendThread::run() {

    printf( "Establishing a connection to %s:%d\n",
            mForwardAddress,
            mForwardPort );

    HostAddress *address = new HostAddress(
        stringDuplicate( mForwardAddress ),
        mForwardPort );
    
    Socket *forwardSocket = SocketClient::connectToServer( address );
        
    if( forwardSocket != NULL ) {
        printf( "Connected to %s:%d\n",
                mForwardAddress,
                mForwardPort );

        ReceiveThread *thread =
            new ReceiveThread( mListenSocket, forwardSocket, mID );
        
        thread->start();
        
        char *fileName = new char[99];
        sprintf( fileName, "s%d.txt", mID );
        
        FILE *logFile = fopen( fileName, "w" );
        delete [] fileName;
        

        unsigned char receivedByte;
        int numReceived = 1;
        int numSent = 1;
        
        int numTransmitted = 0;
        
        while( numReceived == 1 && numSent == 1) {

            numReceived = mListenSocket->receive( &receivedByte, 1, -1 );
            if( numReceived == 1 ) {
                numSent = forwardSocket->send( &receivedByte, 1 );
                numTransmitted++;
                //printf( "Received byte = %d\n", receivedByte );
                
                fputc( receivedByte, logFile );
                fflush( logFile );
                }
            else {
                numSent = 0;
                }                
            }
        delete forwardSocket;
        
        fclose( logFile );
        
        printf( "Connection broken, %d total bytes sent.\n",
                numTransmitted );
        
        // thread should notice broken socket soon
        thread->join();
        delete thread;
            
        }
    else {
        printf( "Connecting to %s:%d failed\n",
                mForwardAddress,
                mForwardPort );
        }

    delete address;

    delete mListenSocket;
    }



/**
 * Class that handles joining and destroying spawned threads.
 */
class ThreadManager : public Thread {

    public:
        ThreadManager();
        ~ThreadManager();

        void addThread( Thread *inThread );

        void run();

    private:
        MutexLock *mLock;
        SimpleVector< Thread* > *mThreads;
    };



ThreadManager::ThreadManager()
    : mLock( new MutexLock() ),
      mThreads( new SimpleVector< Thread* >() ) {

    }


ThreadManager::~ThreadManager() {

    delete mLock;

    for( int i=0; i<mThreads->size(); i++ ) {
        Thread *thread = *( mThreads->getElement( mThreads->size() - 1 ) );
        delete thread;
        }
    delete mThreads;
    }



void ThreadManager::addThread( Thread *inThread ) {
    mLock->lock();

    mThreads->push_back( inThread );

    mLock->unlock();
    }



void ThreadManager::run() {

    while( true ) {

        Thread *thread = NULL;
        
        mLock->lock();

        if( mThreads->size() != 0 ) {
            thread = *( mThreads->getElement( mThreads->size() - 1 ) );
            mThreads->deleteElement( mThreads->size() - 1 );            
            }
        mLock->unlock();

        if( thread != NULL ) {

            thread->join();
            delete thread;
            }
        else {
            // sleep for 5 seconds if no threads availabel to join
            sleep( 5000 );
            }
        }
    
    }



// prototype
void usage( char *inAppName );



int main( char inNumArgs, char **inArgs ) {

    if( inNumArgs != 4 ) {
        usage( inArgs[0] );
        }

    int listenPort;
    int forwardPort;
    int numRead;

    numRead = sscanf( inArgs[1], "%d", &listenPort );
    if( numRead != 1 ) {
        usage( inArgs[0] );
        }

    char *forwardAddress = inArgs[2];
    
    numRead = sscanf( inArgs[3], "%d", &forwardPort );
    if( numRead != 1 ) {
        usage( inArgs[0] );
        }

    
    ThreadManager *manager = new ThreadManager();
    manager->start();
    
    SocketServer *server = new SocketServer( listenPort, 1 );

    int nextID = 1;
    
    while( true ) {
        printf( "Waiting for connection on port %d\n", listenPort );
        Socket *listenSocket = server->acceptConnection();

        if( listenSocket != NULL ) {
            printf( "Connection received.\n" );
            
            SendThread *thread = new SendThread( listenSocket,
                                                 forwardAddress,
                                                 forwardPort,
                                                 nextID );
            nextID++;

            thread->start();

            manager->addThread( thread );
            }
        else {
            printf( "Accepting connection failed.\n" );
            }
        }

    delete server;
    delete manager;
    }



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s listent_port forward_address forward_port\n", inAppName );

	printf( "Example:\n" );
	printf( "\t%s 5888 ftp.domain.com 21\n", inAppName );
	
	exit( 1 );
	}
