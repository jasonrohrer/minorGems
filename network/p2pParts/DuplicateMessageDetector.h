/*
 * Modification History
 *
 * 2003-August-7   Jason Rohrer
 * Created.
 *
 * 2003-August-14   Jason Rohrer
 * Changed to output message history to file.
 */



#ifndef DUPLICATE_MESSAGE_DETECTOR_INCLUDED_H
#define DUPLICATE_MESSAGE_DETECTOR_INCLUDED_H



#include "minorGems/system/MutexLock.h"
#include "minorGems/util/SimpleVector.h"



#include <stdio.h>



/**
 * Class that detects duplicates of past messages so that they can be
 * discarded.
 *
 * @author Jason Rohrer
 */
class DuplicateMessageDetector {


        
    public:

        
        /**
         * Constructs a detector.
         *
         * @param inMessageHistorySize the number of message IDs to
         *   maintain in our history.
         *   Defaults to 1000.
         */        
        DuplicateMessageDetector( int inMessageHistorySize = 1000 );

        ~DuplicateMessageDetector();

        

        /**
         * Checks if a message has been seen in the past.
         *
         * @param inMessageUniqueID the unique ID for the message.
         *   Must be destroyed by caller.
         *
         * @return true if the message has already been seen, or
         *   false if the message is new.
         */
        char checkIfMessageSeen( char *inMessageUniqueID );
        

        
    protected:

        int mMaxHistorySize;
        MutexLock *mLock;
        SimpleVector<char *> *mSeenIDs;
        

        int mTotalMessageCount;
        FILE *mHistoryOutputFile;
        
    };



#endif
