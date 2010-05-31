/*
 * Modification History
 *
 * 2001-April-26   Jason Rohrer
 * Created.
 *
 * 2001-April-30   Jason Rohrer
 * Worked on implementation.
 *
 * 2001-May-1   Jason Rohrer
 * Continued work on implementation.
 * Changed to use timing for distance measurement instead of
 * exact odometry.
 *
 * 2001-May-2   Jason Rohrer
 * Fixed several bugs and got this initial version working.
 * It actually goes around obstacles, but it is slow.
 * Doubled the rotation velocity.
 * Increased the step size from 1cm to 10cm.
 *
 * 2001-May-6   Jason Rohrer
 * Fixed several bugs.
 * Changed to strobe ready pin so that camera can read it
 * (camera only detects changes in the pin state).
 * Replaced byte-reading function with a more intelligent version.
 * Increased bit time to 100ms for testing.
 *
 * 2001-May-7   Jason Rohrer
 * Camera is now being told to output 10ms per bit, but
 * it actually outputs approximately (unfortunately) 20ms per bit.
 * Changed code here to account for this.
 *
 * 2001-May-8   Jason Rohrer
 * Fixed an indexing bug.
 * Fixed a bug in the left rotation.  Removed a debugging delay.
 * Fixed a bug in the delay involved in a rotation.
 * Fixed a bug in the start-bit delay.  Updated some comments.
 *
 * 2001-May-10   Jason Rohrer
 * Fixed a bug in dealing with negative translations.
 * Changed all increment and decrement operations to ++ and --,
 * which is supported by the c2c compiler.
 * Made ready strobe faster.
 *
 * 2001-May-13   Jason Rohrer
 * Added support for a light-flashing command.
 * Added a limp command to fix a shuttering problem after small rotations.
 * Optimized for code size so that it will fit on a pic16F84.
 * Note that we are down to 3 free program words.
 *
 * 2001-May-14   Jason Rohrer
 * Changed to use directional sonar information for picking
 * intial rotation direction during obstacle avoidance.
 * Removed some unused variables.
 *
 * 2001-May-16   Jason Rohrer
 * Changed to ignore translation commands that seem erroneous.
 */

/**
 * A PIC program that accepts remote commands and executes them
 * via a B12 mobile robot base.
 *
 * Avoids obstacles by using an external obstacle flag.
 */


/*
 * Note that in general, we're avoiding local variable declarations inside
 * function bodies.  According to the c2c documentation, these can be
 * overwritten when calling functions that also declare local variables.
 */

// tells serial to use a special baud rate
#define B12_CONTROL


#include "serial.h"
#include "asciihex.h"


// where getRemoteCommand returns values
char commandCode = 0;
char commandParameter = 0;

// fields for temporary work
int rotateWorkingInt;
char delayAmount;
char netForwardTranslation;
char netNegativeTranslation;
char forwardAvoidWorkingChar2;
char avoidNumRotations;
char movingForward;

char flushWorkingChar1;
char flushWorkingChar2;

char bitsRead;
char bitValue;
char byteValue;

char strobeState;
char strobeCounter;

char flashCounter;

char rotateDirection;

char firstHit;

// prototypes


/**
 * Note that all functions writing to the serial port handle
 * reading the echo.
 */



/**
 * Puts the base into half-duplex serial mode.
 * (Echo off.)
 */
void halfDuplex();


/**
 * Puts the base into full-duplex serial mode.
 * (Echo on.)
 */
void fullDuplex();


/**
 * Sets reasonable initial rotation and translation velocities.
 * Does not start the base moving.
 */
void setInitialVelocities();


/**
 * Stops both rotation and translation.
 */
void halt();

/**
 * Sets the rotation motors into limp mode.
 */
void rotateLimp();


/** * Waits for and receives a command from the input pin.
 *
 * The command and its parameter are returned in commandCode
 * and commandParameter, respectively.
 */
void getRemoteCommand();


/**
 * Reads an 8-bit value from input pin B1.
 *
 * Assumes 10ms per bit, and a 5ms delay before
 * the start of the first bit.
 *
 * @return the 8-bit value read.
 */
char read8BitValue();

	 
/**
 * Translates the robot forward while performing obstacle avoidance.
 *
 * @param inAmount the amount in decimeters to translate
 */
void forwardAvoid( char inAmount );



/**
 * Rotates the robot.
 *
 * @param inRight true iff the rotation is to the right, and
 *   false if the rotation is to the left.
 * @param inAmount the amount to rotate in 1/256 rotations.
 */
void rotate( char inRight, int inAmount ); 



/**
 * Flashes a light attached to pin B3.  Flashes
 * are 0.5 sec long with 0.5 sec separation.
 *
 * @param inNumFlashes the number of times to flash.
 */
void flashLight( char inNumFlashes );



/**
 * Waits for the communication buffer of the B12 base to flush.
 */
void waitForComBufferFlush();

	 
/**
 * Writes a newline sequence (CR LF) to the serial port.
 */
void writeNewLine();


/**
 * Shifts an 8-bit value to the left.
 *
 * @param inValue the value to shift.
 * @param inShiftAmount the amount to shift.
 *
 * @return the shifted value.
 */
char shiftLeft( char inValue, char inShiftAmount );


// end prototypes



/**
 * Main function.
 */
void main( void ) {

	
	set_bit( STATUS, RP0 );
	
	// set B0 to input (flag from sonar unit)
	set_bit( TRISB, 0 ); 

	// set B1 to input (remote command pin)
	set_bit( TRISB, 1 );
	
	// set B2 to output (ready-for-command flag)
	clear_bit( TRISB, 2 );

	// set B3 to output (flashing light)
	clear_bit( TRISB, 3 );

	// set B4 to input (left flag from the sonar unit)
	set_bit( TRISB, 4 );
	
	// set B5 to input (right flag from the sonar unit)
	set_bit( TRISB, 5 );
	
	clear_bit( STATUS, RP0 );
	
	serialSetup();

	/*
	  clear serial "junk" by sending several characters
	   with delays.
	*/
	
	delay_s( 1 );
	
	writeNewLine();
	delay_ms( 20 );
	writeNewLine();
	delay_ms( 20 );
	writeNewLine();
	delay_ms( 20 );

	
	
	// go into half duplex mode
	halfDuplex();

	// set velocities
	setInitialVelocities();
	
	
	while( 1 ) {

		// wait for the next command
		// turn pin B2 on (our ready signal)
		set_bit( PORTB, 2 );

		getRemoteCommand();

		// turn our ready pin off
		clear_bit( PORTB, 2 );

		if( commandCode == 1 ) {
			// forwardAvoid
			if( commandParameter == 1 ) {
				forwardAvoid( commandParameter );
				}
			else {
				flashLight( 3 );
				}
			}
		else if( commandCode == 2 ) {
			// rotateLeft
			rotate( 0, commandParameter );
			}
		else if( commandCode == 3 ) {
			// rotateRight
			rotate( 1, commandParameter );
			}
		else if( commandCode == 4 ) {
		    // flashLight
			flashLight( commandParameter );
			}
		else {
			// nop
			}
		
 		}

	}



void writeNewLine() {
	putchar( 13 );
	putchar( 10 );
	// read the echo
	//skipBytes( 2 );
	}



void halfDuplex() {
	putchar( 'H' );
	putchar( 'D' );

	writeNewLine();
	}



void fullDuplex() {
	putchar( 'F' );
	putchar( 'D' );

	writeNewLine();
	}



void setInitialVelocities() {
	// rotational velocity
	putchar( 'R' );
	putchar( 'V' );

	putchar( ' ' );

	putchar( '8' );
	putchar( '0' );
	
	writeNewLine();

	// translational velocity
	putchar( 'T' );
	putchar( 'V' );

	putchar( ' ' );

	putchar( '6' );
	putchar( '0' );
	putchar( '0' );
	
	writeNewLine();
	}



void halt() {
	// halt rotation
	putchar( 'R' );
	putchar( 'H' );
	writeNewLine();

	// halt translation
	putchar( 'T' );
	putchar( 'H' );
	writeNewLine();
	}



void rotateLimp() {
	// set rotation limp
	putchar( 'R' );
	putchar( 'L' );
	writeNewLine();
	}



void forwardAvoid( char inAmount ) {
	netForwardTranslation = 0;
	netNegativeTranslation = 0;
	avoidNumRotations = 0;

	// used to pick rotateDirection, since we only
	// want to pick the direction once for each translation command executed
	// to avoid getting stuck in short obstacle avoidance loops
	firstHit = 1;

	
	movingForward = 0;
	
	while( netForwardTranslation < inAmount ) {

		// now our obstacle flag pin
		if( PORTB & 0x01 ) {
			// obstacle pin is high
			halt();
			movingForward = 0;

			if( firstHit ) {
				// check pin 4
				if( PORTB & 0x10 ) {
					// obstacle on left, so rotate right
					rotateDirection = 1;
					}
				else {
					// obstacle on right, so rotate left
					rotateDirection = 0;
					}
				firstHit = 0;
				}
			
			// while pin is high
			while( PORTB & 0x01 ) {
				// rotate 90 degrees
				rotate( rotateDirection, 64 );

				// alter rotation counters based on rotation direction
				if( rotateDirection ) {
					avoidNumRotations++;
					if( avoidNumRotations == 4 ) {
						// perform mod 4 on rotations
						avoidNumRotations = 0;
						}
					}
				else {
					avoidNumRotations--;
					if( avoidNumRotations == 255 ) {
						// perform mod 0 on rotations
						avoidNumRotations = 3;
						}
					}
				
				}

			// now we have a clear path ahead
			}
		
		
		if( !movingForward ) {
			// start moving forward
			putchar( 'T' );
			putchar( '+' );
			writeNewLine();
			movingForward = 1;
			}

		// delay to allow for 10 cm of motion
 
		// we are traveling at 600 encoder counts per second
		// and 1cm is 140 encoder counts, so we need to delay for 2.3333sec
		delay_s( 2 );
		delay_ms( 78 );
		delay_ms( 255 );

		
		if( avoidNumRotations == 0 ) {
			// we're moving forward

			if( netNegativeTranslation == 0 ) {
				netForwardTranslation++;
				}
			else {
				// we have negative translation points to use up
				netNegativeTranslation--;
				}
			}
		else if( avoidNumRotations == 2 ) {
			// we're moving backward

			// handle negative values, which are unsupported
			// by the compiler and wrap back to 255
			if( netForwardTranslation != 0 ) {
				netForwardTranslation--;
				}
			else {
				// we're moving back behind where we started.
				netNegativeTranslation++;
				}
			}
		// else we're moving to the left or right, so
		// no net forward translation


		// now check if we can rotate back towards forward by one step
		if( avoidNumRotations != 0 ) {
			halt();
			movingForward = 0;
			
			// rotate back to the right by 90 degrees
			rotate( 1 - rotateDirection, 64 );
			if( rotateDirection ) {
				avoidNumRotations--;
				}
			else {
				avoidNumRotations++;
				if( avoidNumRotations == 4 ) {
					// perform mod 4 on rotations
					avoidNumRotations = 0;
					}
				}
			}
		
		// go on to next iteration
		}

	// we have moved forward by the desired amount
	halt();
	}



void rotate( char inRight, int inAmount ) {
	putchar( 'R' );
	if( inRight ) {
		putchar( '>' );
		}
	else {
		putchar( '<' );
		}
	putchar( ' ' );

	// convert from 1/256ths to 1/1024ths
	rotateWorkingInt = inAmount;
	rotateWorkingInt = rotateWorkingInt << 2;   // * 4
	
	writeSerialASCIIHex( rotateWorkingInt );

	writeNewLine();

	// wait for the rotation to finish
	// divide by 128, our rotation speed
	delayAmount = rotateWorkingInt >> 7;

	if( delayAmount > 0 ) {
		// never delay by 0 sec, since
		// it seems to be equivalent to delaying for 255 sec
		// (a c2c compiler bug?)
		delay_s( delayAmount );
		}
	else {
		// delay by less than 1 sec...
		// no reason to compute this tiny delay amount here.
		// simply delay by 500ms
		delay_ms( 250 );
		delay_ms( 250 );
		}

	// delay an additional 1/4 sec to correct for round-off error
	// in the above shift operation
	delay_ms( 250 );

	// go limp to avoide a shuddering problem on small rotations
	rotateLimp();
	
	// note that waiting for input from the B12 base
	// doesn't seem to be working, so we're doing this with delays.
	
	/*
	  putchar( 'W' );
	putchar( 'E' );
	putchar( ' ' );

	putchar( '8' );
	putchar( '1' );

	writeNewLine();
	
	waitForComBufferFlush();
	*/
	}



void flashLight( char inNumFlashes ) {
	for( flashCounter=0; flashCounter<inNumFlashes; flashCounter++ ) {
		delay_ms( 250 );
		delay_ms( 250 );

		// turn pin B3 on
		set_bit( PORTB, 3 );

		delay_ms( 250 );
		delay_ms( 250 );

		// turn pin B3 off
		clear_bit( PORTB, 3 );
		}
	}
	


void waitForComBufferFlush() {
	
	// send a user message to detect a flushed communications buffer
	putchar( 'U' );
	putchar( 'M' );
	putchar( ' ' );

	putchar( 'F' );
	putchar( 'F' );

	writeNewLine();
	
	// user messages are returned even though we're in half-duplex mode
	//	fullDuplex();
	
	// now wait for the user message to echo
	flushWorkingChar1 = getchar();
	flushWorkingChar2 = getchar();

	while( !( flushWorkingChar1 == 'F' && flushWorkingChar2 == 'F' ) ) {
		flushWorkingChar1 = flushWorkingChar2;

		flushWorkingChar2 = getchar();
		}

	//	halfDuplex();
	}



void getRemoteCommand() {

	strobeState = 1;
	strobeCounter = 0;
	set_bit( PORTB, 2 );
	
	// wait for pin B1 to become high (AND port B with 00000010)
	while( !( PORTB & 0x02 ) ) {
		// strobe our ready pin while we're waiting


		// delay for 1/8 sec while checking for start bit
		// this seems to be long enough for the camera's pin
		// to detect the change
		while( !( PORTB & 0x02 ) && strobeCounter < 125 ) {
			delay_ms( 1 );
			strobeCounter++;
			}
		
		strobeCounter = 0;
		
		if( strobeState ) {
			clear_bit( PORTB, 2 );
			}
		else {
			set_bit( PORTB, 2 );
			}
		
		strobeState = 1 - strobeState;
		}

	clear_bit( PORTB, 2 );

	
	// pin B1 is high

	// wait for 9ms to get to the 9-10ms into the middle of the start bit
	delay_ms( 9 );

	commandCode = read8BitValue();
	commandParameter = read8BitValue();

	//delay_s( 5 );
	
	// for now, simply return "forward" all the time
	// commandCode = 1;
	// commandParameter = 255;
	}



char read8BitValue() {

	bitsRead = 0;
	byteValue = 0;


	while( bitsRead < 8 ) {

		// delay 20ms to get to the middle of this bit
		delay_ms( 20 );

		if( PORTB & 0x02 ) {
			// high bit
			bitValue = 1;
			}
		else {
			// low bit
			bitValue = 0;
			}

		byteValue = ( byteValue << 1 ) | bitValue;

		bitsRead++;
		}

	return byteValue;
	}



// YUCK!  The second operand to a shift must be a numerical constant!
char shiftLeft( char inValue, char inShiftAmount ) {

	while( inShiftAmount > 0 ) {
		inValue << 1;
		inShiftAmount--;
		}
   			
	// Would rather use:
	// return inValue << inShiftAmount;
	}
