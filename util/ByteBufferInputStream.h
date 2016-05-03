#ifndef BYTE_BUFFER_INPUT_STREAM_INCLUDED
#define BYTE_BUFFER_INPUT_STREAM_INCLUDED


#include "minorGems/io/InputStream.h"

class ByteBufferInputStream : public InputStream {

    public:
        
        // inBytes not destroyed by this class and NOT copied internally
        // must be destroyed by caller after class is destroyed
        ByteBufferInputStream( unsigned char *inBytes, int inLength );

        virtual long read( unsigned char *inBuffer, long inNumBytes );


    protected:
        unsigned char *mBytes;
        int mLength;
        
        int mCurrentPos;
        
    };



#endif
