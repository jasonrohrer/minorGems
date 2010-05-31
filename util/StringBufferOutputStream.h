/*
 * Modification History
 *
 * 2002-August-1  Jason Rohrer
 * Created.
 *
 * 2004-May-9  Jason Rohrer
 * Added function for getting data as a byte array.
 */

#include "minorGems/common.h"



#ifndef STRING_BUFFER_OUTPUT_STREAM_INCLUDED
#define STRING_BUFFER_OUTPUT_STREAM_INCLUDED



#include "minorGems/util/SimpleVector.h"
#include "minorGems/io/OutputStream.h"



/**
 * An output stream that fills a string buffer.
 *
 * @author Jason Rohrer
 */ 
class StringBufferOutputStream : public OutputStream {


        
	public:
		

        
		StringBufferOutputStream();

        ~StringBufferOutputStream();



        /**
         * Gets the data writen to this stream since construction as a
         * \0-terminated string.
         *
         * @return a string containing all data written to this stream.
         *   Must be destroyed by caller.
         */
        char *getString();


        
        /**
         * Gets the data writen to this stream since construction as a
         * byte array.
         *
         * @param outNumBytes pointer to where the array length should be
         *   returned.
         *
         * @return an array containingdata written to this stream.
         *   Must be destroyed by caller.
         */
        unsigned char *getBytes( int *outNumBytes );


        
		// implements the OutputStream interface
		long write( unsigned char *inBuffer, long inNumBytes );
		

		
	protected:


        
        SimpleVector<unsigned char> *mCharacterVector;


        
	};		
	
	
	
#endif
