/*
 * Modification History
 *
 * 2004-July-22   Jason Rohrer
 * Created.
 */



#include "ReverbSoundFilter.h"








ReverbSoundFilter::ReverbSoundFilter( unsigned long inDelayInSamples,
                                      double inGain,
                                      int inSampleRate )
    : mDelayBuffer( new SoundSamples( inDelayInSamples ) ),
      mGain( inGain ),
      mDelayBufferPosition( 0 ),
      mLowPassStateL( initLowPass( 500, inSampleRate, 0.7 ) ),
      mLowPassStateR( initLowPass( 500, inSampleRate, 0.7 ) ) {
    

    }

        

ReverbSoundFilter::~ReverbSoundFilter() {
    delete mDelayBuffer;
    }



SoundSamples *ReverbSoundFilter::filterSamples( SoundSamples *inSamples ) {

    unsigned long delaySize = mDelayBuffer->mSampleCount;

    unsigned long numSamples = inSamples->mSampleCount;

    // pass the input through to the output
    SoundSamples *outputSamples = new SoundSamples( inSamples );


    for( unsigned long i=0; i<numSamples; i++ ) {

        
        // add in reverb from the buffer to our output
        outputSamples->mLeftChannel[i] +=
            mDelayBuffer->mLeftChannel[ mDelayBufferPosition ];
        outputSamples->mRightChannel[i] +=
            mDelayBuffer->mRightChannel[ mDelayBufferPosition ]; 

        // save our gained output in the delay buffer
        mDelayBuffer->mLeftChannel[ mDelayBufferPosition ] =
            mGain * coeffFilter( outputSamples->mLeftChannel[i], 
                                 &mLowPassStateL );
        mDelayBuffer->mRightChannel[ mDelayBufferPosition ] =
            mGain * coeffFilter( outputSamples->mRightChannel[i],
                                 &mLowPassStateR );
        
        // step through delay buffer, wrapping around at end
        mDelayBufferPosition++;
        if( mDelayBufferPosition >= delaySize ) {
            mDelayBufferPosition = 0;
            }
        }

    return outputSamples;    
    }
