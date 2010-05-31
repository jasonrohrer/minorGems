/*
 * Modification History
 *
 * 2004-January-26		Jason Rohrer
 * Created.
 *
 * 2004-January-27		Jason Rohrer
 * Changed to support externally-specified file name prefixes.
 */

#include "minorGems/common.h"


#ifndef LOGGING_SOCKET_STREAM_CLASS_INCLUDED
#define LOGGING_SOCKET_STREAM_CLASS_INCLUDED

#include "SocketStream.h"

#include "minorGems/util/stringUtils.h"


#include <stdio.h>



/**
 * A SocketStream that logs inbound and outbound data.
 *
 * @author Jason Rohrer
 */ 
class LoggingSocketStream : public SocketStream {

    public:
        
        
        /**
         * Constructs a SocketStream.
         *
         * @param inSocket the newtork socket wrapped by this stream.
         *   inSocket is NOT destroyed when the stream is destroyed.
         * @param inEnableInboundLog set to true to enable inbound logging.
         * @param inEnableOutboundLog set to true to enable outbound logging.
         * @param inLogSizeLimit the size limit of each log (both inbound
         *   and outbound), in bytes.
         *   Logging will stop after inLogSizeLimit bytes have been
         *   written to each log.
         * @param inLogNamePrefix a string that will be used to name
         *   the log files (appending .in and .out for the two logs).
         *   Should be unique.
         *   Must be destroyed by caller.
         */
        LoggingSocketStream( Socket *inSocket, char inEnableInboundLog,
                             char inEnableOutboundLog,
                             unsigned long inLogSizeLimit,
                             char *inLogNamePrefix );

        
        virtual ~LoggingSocketStream();
        
        
        
        // overrides the SocketStream implementations

        long read( unsigned char *inBuffer, long inNumBytes );

        long write( unsigned char *inBuffer, long inNumBytes );
        
        
    protected:
        char mEnableInboundLog;
        char mEnableOutboundLog;
        unsigned long mLogSizeLimit;
        FILE *mInboundLogFile;
        FILE *mOutboundLogFile;
        unsigned long mInboundSizeSoFar;
        unsigned long mOutboundSizeSoFar;
        
    };        



inline LoggingSocketStream::LoggingSocketStream(
    Socket *inSocket,
    char inEnableInboundLog,
    char inEnableOutboundLog,
    unsigned long inLogSizeLimit,
    char *inLogNamePrefix ) 
    : SocketStream( inSocket ),
      mEnableInboundLog( inEnableInboundLog ),
      mEnableOutboundLog( inEnableOutboundLog ),
      mLogSizeLimit( inLogSizeLimit ),
      mInboundLogFile( NULL ),
      mOutboundLogFile( NULL ),
      mInboundSizeSoFar( 0 ),
      mOutboundSizeSoFar( 0 ) {

    if( mEnableInboundLog ) {
        char *inboundFileName = autoSprintf( "%s.in", inLogNamePrefix );

        mInboundLogFile = fopen( inboundFileName, "wb" );

        delete [] inboundFileName;
        }
    if( mEnableOutboundLog ) {

        char *outboundFileName = autoSprintf( "%s.out", inLogNamePrefix );

        mOutboundLogFile = fopen( outboundFileName, "wb" );
        
        delete [] outboundFileName;
        }
    
    }



inline LoggingSocketStream::~LoggingSocketStream() {
    if( mInboundLogFile != NULL ) {
        fclose( mInboundLogFile );
        mInboundLogFile = NULL;
        }
    if( mOutboundLogFile != NULL ) {
        fclose( mOutboundLogFile );
        mOutboundLogFile = NULL;
        }
    }



inline long LoggingSocketStream::read( unsigned char *inBuffer,
                                       long inNumBytes ) {
    long returnVal = SocketStream::read( inBuffer, inNumBytes );

    if( mEnableInboundLog &&
        mInboundSizeSoFar < mLogSizeLimit &&
        returnVal == inNumBytes ) {

        int numToLog = inNumBytes;
        if( mInboundSizeSoFar + numToLog > mLogSizeLimit ) {
            numToLog = mLogSizeLimit - mInboundSizeSoFar;
            }
        
        if( mInboundLogFile != NULL ) {
            fwrite( inBuffer, 1, numToLog, mInboundLogFile );
            fflush( mInboundLogFile );
            }
        mInboundSizeSoFar += numToLog;
        }
    
    return returnVal;    
    }
        
        

inline long LoggingSocketStream::write( unsigned char *inBuffer,
                                        long inNumBytes ) {

    long returnVal = SocketStream::write( inBuffer, inNumBytes );
        
    if( mEnableOutboundLog &&
        mOutboundSizeSoFar < mLogSizeLimit &&
        returnVal == inNumBytes ) {

        int numToLog = inNumBytes;
        if( mOutboundSizeSoFar + numToLog > mLogSizeLimit ) {
            numToLog = mLogSizeLimit - mOutboundSizeSoFar;
            }
        
        if( mOutboundLogFile != NULL ) {
            fwrite( inBuffer, 1, numToLog, mOutboundLogFile );
            fflush( mOutboundLogFile );
            }
        mOutboundSizeSoFar += numToLog;
        }
    
    return returnVal;    
    }
    
    
    
#endif
