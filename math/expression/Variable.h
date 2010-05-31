/*
 * Modification History 
 *
 * 2003-April-28   Jason Rohrer
 * Created.
 *
 * 2003-April-29   Jason Rohrer
 * Added missing destructor.
 */



#ifndef VARIABLE_INCLUDED
#define VARIABLE_INCLUDED 



#include "minorGems/util/stringUtils.h"



/**
 * Wrapper for a variable value. 
 *
 * @author Jason Rohrer 
 */
class Variable { 
    
    public:


        
        /**
         * Constructs a variable, setting its value.
         *
         * @param inName the name of the variable.
         *   Must be destroyed by caller if non-const.
         * @param inValue the initial value of the variable.
         */
        Variable( char *inName, double inValue );

        virtual ~Variable();


        
        /**
         * Gets the value of this variable.
         *
         * @return the variable's value.
         */
        virtual double getValue();


        
        /**
         * Sets the value for this variable.
         *
         * @param inValue the value.
         */
        virtual void setValue( double inValue );



        /**
         * Gets the name of this variable.
         *
         * @return the name.
         *   Must be destroyed by caller.
         */
        virtual char *getName();

        
        
    protected:
        char *mName;
        double mValue;
        
    };


inline Variable::Variable( char *inName, double inValue )
    : mName( stringDuplicate( inName ) ), mValue( inValue ) {

    }



inline Variable::~Variable() {
    delete [] mName;
    }



inline double Variable::getValue() {
    return mValue;
    }



inline void Variable::setValue( double inValue ) {
    mValue = inValue;
    }



inline char *Variable::getName() {
    return stringDuplicate( mName );
    }


    
#endif
