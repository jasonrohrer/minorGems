/*
 * Modification History
 *
 * 2002-October-17 Jason Rohrer
 * Created.
 *
 * 2002-October-18  Jason Rohrer
 * Added static initialization counting class for MemoryTrack.
 *
 * 2002-October-19  Jason Rohrer
 * Added more detail to error message.
 * Improved printing behavior.
 * Moved include of debugMemory.h to work better with IDE compilers.
 * Fixed to deal with differences between malloc and new[] on some platforms.
 *
 * 2002-October-20  Jason Rohrer
 * Removed delete macro trick that was causing crashes in tinyxml.
 * Removed function that was no longer being used.
 */



#include "minorGems/util/development/memory/debugMemory.h"


#ifdef DEBUG_MEMORY

#include "minorGems/util/development/memory/MemoryTrack.h"

#include "stdlib.h"
#include "stdio.h"



void *debugMemoryNew( unsigned int inSize,
                      const char *inFileName, int inLine ) {

    
    void *allocatedPointer = (void *)malloc( inSize );

    MemoryTrack::addAllocation( allocatedPointer, inSize,
                                SINGLE_ALLOCATION,
                                inFileName, inLine );
    
    return allocatedPointer;    
    }



void *debugMemoryNewArray( unsigned int inSize,
                           const char *inFileName, int inLine ) {

    unsigned int mallocSize = inSize;
    if( inSize == 0 ) {
        // always allocate at least one byte to circumvent differences
        // between malloc and new[] on some platforms
        // (new int[0] returns a pointer to an array of length 0, while
        //  malloc( 0 ) can return NULL on some platforms)
        mallocSize = 1;
        }

    void *allocatedPointer = (void *)malloc( mallocSize );

    MemoryTrack::addAllocation( allocatedPointer, inSize,
                                ARRAY_ALLOCATION,
                                inFileName, inLine );
    
    return allocatedPointer;    
    }



void debugMemoryDelete( void *inPointer ) {
    MemoryTrack::addDeallocation( inPointer, SINGLE_ALLOCATION );
    free( inPointer );
    }



void debugMemoryDeleteArray( void *inPointer ) {
    MemoryTrack::addDeallocation( inPointer, ARRAY_ALLOCATION );
    free( inPointer );
    }



#endif








