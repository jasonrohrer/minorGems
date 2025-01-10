/*
 * Modification History
 *
 * 2025-January-10    Jason Rohrer
 * Created.
 */



#ifndef XOSHIRO_RANDOM_SOURCE_INCLUDED
#define XOSHIRO_RANDOM_SOURCE_INCLUDED

#include "RandomSource32.h"


#include <time.h>
#include <stdint.h>


/**
 * Implementation of RandomSource based on xoshiro128++ generator, which is
 * fast, has a long period, and passes all known tests.
 *
 * https://prng.di.unimi.it/
 *
 * https://prng.di.unimi.it/xoshiro128plusplus.c
 *
 * Maintains its own internal state.
 */
class XoshiroRandomSource : public RandomSource32 {

    public:        
        
        // seeds itself with current time
        XoshiroRandomSource();
        
        // specify the seed
        XoshiroRandomSource( unsigned int inSeedA,
                             unsigned int inSeedB,
                             unsigned int inSeedC,
                             unsigned int inSeedD );
        
        // for drop-in use in place of other single-seed generators
        // uses same seed 4 times
        XoshiroRandomSource( unsigned int inSeedA );
        

        void reseed( unsigned int inSeedA,
                     unsigned int inSeedB,
                     unsigned int inSeedC,
                     unsigned int inSeedD );
        

        // for drop-in use in place of other single-seed generators
        // uses same seed 4 times
        void reseed( unsigned int inSeed );
        


    protected:        

        unsigned int mState[4];
        

        // implements this core function
        // returns next number and updates state
        virtual unsigned int genRand32();
        
    };



inline XoshiroRandomSource::XoshiroRandomSource() {
    reseed( (unsigned)( time( NULL ) ) );
    }

        
        
inline XoshiroRandomSource::XoshiroRandomSource( unsigned int inSeedA,
                                                 unsigned int inSeedB,
                                                 unsigned int inSeedC,
                                                 unsigned int inSeedD ) {
    reseed( inSeedA, inSeedB, inSeedC, inSeedD );
    }

        
inline XoshiroRandomSource::XoshiroRandomSource( unsigned int inSeedA ) {
    reseed( inSeedA );
    }

        

void XoshiroRandomSource::reseed( unsigned int inSeedA,
                                  unsigned int inSeedB,
                                  unsigned int inSeedC,
                                  unsigned int inSeedD ) {
    
    if( inSeedA == 0 && 
        inSeedB == 0 &&
        inSeedC == 0 &&
        inSeedD == 0 ) {
        
        // edge case, state is 0 everywhere, which breaks xoshiro
        // force-fix it
        inSeedA = 1;
        }
    
    mState[0] = inSeedA;
    mState[1] = inSeedB;
    mState[2] = inSeedC;
    mState[3] = inSeedD;
    }



void XoshiroRandomSource::reseed( unsigned int inSeed ) {
    reseed( inSeed, inSeed, inSeed, inSeed );
    }



static inline uint32_t rotl( const uint32_t x, int k ) {
	return (x << k) | (x >> (32 - k));
    }



inline unsigned int XoshiroRandomSource::genRand32() {
    uint32_t *s = this->mState;
    
	const uint32_t result = rotl( s[0] + s[3], 7 ) + s[0];

	const uint32_t t = s[1] << 9;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 11);

	return result;
    }



#endif
