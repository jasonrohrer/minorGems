/*
 * Modification History
 *
 * 2001-June-4   Jason Rohrer
 * Created.
 *
 * 2001-June-5   Jason Rohrer
 * Changed to trigger notes on an input signal.
 * Changed to dynamically set note velocity based on
 * trigger time.
 *
 * 2001-June-6   Jason Rohrer
 * Made note velocity pluck times twice as long.
 * Added a randomized pitch element.
 */

#include "midi.h"


// avoid local variables
char tempI;
char noteHoldTime;
char tempVelocity;
char tempPitch;

char betweenNoteTime;

/**
 * Tests midi implementation by sending a stream of notes.
 *
 * @author Jason Rohrer
 */
void main( void ) {

	midiSetup();

	// set B0 to output
	set_bit( STATUS, RP0 );

	// set B0 to output
	clear_bit( TRISB, 0 );

	// set B1 to input
	set_bit( TRISB, 1 );
	
	clear_bit( STATUS, RP0 );

	noteHoldTime = 0;
	tempVelocity = 0;
	betweenNoteTime = 0;
	
	while( 1 ) {
		noteHoldTime = 0;
		
		
		// if pin is on
		if( PORTB & 00000010B ) {
			turnNoteOff( 0, tempPitch, tempVelocity );
			
			// time how long the pin is on in 2's of ms
			while( PORTB & 00000010B ) {
				delay_ms( 2 );
				if( noteHoldTime < 127 ) {
					noteHoldTime++;
					}
				}
			// pin is off
			// sound a note with a velocity
			// corresponding to how long the pin was on
			tempVelocity = 127 - noteHoldTime;

			// betweenNoteTime contains a rather random
			// number in 0-255
			tempPitch = betweenNoteTime >> 1;
			
			turnNoteOn( 0, tempPitch, tempVelocity );

			// start counting again
			betweenNoteTime = 0;
			}
		else {
			// pin is off

			// this will cycle when it hits 255
			// we will be essentially generating
			// a random number by timing how
			// long the note is off
			betweenNoteTime++;

			// do nothing
			}
		/*
		for( tempI=0; tempI<=127; tempI++ ) {
			set_bit( PORTB, 0 );

			turnNoteOn( 0, tempI, 64 );
			delay_s( 1 );
			
			clear_bit( PORTB, 0 );
			delay_s( 1 );

			turnNoteOff( 0, tempI, 64 );
			}
		*/
		}
	}

