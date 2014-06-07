/*
 * Modification History
 *
 * 2009-January-14    Jason Rohrer
 * Created.
 *
 * 2009-February-5    Jason Rohrer
 * Added support for restoring from saved state.
 *
 * 2014-June-6    Jason Rohrer
 * Moved shared functionality into RandomSourc32 base class.
 */



#ifndef CUSTOM_RANDOM_SOURCE_INCLUDED
#define CUSTOM_RANDOM_SOURCE_INCLUDED

#include "RandomSource32.h"


/**
 * Implementation of RandomSource that does not depend on platform or library.
 *
 * Maintains its own internal state.
 */
class CustomRandomSource : public RandomSource32 {

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



    protected:

        

        unsigned int mState;
        
        unsigned int mSavedState;
        
        // implements this core function
        // returns next number and updates state
        virtual unsigned int genRand32();
        
    };




inline CustomRandomSource::CustomRandomSource()
        : mState( (unsigned)( time(NULL) ) ) {
    
    saveState();
    }



inline CustomRandomSource::CustomRandomSource( unsigned int inSeed )
        : mState( inSeed ) {
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


#endif
