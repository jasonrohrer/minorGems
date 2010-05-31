/*
 * Modification History
 *
 * 2008-October-21   Jason Rohrer
 * Created.
 *
 * 2009-February-3   Jason Rohrer
 * Changed to subclass FinishedSignalThread.
 *
 * 2009-February-14   Jason Rohrer
 * Changed to copy inAddress internally.
 */



#ifndef LOOKUP_THREAD_CLASS_INCLUDED
#define LOOKUP_THREAD_CLASS_INCLUDED

#include "minorGems/system/FinishedSignalThread.h"
#include "minorGems/system/MutexLock.h"

#include "minorGems/network/HostAddress.h"



/**
 * Thread that performs DNS lookup on a host name.
 *
 * @author Jason Rohrer
 */
class LookupThread : public FinishedSignalThread {
	
	public:
		/**
         * Constructs and starts a lookup thread.
         *
		 * @param inAddress the address to lookup.  Destroyed by caller,
         *   copied internally.
		 */
		LookupThread( HostAddress *inAddress );
        
        
        // joins and destroys this thread
        ~LookupThread();
        
		

        /**
         * Returns true if lookup done.
         */
        char isLookupDone();
        


        /**
         * Returns numerical address result, or NULL if lookup failed.
         *
         * Must be destroyed by caller if non-NULL.
         */
        HostAddress *getResult();

        

		// override the run method from Thread
		void run();
	
	private:
        MutexLock mLock;
        
        HostAddress *mAddress;
        
        HostAddress *mNumericalAddress;
        
        char mLookupDone;
                
        
        
	};

	
#endif
