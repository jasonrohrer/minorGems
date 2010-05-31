/*
 * Modification History
 *
 * 2002-March-9    Jason Rohrer
 * Created.
 *
 * 2002-March-10    Jason Rohrer
 * Made destructor public.
 *
 * 2002-March-11    Jason Rohrer
 * Changed so that destructor joins thread.
 *
 * 2002-April-4    Jason Rohrer
 * Changed name of lock to avoid confusion with subclass-provided locks.
 *
 * 2004-April-1    Jason Rohrer
 * Moved from konspire2b into minorGems.
 * Changed so that destructor does not join the thread.
 *
 * 2004-November-19   Jason Rohrer
 * Changed to virtual inheritance from Thread class.
 */



#ifndef FINISHED_SIGNAL_THREAD_INCLUDED
#define FINISHED_SIGNAL_THREAD_INCLUDED



#include "minorGems/system/Thread.h"
#include "minorGems/system/MutexLock.h"



/**
 * Abstract subclass if thread that has a
 * synchronized finished signal.
 *
 * @author Jason Rohrer
 */
class FinishedSignalThread : public virtual Thread {



    public:


        /**
         * Only destroys this thread.
         * Does not join.
         */
        virtual ~FinishedSignalThread();


        
        /**
         * Gets whether this thread is finished and
         * ready to be destroyed.
         *
         * @return true iff this thread is finished.
         */
        char isFinished();


        


    protected:


        
        FinishedSignalThread();

        
        
        /**
         * Sets that this thread is finished and
         * ready to be destroyed.
         *
         * For this class to work properly, the subclass
         * MUST call this function at the end of its run method.
         */
        void setFinished();

        

    private:

        MutexLock *mFinishedLock;

        char mFinished;

        
        
    };



#endif
