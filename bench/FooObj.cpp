/*
 * Modification History
 *
 * 2002-February-13   Jason Rohrer
 * Created.
 *
 * 2002-February-14   Jason Rohrer
 * Added a remainder function call to the inner loop.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>


// prototypes:
void usage( char *inAppName );

// a brute-force, naive prime detector

class PrimeFinder {



    public:



        char isPrime( int n );

        int remainder( int quotient, int divisor );

    };




// a computationally-intense test application
int main( char inNumArgs, char **inArgs ) {

	int limit;
	struct timeb timeStart;
	struct timeb timeEnd;
	int deltaMS;
	int deltaS;
	
	int i;
	int numPrimes;

	if( inNumArgs != 2 ) {
		usage( inArgs[0] );
		}
	
	if( 1 != sscanf( inArgs[1], "%d", &limit ) ) {
		usage( inArgs[0] );
		}

	numPrimes = 0;

	
	// TIMED portion starts here
	ftime( &timeStart );

    PrimeFinder *finder = new PrimeFinder();
    
	for( i=5; i<limit; i++ ) {
		if( finder->isPrime(i) ) {
			numPrimes++;
			}
		}

    delete finder;
    
	ftime( &timeEnd );
	// TIMED portion ends here

	
	deltaMS = timeEnd.millitm - timeStart.millitm;
	deltaS = timeEnd.time - timeStart.time;

	// carry, if needed
	if( deltaMS < 0 ) {
		deltaS--;
		deltaMS += 1000;
		}
	
	printf( "Running time = %d\n", 1000 * deltaS + deltaMS );

	printf( "Number of primes found = %d\n", numPrimes );
	}



char PrimeFinder::isPrime( int inNumToCheck) {
	int i;
	
	for( i=2; i<inNumToCheck; i++ ) {
		if( this->remainder( inNumToCheck, i ) == 0 ) {
			return 0;
			}
		}
	return 1;
	}



int PrimeFinder::remainder( int quotient, int divisor ) {
    return quotient % divisor;
    }



void usage( char *inAppName ) {
	
	printf( "usage:\n" );
	printf( "%s prime_limit\n", inAppName );

	printf( "example:\n\t" );
	printf( "%s 10000\n", inAppName );
	
	exit( 1 );
	}
