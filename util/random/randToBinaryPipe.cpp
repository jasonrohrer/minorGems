#include <stdio.h>


#include "minorGems/util/random/RandomSource.h"


void genRandom( RandomSource *inSource ) {
    
    while( true ) {
        
        unsigned int v = inSource->getRandomInt();
        
        
        fwrite( &v, sizeof(int), 1, stdout );
        }
    }




#include "CustomRandomSource.h"
#include "StdRandomSource.h"
#include "JenkinsRandomSource.h"
#include "XoshiroRandomSource.h"

int main() {
    
    //JenkinsRandomSource randSource( 11234258 );
    //CustomRandomSource randSource( 11234258 );
    //StdRandomSource randSource( 11 );
    XoshiroRandomSource randSource( 0 );
    
    genRandom( &randSource );
    

    return 0;
    }
