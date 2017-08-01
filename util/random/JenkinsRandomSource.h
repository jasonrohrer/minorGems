/*
 * Modification History
 *
 * 2014-June-6    Jason Rohrer
 * Created.
 */



#ifndef JENKINS_RANDOM_SOURCE_INCLUDED
#define JENKINS_RANDOM_SOURCE_INCLUDED

#include "RandomSource32.h"

#include "minorGems/system/Time.h"

#include <math.h>
#include <limits.h>



/**
 * Implementation of RandomSource based on Bob Jenkins smallprng:
 *
 * http://burtleburtle.net/bob/rand/smallprng.html
 *
 * Maintains its own internal state.
 *
 * Ends up being about 5% slower than CustomRandomSource, but has a much
 * longer expected period.
 */
class JenkinsRandomSource : public RandomSource32 {

    public:        
        
        // seeds itself with current time
        JenkinsRandomSource();
        
        // specify the seed
        JenkinsRandomSource( unsigned int inSeed );
        
                

        void reseed( unsigned int inSeed );



    protected:

        

        unsigned int mA, mB, mC, mD;
        
        // implements this core function
        // returns next number and updates state
        virtual unsigned int genRand32();
        
    };




inline JenkinsRandomSource::JenkinsRandomSource() {
    
    reseed( (unsigned int)fmod( Time::timeSec(), UINT_MAX ) );
    }



inline JenkinsRandomSource::JenkinsRandomSource( unsigned int inSeed ) {
    reseed( inSeed );
    }




inline void JenkinsRandomSource::reseed( unsigned int inSeed ) {
    
    mA = 0xf1ea5eed;
    mB = mC = mD = inSeed;
    for( int i=0; i<20; i++ ) {
        genRand32();
        }
    }

    


#define rot(x,k) (((x)<<(k))|((x)>>(32-(k))))

inline unsigned int JenkinsRandomSource::genRand32() {
    
    unsigned int e = mA - rot( mB, 27 );
    mA = mB ^ rot( mC, 17 );
    mB = mC + mD;
    mC = mD + e;
    mD = e + mA;
    return mD;
    }



#endif
