/*
 * Modification History
 *
 * 2002-October-17 Jason Rohrer
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
 * Fixed a bug in adding to the alloc list.
 * Improved printing behavior.
 * Added support for clearing memory on allocation and deallocation.
 * Fixed to ignore deallocation of our own static lock.
 * Fixed bug in allocation count.
 * Added message for NULL pointer deallocation.
 * Put locks in place around print statements, which are not atomic on win32.
 * Changed to use hex notation when printing pointers.
 *
 * 2002-October-19  Jason Rohrer
 * Added ifdef for DEBUG_MEMORY.
 *
 * 2002-October-20  Jason Rohrer
 * Removed file and line arguments from deallocation calls.
 */



#ifdef DEBUG_MEMORY


#include "minorGems/util/development/memory/MemoryTrack.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



int MemoryTrackStaticInitCounter::mCount = 0;

MutexLock *MemoryTrack::mLock;

AllocationList *MemoryTrack::mListHead;

char MemoryTrack::mTracking = false;

int MemoryTrack::mTotalAllocationSize = 0;
int MemoryTrack::mTotalDeallocationSize = 0;
int MemoryTrack::mNumberOfAllocations = 0;



void MemoryTrack::addAllocation( void *inPointer,
                                 unsigned int inAllocationSize,
                                 int inAllocationType,
                                 const char *inFileName,
                                 int inLineNumber ) {

    mLock->lock();

    if( !mTracking ) {
        printf( "Tracking off on allocation (0x%x) [%d bytes] %s:%d.\n",
                (unsigned int)inPointer,
                inAllocationSize,
                inFileName, inLineNumber );

        mLock->unlock();
        return;
        }
    

    // insert after head of list
    AllocationList *element =
        (AllocationList *)malloc( sizeof( AllocationList ) );
    element->mPrevious = (void *)mListHead;
    element->mNext = mListHead->mNext;

    mListHead->mNext = (void *)element;


    AllocationList *nextElement = (AllocationList *)( element->mNext );
    if( nextElement != NULL ) {
        nextElement->mPrevious = (void *)element;
        }
    
    element->mPointer = inPointer;
    element->mAllocationSize = inAllocationSize;
    element->mAllocationType = inAllocationType;
    element->mFileName = inFileName;
    element->mLineNumber = inLineNumber;


    mTotalAllocationSize += inAllocationSize;

    mNumberOfAllocations ++;
    
    // wipe this block of memory
    clearMemory( inPointer, inAllocationSize );
    
    mLock->unlock();
    }



int MemoryTrack::addDeallocation( void *inPointer,
                                  int inDeallocationType ) {

    mLock->lock();

    if( inPointer == NULL ) {
        printf( "NULL pointer (0x%x) deallocated\n",
                (unsigned int)inPointer );
        }

    
    if( inPointer == (void *)mLock ) {
        // we're seeing the deallocation of our own static lock as
        // the system exits
        // ignore it
        mLock->unlock();
        return 0;
        }
    
    if( !mTracking ) {
        printf( "Tracking off on deallocation (0x%x)\n",
                (unsigned int)inPointer );
        mLock->unlock();
        return 0;
        }

    

    AllocationList *element = (AllocationList *)( mListHead->mNext );

    while( element != NULL ) {
        
        void *pointer = element->mPointer;
        
        if( pointer == inPointer ) {

            unsigned int allocationSize = element->mAllocationSize;
            int allocationType = element->mAllocationType;
            const char *allocFileName = element->mFileName;            
            int allocLineNumber = element->mLineNumber;
            
            // remove from list, whether or not types match
            AllocationList *previousElement =
                (AllocationList *)( element->mPrevious );
            AllocationList *nextElement =
                (AllocationList *)( element->mNext );
            
            // patch list
            previousElement->mNext = (void *)( nextElement );

            if( nextElement != NULL ) {
                nextElement->mPrevious = (void *)( previousElement );
                }
            
            free( element );
            
            mTotalDeallocationSize += allocationSize;
            
            
            if( allocationType == inDeallocationType ) {
                // found and types match
                mLock->unlock();

                // wipe this block of memory
                clearMemory( inPointer, allocationSize );
                return 0;
                }
            else {
                // allocation types don't match
                
                printf( "Attempt to deallocate (0x%x) [%d bytes] with wrong"
                        " delete form\n"
                        "    %s:%d (location of original allocation)\n",
                        (unsigned int)inPointer,
                        allocationSize,
                        allocFileName, allocLineNumber );

                mLock->unlock();
                
                return 2;
                }
            
            }

        element = (AllocationList *)( element->mNext );
        }

    // not found (delete of unallocated memory)
    printf( "Attempt to deallocate (0x%x) unallocated memory\n",
            (unsigned int)inPointer );

    mLock->unlock();
    
    return 1;
    }



void MemoryTrack::printLeaks() {
    mLock->lock();

    printf( "\n\n---- debugMemory report ----\n" );
    
    printf( "Number of Allocations:  %d\n", mNumberOfAllocations );
    printf( "Total allocations:      %d bytes\n", mTotalAllocationSize );
    printf( "Total deallocations:    %d bytes\n", mTotalDeallocationSize );
    
    int leakEstimate = mTotalAllocationSize - mTotalDeallocationSize;


    
    AllocationList *element = (AllocationList *)( mListHead->mNext );

    if( element == NULL ) {
        printf( "No leaks detected.\n" );
        }
    else {
        printf( "Leaks detected:\n" );
        }

    int leakSum = 0;
    while( element != NULL ) {
    
        printf( "Not deallocated (0x%x) [%d bytes]\n"
                "    %s:%d (location of original allocation)\n",
                (unsigned int)( element->mPointer ),
                element->mAllocationSize,
                element->mFileName,
                element->mLineNumber );

        leakSum += element->mAllocationSize;
        
        element = (AllocationList *)( element->mNext );
        }

    
    if( leakSum != leakEstimate ) {
        printf( "Warning:  Leak sum does not equal leak estimate.\n" );
        }

    
    printf( "Leaked memory:          %d bytes\n", leakSum );
    
    printf( "---- END debugMemory report ----\n\n" );

    
    mLock->unlock();
    }



void MemoryTrack::clearMemory( void *inPointer, unsigned int inSize ) {

    unsigned char *charArray = (unsigned char *)inPointer;

    for( unsigned int i=0; i<inSize; i++ ) {
        charArray[i] = (unsigned char)0xAA;
        }

    }



#endif




