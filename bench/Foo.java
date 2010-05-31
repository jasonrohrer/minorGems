/*
 * Modification History
 *
 * 2001-May-18   Jason Rohrer
 * Created.
 */


/**
 * A simple, naive prime-finding benchmark.
 *
 * @author Jason Rohrer
 */
public class Foo {

		public static void main( String argv[] ) {

			if( argv.length != 1 ) {
				usage( "Foo" );
				}
			
			int limit = Integer.parseInt( argv[0] );

			int numPrimes = 0;
			
			long timeStart = System.currentTimeMillis();

			
			for( int i=5; i<limit; i++ ) {
				if( isPrime(i) ) {
					numPrimes++;
					}
				}
			
			long timeEnd = System.currentTimeMillis();
			long netTime = timeEnd - timeStart;
			System.out.println( "Running time = " + netTime );

			System.out.println( "Number of primes found = " +
								numPrimes );
			}

		

		/**
		 * A naive prime detector.
		 *
		 * @param inNumToCheck the number to check for primality.
		 *
		 * @return ture iff inNumToCheck is prime.
		 */
		public static boolean isPrime( int inNumToCheck ) {
			for( int i=2; i<inNumToCheck; i++ ) {
				if( inNumToCheck % i == 0 ) {
					return false;
					}
				}
			return true;
			}


		
		/**
		 * Prints a usage message and exits.
		 *
		 * @param inAppName the name of the application.
		 */
		public static void usage( String inAppName ) {
	
			System.out.println( "usage:" );
			System.out.println( inAppName + " prime_limit" );
			
			System.out.println( "example:" );
			System.out.println( "\t" + inAppName + " 10000" );
			
			System.exit( 1 );
			}
		}
