/*
 * Modification History
 *
 * 2001-April-15   Jason Rohrer
 * Created.
 * Changed so that it doesn't use arrays (since more than one array
 * doesn't seem to work with this compiler).
 */

#include "serial.h"


void main() {
	serialSetup();
	
	while( 1 ) {
		putchar( 'T' );
		putchar( 'e' );
		putchar( 's' );
		putchar( 't' );
		putchar( 'i' );
		putchar( 'n' );
		putchar( 'g' );
		putchar( 'A' );
		skipBytes( 8 );
		
		putchar( 'T' );
		putchar( 'e' );
		putchar( 's' );
		putchar( 't' );
		putchar( 'i' );
		putchar( 'n' );
		putchar( 'g' );
		putchar( 'A' );
		putchar( 'A' );
		skipBytes( 9 );
		
		putchar( 'T' );
		putchar( 'e' );
		putchar( 's' );
		putchar( 't' );
		putchar( 'i' );
		putchar( 'n' );
		putchar( 'g' );
		putchar( 'A' );
		putchar( 'A' );
		putchar( 'A' );
		skipBytes( 10 );
		
		}
	}



