
#include "SerialPort.h"

#include <stdio.h>

int main() {

    printf( "Constructing serial port.\n" ); 
    SerialPort *port = new SerialPort( 4800, SerialPort::PARITY_NONE, 8, 1 );

    char *line = port->receiveLine();
    
    // specific to GPS unit
    port->sendLine( "ASTRAL" );

    while( line != NULL ) {
        printf( "received:  %s\n", line );

        delete [] line;
        line = port->receiveLine();
        }
    
    printf( "Deleting serial port.\n" ); 
    delete port;
    
    return 0;
    }
