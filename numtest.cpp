/*
 * Modification History
 *
 * 2001-February-3		Jason Rohrer
 * Created.
 *
 * 2001-February-4		Jason Rohrer
 * Fixed a bug that made this test not compatible with TypeIO.
 * Fixed comment.  
 */

#include <stdio.h>

// function for testing how doubles are stored

/*
 * The output from a big-endian linux is as follows:
 *	
 *	size of double = 8
 *	byte 0 = 63
 *	byte 1 = 255
 *	byte 2 = 189
 *	byte 3 = 137
 *	byte 4 = 176
 *	byte 5 = 126
 *	byte 6 = 158
 *	byte 7 = 168
 *
 */

int main() {
	printf( "size of double = %d\n", sizeof( double ) );
	
	
	
	
	
	double x = 1.983773889;
	
	
	unsigned char *doubleBuffer = (unsigned char*)( &x );
	
	for( int i=0; i<8; i++ ) {
		
		printf( "byte %d = %d\n", i, doubleBuffer[i] );
		}
		
	return 0;
	}
