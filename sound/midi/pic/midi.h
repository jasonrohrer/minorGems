/*
 * Modification History
 *
 * 2001-June-4   Jason Rohrer
 * Created.
 *
 * 2001-June-5   Jason Rohrer
 * Changed back to non-inverted serial output.
 * This output type has been tested and works with MIDI.
 */



/**
 * A collection of midi utility functions.
 *
 * Uses C2C serial to send MIDI.  Note that
 * MIDI operates at TTL [0-5] levels, not RS232 [0-10] levels, so
 * no external MAX-232 chip is needed.
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

//

// BAUD for MIDI is 31.25 Kbps
#pragma RS232_BAUD 31250


// inverted bit values for midi
#pragma TRUE_RS232 1

//Timing settings
#pragma CLOCK_FREQ 10000000

// end copied code



/**
 * Sets up the chip for midi transmission.  Must be called
 * before calling any of the other midi routines.
 */
void midiSetup();



/**
 * Turns a note on.
 *
 * @param inChannel the channel number, in [0,15]
 *   (which correspond to midi channels 1-16).
 * @param inKey the key number, in [0,127].
 *   Middle C is 60.
 * @param inVelocity the note velocity, in [0,127].
 */
void turnNoteOn( char inChannel, char inKey, char inVelocity );



/**
 * Turns a note off.
 *
 * @param inChannel the channel number, in [0,15]
 *   (which correspond to midi channels 1-16).
 * @param inKey the key number, in [0,127].
 *   Middle C is 60.
 * @param inVelocity the note velocity, in [0,127].
 */
void turnNoteOff( char inChannel, char inKey, char inVelocity );
















