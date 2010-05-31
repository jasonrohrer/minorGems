/*
 * Modification History
 *
 * 2004-November-9    Jason Rohrer
 * Created.
 * Modified from MUTE's ChannelReceivingThreadManager.
 *
 * 2005-January-9    Jason Rohrer
 * Changed to sleep on a semaphore to allow sleep to be interrupted.
 */



#ifndef FINISHED_SIGNAL_THREAD_MANAGER_INCLUDED
#define FINISHED_SIGNAL_THREAD_MANAGER_INCLUDED



#include "minorGems/system/FinishedSignalThread.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"
#include "minorGems/system/BinarySemaphore.h"



/**
 * A thread that manages the destruction of FinishedSignalThreads.
 *
 * @author Jason Rohrer.
 */
class FinishedSignalThreadManager : public Thread {



    public:
        
        /**
         * Constructs and starts this manager.
         */
        FinishedSignalThreadManager();


        
        /**
         * Stops and destroys this manager.
         */
        ~FinishedSignalThreadManager();

        

        /**
         * Adds a thread to this manager.
         *
         * @param inThread the thread to add.
         *   Will be destroyed by this class.
         */
        void addThread( FinishedSignalThread *inThread );

        

        // implements the Thread interface
        void run();


        
    protected:
        MutexLock *mLock;

        SimpleVector<FinishedSignalThread *> *mThreadVector;
        
        char mStopSignal;

        BinarySemaphore *mSleepSemaphore;
        
    };




#endif
