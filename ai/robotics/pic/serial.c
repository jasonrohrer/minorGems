/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.
 * Made all local function variables global.
 *
 * 2001-April-13   Jason Rohrer
 * Added functions for direct hardware serial setup and sends.
 *
 * 2001-April-15   Jason Rohrer
 * Fixed indentation.
 * Added a new setup function for software serial, and removed
 * failed hardware serial functions.
 * Changed name of setup function.
 * Made array extraction more explicit.  Changed parameters
 * to chars (versus ints) to make it easier for the compiler.
 * The max size of a buffer (char array) is 255 anyway.
 * Changed writeBuffer to take null-terminated strings only.
 * Discovered that having more than one array in a program
 * doesn't work, so removed the writeBuffer function.
 */

#include "serial.h"



char byteIndex = 0;
char charSeen = 0;
char readValue = 0;


void serialSetup() {
	// Hardware Initialization for software serial
	// code copied from:
	// http://www.iptel-now.de/HOWTO/PIC/pic.html
	
    disable_interrupt( GIE );
    set_bit( STATUS, RP0 );
	// turn on pin 3 for reading, and leave others as output pins
	set_tris_a( 00010000b );
    set_tris_b( 0 );
    clear_bit( STATUS, RP0 );
    output_port_a( 0 );
    output_port_b( 0 );
	// end copied code
	}



void skipBytes( char inNumBytes ) {
	for( byteIndex=0; byteIndex<inNumBytes; byteIndex++ ) {
		getchar();
		}
	}



void waitForByte( char inByte ) {
	charSeen = 0;
	
	while( !charSeen ) {
		readValue = getchar();
		if( readValue == inByte ) {
			charSeen = 1;
			}
		else {
			charSeen = 0;
			}
		}
	}




