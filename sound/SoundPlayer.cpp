/*
 * Modification History
 *
 * 2006-April-30   Jason Rohrer
 * Created.    Adapted from Transcend/game/SoundPlayer.cpp
 *
 * 2006-May-4   Jason Rohrer
 * Added current play position timestamp to SampleSource interface.
 */



#include "SoundPlayer.h"

#include <stdio.h>


// callback passed into portaudio
static int portaudioCallback( void *inputBuffer, void *outputBuffer,
                              unsigned long framesPerBuffer,
                              PaTimestamp outTime, void *userData ) {


    SoundPlayer *player = (SoundPlayer *)userData;

    int numSamples = framesPerBuffer;

    if( player->isStereo() ) {
        numSamples = numSamples * 2;
        }

    SampleSource *source = player->getSampleSource();

    double playTime = outTime / player->getSampleRate();
    
    source->getSamples( numSamples, (float *)outputBuffer, playTime );

    return 0;
    }




SoundPlayer::SoundPlayer( int inSampleRate, char inStereo,
                          SampleSource *inSampleSource )
    : mSampleRate( inSampleRate ), mStereo( inStereo ),
      mSampleSource( inSampleSource ) {

    int numChannels = 1;

    if( mStereo ) {
        numChannels = 2;
        }
    
    PaError error = Pa_Initialize();

    if( error == paNoError ) {
    
        error = Pa_OpenStream(
            &mAudioStream,
            paNoDevice,// default input device 
            0,              // no input 
            paFloat32,  // 32 bit floating point input 
            NULL,
            Pa_GetDefaultOutputDeviceID(),
            numChannels,          // mono or stereo output 
            paFloat32,      // 32 bit floating point output 
            NULL,
            mSampleRate,
            1024,   // frames per buffer
            0,    // number of buffers, if zero then use default minimum 
            paClipOff, // we won't output out of range samples so
                       // don't bother clipping them 
            portaudioCallback,
            (void *)this );  // pass self-pointer to callback function
        
        if( error == paNoError ) {

            error = Pa_StartStream( mAudioStream );
            
            if( error == paNoError ) {
                mAudioInitialized = true;
                }
            else {
                fprintf( stderr, "Error starting audio stream\n" );
                Pa_CloseStream( mAudioStream );
                }            
            }
        else {
            fprintf( stderr, "Error opening audio stream\n" );
            Pa_Terminate();
            }
        }
    else {
        fprintf( stderr, "Error initializing audio framework\n" );
        }

    
    if( error != paNoError ) {
        fprintf( stderr, "Error number: %d\n", error );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( error ) );
        mAudioInitialized = false;
        }
    }



SoundPlayer::~SoundPlayer() {
    if( mAudioInitialized ) {
        PaError error = Pa_StopStream( mAudioStream );

        if( error == paNoError ) {
            error = Pa_CloseStream( mAudioStream );

            if( error != paNoError ) {
                fprintf( stderr, "Error closingaudio stream\n" );
                }
            }
        else {
            fprintf( stderr, "Error stopping audio stream\n" );
            }

        Pa_Terminate();
        
        if( error != paNoError ) {
            fprintf( stderr, "Error number: %d\n", error );
            fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( error) );
            }
        }
    }

