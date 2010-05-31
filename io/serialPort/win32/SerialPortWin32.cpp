/*
 * Modification History
 *
 * 2003-March-24   Jason Rohrer
 * Created.
 *
 * 2003-March-26   Jason Rohrer
 * Fixed a line parsing bug (lines end with newline, not CR).
 *
 * 2003-April-4   Jason Rohrer
 * Added function for dumping the read buffer.
 */



#include "minorGems/io/serialPort/SerialPort.h"
#include "minorGems/util/stringUtils.h"


// For now, we are using BBDSoft's COM port code
#include "COMPort.h"



SerialPort::SerialPort( int inBaud, int inParity, int inDataBits,
                        int inStopBits ) {

    COMPort *port = new COMPort( "COM1" );
    port->setHandshaking( false );

    unsigned long baudRate = COMPort::br2400;

    switch( inBaud ) {
        case 1200:
            baudRate = COMPort::br1200;
            break;
        case 2400:
            baudRate = COMPort::br2400;
            break;
        case 4800:
            baudRate = COMPort::br4800;
            break;
        case 9600:
            baudRate = COMPort::br9600;
            break;
        case 19200:
            baudRate = COMPort::br19200;
            break;
        case 38400:
            baudRate = COMPort::br38400;
            break;
        case 57600:
            baudRate = COMPort::br57600;
            break;
        default:
            break;
        }

    port->setBitRate( baudRate );
    
    switch( inParity ) {
        case SerialPort::PARITY_NONE:
            port->setParity( COMPort::None );
            break;
        case SerialPort::PARITY_EVEN:
            port->setParity( COMPort::Even );
            break;
        case SerialPort::PARITY_ODD:
            port->setParity( COMPort::Odd );
            break;
        default:
            port->setParity( COMPort::None );
            break;
        }

    switch( inDataBits ) {
        case 5:
            port->setDataBits( COMPort::db5 );
            break;
        case 6:
            port->setDataBits( COMPort::db6 );
            break;
        case 7:
            port->setDataBits( COMPort::db7 );
            break;
        case 8:
            port->setDataBits( COMPort::db8 );
            break;
        default:
            port->setDataBits( COMPort::db8 );
            break;
            }
        
    switch( inStopBits ) {
        case 1:
            port->setStopBits( COMPort::sb1 );
            break;
        case 2:
            port->setStopBits( COMPort::sb2 );
            break;
        default:
            port->setStopBits( COMPort::sb1 );
            break;
        }

    mNativeObjectPointer = (void *)port;
    }


        
SerialPort::~SerialPort() {

    if( mNativeObjectPointer != NULL ) {
        COMPort *port = (COMPort *)mNativeObjectPointer;
        delete port;
        }
    }



int SerialPort::sendLine( char *inLine ) {
    if( mNativeObjectPointer != NULL ) {
        COMPort *port = (COMPort *)mNativeObjectPointer;
        
        int stringLength = strlen( inLine );
        int numWritten = port->write( (void *)inLine, stringLength );

        char *endLineString = "\n";

        int numWrittenEndLine = port->write( (void *)endLineString, 1 );
        
        if( numWritten == stringLength &&
            numWrittenEndLine == 1 ) {
            return 1;
            }
        else {
            return -1;
            }
        }
    else {
        return -1;
        }
    }



char *SerialPort::receiveLine() {
    if( mNativeObjectPointer != NULL ) {
        COMPort *port = (COMPort *)mNativeObjectPointer;

        char *buffer = new char[500];

        // read up to first newline
        int index = 0;

        char lastCharRead = port->read();

        while( lastCharRead != '\n' && index < 499 ) {
            buffer[index] = lastCharRead;
            lastCharRead = port->read();
            index++;    
            }

        
        char *returnString;
        
        if( index > 0 ) {
            buffer[ index ] = '\0';

            returnString = stringDuplicate( buffer );
            }
        else {
            returnString = NULL;
            }
        
        delete [] buffer;
        return returnString;
        }
    else {
        return NULL;
        }
    }



void SerialPort::dumpReceiveBuffer() {
    if( mNativeObjectPointer != NULL ) {
        COMPort *port = (COMPort *)mNativeObjectPointer;

        port->dumpReceiveBuffer();
        }
    }







