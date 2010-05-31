/*
 * Modification History
 *
 * 2003-August-7   Jason Rohrer
 * Created.
 *
 * 2003-August-14   Jason Rohrer
 * Changed to output message history to file.
 */



#include "DuplicateMessageDetector.h"

#include "minorGems/util/stringUtils.h"

#include <time.h>
#include <stdio.h>



DuplicateMessageDetector::DuplicateMessageDetector( int inMessageHistorySize )
    : mMaxHistorySize( inMessageHistorySize ),
      mLock( new MutexLock() ),
      mSeenIDs( new SimpleVector<char *>() ),
      mTotalMessageCount( 0 ),
      mHistoryOutputFile( NULL ) {

    mHistoryOutputFile = fopen( "messageHistory.log", "w" );
    }



DuplicateMessageDetector::~DuplicateMessageDetector() {
    int numIDs = mSeenIDs->size();

    for( int i=0; i<numIDs; i++ ) {
        delete [] *( mSeenIDs->getElement( i ) );
        }
    delete mSeenIDs;

    delete mLock;

    fclose( mHistoryOutputFile );
    }



char DuplicateMessageDetector::checkIfMessageSeen( char *inMessageUniqueID ) {
    mLock->lock();

    mTotalMessageCount++;
    
    int numIDs = mSeenIDs->size();

    char matchSeen = false;
    for( int i=0; i<numIDs && !matchSeen; i++ ) {

        char *otherID = *( mSeenIDs->getElement( i ) ); 

        if( strcmp( otherID, inMessageUniqueID ) == 0 ) {
            // match

            // push the ID back to the end of the queue
            mSeenIDs->deleteElement( i );
            mSeenIDs->push_back( otherID );
            matchSeen = true;
            }
        }

    if( mHistoryOutputFile != NULL ) {
        fprintf( mHistoryOutputFile,
                 "%d %d %s",
                 mTotalMessageCount,
                 (int)( time( NULL ) ),
                 inMessageUniqueID );
        }
    
    
    if( !matchSeen ) {

        // add the message
        mSeenIDs->push_back( stringDuplicate( inMessageUniqueID ) );

        // make sure history not too long
        if( mSeenIDs->size() > mMaxHistorySize ) {
            delete [] *( mSeenIDs->getElement( 0 ) );
            mSeenIDs->deleteElement( 0 );
            }

        if( mHistoryOutputFile != NULL ) {
            fprintf( mHistoryOutputFile, "\n" );
            }
        }
    else {
        // add duplicate tag
        if( mHistoryOutputFile != NULL ) {
            fprintf( mHistoryOutputFile, " D\n" );
            }
        }

    if( mHistoryOutputFile != NULL ) {
        fflush( mHistoryOutputFile );
        }
    
    mLock->unlock();    
    return matchSeen;    
    }


