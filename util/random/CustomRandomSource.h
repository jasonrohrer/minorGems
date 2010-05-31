/*
 * Modification History
 *
 * 2009-January-14    Jason Rohrer
 * Created.
 *
 * 2009-February-5    Jason Rohrer
 * Added support for restoring from saved state.
 *
 */



#ifndef CUSTOM_RANDOM_SOURCE_INCLUDED
#define CUSTOM_RANDOM_SOURCE_INCLUDED

#include <stdlib.h>
#include <time.h>
#include "RandomSource.h"


/**
 * Implementation of RandomSource that does not depend on platform or library.
 *
 * Maintains its own internal state.
 */
class CustomRandomSource : public RandomSource {

    public:        
        
        // seeds itself with current time
        CustomRandomSource();
        
        // specify the seed
        CustomRandomSource( unsigned int inSeed );
        
        // save for rewind later
        void saveState();
        
        void rewindState();
        
        
        // can be used to save state to disk
        unsigned int getSavedState();
        
        void restoreFromSavedState( unsigned int inSavedState );
        
        

        void reseed( unsigned int inSeed );
        
            
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

        
    private:
        double mInvMAXPlusOne; //  1 / ( MAX + 1 )

        unsigned int mState;
        
        unsigned int mSavedState;
        

        // returns next number and updates state
        unsigned int genRand32();
        
    };



inline CustomRandomSource::CustomRandomSource() {
    MAX = 4294967295U;
    mState = (unsigned)( time(NULL) );
    invMAX = (float)1.0 / ((float)MAX);
    invDMAX = 1.0 / ((double)MAX);
    mInvMAXPlusOne = 1.0 / ( ( (float)MAX ) + 1.0 );

    saveState();
    }



inline CustomRandomSource::CustomRandomSource( unsigned int inSeed ) {
    MAX = 4294967295U;
    mState = inSeed;
    invMAX = (float)1.0 / ((float)MAX);
    invDMAX = 1.0 / ((double)MAX);
    mInvMAXPlusOne = 1.0 / ( ( (double)MAX ) + 1.0 );

    saveState();
    }



inline void CustomRandomSource::saveState() {
    mSavedState = mState;
    }


        
inline void CustomRandomSource::rewindState() {
    mState = mSavedState;
    }



inline unsigned int CustomRandomSource::getSavedState() {
    return mSavedState;
    }


inline void CustomRandomSource::restoreFromSavedState( 
    unsigned int inSavedState) {
    
    mState = inSavedState;
    }




inline void CustomRandomSource::reseed( unsigned int inSeed ) {
    mState = inSeed;
    }

    


// from Cultivation/Passage's landscape.cpp
// faster as a set of macros
#define CustNum1( inSeed ) \
    ( ( inSeed * 0xFEA09B9DU ) + 1 )

#define CustNum2( inSeed ) \
    ( ( ( inSeed ^ CustNum1( inSeed ) ) * 0x9C129511U ) + 1 )

#define CustNum3( inSeed ) \
    ( ( inSeed * 0x2512CFB8U ) + 1 )

#define CustNum4( inSeed ) \
    ( ( ( inSeed ^ CustNum3( inSeed ) ) * 0xB89C8895U ) + 1 )

#define CustNum5( inSeed ) \
    ( ( inSeed * 0x6BF962C1U ) + 1 )

#define CustNum6( inSeed ) \
    ( ( ( inSeed ^ CustNum5( inSeed ) ) * 0x4BF962C1U ) + 1 )




inline unsigned int CustomRandomSource::genRand32() {
    mState = 
        CustNum2( mState ) ^ 
        (CustNum4( mState ) >> 11) ^ 
        (CustNum6( mState ) >> 22);
    
    return mState;
    }


    


inline float CustomRandomSource::getRandomFloat() {
    
    return (float)(genRand32()) * invMAX;
    }



inline double CustomRandomSource::getRandomDouble() {
    
    return (double)(genRand32()) * invDMAX;
    }



inline unsigned int CustomRandomSource::getRandomInt() {
    
    return genRand32();
    }



inline unsigned int CustomRandomSource::getIntMax() {
    
    return MAX;
    }



inline int CustomRandomSource::getRandomBoundedInt( int inRangeStart,
    int inRangeEnd ) {
    
    // float in range [0,1)
    double randFloat = (double)( genRand32() ) * mInvMAXPlusOne;

    int onePastRange = inRangeEnd + 1;

    int magnitude = (int)( randFloat * ( onePastRange - inRangeStart ) );
    
    return magnitude + inRangeStart;
    }


inline double CustomRandomSource::getRandomBoundedDouble( double inRangeStart,
    double inRangeEnd ) {
    
    // double in range [0,1]
    double randDouble = getRandomDouble();

    double magnitude = randDouble * ( inRangeEnd - inRangeStart );
    
    return magnitude + inRangeStart;
    }



inline char CustomRandomSource::getRandomBoolean() {

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
