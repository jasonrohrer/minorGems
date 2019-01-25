#include <stdint.h>
#include <stdio.h>


int main( int inNumArgs, char **inArgs ) {
    uint64_t x = 0;
    
    double numRoundsDouble = 10000000.0;
    
    if( inNumArgs == 2 ) {
        sscanf( inArgs[1], "%lf", &numRoundsDouble );
        }
    
    uint64_t numRounds = (uint64_t)numRoundsDouble;


    printf( "Running %.0lf rounds\n", numRoundsDouble );
    
    for( uint64_t i=0; i<numRounds; i++ ) {
    
        x += i;
        }
    
    double resultDouble = (double)x;
    
    printf( "Result = %lf\n", resultDouble );


    return 0;
    }
