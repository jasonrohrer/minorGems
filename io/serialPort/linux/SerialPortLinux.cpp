/*
 * Modification History
 *
 * 2003-February-17   Jason Rohrer
 * Created.
 *
 * 2003-April-4   Jason Rohrer
 * Added function for dumping the read buffer.
 */



#include "minorGems/io/serialPort/SerialPort.h"
#include "minorGems/util/stringUtils.h"


// Much of the code in this implementation was copied
// from the Serial Programming FAQ, by Gary Frefking
// http://en.tldp.org/HOWTO/Serial-Programming-HOWTO/



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
// baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>



class LinuxSerialPortObject {

    public:
        int mFileHandle;
        struct termios mOldTermIO;
    };



SerialPort::SerialPort( int inBaud, int inParity, int inDataBits,
                        int inStopBits ) {
    
    int fileHandle = open( "/dev/ttyS0", O_RDWR | O_NOCTTY );

    if( fileHandle < 0 ) {
        
        mNativeObjectPointer = NULL;        
        }
    else {

        LinuxSerialPortObject *serialPortObject = new LinuxSerialPortObject();
        serialPortObject->mFileHandle = fileHandle;

        mNativeObjectPointer = (void *)serialPortObject;
                
        
        struct termios newTermIO;

        //save current serial port settings
        tcgetattr( fileHandle, &( serialPortObject->mOldTermIO ) );
        
        // clear struct for new port settings
        bzero( &newTermIO, sizeof( newTermIO ) ); 

        unsigned int baudRate;

        switch( inBaud ) {
            case 1200:
                baudRate = B1200;
                break;
            case 2400:
                baudRate = B2400;
                break;
            case 4800:
                baudRate = B4800;
                break;
            case 9600:
                baudRate = B9600;
                break;
            case 19200:
                baudRate = B19200;
                break;
            case 38400:
                baudRate = B38400;
                break;
            case 57600:
                baudRate = B57600;
                break;
            default:
                break;
            }

        unsigned int parity = 0;

        switch( inParity ) {
            case SerialPort::PARITY_NONE:
                // disable parity
                parity = 0;
                break;
            case SerialPort::PARITY_EVEN:
                // enable parity, defaults to even
                parity = PARENB;
                break;
            case SerialPort::PARITY_ODD:
                // enable parity, and set to odd
                parity = PARENB | PARODD;
                break;
            default:
                break;
            }

        unsigned int dataBits = 0;
        switch( inDataBits ) {
            case 5:
                dataBits = CS5;
                break;
            case 6:
                dataBits = CS6;
                break;
            case 7:
                dataBits = CS7;
                break;
            case 8:
                dataBits = CS8;
                break;
            default:
                break;
            }
        
        unsigned int stopBits = 0;
        switch( inStopBits ) {
            case 1:
                stopBits = 0;
                break;
            case 2:
                stopBits = CSTOPB;
                break;
            default:
                break;
            }
        
        newTermIO.c_cflag = baudRate | parity | dataBits |
                            stopBits | CLOCAL | CREAD;

        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          IGNCR   : ignore CR, so only one line is read when other
                    machine sends CRLF
          otherwise make device raw (no other input processing)
        */
        newTermIO.c_iflag = IGNPAR | ICRNL | IGNCR;

        /*
          Raw output.
        */
        newTermIO.c_oflag = 0;
        
        /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to
          calling program
        */
        newTermIO.c_lflag = ICANON;

        /*
          now clean the modem line and activate the settings for the port
        */
        tcflush( fileHandle, TCIFLUSH );
        tcsetattr( fileHandle, TCSANOW, &newTermIO );
        }
    }


        
SerialPort::~SerialPort() {

    if( mNativeObjectPointer != NULL ) {
        LinuxSerialPortObject *serialPortObject =
            (LinuxSerialPortObject *)mNativeObjectPointer;

        int fileHandle = serialPortObject->mFileHandle;


        tcsetattr( fileHandle, TCSANOW, &( serialPortObject->mOldTermIO ) );
    
    
        delete serialPortObject;

        }
    }



int SerialPort::sendLine( char *inLine ) {
    if( mNativeObjectPointer != NULL ) {
        LinuxSerialPortObject *serialPortObject =
            (LinuxSerialPortObject *)mNativeObjectPointer;
        int fileHandle = serialPortObject->mFileHandle;
        
        int stringLength = strlen( inLine );
        int numWritten = write( fileHandle, inLine, stringLength );

        char *endLineString = "\n";

        int numWrittenEndLine = write( fileHandle, endLineString, 1 );
        
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
        LinuxSerialPortObject *serialPortObject =
            (LinuxSerialPortObject *)mNativeObjectPointer;
        int fileHandle = serialPortObject->mFileHandle;

        char *buffer = new char[500];

        int numRead = read( fileHandle, buffer, 500 );

        char *returnString;
        
        if( numRead != -1 ) {
            buffer[ numRead ] = '\0';

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
       LinuxSerialPortObject *serialPortObject =
           (LinuxSerialPortObject *)mNativeObjectPointer;
       int fileHandle = serialPortObject->mFileHandle;

       // from man page:
       // flushes data received but not read
       tcflush( fileHandle, TCIFLUSH );
       }
    }









