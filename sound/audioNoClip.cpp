#include "audioNoClip.h"

#include <math.h>
#include <stdio.h>






NoClip resetAudioNoClip( double inMaxVolume, int inHoldTimeInSamples, 
                         int inDecayTimeInSamples ) {
    
    NoClip a;
    
    a.maxVolume = inMaxVolume;

    a.gain = 1.0;

    a.decayTime = inDecayTimeInSamples;

    a.holdTime = inHoldTimeInSamples;
    
    a.currentHoldTime = 0;

    return a;
    }



void audioNoClip( NoClip *inC,
                  double *inSamplesL, double *inSamplesR, int inNumSamples ) {
    
    for( int i=0; i<inNumSamples; i++ ) {
        
        double maxValL = fabs( inSamplesL[i] );
        
        double maxValR = fabs( inSamplesR[i] );
        
        double maxVal = maxValL;
        
        if( maxValR > maxValL ) {
            maxVal = maxValR;
            }
        
        // do nothing if signal is not clipping and gain is full
        if( inC->gain != 1.0 || maxVal > inC->maxVolume ) {
            
            if( inC->gain != 1.0 
                && 
                ( inC->gain + inC->gainDecayPerSample ) * maxVal <= 
                inC->maxVolume ) {
                
                inC->currentHoldTime++;
                
                if( inC->currentHoldTime > inC->holdTime ) {
                    
                    // printf( "Gain decay step at sample %d, "
                    //        "current hold time %d\n", i, currentHoldTime );
                    
                    inC->gain += inC->gainDecayPerSample;
                
                    if( inC->gain > 1.0 ) {
                        inC->gain = 1.0;
                        }
                    }
                }
            else if( maxVal * inC->gain > inC->maxVolume ) {
                
                // new peak

                // restart hold
                inC->currentHoldTime = 0;
                
                // printf( "Current hold time resetA at sample %d\n", i );
                
                // prevent clipping
                
                // printf( "Max val = %f, old gain = %f\n", maxVal, gain );

                inC->gain = inC->maxVolume / maxVal;
                // printf( "   new gain = %f\n", gain );
                
                inC->gainDecayPerSample = ( 1.0 - inC->gain ) / inC->decayTime;
                }
            else {
                // not a new peak, not way under peak value
                // hit old peak again, continue holding
                inC->currentHoldTime = 0;
                //printf( "Current hold time resetB at sample %d\n", i );

                }
            

            inSamplesL[i] *= inC->gain;
            inSamplesR[i] *= inC->gain;
            }
        }

    
    }

