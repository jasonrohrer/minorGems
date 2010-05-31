/*
 * Modification History
 *
 * 2006-April-30   Jason Rohrer
 * Created.  Adapted from Transcend/game/SoundPlayer.h
 *
 * 2006-May-4   Jason Rohrer
 * Added current play position timestamp to SampleSource interface.
 */



#ifndef SOUND_PLAYER_INCLUDED
#define SOUND_PLAYER_INCLUDED

#include "minorGems/sound/portaudio/pa_common/portaudio.h"
#include "minorGems/sound/portaudio/pablio/pablio.h"



/**
 * Abstract interface for a source of samples.
 *
 * @author Jason Rohrer.
 */
class SampleSource {
        
    public:

        // virtual destructor so subclasses can override it
        virtual ~SampleSource() {
            }

        
        /**
         * Gets samples from this sample source.
         *
         * @param inNumSamples the number of samples to get.
         * @param outSampleBuffer the buffer where the samples should be
         *   returned.
         *   Allocated and destroyed by caller.
         * @param inPlayTime the timestamp of the current play position
         *   in seconds.
         */
        virtual void getSamples( int inNumSamples,
                                 float *outSampleBuffer,
                                 double inPlayTime ) = 0; 

    };



/**
 * Class that plays sound through portaudio.
 *
 * @author Jason Rohrer
 */
class SoundPlayer {


        
    public:


        
        /**
         * Constructs a sound player.
         *
         * @param inSampleRate the number of samples per second.
         *   Should be a "standard" rate, like 44100, 22050, etc.
         * @param inStereo true if this player should play stereo.
         * @param inSampleSource the source that this player will
         *   use whenever portaudio needs more samples.
         *   Must be destroyed by caller after this class is destroyed.
         */
        SoundPlayer( int inSampleRate, char inStereo,
                     SampleSource *inSampleSource );

        ~SoundPlayer();


        
        /**
         * Gets whether this player is stereo.
         *
         * @return true if stereo.
         */
        char isStereo() {
            return mStereo;
            }



        /**
         * Gets the sample rate of this player.
         *
         * @return the sample rate.
         */
        int getSampleRate() {
            return mSampleRate;
            }

        
        
        /**
         * Gets the sample source used by this player.
         *
         * @return the sample source.  Not destroyed by caller.
         */
        SampleSource *getSampleSource() {
            return mSampleSource;
            }

        
        
    protected:

        unsigned long mSampleRate;
        char mStereo;
        SampleSource *mSampleSource;
        
        char mAudioInitialized;

        PortAudioStream *mAudioStream;
        
    };



#endif
