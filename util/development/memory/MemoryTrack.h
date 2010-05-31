/*
 * Modification History
 *
 * 2002-October-17   Jason Rohrer
 * Created.
 *
 * 2002-October-18  Jason Rohrer
 * Changed to use custom list instead of SimpleVector because SimpleVector
 * uses debugMemory.
 * Added static initialization counting class.
 * Changed to use struct and malloc for AllocationList to avoid
 * circular new and delete calls.
 *
 * 2002-October-19  Jason Rohrer
 * Changed to print leak list upon final destruction.
 * Improved printing behavior.
 * Added support for clearing memory on allocation and deallocation.
 *
 * 2002-October-20  Jason Rohrer
 * Removed file and line arguments from deallocation calls.
 */



#ifndef MEMORY_TRACK_INCLUDED
#define MEMORY_TRACK_INCLUDED



#include "minorGems/system/MutexLock.h"
#include <stdio.h>
#include <stdlib.h>


#define SINGLE_ALLOCATION  0
#define ARRAY_ALLOCATION   1



/**
 * Linked list of memory allocations.
 *
 * @author Jason Rohrer
 */
typedef struct {
        
        void *mPrevious;
        void *mNext;

        void *mPointer;
        unsigned int mAllocationSize;
        int mAllocationType;
        const char *mFileName;
        int mLineNumber;
        
    } AllocationList;



/**
 * Class that tracks memory allocations and deallocations.
 *
 * @author Jason Rohrer
 */
class MemoryTrack {


        
    public:

        
        
        /**
         * Adds an allocation to this tracker and clears the allocated
         * memory block.
         *
         * @param inPointer a pointer to the allocated memory.
         * @param inAllocationType the type of allocation,
         *   either SINGLE_ALLOCATION or ARRAY_ALLOCATION.
         * @param inAllocationSize the size of the allocation in bytes.
         * @param inFileName the name of the source file in which the
         *   allocation took place.
         * @param inLineNumber the line number in the source file
         *   on which the allocation took place.
         */
        static void addAllocation( void *inPointer,
                                   unsigned int inAllocationSize,
                                   int inAllocationType,
                                   const char *inFileName,
                                   int inLineNumber );
        
        

        /**
         * Adds a deallocation to this tracker and clears the block
         * to be deallocated.
         * Must be called *before* the memory is deallocated
         *
         * @param inPointer a pointer to the memory being deallocated.
         * @param inDeallocationType the type of deallocation,
         *   either SINGLE_ALLOCATION or ARRAY_ALLOCATION.
         * @return 0 if the deallocation deallocates
         *   an allocated block of memory, or 1 if it
         *   deallocates a block of memory that is not currently allocated,
         *   and 2 if it is the wrong deallocation type for the specified
         *   block.
         */
        static int addDeallocation( void *inPointer, int inDeallocationType );




        /**
         * Prints a list of all memory leaks (allocations that have never
         * been deallocated).
         */
        static void printLeaks();
        

        
        // these are public so initializer can get to them

        static MutexLock *mLock;

        // dummy place holder for list head
        static AllocationList *mListHead;

        // true if we're tracking
        static char mTracking;
        
        static int mTotalAllocationSize;

        static int mTotalDeallocationSize;

        static int mNumberOfAllocations;
        
    protected:


        
        /**
         * Clears memory so that reading from it will not produce
         * anything useful.  Good for checking for reads to memory that
         * has been deallocated.
         *
         * @param inPointer pointer to the memory to clear.
         * @Param inSize the number of bytes to clear starting at inPointer.
         */
        static void clearMemory( void *inPointer, unsigned int inSize );


        
    };



/**
 * Class that initializes MemoryTrack's static members.
 *
 * *All* files that use MemoryTrack will instantiate a static
 * instance of this class (see static instance below).
 *
 * This class counts how many static instantiations have happened so
 * far, making sure to init/destroy MemoryTrack's static members only once.
 *
 * Adapted from:
 * http://www.hlrs.de/organization/par/services/tools/docu/kcc/
 *       tutorials/static_initialization.html
 */
class MemoryTrackStaticInitCounter {
    

    public:


        
        MemoryTrackStaticInitCounter() {
            if( mCount == 0 ) {
                // allocate static members
                MemoryTrack::mLock = new MutexLock();

                MemoryTrack::mListHead =
                    (AllocationList *)
                        malloc( sizeof( AllocationList ) );
                MemoryTrack::mListHead->mPrevious = NULL;
                MemoryTrack::mListHead->mNext = NULL;

                MemoryTrack::mTotalAllocationSize = 0;
                MemoryTrack::mTotalDeallocationSize = 0;
                MemoryTrack::mNumberOfAllocations = 0;
                
                MemoryTrack::mTracking = true;
                }
            mCount++;
            }


        
        ~MemoryTrackStaticInitCounter() {
            mCount--;
            if( mCount == 0 ) {
                // print leaks... we should only get here after
                // all static members of classes that use MemoryTrack
                // have been destroyed.
                MemoryTrack::printLeaks();

                MemoryTrack::mTracking = false;
                // deallocate static members
                free( MemoryTrack::mListHead );
                delete MemoryTrack::mLock;
                }
            }

        
    private:
        // only allocate/deallocate when mCount == 0
        static int mCount;
        
    };



// This will be included in *every* file that includes MemoryTrack.h
static MemoryTrackStaticInitCounter memoryTrackInitializer;



#endif


