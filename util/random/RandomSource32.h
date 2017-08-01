/*
 * Modification History
 *
 * 2014-June-6    Jason Rohrer
 * Created.
 */



#ifndef RANDOM_SOURCE_32_INCLUDED
#define RANDOM_SOURCE_32_INCLUDED

#include <stdlib.h>
#include "RandomSource.h"


/**
 * Base class for RandomSource implementation that have an unsigned 32-bit
 * random number generator at their core.
 */
class RandomSource32 : public RandomSource {

    public:        
        
        RandomSource32();

        
        // implements these functions
        float getRandomFloat();    // in interval [0,1.0]
        double getRandomDouble(); // in interval [0,1.0]
        unsigned int getRandomInt();        // in interval [0,MAX]
        unsigned int getIntMax();    // returns MAX
        
        int getRandomBoundedInt( int inRangeStart,
            int inRangeEnd );    

        double getRandomBoundedDouble( double inRangeStart,
                                       double inRangeEnd );
        char getRandomBoolean();

        
    protected:
        double mInvMAXPlusOne; //  1 / ( MAX + 1 )

        

        // this must be implemented by subclasses
        // returns next number and updates state
        virtual unsigned int genRand32() = 0;
        
    };



inline RandomSource32::RandomSource32() {
    MAX = 4294967295U;
    invMAX = (float)1.0 / ((float)MAX);
    invDMAX = 1.0 / ((double)MAX);
    mInvMAXPlusOne = 1.0 / ( ( (float)MAX ) + 1.0 );
    }


    


inline float RandomSource32::getRandomFloat() {
    
    return (float)(genRand32()) * invMAX;
    }



inline double RandomSource32::getRandomDouble() {
    
    return (double)(genRand32()) * invDMAX;
    }



inline unsigned int RandomSource32::getRandomInt() {
    
    return genRand32();
    }



inline unsigned int RandomSource32::getIntMax() {
    
    return MAX;
    }



inline int RandomSource32::getRandomBoundedInt( int inRangeStart,
    int inRangeEnd ) {
    
    // float in range [0,1)
    double randFloat = (double)( genRand32() ) * mInvMAXPlusOne;

    int onePastRange = inRangeEnd + 1;

    int magnitude = (int)( randFloat * ( onePastRange - inRangeStart ) );
    
    return magnitude + inRangeStart;
    }


inline double RandomSource32::getRandomBoundedDouble( double inRangeStart,
    double inRangeEnd ) {
    
    // double in range [0,1]
    double randDouble = getRandomDouble();

    double magnitude = randDouble * ( inRangeEnd - inRangeStart );
    
    return magnitude + inRangeStart;
    }



inline char RandomSource32::getRandomBoolean() {

    // float in range [0,1]
    double randFloat = getRandomFloat();

    if( randFloat < 0.5 ) {
        return true;
        }
    else {
        return false;
        }
    }


#endif
