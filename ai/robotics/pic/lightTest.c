/*
 * Modification History
 *
 * 2001-June-4   Jason Rohrer
 * Created.
 */


/*
 * Note that in general, we're avoiding local variable declarations inside
 * function bodies.  According to the c2c documentation, these can be
 * overwritten when calling functions that also declare local variables.
 */



//Timing settings
#pragma CLOCK_FREQ 10000000


/**
 * Main function.  Sends a light-flashing signal with a
 * frequecy of 0.5 Hz to pin B0.
 */
void main( void ) {

	// set B0 to output
	set_bit( STATUS, RP0 );

	clear_bit( TRISB, 0 );
	
	clear_bit( STATUS, RP0 );

	while( 1 ) {
		set_bit( PORTB, 0 );
		delay_s( 1 );
		clear_bit( PORTB, 0 );
		delay_s( 1 );
		}

	}
