/*
 * Modification History
 *
 * 2001-April-11   Jason Rohrer
 * Created.
 *
 * 2001-April-15   Jason Rohrer
 * Made hex conversion steps more explicit, whcih seems to have fixed the
 * conversion problems ( the compiler probably does not do well with
 * complicated 16-bit integer expressions ).
 *
 * 2001-May-14   Jason Rohrer
 * Replaced switch statements with if blocks to
 * reduce code size.
 */

#include "serial.h"

// global variables for convertASCIIHexToInt
int nibbleA = 0;
int nibbleB = 0;
int nibbleC = 0;
int nibbleD = 0;
int completeInt = 0;

// global temp char variable
char tempChar;


int convertASCIIHexToInt( char inCharA, char inCharB,
						  char inCharC, char inCharD ) {

	// convert nibbles and shift them into position in
	// an int
	nibbleA = convertASCIIHexNibbleToInt( inCharA );
	nibbleA = nibbleA << 12;

	nibbleB = convertASCIIHexNibbleToInt( inCharB );
	nibbleB = nibbleB << 8;

	nibbleC = convertASCIIHexNibbleToInt( inCharC );
	nibbleC = nibbleC << 4;

	nibbleD = convertASCIIHexNibbleToInt( inCharD );
	// no shifting for low order nibble

	completeInt = nibbleA;
	completeInt = completeInt | nibbleB;
	completeInt = completeInt | nibbleC;
	completeInt = completeInt | nibbleD;

	return completeInt;
	}



int readSerialASCIIHex() {
	// read the 4 character response, and convert it to a 16-bit int
	return convertASCIIHexToInt( getchar(), getchar(),
								 getchar(), getchar() );
	}



void writeSerialASCIIHex( int inInteger ) {

	// use truncation to our advantage
	
	// high order nibble first
	putchar( convertIntToASCIIHex( inInteger >> 12 ) );
	putchar( convertIntToASCIIHex( inInteger >> 8 ) );
	putchar( convertIntToASCIIHex( inInteger >> 4 ) );
	// low order nibble last
	putchar( convertIntToASCIIHex( inInteger ) );
	}



char convertASCIIHexNibbleToInt( char inChar ) {
	// space optimization:
	// don't use elses
	// this gets rid of an extra goto per else
	// (we don't need elses, since we're returning
	// in the body of each if)
	
	if( inChar == '1' ) {
		return 1;
		}
	if( inChar == '2' ) {
		return 2;
		}
	if( inChar == '3' ) {
		return 3;
		}
	if( inChar == '4' ) {
		return 4;
		}
	if( inChar == '5' ) {
		return 5;
		}
	if( inChar == '6' ) {
		return 6;
		}
	if( inChar == '7' ) {
		return 7;
		}
	if( inChar == '8' ) {
		return 8;
		}
	if( inChar == '9' ) {
		return 9;
		}
	if( inChar == 'A' ) {
		return 10;
		}
	if( inChar == 'B' ) {
		return 11;
		}
	if( inChar == 'C' ) {
		return 12;
		}
	if( inChar == 'D' ) {
		return 13;
		}
	if( inChar == 'E' ) {
		return 14;
		}
	if( inChar == 'F' ) {
		return 15;
		}
	
	// default, or when inNumericalValue == '0'
	return 0;

	
	return tempChar;
	}



char convertIntToASCIIHex( char inNumericalValue ) {
	// space optimization:
	// don't use elses
	// this gets rid of an extra goto per else
	// (we don't need elses, since we're returning
	// in the body of each if)
	
	if( inNumericalValue == 1 ) {
		return '1';
		}
	if( inNumericalValue == 2 ) {
		return '2';
		}
	if( inNumericalValue == 3 ) {
		return '3';
		}
	if( inNumericalValue == 4 ) {
		return '4';
		}
	if( inNumericalValue == 5 ) {
		return '5';
		}
	if( inNumericalValue == 6 ) {
		return '6';
		}
	if( inNumericalValue == 7 ) {
		return '7';
		}
	if( inNumericalValue == 8 ) {
		return '8';
		}
	if( inNumericalValue == 9 ) {
		return '9';
		}
	if( inNumericalValue == 10 ) {
		return 'A';
		}
	if( inNumericalValue == 11 ) {
		return 'B';
		}
	if( inNumericalValue == 12 ) {
		return 'C';
		}
	if( inNumericalValue == 13 ) {
		return 'D';
		}
	if( inNumericalValue == 14 ) {
		return 'E';
		}
	if( inNumericalValue == 15 ) {
		return 'F';
		}

	// default, or when inNumericalValue == 0
	return '0';
	
	}





