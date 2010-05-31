/*
 * Modification History
 *
 * 2002-August-1   Jason Rohrer
 * Created.
 */


#include "Path.h"

#include <stdio.h>



int main() {

    char *pathString = "/test/this/thing";
    
    printf( "using path string = %s\n", pathString );

    printf( "Constructing path.\n" );
    Path *path = new Path( pathString );

    
    printf( "Extracting path string.\n" );
    char *extractedPathString = path->getPathStringTerminated();
    
    printf( "extracted path string = %s\n", extractedPathString );

    
    
    delete [] extractedPathString;

    delete path;
    
    return 1;
    }


