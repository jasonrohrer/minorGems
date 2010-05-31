/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.
 */



/**
 * A collection of utility functions for dealing with ASCII coded hex values.
 *
 * @author Jason Rohrer
 */



/**
 * Converts a numerical value to an ASCII hex representation.
 *
 * Example:
 * convertToHexASCII( 10 ) returns 'A'.
 *
 * @param inNumericalValue a value in [0-15].
 *
 * @return the value converted to hex in ASCII.
 */
char convertIntToASCIIHex( char inNumericalValue );



/**
 * Converts an ASCII hex representation to a numerical value.
 *
 * Example:
 * convertToHexASCII( 'A' ) returns 10.
 *
 * @param inChar character of ascii hex.
 *
 * @return a integer value in [0..15].
 */
char convertASCIIHexNibbleToInt( char inChar );



/**
 * Converts an ASCII hex representation to a numerical value.
 *
 * Example:
 * convertToHexASCII( "000A" ) returns 10.
 *
 * @param inCharA high-order character of ascii hex
 *   representation of a 16-bit number.
 * @param inCharB character of ascii hex
 *   representation of a 16-bit number.
 * @param inCharC character of ascii hex
 *   representation of a 16-bit number.
 * @param inCharD low-order character of ascii hex
 *   representation of a 16-bit number.
 *
 * @return the value of the 4 ascii characters as a 16-bit integer.
 */
int convertASCIIHexToInt( char inCharA, char inCharB, char inCharC, char inCharD );



/**
 * Reads a 16-bit integer from the serial port in ASCII hex format.
 *
 * @return the integer, converted to standard 16-bit format.
 */
int readSerialASCIIHex();



/**
 * Writes a 16-bit integer to the serial port in ASCII hex format.
 *
 * @param inInteger the 16-bit integer to write as 4 ASCII hex characters.
 */
void writeSerialASCIIHex( int inInteger );
