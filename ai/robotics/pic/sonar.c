/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.
 * Moved ascii hex functions int a separate file.
 * Finished implementation, and fixed compile bugs.
 * Added setting output pin when objects are "too close".
 *
 * 2001-April-13   Jason Rohrer
 * Fixed a bug in the protocol (was not sending a newline before
 * waiting for a prompt).
 *
 * 2001-April-15   Jason Rohrer
 * Fixed indentation.
 * Updated to use new serial setup functions.
 * Removed all array usage.
 * Changed from port A to port B for output to avoid apparent
 * conflict with serial output.
 * Tested with a terminal program, and it seems to be working.
 *
 * 2001-April-17   Jason Rohrer
 * Removed all skipBytes calls, since we miss the byte automatically
 * (they are not being buffered).
 * Fixed bug in indexing of transducers (there is no transducer 0000).
 * Added several delay hacks to make the whole thing work (the serial
 * transmission to the sonar board seems very finicky).  The various
 * delays were added in a way that simply seem to work.  There is not
 * much rationale behind most of them.
 *
 * 2001-April-20   Jason Rohrer
 * Changed to use 4 sonar units.
 *
 * 2001-May-14   Jason Rohrer
 * Changed to use pins B1 and B2 for directional obstacle information.
 */


/*
 * Note that in general, we're avoiding local variable declarations inside
 * function bodies.  According to the c2c documentation, these can be
 * overwritten when calling functions that also declare local variables.
 */

/*
 * Can be compiled for a 16F84 without RAM problems.
 */

#include "serial.h"
#include "asciihex.h"


// the number of sonar units
char numUnits = 4;

char numGroups = 2;
char numInGroup = 2;

// the index of the current group of units
char groupIndex = 0;
// the index of the current unit in the group
char inGroupIndex = 0;

// the index of the current unit 
char unitIndex = 0;
// the echo value of the current unit
int echoTime = 0;


// set to 1 if one of the sensors has produced a reading that is too close
char tooClose = 0;

// set to 1 if one of the first two senseors has produced a close reading
char tooCloseLeft = 0;

// set to 1 if one of the second two senseors has produced a close reading
char tooCloseRight = 0;


// min echo time that is "far enough away"
// 50 cycles per inch if sound travels at 1000 feet per second
// about 3 feet
int minEchoTime = 1800;


char receivedChar = 0;

// prototypes


/**
 * Note that all functions writing to the serial port handle
 * reading the echo.
 */


/**
 * Waits for a serial prompt, then returns.
 */
void waitForPrompt();



/**
 * Writes a newline sequence (CR LF) to the serial port.
 */
void writeNewLine();


	 
/**
 * Turns the driver board power on.
 *
 * @param inPowerOn 1 to turn the power on, 0 to turn it off.
 */
void setPower( char inPowerOn );


/**
 * Sets the initial blanking time of the sonar board.  This is a time
 * delay between when the transducers are fired and when the board
 * starts listening for echos (to prevent transducer ringing from
 * being falsely detected as an early echo).
 *
 * Note that the sonar board defaults to an IB value of B8, which was
 * found to be a little too short for some transducers during experiment.
 */
void setInitialBlanking();



/**
 * Fires a specified sonar unit (on main chain) and returns its echo time.
 *
 * @param inUnit the unit number, in [1-15].
 *
 * @return the echo time for this unit.
 */
int fireSonarUnit( char inUnit );



// end prototypes



/**
 * Main function.
 */
void main( void ) {

	// set B0, B1, and B2 to output
	set_bit( STATUS, RP0 );

	clear_bit( TRISB, 0 );
	clear_bit( TRISB, 1 );
	clear_bit( TRISB, 2 );
	
	clear_bit( STATUS, RP0 );

	
	serialSetup();
	
	// testing serial
	/*while( 1 ) {
		setBufferSize( 23 );
		writeBuffer( "Testing Testing Testing" );
		writeNewLine();
		}
	*/
	
	//writeNewLine();
	//waitForPrompt();
	delay_ms( 100 );

	writeNewLine();
	delay_ms( 20 );
	writeNewLine();
	delay_ms( 20 );
	writeNewLine();
	delay_ms( 20 );

	// turn the power on
	setPower( 1 );

	// wait for one secont for the power to come up
	delay_s( 1 );

	setInitialBlanking();
	delay_ms( 20 );
	
	while( 1 ) {
		tooClose = 0;
		tooCloseLeft = 0;
		tooCloseRight = 0;

		// for each group of units
		for( groupIndex = 0; groupIndex < numGroups; groupIndex++ ) {

			// for each unit in this group
			for( inGroupIndex = 0;
				 inGroupIndex < numInGroup; inGroupIndex++ ) {

				unitIndex = groupIndex * numInGroup + inGroupIndex;

				echoTime = fireSonarUnit( unitIndex + 1 );

				if( ( echoTime != 0 ) && ( echoTime < minEchoTime ) ) {
					if( groupIndex == 0 ) {
						tooCloseLeft = 1;
						}
					else {
						tooCloseRight = 1;
						}
					}
				}
			}

		tooClose = tooCloseLeft || tooCloseRight;
	
		if( tooClose ) {
			set_bit( PORTB, 0 );
			}
		else {
			clear_bit( PORTB, 0 );
			}
		
		if( tooCloseLeft ) {
			set_bit( PORTB, 1 );
			}
		else {
			clear_bit( PORTB, 1 );
			}

		if( tooCloseRight ) {
			set_bit( PORTB, 2 );
			}
		else {
			clear_bit( PORTB, 2 );
			}
		
		}
	
	}



void waitForPrompt() {
	//writeNewLine();
	//writeNewLine();
	//writeNewLine();
	delay_ms( 20 );
	writeNewLine();
	waitForByte( '*' );
	}



void writeNewLine() {
	putchar( 13 );
	putchar( 10 );
	// read the echo
	//skipBytes( 2 );
	}



void setPower( char inPowerOn ) {
	// output the same command header in either case
	putchar( 'C' );
	putchar( 'P' );
	putchar( ' ' );
	putchar( '0' );

	if( inPowerOn ) {
		// turn power on to chain pair A
		putchar( 'F' );
		}
	else {
		// turn power off to chain pair A
		putchar( '0' );
		}

	// read the echo
	//skipBytes( 5 );
	
	writeNewLine();
	}



void setInitialBlanking() {
	// set a blanking time that was found to work by experiment
	putchar( 'I' );
	putchar( 'B' );
	putchar( ' ' );
	putchar( '0' );
	putchar( '0' );
	putchar( 'C' );
	putchar( '8' );
	
	writeNewLine();
	}




int fireSonarUnit( char inUnit ) {
	putchar( 'R' );
	putchar( 'T' );
	putchar( ' ' );
	putchar( '0' );
	putchar( '0' );
	putchar( '0' );
	
	// read the echo
	//skipBytes( 6 );

	// write the ASCII hex equivalent of inUnit
	putchar( convertIntToASCIIHex( inUnit ) );

	// read the echo
	//skipBytes( 1 );

	// add a delay here to allow the echos to come back for the
	// above... we're not sure how many to wait for, but
	// we can wait as long as we want here without violating the protocol

	// 20 ms should be long enough
	delay_ms( 20 );
	
	writeNewLine();

	// skip the echoed newline
	skipBytes( 2 );
	
	// thus to fire unit 10, we will be sending:
	// "RT 000A" followed by a new line

	// read the response value
	return readSerialASCIIHex();
	}








