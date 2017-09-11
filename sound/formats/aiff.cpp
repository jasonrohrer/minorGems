/*
 * Modification History
 *
 * 2004-May-9   Jason Rohrer
 * Created.
 */



#include "aiff.h"

#include "minorGems/util/StringBufferOutputStream.h"



unsigned char *getAIFFHeader( int inNumChannels, int inSampleSizeInBits,
                              int inSampleRateInHertz,
                              int inNumSampleFrames, int *outHeaderLength ) {

    /*   Header Information
        
         32 bits            'FORM'
         32 bits            ckSize
         
         32 bits            'AIFF'
         32 bits            'COMM'
         32 bits            ckSize                            '18'
         
         16 bits            numChannels
         32 bits            num SampleFrames
         16 bits            sampleSize                        '16'
         80 bits            sampleRate {
                                16 bits =                     '16398'
                                16 bits =                     '44100'
                                remaining 48 bits =           '0'
                                }
         32 bits            'SSND'
         32 bits            ckSize
         
         32 bits            offset                            '0'
         32 bits            block size                        '0'
         
         FINALLY:           sound data
        
        
        
         #bytes in 'FORM' chunk = bytes in sound data + 46
         #bytes in 'SSND' chunk = bytes in sound data + 8
            
        
    */


    int soundSizeInBytes =
        ( inNumChannels * inNumSampleFrames * inSampleSizeInBits ) / 8;

    
    
    StringBufferOutputStream *headerStream = new StringBufferOutputStream();


    headerStream->writeString( "FORM" );			// form chunk ID
    headerStream->writeLong( 46 + soundSizeInBytes );	// size of form chunk
			
    headerStream->writeString( "AIFF" );			// form type
    headerStream->writeString( "COMM" );			// common chunk ID
    headerStream->writeLong( 18 );				// common chunk size
    headerStream->writeShort( inNumChannels );
    headerStream->writeLong( inNumSampleFrames );  // number of frames in sound
                                                   // data
    
    headerStream->writeShort( inSampleSizeInBits );	 // size of each sample

    headerStream->writeLong(
        inSampleRateInHertz | 16398<<16 );	 // sample rate in Hz plus
                                             // mysterious most significant
                                             // bits

    headerStream->writeLong( 0 );   // 48 bits of 0 padding
    headerStream->writeShort( 0 );
			
    headerStream->writeString( "SSND" );			// Sound chunk ID
    headerStream->writeLong( 8 + soundSizeInBytes );   // size of sound chunk
    headerStream->writeLong( 0 );				// offset
    headerStream->writeLong( 0 );				// block size

    
    
    unsigned char *returnBuffer = headerStream->getBytes( outHeaderLength );
    
    delete headerStream;

    return returnBuffer;
    }



int16_t *readMono16AIFFData( unsigned char *inData, int inNumBytes,
                             int *outNumSamples, int *outSampleRate ) {
    
    if( inNumBytes < 34 ) {
        printf( "AIFF not long enough for header\n" );
        return NULL;
        }
    
    // byte 20 and 21 are num channels

    if( inData[20] != 0 || inData[21] != 1 ) {
        printf( "AIFF not mono\n" );
        return NULL;
        }
    
    if( inData[26] != 0 || inData[27] != 16 ) {
        printf( "AIFF not 16-bit\n" );
        return NULL;
        }
    

    int numSamples =
        inData[22] << 24 |
        inData[23] << 16 |
        inData[24] << 8 |
        inData[25];


    int sampleRate =
        inData[30] << 8 |
        inData[31];
    
    if( outSampleRate != NULL ) {
        *outSampleRate = sampleRate;
        }


    int sampleStartByte = 54;
                        
    
    int numBytes = numSamples * 2;
                        
    if( inNumBytes < sampleStartByte + numBytes ) {
        printf( "AIFF not long enough for inData\n" );
        return NULL;
        }
    
    int16_t *samples = new int16_t[numSamples];
                            

    int b = sampleStartByte;
    for( int i=0; i<numSamples; i++ ) {
        samples[i] = 
            ( inData[b] << 8 ) |
            inData[b+1];
        b += 2;
        }
    
    *outNumSamples = numSamples;
                            
    return samples;
    }
