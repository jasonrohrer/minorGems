/*
 * Modification History
 *
 * 2001-April-25   Jason Rohrer
 * Created.
 *
 * 2001-April-26   Jason Rohrer
 * Added a description.
 */

/**
 * A PIC program that executes a simple obstacle avoidance routine
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
//#include "asciihex.h"


// the number of sonar units
char numUnits = 4;
// the index of the current unit 
char unitIndex = 0;
// the echo value of the current unit
int echoTime = 0;

// set to 1 if one of the sensors has produced a reading that is too close
char tooClose = 0;


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
 * Starts translating the robot forward.
 */
void forward();


/**
 * Starts rotating the robot.
 */
void rotate();


/**
 * Writes a newline sequence (CR LF) to the serial port.
 */
void writeNewLine();


// end prototypes



/**
 * Main function.
 */
void main( void ) {

	// set B0 to input (flag from sonar unit)
	set_bit( STATUS, RP0 );
	set_bit( TRISB, 0 ); 
	clear_bit( STATUS, RP0 );
	
	serialSetup();

	/*
	  clear serial "junk" by sending several characters
	   with delays.
	*/
	
	delay_ms( 100 );

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

		// stop whatever activity is left over from the last loop
		halt();
		
		// start rotating
		rotate();
		
		// while input pin is still high
		while( PORTB & 0x01 ) {
			// do nothing
			nop();
			}
		
		// pin has gone low, so there is an opening in front of us

		// stop the rotation
		halt();

		// start going forward
		forward();

		// while input pin is still low
		while( !( PORTB & 0x01 ) ) {
			// do nothing
			nop();
			}

		// pin has gone high, so we should repeat this loop
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

	putchar( '4' );
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



void forward() {
	putchar( 'T' );
	putchar( '+' );
	writeNewLine();
	}



void rotate() {
	putchar( 'R' );
	putchar( '+' );
	writeNewLine();
	}



