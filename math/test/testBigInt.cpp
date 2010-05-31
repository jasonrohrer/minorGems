/*
 * Modification History
 *
 * 2002-May-25    Jason Rohrer
 * Created.
 */



#include "minorGems/math/BigInt.h"

#include <stdio.h>



int main() {


    unsigned char intA[] = { 100, 200, 99, 23, 89, 100, 233 };

    unsigned char intB[] = {  10, 78, 243, 9, 0 };

    BigInt *bigIntA = new BigInt( 1, 7, intA );

    BigInt *bigIntB = new BigInt( -1, 5, intB );

    char *hexA = bigIntA->convertToHexString();
    char *hexB = bigIntB->convertToHexString();
    

    printf( "A =   %s\n", hexA );
    printf( "B =       %s\n", hexB );

    BigInt *sum = bigIntA->subtract( bigIntB );

    char *hexSum = sum->convertToHexString();
    printf( "Sum = %s\n", hexSum );
    
    
    delete [] hexA;
    delete [] hexB;
    delete [] hexSum;
    
    delete bigIntA;
    delete bigIntB;
    delete sum;


    int c = 0x58B11F;
    
    BigInt *intC = new BigInt( c );
    char *hexC = intC->convertToHexString();

    printf( "C = %s\n", hexC );

    int extractedC = intC->convertToInt();

    if( extractedC == c ) {
        printf( "equal\n" );
        }
    else {
        printf( "not equal\n" );
        }
    
    delete [] hexC;
    delete intC;
    
    
    

    int limit = 300;
    printf( "Testing pair operations for all pairs in -%d..%d ...\n",
            limit, limit );
    
    char failed = false;

    for( int i=-limit; i<limit && !failed; i++ ) {
        BigInt *intI = new BigInt( i );
        
        for( int j=-limit; j<limit && !failed; j++ ) {
            BigInt *intJ = new BigInt( j );

            BigInt *intSum = intI->add( intJ );
            BigInt *intDiff = intI->subtract( intJ );
            
            int sum = i + j;
            int diff = i - j;

            if( sum != intSum->convertToInt() ) {
                printf( "sum test failed for %d, %d\n", i, j );
                printf( "    real sum = %d, computed sum = %d\n",
                        sum, intSum->convertToInt() );
                failed = true;
                }

            if( diff != intDiff->convertToInt() ) {
                printf( "diff test failed for %d, %d\n", i, j );
                printf( "    real diff = %d, computed diff = %d\n",
                        diff, intDiff->convertToInt() );
                failed = true;
                }

            if( intI->isLessThan( intJ ) && ( i >= j ) ) {
                printf( "first less than test failed for %d, %d\n", i, j );
                failed = true;
                }

            if( intJ->isLessThan( intI ) && ( j >= i ) ) {
                printf( "second less than test failed for %d, %d\n", i, j );
                failed = true;
                }

            if( intI->isEqualTo( intJ ) && ( i != j ) ) {
                printf( "equality test failed for %d, %d\n", i, j );
                failed = true;
                }
            
            
            delete intSum;
            delete intDiff;
            delete intJ;
            
            }

        delete intI;
        }

    if( !failed ) {
        printf( "test passed\n" );
        }

    return 0;
    }

