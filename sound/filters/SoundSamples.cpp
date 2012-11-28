/*
 * Modification History
 *
 * 2004-July-17   Jason Rohrer
 * Created.
 *
 * 2004-August-12   Jason Rohrer
 * Added a constructor that can specify all sound data.
 */



#include "SoundSamples.h"


#include <string.h>



SoundSamples::SoundSamples( unsigned long inSampleCount,
                            float *inLeftChannel, float *inRightChannel )
    : mSampleCount( inSampleCount ),
      mLeftChannel( inLeftChannel ),
      mRightChannel( inRightChannel ) {

    }



SoundSamples::SoundSamples( unsigned long inSampleCount )
    : mSampleCount( inSampleCount ),
      mLeftChannel( new float[ inSampleCount ] ),
      mRightChannel( new float[ inSampleCount ] ) {

    // zero out the channels;
    for( unsigned long i=0; i<mSampleCount; i++ ) {
        mLeftChannel[ i ] = 0;
        mRightChannel[ i ] = 0;
        }
    }



SoundSamples::SoundSamples( SoundSamples *inSamplesToCopy ) {
    mSampleCount = inSamplesToCopy->mSampleCount;

    mLeftChannel = new float[ mSampleCount ];
    mRightChannel = new float[ mSampleCount ];

    
    memcpy( (void *)( mLeftChannel ),
            (void *)( inSamplesToCopy->mLeftChannel ),
            mSampleCount * sizeof( float ) );

    memcpy( (void *)( mRightChannel ),
            (void *)( inSamplesToCopy->mRightChannel ),
            mSampleCount * sizeof( float ) );
    }



SoundSamples::SoundSamples( SoundSamples *inSamplesToCopy,
                            unsigned long inNumToCopy ) {
    mSampleCount = inSamplesToCopy->mSampleCount;

    if( inNumToCopy < mSampleCount ) {
        mSampleCount = inNumToCopy;
        }
    
    mLeftChannel = new float[ mSampleCount ];
    mRightChannel = new float[ mSampleCount ];

    
    memcpy( (void *)( mLeftChannel ),
            (void *)( inSamplesToCopy->mLeftChannel ),
            mSampleCount * sizeof( float ) );

    memcpy( (void *)( mRightChannel ),
            (void *)( inSamplesToCopy->mRightChannel ),
            mSampleCount * sizeof( float ) );
    }



SoundSamples::~SoundSamples() {
    delete [] mRightChannel;
    delete [] mLeftChannel;
    }



void SoundSamples::trim( unsigned long inNumSamplesToDrop ) {

    unsigned long newSampleCount = mSampleCount - inNumSamplesToDrop;

    float *newLeftChannel = new float[ newSampleCount ];
    float *newRightChannel = new float[ newSampleCount ];

    // copy samples, skipping inNumSamplesToDrop
    memcpy( (void *)( newLeftChannel ),
            (void *)( &( mLeftChannel[ inNumSamplesToDrop ] ) ),
            newSampleCount * sizeof( float ) );
    memcpy( (void *)( newRightChannel ),
            (void *)( &( mRightChannel[ inNumSamplesToDrop ] ) ),
            newSampleCount * sizeof( float ) );

    delete [] mLeftChannel;
    delete [] mRightChannel;

    mSampleCount = newSampleCount;

    mLeftChannel = newLeftChannel;
    mRightChannel = newRightChannel;
    }
