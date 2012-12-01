/*
 * Modification History
 *
 * 2004-July-22   Jason Rohrer
 * Created.
 */



#ifndef REVERB_SOUND_FILTER_INCLUDED
#define REVERB_SOUND_FILTER_INCLUDED



#include "SoundFilter.h"

#include "coefficientFilters.h"


/**
 * A reverb filter.
 *
 * @author Jason Rohrer
 */
class ReverbSoundFilter : public SoundFilter {

        

    public:

        

        /**
         * Constructs a filter.
         *
         * @param inDelayInSamples the delay of the reverb.
         * @param inGain the gain level in the range [0,1].
         */
        ReverbSoundFilter( unsigned long inDelayInSamples, double inGain,
                           int inSampleRate );

        

        virtual ~ReverbSoundFilter();


        
        // implements the SoundFilter interface
        virtual SoundSamples *filterSamples( SoundSamples *inSamples );

        

    private:
        
        SoundSamples *mDelayBuffer;
        
        double mGain;


        unsigned long mDelayBufferPosition;

        CoeffFilterState mLowPassStateL;
        CoeffFilterState mLowPassStateR;
    };



#endif
