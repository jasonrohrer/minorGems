/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.
 *
 * 2001-April-13   Jason Rohrer
 * Added interfaces for direct hardware serial setup and sends.
 *
 * 2001-April-15   Jason Rohrer
 * Added a new setup function for software serial (and settings),
 * and removed failed hardware serial functions.
 * Changed name of setup function.
 * Changed writeBuffer to take null-terminated strings only.
 * Discovered that having more than one array in a program
 * doesn't work, so removed the writeBuffer function.
 *
 * 2001-April-25   Jason Rohrer
 * Added an additional baud rate controled by a preprocessor
 * definition.
 */



/**
 * A collection of serial utility functions.
 *
 * @author Jason Rohrer
 */


// settings for software serial
// code copied from:
// http://www.iptel-now.de/HOWTO/PIC/pic.html

//RS232 settings
#pragma RS232_TXPORT PORTA
#pragma RS232_RXPORT PORTA
#pragma RS232_TXPIN  1
#pragma RS232_RXPIN  4

// switch BAUD rates for different applications
#ifndef B12_CONTROL
#pragma RS232_BAUD 9600
#endif

#ifdef B12_CONTROL
#pragma RS232_BAUD 19200
#endif

#pragma TRUE_RS232 1

//Timing settings
#pragma CLOCK_FREQ 10000000

// end copied code



/**
 * Sets up the chip for serial transmission.  Must be called
 * before calling any of the other serial routines.
 */
void serialSetup();



/**
 * Skips a specified number of input bytes on the serial port.
 *
 * @param inNumBytes the number of bytes to skip.
 */
void skipBytes( int inNumBytes );



/**
 * Reads bytes from the serial port and discards them
 * up to and including the first occurence of inByte.
 *
 * @param inByte the byte to wait for.
 */
void waitForByte( char inByte );
















