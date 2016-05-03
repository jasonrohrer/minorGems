#include "ByteBufferInputStream.h"



ByteBufferInputStream::ByteBufferInputStream( unsigned char *inBytes, 
                                              int inLength )
        : mBytes( inBytes ),
          mLength( inLength ),
          mCurrentPos( 0 ) {

    }



long ByteBufferInputStream::read( unsigned char *inBuffer, 
                                  long inNumBytes ) {
    int numToRead = inNumBytes;
    
    if( mLength - mCurrentPos < numToRead ) {
        numToRead = mLength - mCurrentPos;
        }
    
    if( numToRead > 0 ) {
        memcpy( inBuffer, &( mBytes[ mCurrentPos ] ), numToRead );

        mCurrentPos += numToRead;
        }

    return numToRead;
    }
