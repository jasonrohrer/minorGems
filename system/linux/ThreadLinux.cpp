/*
 * Modification History
 *
 * 2000-December-13		Jason Rohrer
 * Created.
 *
 * 2001-January-11		Jason Rohrer
 * Added missing sleep() implementation. 
 *
 * 2002-March-27   Jason Rohrer
 * Added support for gprof-friendly thread wrappers.
 * Fixed a compile bug when gprof threads disabled.
 *
 * 2002-August-5   Jason Rohrer
 * Removed an unused variable.
 *
 * 2003-February-3   Jason Rohrer
 * Fixed sleep to be thread safe (signals were interrupting thread sleeps).
 *
 * 2004-March-31   Jason Rohrer
 * Added support for detatched mode.
 *
 * 2005-January-22   Jason Rohrer
 * Added a static sleep function.
 */



#include <minorGems/system/Thread.h>
#include <pthread.h>

#include <unistd.h>
#include <time.h>
#include <stdio.h>



/**
 * Linux-specific implementation of the Thread class member functions.
 *
 * May also be compatible with other POSIX-like systems.
 *
 * To compile:
 * g++ -lpthread
 * If thread profiling is desired for gprof on linux, compile
 * with -DUSE_GPROF_THREADS (otherwise, only main thread is profiled).
 */



#ifdef USE_GPROF_THREADS
// prototype
int gprof_pthread_create( pthread_t * thread, pthread_attr_t * attr,
                          void * (*start_routine)(void *), void * arg );
#endif



// prototype
/**
 * A wrapper for the run method, since pthreads won't take
 * a class's member function.  Takes a pointer to the Thread to run,
 * cast as a void*;
 */
void *linuxThreadFunction( void * );



Thread::Thread() {
	// allocate a pthread structure on the heap
	mNativeObjectPointer = (void *)( new pthread_t[1] );
	}



Thread::~Thread() {
	// de-allocate the pthread structure from the heap
	pthread_t *threadPointer = (pthread_t *)mNativeObjectPointer;
	delete [] threadPointer;
	}



void Thread::start( char inDetach ) {

    mIsDetached = inDetach;
    
	// get a pointer to the pthread
	pthread_t *threadPointer = (pthread_t *)mNativeObjectPointer;
	
	// create the pthread, which also sets it running
#ifdef USE_GPROF_THREADS
    gprof_pthread_create( &( threadPointer[0] ), NULL, 
		linuxThreadFunction, (void*)this );
#else
    pthread_create( &( threadPointer[0] ), NULL, 
		linuxThreadFunction, (void*)this );
#endif

    if( mIsDetached ) {
        pthread_detach( threadPointer[0] );
        }
	}



void Thread::join() {
	void *joinStat;
	
	pthread_t *threadPointer = (pthread_t *)mNativeObjectPointer;
	
	pthread_join( threadPointer[0], &joinStat );
	}



void Thread::staticSleep( unsigned long inTimeInMilliseconds ) {

    unsigned long seconds = inTimeInMilliseconds / 1000;
    unsigned long milliseconds = inTimeInMilliseconds % 1000;
    
    struct timespec remainingSleepTimeStruct;
    remainingSleepTimeStruct.tv_sec = seconds;
    remainingSleepTimeStruct.tv_nsec = milliseconds * 1000000;

    struct timespec timeToSleepStruct;
    
    // sleep repeatedly, ignoring signals, untill we use up all of the time
    int sleepReturn = -1;
    while( sleepReturn == -1 ) {

        timeToSleepStruct.tv_sec = remainingSleepTimeStruct.tv_sec;
        timeToSleepStruct.tv_nsec = remainingSleepTimeStruct.tv_nsec;
        
        sleepReturn =
            nanosleep( &timeToSleepStruct, &remainingSleepTimeStruct );
        }    
	}



// takes a pointer to a Thread object as the data value
void *linuxThreadFunction( void *inPtrToThread ) {
	Thread *threadToRun = (Thread *)inPtrToThread;
	threadToRun->run();

    if( threadToRun->isDetatched() ) {
        // thread detached, so we must destroy it
        delete threadToRun;
        }
    
	return inPtrToThread;
	}





#ifdef USE_GPROF_THREADS



// found at http://sam.zoy.org/doc/programming/gprof.html
#include <sys/time.h>
/*
 * pthread_create wrapper for gprof compatibility
 *
 * needed headers: <pthread.h>
 *                 <sys/time.h>
 */
typedef struct wrapper_s {
        void * (*start_routine)(void *);
        void * arg;
        pthread_mutex_t lock;
        pthread_cond_t  wait;
        struct itimerval itimer;
    } wrapper_t;



static void * wrapper_routine(void *);



/**
 * Same prototype as pthread_create; use some #define magic to
 * transparently replace it in other files
 */
int gprof_pthread_create( pthread_t * thread, pthread_attr_t * attr,
                          void * (*start_routine)(void *), void * arg ) {

    wrapper_t wrapper_data;
    int i_return;

    /* Initialize the wrapper structure */
    wrapper_data.start_routine = start_routine;
    wrapper_data.arg = arg;
    getitimer(ITIMER_PROF, &wrapper_data.itimer);
    pthread_cond_init(&wrapper_data.wait, NULL);
    pthread_mutex_init(&wrapper_data.lock, NULL);
    pthread_mutex_lock(&wrapper_data.lock);

    /* The real pthread_create call */
    i_return = pthread_create(thread, attr, &wrapper_routine,
                                            &wrapper_data);

    /* If the thread was successfully spawned, wait for the data
     * to be released */
    if( i_return == 0 ) {
        pthread_cond_wait(&wrapper_data.wait, &wrapper_data.lock);
        }

    pthread_mutex_unlock(&wrapper_data.lock);
    pthread_mutex_destroy(&wrapper_data.lock);
    pthread_cond_destroy(&wrapper_data.wait);
    return i_return;
    }



/**
 * The wrapper function in charge for setting the itimer value
 */
static void * wrapper_routine( void * data ) {

    /* Put user data in thread-local variables */
    void * (*start_routine)(void *) = ((wrapper_t*)data)->start_routine;
    void * arg = ((wrapper_t*)data)->arg;

    /* Set the profile timer value */
    setitimer(ITIMER_PROF, &((wrapper_t*)data)->itimer, NULL);

    /* Tell the calling thread that we don't need its data anymore */
    pthread_mutex_lock(&((wrapper_t*)data)->lock);
    pthread_cond_signal(&((wrapper_t*)data)->wait);
    pthread_mutex_unlock(&((wrapper_t*)data)->lock);

    /* Call the real function */
    return start_routine(arg);
    }



#endif



