/*
 * Modification History 
 *
 * 2003-May-4   Jason Rohrer
 * Created.
 */



#ifndef RANDOM_VARIABLE_INCLUDED
#define RANDOM_VARIABLE_INCLUDED 



#include "minorGems/util/stringUtils.h"
#include "minorGems/util/random/RandomSource.h"



/**
 * Wrapper for a random-valued variable. 
 *
 * @author Jason Rohrer 
 */
class RandomVariable : public Variable{ 
    
    public:


        
        /**
         * Constructs a variable
         *
         * @param inName the name of the variable.
         *   Must be destroyed by caller if non-const.
         * @param inRandSource the source for random numbers.
         *   Must be destroyed by caller after this class is destroyed.
         */
        RandomVariable( char *inName, RandomSource *inRandSource );

        virtual ~RandomVariable();


        
        // overrides Variable functions
        virtual double getValue();
        virtual void setValue( double inValue );

        
        
    protected:
        RandomSource *mRandSource;
    };


inline RandomVariable::RandomVariable( char *inName,
                                       RandomSource *inRandSource )
    : Variable( inName, 0 ),
      mRandSource( inRandSource ) {

    }



inline RandomVariable::~RandomVariable() {

    }



inline double RandomVariable::getValue() {
    return mRandSource->getRandomDouble();
    }



inline void RandomVariable::setValue( double inValue ) {
    // do nothing
    }


    
#endif
