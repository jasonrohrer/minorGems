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
 *
 * This is 56% slower than Jenkins, and 2x slower than CustomRandomSource.
 *
 * BUT, this passes all tests in PractRand
 * https://github.com/MartyMacGyver/PractRand
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

        

// the authors of Xoshiro suggest using splitmix64 to create good seeds
//
// I've noticed that if seeding with low numbers, the first few outputs of
// Xoshiro are 0 or low, which isn't great.
//
// Thus, I think that fluffing up seeds is a good idea.
//
// This is, in part, because Xoshiro has no noisy constants in it, so
// the first values aren't very noisy, if the seed has mostly 0 bits.
//
// In our case, we need 4 32-bit seeds, so I'm using splitmix32, which I found
// here:
//
// https://stackoverflow.com/questions/17035441/looking-for-decent-quality-prng-with-only-32-bits-of-state
//
// There's also a bad edge case when Xoshiro is seeded with all-zero values
// which might happen, depending on the application (if seeing with world
// position x,y, for example)
//
// Generating seeds with splitmix32 based on the provided seeds also handles
// this edge case.
//

static uint32_t splitmix32( uint32_t *inState ) {
    uint32_t z = ( *inState += 0x9e3779b9 );
    z ^= z >> 16; z *= 0x21f0aaad;
    z ^= z >> 15; z *= 0x735a2d97;
    z ^= z >> 15;
    return z;
    }



void XoshiroRandomSource::reseed( unsigned int inSeedA,
                                  unsigned int inSeedB,
                                  unsigned int inSeedC,
                                  unsigned int inSeedD ) {
    uint32_t splitMixState = inSeedA;
    
    mState[0] = splitmix32( &splitMixState );

    
    splitMixState += inSeedB;

    mState[1] = splitmix32( &splitMixState );


    splitMixState += inSeedC;

    mState[2] = splitmix32( &splitMixState );


    splitMixState += inSeedD;

    mState[3] = splitmix32( &splitMixState );
    }



void XoshiroRandomSource::reseed( unsigned int inSeed ) {
    // if using a single seed, specify 0 for the other ones
    // this allows us to generate a sequence of 4 values directly from
    // splitmix32 above
    reseed( inSeed, 0, 0, 0 );
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
