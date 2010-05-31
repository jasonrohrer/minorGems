/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-April-5    Jason Rohrer
 * Changed to extend StopSignalThread.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */


 
#ifndef THREAD_HANDLING_THREAD_INCLUDED
#define THREAD_HANDLING_THREAD_INCLUDED 

#include "RequestHandlingThread.h"

#include "minorGems/system/StopSignalThread.h"


#include "minorGems/system/MutexLock.h"

#include "minorGems/util/SimpleVector.h"


#include <string.h>
#include <stdio.h>


/**
 * Thread handler for the microWeb server.  Runs periodically
 * and checks for finished threads that can be deleted.
 *
 * @author Jason Rohrer.
 */
class ThreadHandlingThread : public StopSignalThread {


        
    public:



        /**
         * Constructs and starts a handler.
         */
        ThreadHandlingThread();



        /**
         * Stops and destroys this handler.
         */
        ~ThreadHandlingThread();


        
        /**
         * Adds a thread to the set managed by this handler.
         *
         * @param inThread the thread to add.
         */
        void addThread( RequestHandlingThread *inThread );

        
        
        // implements the Thread interface
        virtual void run();


        
    private:

        SimpleVector<RequestHandlingThread*> *mThreadVector;
        
        MutexLock *mLock;


    };



#endif
