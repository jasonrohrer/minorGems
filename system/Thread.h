/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2001-March-4		Jason Rohrer
 * Made sleep() static so it can be called by non-Thread classes.
 *
 * 2001-May-12   Jason Rohrer
 * Added comments about joining before destroying.
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-August-5    Jason Rohrer
 * Made destructor virtual.
 *
 * 2004-March-31   Jason Rohrer
 * Added support for detatched mode.
 *
 * 2005-January-9   Jason Rohrer
 * Made sleep function virtual to allow overrides.
 *
 * 2005-January-22   Jason Rohrer
 * Added a static sleep function.
 */

#include "minorGems/common.h"



#ifndef THREAD_CLASS_INCLUDED
#define THREAD_CLASS_INCLUDED



#ifdef FORTIFY
#include "minorGems/util/development/fortify/fortify.h"
#endif



/**
 * Base class to be subclassed by all threads.
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the mac/ linux/ and win32/ 
 *   subdirectories).
 *
 * @author Jason Rohrer
 */ 
class Thread {

	public:
	
		Thread();		
		virtual ~Thread();
	
		
		/**
		 * Starts this Thread.
		 *
		 * Note that after starting a non-detached thread, it _must_ be
         * joined before being destroyed to avoid memory leaks.
         *
         * Threads running in detatched mode handle their own destruction
         * as they terminate and do not need to be joined at all.
         *
         * @param inDetach true if this thread should run in detatched mode,
         *   or false to run in non-detached mode.  Defaults to false.    
		 */
		void start( char inDetach = false );
		
		
		/**
		 * To be overriden by subclasses.
		 * This method will be run by the Thread after start() has been called.
		 */
		virtual void run() = 0;
		
		
		/**
		 * Blocks until this thread finishes executing its run() method.
		 *
		 * Must be called before destroying this thread, if this thread
		 * has been started.
		 */
		void join();

		
		/**
		 * Puts the current thread to sleep for a specified amount of time.
		 *
		 * Note that given a thread instance threadA, calling threadA.sleep()
		 * will put the calling thread to sleep.
		 *
		 * @param inTimeInMilliseconds the number of milliseconds to sleep.
		 */
		virtual void sleep( unsigned long inTimeInMilliseconds ) {
            staticSleep( inTimeInMilliseconds );
            }


        
        /**
         * Same as sleep, but can be called without constructing a thread.
         */
        static void staticSleep( unsigned long inTimeInMilliseconds );

        

        /**
         * Gets whether this thread is detached.
         *
         * @return true if this thread is detached.
         */
        char isDetatched() {
            return mIsDetached;
            }


        
	private:
		
		/**
		 * Used by platform-specific implementations.
		 */		
		void *mNativeObjectPointer;


        char mIsDetached;
		
	};		
	
#endif
