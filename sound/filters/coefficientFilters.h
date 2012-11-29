// filter algorithms found here:
// http://www.musicdsp.org/archive.php?classid=3#243
// Posted by Patrice Tarrabia

typedef struct CoeffFilterState {
        double a1, a2, a3, b1, b2;

        double lastIn[2];
        double lastOut[2];

    } CoeffFilterState;



// feeds another sample to a filter, returning the filtered sample
double coeffFilter( double inSample, CoeffFilterState *s );


// zeros out filter's buffers to prepare it for new audio
void resetCoeffFilter( CoeffFilterState *s );


CoeffFilterState initHighPass( double inCutoffFreq, int inSampleRate,
                               double inRez );



CoeffFilterState initLowPass( double inCutoffFreq, int inSampleRate,
                              double inRez );
