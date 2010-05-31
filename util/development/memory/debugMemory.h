/*
 * Modification History
 *
 * 2002-October-17 Jason Rohrer
 * Created.
 *
 * 2002-October-18  Jason Rohrer
 * Added static initialization counting class for MemoryTrack.
 *
 * 2002-October-20  Jason Rohrer
 * Removed delete macro trick that was causing crashes in tinyxml.
 */


#ifndef DEBUG_MEMORY_INCLUDED
#define DEBUG_MEMORY_INCLUDED




#ifdef DEBUG_MEMORY

#include "minorGems/util/development/memory/MemoryTrack.h"



// internal function prototypes
void *debugMemoryNew( unsigned int inSize,
                      const char *inFileName, int inLine );

void *debugMemoryNewArray( unsigned int inSize,
                           const char *inFileName, int inLine );

void debugMemoryDelete( void *inPointer );

void debugMemoryDeleteArray( void *inPointer );



// overrided primitive operators... must be inline?



/**
 * Overrides the new operator to track memory allocations.
 *
 * @param inSize the size of the allocation.
 * @param inFileName the name of the source file where the allocation
 *   occurred.
 * @param inLine the line in the source file where the allocation occurred.
 */
inline void *operator new( unsigned int inSize,
                           const char *inFileName, int inLine ) {
    return debugMemoryNew( inSize, inFileName, inLine );
    }



/**
 * Overrides the new [] operator to track memory allocations.
 *
 * @param inSize the size of the allocation.
 * @param inFileName the name of the source file where the allocation
 *   occurred.
 * @param inLine the line in the source file where the allocation occurred.
 */
inline void * operator new [] ( unsigned int inSize,
                                const char *inFileName, int inLine ) {

    return debugMemoryNewArray( inSize, inFileName, inLine );
    }




/**
 * Overrides the delete operator to track memory allocations.
 *
 * @param inPointer a pointer to the memory to deallocate.
 */
inline void operator delete( void *inPointer ) {
    debugMemoryDelete( inPointer );
    }



/**
 * Overrides the delete [] operator to track memory allocations.
 *
 * @param inPointer a pointer to the memory to deallocate.
 */
inline void operator delete [] ( void *inPointer ) {
    debugMemoryDeleteArray( inPointer );
    }



#endif





// macro trickery to pass file name and line number into new

#ifdef DEBUG_MEMORY
#define DEBUG_NEW new( __FILE__, __LINE__ )
#else
#define DEBUG_NEW new
#endif



#define new DEBUG_NEW



#endif









