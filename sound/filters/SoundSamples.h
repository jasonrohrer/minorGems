/*
 * Modification History
 *
 * 2004-July-17   Jason Rohrer
 * Created.
 *
 * 2004-August-12   Jason Rohrer
 * Added a constructor that can specify all sound data.
 */



#ifndef SOUND_SAMPLES_INCLUDED
#define SOUND_SAMPLES_INCLUDED



/**
 * Class that encapsulates a buffer of sound samples.
 *
 * @author Jason Rohrer
 */
class SoundSamples {


    public:

        unsigned long mSampleCount;
        
        float *mLeftChannel;
        float *mRightChannel;


        
        /**
         * Constructs a sound samples object.
         *
         * @param inSampleCount the number of samples.
         * @param inLeftChannel samples for the left channel.
         *   Will be destroyed when this class is destroyed.
         * @param inRightChannel samples for the right channel.
         *   Will be destroyed when this class is destroyed.
         */
        SoundSamples( unsigned long inSampleCount,
                      float *inLeftChannel, float *inRightChannel );


        
        /**
         * Constructs a sound samples object filled with 0 samples.
         *
         * @param inSampleCount the number of samples.
         */
        SoundSamples( unsigned long inSampleCount );


        
        /**
         * Constructs a sound samples object by copying another object.
         *
         * @param inSamplesToCopy the object to copy.
         *   Must be destroyed by caller.
         */
        SoundSamples( SoundSamples *inSamplesToCopy );

        

        /**
         * Constructs a sound samples object by copying samples from
         * another object.
         *
         * @param inSamplesToCopy the object to copy.
         *   Must be destroyed by caller.
         * @param inNumToCopy the number of samples from the start of
         *   inSamplesToCopy to take.
         */
        SoundSamples( SoundSamples *inSamplesToCopy,
                      unsigned long inNumToCopy );

        
        
        ~SoundSamples();


        
        /**
         * Trims samples from the beginning of this sound.
         *
         * @param inNumSamplesToDrop the number of samples at the beginning
         *   of this sound to drop.
         */
        void trim( unsigned long inNumSamplesToDrop );
        

        
    };



#endif
