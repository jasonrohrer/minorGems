#include <stdio.h>


#include "minorGems/util/random/RandomSource.h"


void testRandom( RandomSource *inSource ) {
    
    for( int i=0; i<1000000000; i++ ) {
        unsigned int v = inSource->getRandomInt();
        }
    }




#include "CustomRandomSource.h"
#include "StdRandomSource.h"
#include "JenkinsRandomSource.h"
#include "XoshiroRandomSource.h"

int main() {
    
    JenkinsRandomSource randSource( 11234258 );
    //CustomRandomSource randSource( 11234258 );
    //StdRandomSource randSource( 11 );
    //XoshiroRandomSource randSource( 0 );
    
    testRandom( &randSource );
    

    return 0;
    }
