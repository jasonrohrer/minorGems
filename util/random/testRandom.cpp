#include "RandomSource.h"

#include <stdio.h>


// perform tests on a random source

// tests found here:
// NIST FIPS 140-1 U.S. Federal Standard
// http://csrc.nist.gov/publications/fips/fips140-1/fips1401.pdf

void testRandom( RandomSource *inSource ) {
    
    int bitsPerStep = 1;
    

    // 20000 bits
    // must be a multiple of bitsPerStep
    int numBits = 20000;
    
    int numSteps = numBits / bitsPerStep;

    unsigned int *drawnNumbers = new unsigned int[ numSteps ];
    
    unsigned int *drawnBits = new unsigned int[ numBits ];
    

    unsigned int bitSum = 0;
    
    // 1 or 0 when we're in a run
    unsigned int currentRunType = 2;
    unsigned int currentRunLength = 0;
    

    unsigned int longestRunLength = 0;
    
    #define maxRunLengthToTrack 100

    unsigned int runLengthCounts[2][ maxRunLengthToTrack ];
        
    int r;
    for( r=0; r<maxRunLengthToTrack; r++ ) {
        runLengthCounts[0][r] = 0;
        runLengthCounts[1][r] = 0;
        }
    

    int i;
    int bitIndex = 0;
    
    for( i=0; i<numSteps; i++ ) {
        drawnNumbers[i] = inSource->getRandomBoundedInt( 0, 1 );
        //drawnNumbers[i] = inSource->getRandomBoolean();


        for( int b=0; b<bitsPerStep; b++ ) {
            unsigned int bit = ( drawnNumbers[i] >> b & 0x01 );
            bitSum += bit;
            
            drawnBits[bitIndex] = bit;
            bitIndex++;
            

            if( bit == currentRunType ) {
                currentRunLength++;
                

                if( currentRunLength > longestRunLength ) {
                    longestRunLength = currentRunLength;
                    }
                }
            else {
                // end of run

                if( currentRunLength > 0 && 
                    currentRunLength < maxRunLengthToTrack ) {
                    // count it
                    runLengthCounts[currentRunType][ currentRunLength ] ++;
                    }
                
                currentRunType = bit;
                currentRunLength = 1;
                }
            }
        }

    float mean = (float)bitSum / numBits;

    printf( "Mean = %f\n", mean );

    
    float varSum = 0;

    for( i=0; i<numBits; i++ ) {
        unsigned int bit = drawnBits[i];
        
        varSum += (bit - mean) * (bit - mean);
        }


    float variance = varSum / numBits;
    
    printf( "Var = %f\n", variance );

    printf( "Monobit count = %d  (Allowed [9654 .. 10346])\n", bitSum );
    
    printf( "Longest run = %d\n", longestRunLength );
    
    printf( "0 Run Length\tTimes Seen\n"
            "------------\t----------\n" );
    
    for( r=0; r<maxRunLengthToTrack && r <= longestRunLength; r++ ) {
        printf( "%d\t\t%d\n", r, runLengthCounts[0][r] );
        }
    printf( "1 Run Length\tTimes Seen\n"
            "------------\t----------\n" );
    
    for( r=0; r<maxRunLengthToTrack && r <= longestRunLength; r++ ) {
        printf( "%d\t\t%d\n", r, runLengthCounts[1][r] );
        }


    // poker test
    int j;
    int numCValues = numBits / 4;
    
    unsigned int *c = new unsigned int[numCValues];
    unsigned int *f = new unsigned int[16];
    
    for( i=0; i<16; i++ ) {
        f[i] = 0;
        }
    
    
    for( j=1; j<numCValues; j++ ) {
        //printf( "bit[%d] = %d\n", j, drawnBits[j] );
        
        c[j] = 
            8 * drawnBits[ 4 * j - 3]
            +
            4 * drawnBits[ 4 * j - 2]
            +
            2 * drawnBits[ 4 * j - 1]
            +
            1 * drawnBits[ 4 * j ];

        f[ c[j] ] ++;
        }
    
    int fSum = 0;
    //printf( "Poker hand counts:\n" );
    for( i=0; i<16; i++ ) {
        //printf( "f[%d] = %d\n", i, f[i] );
        
        fSum += f[i] * f[i];
        }
    
    float Y = 16.0f/numCValues * fSum - numCValues;
    
    printf( "Poker test = %f   (Allowed [1.03 .. 57.4])\n", Y );
    
    delete [] c;
    delete [] f;
    
    

    // autocorrelation
    char failed = false;
    
    int numShifts = 5000;
    unsigned int *Z = new unsigned int[ numShifts ];
    
    for( int t=1; t<numShifts; t++ ) {
        Z[t] = 0;
        

        for( int j=0; j<5000; j++ ) {
            Z[t] += drawnBits[j] ^ drawnBits[ j + t ];
            }

        if( Z[t] > 2326 && Z[t] < 2674 ) {
            }
        else {
            failed = true;
            //printf( "autocorrelation test failed for Z[%d] = %d\n", t, Z[t] );
            }        
        }
    if( !failed ) {
        printf( "Autocorrelation test passed.\n" );
        }
    
    


    delete [] drawnNumbers;
    delete [] drawnBits;
    }




#include "CustomRandomSource.h"
#include "StdRandomSource.h"

int main() {
    
    CustomRandomSource randSource( 11234258 );
    //StdRandomSource randSource( 11 );
    
    testRandom( &randSource );
    

    return 0;
    }

