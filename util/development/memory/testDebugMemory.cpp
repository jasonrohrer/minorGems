/*
 * Modification History
 *
 * 2002-October-17 Jason Rohrer
 * Created.
 *
 * 2002-October-18  Jason Rohrer
 * Added static initialization counting class for MemoryTrack.
 *
 * 2002-October-19   Jason Rohrer
 * Removed call to debugMemoryPrintLeaksAndStopTracking.
 * Made test cases more interesting.
 * Added test cases for deleting NULL pointers.
 */


#include "minorGems/util/development/memory/debugMemory.h"


#include <stdio.h>


class TestClass {
    public:
        TestClass() {
            mX = new int[5];
            }

        ~TestClass() {
            delete [] mX;
            }
        
        int *mX;
    };



int main() {

    int *x = new int[5];

    printf( "array contents before initializing elements:\n"
            "%d, %d, %d, %d, %d\n\n",
            x[0], x[1], x[2], x[3], x[4] );
    
    x[0] = 1;
    x[1] = 2;
    x[2] = 3;
    x[3] = 4;
    x[4] = 5;

    printf( "array contents before deleting:\n"
            "%d, %d, %d, %d, %d\n\n",
            x[0], x[1], x[2], x[3], x[4] );

    delete [] x;

    printf( "array contents after deleting:\n"
            "%d, %d, %d, %d, %d\n\n",
            x[0], x[1], x[2], x[3], x[4] );

    
    int *y = new int[4];
    y[0] = 1;
    
    TestClass *t = new TestClass();

    delete t;

    int *z = new int[7];
    delete z;
    
    //delete t;

    int *badPointer = NULL;
    delete badPointer;

    int *badPointer2 = NULL;
    delete [] badPointer2;
    
    return 0;
    }



