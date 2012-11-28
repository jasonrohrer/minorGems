/*
 * Modification History
 *
 * 2004-July-22   Jason Rohrer
 * Created.
 */



#ifndef SOUND_FILTER_INCLUDED
#define SOUND_FILTER_INCLUDED



#include "SoundSamples.h"



/**
 * Interface for a class that can filter sound.
 *
 * @author Jason Rohrer
 */
class SoundFilter {

        

    public:

        /**
         * Filters sound samples.
         *
         * @param inSamples the samples to filter.
         *   Must be destroyed by caller.
         *
         * @return the resulting samples in a newly constructed object.
         *   Must be destroyed by caller.
         */
        virtual SoundSamples *filterSamples( SoundSamples *inSamples ) = 0;



        // virtual destructor to ensure proper destruction of classes that
        // implement this interface
        virtual ~SoundFilter();


        
    };



// does nothing, needed to make compiler happy
inline SoundFilter::~SoundFilter() {

    }



#endif
