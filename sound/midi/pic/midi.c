/*
 * Modification History
 *
 * 2001-June-4   Jason Rohrer
 * Created.
 */

#include "midi.h"



// avoid local function variables
char midiTempCharA;



void midiSetup() {
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



void turnNoteOn( char inChannel, char inKey, char inVelocity ) {
	// the on code
	midiTempCharA = 10010000B;

	// make sure the channel is in range
	if( inChannel > 15 ) {
		return;
		}

	// stick the channel into the last 4 bits
	midiTempCharA = midiTempCharA | inChannel;


	// make sure the key is in range
	if( inKey > 127 ) {
		return;
		}

	// make sure the velocity is in range
	if( inVelocity > 127 ) {
		return;
		}


	// send the channel
	putchar( midiTempCharA );
	// send the key
	putchar( inKey );
	// send the velocity
	putchar( inVelocity );
	}



void turnNoteOff( char inChannel, char inKey, char inVelocity ) {
	// the off code
	midiTempCharA = 10000000B;

	// make sure the channel is in range
	if( inChannel > 15 ) {
		return;
		}

	// stick the channel into the last 4 bits
	midiTempCharA = midiTempCharA | inChannel;


	// make sure the key is in range
	if( inKey > 127 ) {
		return;
		}

	// make sure the velocity is in range
	if( inVelocity > 127 ) {
		return;
		}


	// send the channel
	putchar( midiTempCharA );
	// send the key
	putchar( inKey );
	// send the velocity
	putchar( inVelocity );
	}

