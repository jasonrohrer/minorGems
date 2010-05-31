/*
 * Modification History
 *
 * 2006-November-21   Jason Rohrer
 * Created.
 *
 * 2010-May-18    Jason Rohrer
 * String parameters as const to fix warnings.
 */
 
 
#ifndef PNG_IMAGE_CONVERTER_INCLUDED
#define PNG_IMAGE_CONVERTER_INCLUDED 


#include "BigEndianImageConverter.h"



/**
 * PNG implementation of the image conversion interface.
 *
 * Note that it only supports 32-bit PNG files
 * (3-channel Images are given a solid alpha channel).
 *
 * @author Jason Rohrer
 */
class PNGImageConverter : public BigEndianImageConverter {
	
	public:

        
        // compression level can be from 0 (none, raw data) to 
        // 9 (most, slowest).
        // 1 is supposedly the "fastest" setting that actually does some 
        // compression
        // 3 to 6 are supposed to be "nearly as good" as 7-9, but "much faster"
        //
        // Defaults to 5.
        PNGImageConverter( int inCompressionLevel=5 );
        
        
        
		// implement the ImageConverter interface
		virtual void formatImage( Image *inImage, 
			OutputStream *inStream );
			
		virtual Image *deformatImage( InputStream *inStream );		


    protected:
        
        int mCompressionLevel;
        

        /**
         * Writes a chunk to a stream.
         *
         * @param inChunkType the 4-char type of the chunk.
         * @param inData the data for the chunk.  Can be NULL if no data
         *   in chunk.  Destroyed by caller.
         * @param inNumBytes the length of inData, or 0 if inData is NULL.
         * @param inStream the stream to write the chunk to.  Destroyed by
         *   caller.
         */
        void writeChunk( const char inChunkType[4], unsigned char *inData,
                         unsigned long inNumBytes, OutputStream *inStream );


        
        // precomputed CRCs for all 8-bit messages
        unsigned long mCRCTable[256];


        const static unsigned long mStartCRC = 0xffffffffL;


        
        /**
         * Updates a crc with new data.
         *
         * Note that starting state for a CRC (before it is updated with data)
         * must be mStartCRC.
         * After all data has been added to the CRC, the resulting value
         * must be inverted (crc ^ 0xffffffffL).
         *
         * @param inCRC the current crc value.
         * @param inData the data.  Destroyed by caller.
         * @param inLength the length of the data.
         *
         * @return the new CRC.
         */
        unsigned long updateCRC( unsigned long inCRC, unsigned char *inData,
                                 int inLength );

	};


		
#endif
