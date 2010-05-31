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
   #include "ComPort.h"
#endif

#ifndef _WINDOWS_
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
#endif

#ifndef _STDEXCEPT_
   #include <stdexcept>
#endif

using namespace std;

//----------------------------------------------------------------------------
COMPort::COMPort ( const char * const portName )
  : theDCB (NULL)
{

thePortHandle = (unsigned ) CreateFile ( portName
                                       , GENERIC_READ | GENERIC_WRITE
                                       , 0
                                       , NULL
                                       , OPEN_EXISTING
                                       , FILE_FLAG_NO_BUFFERING
                                       , NULL
                                       );
if (thePortHandle == HFILE_ERROR)
{
   throw runtime_error ("COMPort: failed to open.");
} // endif

theDCB = new char [sizeof(DCB)];
getState();
setBlockingMode();
setHandshaking();

} // end constructor


//----------------------------------------------------------------------------
COMPort::~COMPort()
{

delete [] theDCB;

// close serial port device
if (CloseHandle ((HANDLE)thePortHandle) == FALSE )
{
   throw runtime_error ("COMPort: failed to close.");   
} // endif

} // end destructor


//----------------------------------------------------------------------------
void COMPort::getState () const
{

if (!GetCommState ( (HANDLE) thePortHandle
                  , (LPDCB) theDCB
                  )
   )
{
   throw runtime_error ("COMPort: could not retrieve serial port state.");   
} // endif

} // end COMPort::getState () const


//----------------------------------------------------------------------------
COMPort& COMPort::setState ()
{

if (!SetCommState ( (HANDLE) thePortHandle
                  , (LPDCB) theDCB
                  )
   )
{
   throw runtime_error ("COMPort: could not modify serial port state.");   
} // endif

return *this;
} // end COMPort::setState ()


//-----------------------------------------------------------------------------
COMPort& COMPort::setBitRate ( unsigned long Param )
{

DCB & aDCB = *((LPDCB)theDCB);
aDCB.BaudRate = Param;

return setState();
} // end COMPort::setBitRate (..)


//-----------------------------------------------------------------------------
unsigned long COMPort::bitRate() const
{

DCB & aDCB = *((LPDCB)theDCB);

return aDCB.BaudRate;
} // end COMPort::bitRate () const


//-----------------------------------------------------------------------------
COMPort& COMPort::setLineCharacteristics( char * inConfig )
{

COMMTIMEOUTS aTimeout;
if ( !BuildCommDCBAndTimeouts ( inConfig
                              , (LPDCB)theDCB
                              , &aTimeout
                              )
   )
{
   throw runtime_error ("COMPort: could not set line characteristics.");   
} // endif

if ( ! SetCommTimeouts ( (HANDLE(thePortHandle))
                       , &aTimeout
                       )
   )
{
   throw runtime_error ("COMPort: could not set line characteristics.");   
} // endif

return setState();
}


//----------------------------------------------------------------------------
char COMPort::read ()
{

char buffer;
DWORD charsRead = 0;

do
{
   if (! ReadFile ( (HANDLE(thePortHandle))
                  , &buffer
                  , sizeof(char)
                  , &charsRead
                  , NULL
                  )
      )
   {
     throw runtime_error ("COMPort: read failed.");   
   } // endif

} while ( !charsRead );

return  buffer;
} // end COMPort::read()


//----------------------------------------------------------------------------
void COMPort::dumpReceiveBuffer ()
{
    PurgeComm( (HANDLE(thePortHandle))
             , PURGE_RXCLEAR
             );
} // end COMPort::dumpReceiveBuffer()


//----------------------------------------------------------------------------
unsigned long COMPort::read ( void *inBuffer
                            , const unsigned long inCharsReq
                            )
{

DWORD charsRead = 0;
if ( !ReadFile ( (HANDLE(thePortHandle))
               , inBuffer
               , inCharsReq
               , &charsRead
               , NULL
               )
   )
{
   throw runtime_error ("COMPort: read failed.");   
} // endif

return charsRead;
} // end COMPort::read (..)


//----------------------------------------------------------------------------
COMPort & COMPort::write ( const char inChar )
{

char buffer = inChar;
DWORD charsWritten = 0;

if ( !WriteFile ( (HANDLE(thePortHandle))
                , &buffer
                , sizeof(char)
                , &charsWritten
                , NULL
                )
   )
{
   throw runtime_error ("COMPort: write failed.");   
} // endif

return  *this;
} // end COMPort::write (..)


//----------------------------------------------------------------------------
unsigned long COMPort::write ( const void *inBuffer
                             , const unsigned long inBufSize
                             )
{

DWORD charsWritten = 0;

if ( !WriteFile ( (HANDLE(thePortHandle))
                , inBuffer
                , inBufSize
                , &charsWritten
                , NULL
                )
   )
{
   throw runtime_error ("COMPort: write failed.");   
} // endif

return  charsWritten;
} // end COMPort::write()


//-----------------------------------------------------------------------------
COMPort& COMPort::setxONxOFF ( bool Param )
{

DCB & aDCB = *((LPDCB)theDCB);
aDCB.fOutX = Param ? 1 : 0;
aDCB.fInX = Param ? 1 : 0;

return setState();
} // end COMPort::setxONxOFF (..)


//-----------------------------------------------------------------------------
bool COMPort::isxONxOFF () const
{

DCB & aDCB = *((LPDCB)theDCB);

return (aDCB.fOutX && aDCB.fInX);
} // end COMPort::isxONxOFF () const


//----------------------------------------------------------------------------
COMPort& COMPort::setBlockingMode ( unsigned long inReadInterval
                                  , unsigned long inReadMultiplyer
                                  , unsigned long inReadConstant
                                  )
{

COMMTIMEOUTS commTimeout;
if ( !GetCommTimeouts ( (HANDLE(thePortHandle))
                      , &commTimeout
                      )
   )
{
   throw runtime_error ("COMPort: failed to retrieve timeouts.");
} // endif

commTimeout.ReadIntervalTimeout = inReadInterval;

if ( inReadInterval==MAXDWORD )
{
   commTimeout.ReadTotalTimeoutMultiplier = 0;
   commTimeout.ReadTotalTimeoutConstant = 0;
}
else
{
   commTimeout.ReadTotalTimeoutMultiplier = inReadMultiplyer;     
   commTimeout.ReadTotalTimeoutConstant = inReadConstant;
}  // endifelse

if ( !SetCommTimeouts ( (HANDLE(thePortHandle))
                      , &commTimeout
                      )
   )
{
   throw runtime_error ("COMPort: failed to modify timeouts.");
} // endif

return *this;
} // end COMPort::setBlockingMode (..)


//-----------------------------------------------------------------------------
COMPort & COMPort::setHandshaking ( bool inHandshaking )
{

DCB & aDCB = *((LPDCB)theDCB);
if (inHandshaking)
{
   aDCB.fOutxCtsFlow = TRUE;
   aDCB.fOutxDsrFlow = FALSE;
   aDCB.fRtsControl = RTS_CONTROL_HANDSHAKE;
}
else
{
   aDCB.fOutxCtsFlow = FALSE;
   aDCB.fOutxDsrFlow = FALSE;
   aDCB.fRtsControl = RTS_CONTROL_ENABLE;
} // endifelse

return setState();
} // end COMPort::setHandshaking (..)


//-----------------------------------------------------------------------------
unsigned long COMPort::getMaximumBitRate() const
{

COMMPROP aProp;
if ( !GetCommProperties ( (HANDLE)thePortHandle
                        , &aProp )
   )
{
   throw runtime_error ("COMPort: failed to retrieve port properties.");
} // endif

return aProp.dwMaxBaud;
} // end COMPort::getMaximumBitRate () const


//-----------------------------------------------------------------------------
COMPort::MSPack COMPort::getModemSignals() const
{

MSPack aPack;
// 1 bit - DTR, 2 - bit RTS                           (output signals)
// 4 bit - CTS, 5 bit - DSR, 6 bit - RI, 7 bit - DCD  (input signals)
if ( !GetCommModemStatus ( (HANDLE)thePortHandle
                         , (LPDWORD)&aPack )
   )
{
   throw runtime_error ("COMPort: failed to retrieve modem signals.");
} // endif

return aPack;
} // end COMPort::getModemSignals () const


//-----------------------------------------------------------------------------
COMPort& COMPort::setParity ( Parity Param )
{

DCB & aDCB = *((LPDCB)theDCB);
aDCB.Parity = Param;

return setState();
} // end COMPort::setParity (..)


//-----------------------------------------------------------------------------
COMPort& COMPort::setDataBits ( DataBits Param )
{

DCB & aDCB = *((LPDCB)theDCB);
aDCB.ByteSize = Param;

return setState();
} // end COMPort::setDataBits (..)


//-----------------------------------------------------------------------------
COMPort& COMPort::setStopBits ( StopBits Param )
{

DCB & aDCB = *((LPDCB)theDCB);
aDCB.StopBits = Param;

return setState();
} // end COMPort::setStopBits (..)


//-----------------------------------------------------------------------------
COMPort::Parity COMPort::parity () const
{

DCB & aDCB = *((LPDCB)theDCB);

return (COMPort::Parity)aDCB.Parity;
} // end COMPort::parity () const


//-----------------------------------------------------------------------------
COMPort::DataBits COMPort::dataBits () const
{

DCB & aDCB = *((LPDCB)theDCB);

return (COMPort::DataBits)aDCB.ByteSize;
} // end COMPort::dataBits () const


//-----------------------------------------------------------------------------
COMPort::StopBits COMPort::stopBits () const
{

DCB & aDCB = *((LPDCB)theDCB);

return (COMPort::StopBits)aDCB.StopBits;
} // end COMPort::stopBits () cosnt

