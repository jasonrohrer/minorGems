/*
 * Modification History
 *
 * 2003-April-4   Jason Rohrer
 * Added function for dumping the read buffer.
 */

//=============================================================================
//  General component library for WIN32
//  Copyright (C) 2000, UAB BBDSoft ( http://www.bbdsoft.com/ )
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied. Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted 
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//
//  The author of this program may be contacted at developers@bbdsoft.com
//=============================================================================


#ifndef _COMPORT_
#define _COMPORT_

//-----------------------------------------------------------------------------
class COMPort
{
public:

    enum Parity
    {
         None = 0
       , Odd
       , Even
       , Mark
       , Space
    };

    enum DataBits
    {
         db4 = 4
       , db5
       , db6
       , db7
       , db8
    };

    enum StopBits
    {
       sb1 = 0,
       sb15,
       sb2
    };

    enum BitRate
    {
       br110 = 110,
       br300 = 300,
       br600 = 600,
       br1200 = 1200,
       br2400 = 2400,
       br4800 = 4800,
       br9600 = 9600,
       br19200 = 19200,
       br38400 = 38400,
       br56000 = 56000,
       br57600 = 57600,
       br115200 = 115200,
       br256000 = 256000
    };


    // for function getModemSignals
    struct MSPack
    {
      unsigned char DTR : 1;
      unsigned char RTS : 1;
      unsigned char     : 2;
      unsigned char CTS : 1;
      unsigned char DSR : 1;
      unsigned char RI  : 1;
      unsigned char DCD : 1;
    };

    COMPort ( const char * const portName );
    ~COMPort ();

    // I/O operations
    char read ();
    COMPort & write (const char inChar);

    unsigned long read  ( void *
                        , const unsigned long count
                        );

    unsigned long write ( const void *
                        , const unsigned long count
                        );
    // dumps unread characters in the receive buffer
    void dumpReceiveBuffer();

        
    COMPort& setBitRate ( unsigned long Param );
    unsigned long bitRate () const;

    COMPort& setParity ( Parity Param );
    Parity parity () const;

    COMPort& setDataBits ( DataBits Param );
    DataBits dataBits () const;

    COMPort& setStopBits ( StopBits Param );
    StopBits stopBits () const;

    COMPort & setHandshaking ( bool inHandshaking = true );

    COMPort& setLineCharacteristics ( char * Param );

    unsigned long getMaximumBitRate () const;

    COMPort & setxONxOFF ( bool Param = true);
    bool isxONxOFF () const;

    MSPack getModemSignals () const;

    COMPort& setBlockingMode ( unsigned long inReadInterval  = 0
                             , unsigned long inReadMultiplyer = 0
                             , unsigned long inReadConstant = 0
                             );

protected:

private:

   // disable copy constructor and assignment operator
   COMPort (const COMPort &);
   COMPort& operator= (const COMPort &);

   void getState () const;
   COMPort& setState ();

   unsigned thePortHandle;
   char * theDCB;

}; // End of COMPort class declaration

#endif
