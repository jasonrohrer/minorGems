/*
 * Modification History
 *
 * 2004-May-9   Jason Rohrer
 * Created.
 */


#include "aiff.h"

#include <stdio.h>
#include <math.h>

#include "minorGems/util/random/StdRandomSource.h"


int main() {

    int numChannels = 2;
    int numNotes = 5;
    int noteLengthInSeconds = 1;

    int sampleSize = 16;
    int sampleRate = 44100;

    int samplesPerNote = noteLengthInSeconds * sampleRate;

    // generate the header
    int headerSize;
    unsigned char *aiffHeader =
        getAIFFHeader( 2,
                       sampleSize,
                       sampleRate,
                       numNotes * samplesPerNote, 
                       &headerSize );


    FILE *aiffFile = fopen( "test.aiff", "wb" );

    printf( "Header size = %d\n", headerSize );
    
    fwrite( aiffHeader, 1, headerSize, aiffFile );

    delete [] aiffHeader;


    
    StdRandomSource randSource;

    for( int i=0; i<numNotes; i++ ) {
        float noteFrequency = randSource.getRandomFloat() * 1000;
        
        for( int j=0; j<samplesPerNote; j++ ) {

            short val;        // value to be written
                
            double sinVal =
                sin( ( ((float)j)/(float)sampleRate ) * noteFrequency *
                     2 * M_PI );
            /*
            sinVal = sinVal/3 +
                sin( ( ((float)j)/(float)sampleRate ) * noteFrequency *
                     M_PI ) / 3;

            sinVal = sinVal +
                sin( ( ((float)j)/(float)sampleRate ) * noteFrequency *
                     4 * M_PI )/3;
                
            float slope = 1; //4f/(float)soundSize;
            double coef = sin(M_PI * (float)i / (float)samplesPerNote);
                
            val = (short)( coef*(pow(2,sampleSize-1) -1) * sinVal);
            */
            double coef = (double)( samplesPerNote - j ) / samplesPerNote;
            
            val = (short)( coef * ( pow(2,sampleSize-1) -1) * sinVal);
            

            

            unsigned char msb = val >> 8 & 0xFF;
            unsigned char lsb = val && 0xFF;

            // output twice, once for each channel
            fwrite( &msb, 1, 1, aiffFile );
            fwrite( &lsb, 1, 1, aiffFile );

            fwrite( &msb, 1, 1, aiffFile );
            fwrite( &lsb, 1, 1, aiffFile );
            }
        
        }

    
    fclose( aiffFile );

    
    return 0;
    }
