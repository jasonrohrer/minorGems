#include "coefficientFilters.h"


#include <math.h>



double coeffFilter( double inSample, CoeffFilterState *s ) {
    double nextOut = 
        s->a1 * inSample + s->a2 * s->lastIn[0] + s->a3 * s->lastIn[1] 
        - s->b1 * s->lastOut[0] - s->b2 * s->lastOut[1];
    
    s->lastIn[1] = s->lastIn[0];
    s->lastIn[0] = inSample;
    
    s->lastOut[1] = s->lastOut[0];
    s->lastOut[0] = nextOut;
    
    return nextOut;
    }


void resetCoeffFilter( CoeffFilterState *s ) {
    for( int i=0; i<2; i++ ) {
        s->lastIn[i] = 0;
        s->lastOut[i] = 0;
        }
    }



CoeffFilterState initHighPass( double inCutoffFreq, int inSampleRate, 
                               double inRez ) {
    CoeffFilterState s;
    
    double c = tan( M_PI * inCutoffFreq / inSampleRate );

    s.a1 = 1.0 / ( 1.0 + inRez * c + c * c);
    s.a2 = -2 * s.a1;
    s.a3 = s.a1;
    s.b1 = 2.0 * ( c * c - 1.0) * s.a1;
    s.b2 = ( 1.0 - inRez * c + c * c ) * s.a1;
    
    resetCoeffFilter( &s );
    return s;
    }



CoeffFilterState initLowPass( double inCutoffFreq, int inSampleRate,
                              double inRez ) {
    CoeffFilterState s;
    

    double c = 1.0 / tan( M_PI * inCutoffFreq / inSampleRate );
    
    s.a1 = 1.0 / ( 1.0 + inRez * c + c * c);
    s.a2 = 2 * s.a1;
    s.a3 = s.a1;
    s.b1 = 2.0 * ( 1.0 - c * c ) * s.a1;
    s.b2 = ( 1.0 - inRez * c + c * c) * s.a1;

    resetCoeffFilter( &s );
    return s;
    }
