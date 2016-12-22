

typedef struct NoClip {
        int holdTime;
        int decayTime;

        int currentHoldTime;


        double gain;

        double maxVolume;

        double gainDecayPerSample;
        
    } NoClip;

    



// must be called once to init
NoClip resetAudioNoClip( double inMaxVolume, int inHoldTimeInSamples,
                              int inDecayTimeInSamples );


void audioNoClip( NoClip *inC,
                  double *inSamplesL, double *inSamplesR, int inNumSamples );



