/*
 * Modification History
 *
 * 2011-January-17   Jason Rohrer
 * Created.
 */
 
 
#ifndef FAST_BLUR_FILTER_INCLUDED
#define FAST_BLUR_FILTER_INCLUDED
 
#include "minorGems/graphics/ChannelFilter.h" 
 
/**
 * Fast implementation of a radius-1 box filter.
 *
 * Completely skips edge pixels for speed.
 *
 * @author Jason Rohrer 
 */
class FastBlurFilter : public ChannelFilter { 
    
    public:
    
        // implements the ChannelFilter interface
        void apply( double *inChannel, int inWidth, int inHeight );

    };
        
    
    
inline void FastBlurFilter::apply( double *inChannel, 
                                   int inWidth, int inHeight ) {


    // use pointer tricks to walk through neighbor box of each pixel

    // four "corners" around box
    int cornerOffsetA = inWidth + 1;
    int cornerOffsetB = -inWidth + 1;
    int cornerOffsetC = inWidth - 1;
    int cornerOffsetD = -inWidth - 1;

    // sides around box
    int sideOffsetA = inWidth;
    int sideOffsetB = -inWidth;
    int sideOffsetC = 1;
    int sideOffsetD = -1;

    
    int numPixels = inWidth * inHeight;


    double *sourceData = new double[ numPixels ];
    
    memcpy( sourceData, inChannel, numPixels * sizeof( double ) );
        

    // avoid edges
    int numPixelsInRow = inWidth - 2;
    
    double boxMultiplier = 1.0 / 9.0;
    

    
    // sum boxes right into passed-in channel

    // avoid edges where box falls off edge
    for( int y=1; y<inHeight-1; y++ ) {
        
        // skip first pixel in row to avoid box falling off edge
        int pixelIndex = y * inWidth + 1;

        for( int i=0; i<numPixelsInRow; i++ ) {

            double *sourcePointer = &( sourceData[ pixelIndex ] );

            inChannel[ pixelIndex ] =
                ( sourcePointer[ 0 ] +
                  sourcePointer[ cornerOffsetA ] +
                  sourcePointer[ cornerOffsetB ] +
                  sourcePointer[ cornerOffsetC ] +
                  sourcePointer[ cornerOffsetD ] +
                  sourcePointer[ sideOffsetA ] +
                  sourcePointer[ sideOffsetB ] +
                  sourcePointer[ sideOffsetC ] +
                  sourcePointer[ sideOffsetD ]
                  ) * boxMultiplier;
            
            pixelIndex++;
            }

        }

    delete [] sourceData;
    }


#endif
