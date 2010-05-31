/*
 * Modification History
 *
 * 2003-March-28   Jason Rohrer
 * Created.
 */



#include "minorGems/io/serialPort/SerialPort.h"
#include "minorGems/util/stringUtils.h"



#include <stdio.h>



SerialPort::SerialPort( int inBaud, int inParity, int inDataBits,
                        int inStopBits ) {

    FILE *file = fopen( "gpscap.txt", "r" );

    mNativeObjectPointer = file;
    }


        
SerialPort::~SerialPort() {

    if( mNativeObjectPointer != NULL ) {
        FILE *file = (FILE *)mNativeObjectPointer;

        fclose( file );
        }
    }



int SerialPort::sendLine( char *inLine ) {
    return 1;
    }



char *SerialPort::receiveLine() {
    if( mNativeObjectPointer != NULL ) {
        FILE *file = (FILE *)mNativeObjectPointer;

        char *buffer = new char[500];

        // read up to first newline
        int index = 0;

        char lastCharRead = (char)getc( file );

        while( lastCharRead != '\n' && index < 499 ) {
            buffer[index] = lastCharRead;
            lastCharRead = (char)getc( file );
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







