/*
 * Modification History
 *
 * 2001-April-29   Jason Rohrer
 * Created.
 *
 * 2001-April-30   Jason Rohrer
 * Fixed a bug in input reading code.
 *
 * 2001-May-2   Jason Rohrer
 * Changed the order of operations.  Now sends a command and
 * then waits for a ready signal before returning.
 * Added support for the nop move.
 *
 * 2001-May-8   Jason Rohrer
 * Changed to insert a 20ms positive edge before each bit.
 * Changed back to not use positive edges before each pulse.
 *
 * 2001-May-10   Jason Rohrer
 * Removed many debugging print statements.
 *
 * 2001-May-13   Jason Rohrer
 * Added support for the flashLight command.
 * Fixed a potential bug in that we were not waiting
 * for the camera's response before closing the socket
 * after sending output pin data.
 */
 
 
#ifndef AXIS_MOVE_PROCESSOR_INCLUDED
#define AXIS_MOVE_PROCESSOR_INCLUDED 

#include "minorGems/graphics/converters/JPEGImageConverter.h"

#include "MoveProcessor.h"

#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketClient.h"
#include "minorGems/network/SocketStream.h"

#include "minorGems/network/HostAddress.h"

#include <string.h>
#include <stdio.h>

/**
 * Implementaion of MoveProcessor that sends move commands
 * though an Axis camera.
 *
 * Note that this move processor only supports forward (positive)
 * translations.  Negative translations are ignored.
 *
 * @author Jason Rohrer
 */
class AxisMoveProcessor : public MoveProcessor {
	
	public:

		/**
		 * Constructs an AxisMoveProcessor.
		 *
		 * @param inCameraAddress the address of the axis camera server.
		 *   Is NOT destroyed when this class is destroyed.
		 */
		AxisMoveProcessor( HostAddress *inCameraAddress );

		
		// implements MoveProcessor interface
		virtual void nop();
		virtual void translateForward( double inDistanceInCentimeters );  
		virtual void rotateClockwise( double inRotationFraction );
		virtual void flashLight( int inNumFlashes );

		
	private:
		Socket *mCameraSocket;
		HostAddress *mCameraAddress;
		SocketStream *mSocketStream;


		/**
		 * Waits for a ready signal from the camera and
		 * then sends a message.
		 *
		 * @param inCommandTypeByte the first byte of the message,
		 *   specifying the command type.
		 * @param inParameterByte the second byte of the message,
		 *   specifying the parameter for the command.
		 */
		void sendMessage( unsigned char inCommandTypeByte,
						  unsigned char inParameterByte );

		/**
		 * Waits for a ready signal from the camera.
		 */
		void waitForReady();
	};



inline AxisMoveProcessor::AxisMoveProcessor( HostAddress *inCameraAddress )
	: mCameraAddress( inCameraAddress ) {

	}



inline void AxisMoveProcessor::waitForReady() {

	char ready = false;
	char *pinReadString  =
		"/axis-cgi/io/input.cgi?monitor=1";


	Socket *mCameraSocket =
		SocketClient::connectToServer( mCameraAddress );

	if( mCameraSocket == NULL ) {
		printf( "connection to camera " );
		mCameraAddress->print();
		printf( " failed\n" );

		return;
		}

	// else we connected successfully
	
	mSocketStream = new SocketStream( mCameraSocket );


	// buffer for the request string
	// enough room for the command string
	int commandStringLength = strlen( pinReadString );
	char *requestBuffer = new char[ 99 + commandStringLength ];


	sprintf( requestBuffer,
			 "GET %s HTTP/1.0 \r\n\r\n",
			 pinReadString );
	
	// printf( "Sending URL:\n" );
	// printf( "%s", requestBuffer );

	// send our request buffer through the socket
	mSocketStream->write( (unsigned char *)requestBuffer,
						  strlen( requestBuffer ) );

	unsigned char *charBuffer = new unsigned char[1];

	// the camera will respond with a stream of pin readings
	
	// look for the string "IO:/"

	// printf( "waiting for ready signal from PIC\n" );
	while( !ready ) {
			
		mSocketStream->read( charBuffer, 1 );
		// printf( "%c", charBuffer[0] );

		if( charBuffer[0] == 'I' ) {
			// the start of an IO encoding

			// need to wait for 3 bytes after I
			// before looking for '/'
			
			// skip the next 3 bytes
			mSocketStream->read( charBuffer, 1 );   // skip O
			// printf( "%c", charBuffer[0] );
			mSocketStream->read( charBuffer, 1 );   // skip 0
			// printf( "%c", charBuffer[0] );
			mSocketStream->read( charBuffer, 1 );   // skip :
			/// printf( "%c", charBuffer[0] );
			
			// examine the next byte
			mSocketStream->read( charBuffer, 1 );
			// printf( "%c", charBuffer[0] );

			if( charBuffer[0] == '/' ) {
				// pin is on
				ready = true;
				}
			}
		}

	delete [] charBuffer;
		
	delete [] requestBuffer;


	// close the socket, since the camera doesn't seem
	// to keep it alive anyway
	delete mSocketStream;
	delete mCameraSocket;
	}



inline void AxisMoveProcessor::sendMessage(
	unsigned char inCommandTypeByte,
	unsigned char inParameterByte ) {


	// send the command bytes
	
	
	/*
	 * Protocol:
	 * Send each bit as a 10ms pulse.
	 * The start bit is always high, followed by 16 non-inverted
	 * data bits.
	 */

	/*
	 * HTTP encoding:
	 * The following example string:
	 * /axis-cgi/io/output.cgi?action=1:/300\500/300\
	 * encodes turning pin 1 on for 300ms, then off for 500ms,
	 * then on again for 300ms, and finally leaving the pin
	 * in the off state.
	 *
	 * Note that in our actual string constants, we need
	 * to escape the "\" character as "\\".
	 */

	char *commandString = new char[299];

	// print the start of the command string
	sprintf( commandString, "/axis-cgi/io/output.cgi?action=1:" );
	
	// now append the bits
	char *bitEncodingBuffer = new char[5];

		
	// the first bit is always high
	sprintf( bitEncodingBuffer, "/10" );
	
	// append to our growing command string
	strcat( commandString, bitEncodingBuffer );
	  
	int i, j;

	unsigned char *bytes = new unsigned char[2];
	// first the command type byte
	bytes[0] = inCommandTypeByte;
	// next the parameter byte
	bytes[1] = inParameterByte;

	for( j=0; j<2; j++ ) {
		for( i=0; i<8; i++ ) {
			
			if( ( bytes[j] >> ( 7 - i ) ) & 0x01 == 1 ) {
				// a high byte
				sprintf( bitEncodingBuffer, "/10" );
				}
			else {
				// a low byte
				sprintf( bitEncodingBuffer, "\\10" );
				}
			// append to our growing command string
			strcat( commandString, bitEncodingBuffer );
			}
		}

	// finally, leave the output port in the off position
	sprintf( bitEncodingBuffer, "\\" );
	// append to our growing command string
	strcat( commandString, bitEncodingBuffer );

	delete [] bytes;
	delete [] bitEncodingBuffer;
	
	// now our command URL string is ready

	// connect to the camera
	Socket *mCameraSocket =
		SocketClient::connectToServer( mCameraAddress );

	if( mCameraSocket == NULL ) {
		printf( "connection to camera " );
		mCameraAddress->print();
		printf( " failed\n" );

		return;
		}

	// else we connected successfully
	
	mSocketStream = new SocketStream( mCameraSocket );


	// buffer for the request string
	// enough room for the command string
	int commandStringLength = strlen( commandString );
	char *requestBuffer = new char[ 99 + commandStringLength ];


	sprintf( requestBuffer,
			 "GET %s HTTP/1.0 \r\n\r\n",
			 commandString );
	
	// printf( "Sending URL:\n" );
	// printf( "%s", requestBuffer );

	// send our request buffer through the socket
	mSocketStream->write( (unsigned char *)requestBuffer,
						  strlen( requestBuffer ) );	
	
	delete []requestBuffer;
	delete [] commandString;


	// read any response data (basically, an HTTP OK message)
	// this assures that the command has been received and
	// sent through the output port before we move on
	
	unsigned char *responseBuffer = new unsigned char[50];
	int numRead = 50;
	while( numRead == 50 ) {
		numRead = mSocketStream->read( responseBuffer, 50 );
		responseBuffer[ 49 ] = '\0';
		}
	delete [] responseBuffer;
	
	
	// close the socket, since the camera doesn't seem
	// to keep it alive anyway
	delete mSocketStream;
	delete mCameraSocket;


	// finally, wait for the move to finish
	// (wait for the ready pin to become high)
	waitForReady();
	}



inline void AxisMoveProcessor::nop() {

	sendMessage( 0, 0 );
	}



inline void AxisMoveProcessor::translateForward(
	double inDistanceInCentimeters ) {

	if( inDistanceInCentimeters > 0 ) {
		sendMessage( 1, (unsigned char)inDistanceInCentimeters );
		}
	// else ignore a negative translation value
	
	}



inline void AxisMoveProcessor::rotateClockwise(
	double inRotationFraction ) {

	if( inRotationFraction >= 0 ) {
		// clockwise

		sendMessage( 3, (unsigned char)( inRotationFraction * 255 ) );
		}
	else {
		// counter-clockwise
		sendMessage( 2, (unsigned char)( -inRotationFraction * 255 ) );
		}
	}



inline void AxisMoveProcessor::flashLight( int inNumFlashes ) {
	if( inNumFlashes > 255 ) {
		inNumFlashes = 255;
		}
	else if( inNumFlashes < 0 ) {
		inNumFlashes = 0;
		}
	
	sendMessage( 4, (unsigned char)( inNumFlashes ) );
	}



#endif







