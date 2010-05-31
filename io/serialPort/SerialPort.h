/*
 * Modification History
 *
 * 2003-February-17   Jason Rohrer
 * Created.
 *
 * 2003-April-4   Jason Rohrer
 * Added function for dumping the read buffer.
 */



#ifndef SERIAL_PORT_INCLUDED
#define SERIAL_PORT_INCLUDED



/**
 * Serial port.
 *
 * Note:  Implementation for the functions defined here is provided
 *   separately for each platform (in the mac/ linux/ and win32/ 
 *   subdirectories).
 *
 * @author Jason Rohrer
 */ 
class SerialPort {


        
    public:


        
        static const int PARITY_NONE  = 0;
        static const int PARITY_EVEN  = 1;
        static const int PARITY_ODD   = 2;
        


        /**
         * Constructs a serial port.
         *
         * @param inBaud the baud rate.
         * @param inParity the parity, one of SerialPort:: PARITY_NONE,
         *   PARITY_EVEN, or PARITY_ODD.
         * @param inDataBits the number of data bits, 5, 6, 7, or 8.
         * @param inStopBits the number of stop bits, 1 or 2.
         */
        SerialPort( int inBaud, int inParity, int inDataBits, int inStopBits );


        
        ~SerialPort();



        /**
         * Sends a line of text through this serial port.
         *
         * @param inLine the \0-terminated line of text to send.
         *   Should not contain newline characters.
         *   Must be destroyed by caller if non-const.
         *
         * @return 1 if the line was sent successfully,
         *   or -1 for a port error.
         */
        int sendLine( char *inLine );

        
        
        /**
         * Receives a line of text from this serial port.
         *
         * @return the read line as a \0-terminated string with end of
         *   line characters included, or NULL for a port error.
         *   Must be destroyed by caller if non-NULL.
         */
        char *receiveLine();



        /**
         * Discards all characters in the receive buffer, including
         * unread characters.
         *
         * Can be used to recover from buffer overflow problems.
         */
        void dumpReceiveBuffer();

        

    private:



        /**
         * Used for platform-specific implementations.
         */
        void *mNativeObjectPointer;


        
    };



#endif
