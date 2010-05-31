/*
 * Modification History 
 *
 * 2003-April-28   Jason Rohrer
 * Created.
 */
 

#ifndef TAN_EXPRESSION_INCLUDED
#define TAN_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 

#include <math.h>
 
/**
 * Expression implementation of a unary tan operation. 
 *
 * @author Jason Rohrer 
 */
class TanExpression : public UnaryOperationExpression { 
    
    public:
        
        /**
         * Constructs a unary tan operation expression.
         *
         * Argument is destroyed when the class is destroyed.
         *
         * @param inArgument the argument.
         */
        TanExpression( Expression *inArgument );
        
        
        /**
         * A static version of getID().
         */
        static long staticGetID();
        
        
        // implements the Expression interface
        virtual double evaluate();
        virtual long getID();
        virtual void print();
        virtual Expression *copy();
        
    protected:
        static long mID;
        
    };



// static init
long TanExpression::mID = 12;



inline TanExpression::TanExpression( Expression *inArgument ) 
    : UnaryOperationExpression( inArgument ) {
    
    }



inline double TanExpression::evaluate() {
    return tan( mArgument->evaluate() );
    }



inline long TanExpression::getID() {    
    return mID;
    }    



inline long TanExpression::staticGetID() {
    return mID;
    }



inline void TanExpression::print() {
    printf( "( tan" );
    
    mArgument->print();
    printf( " )" );
    }
            


inline Expression *TanExpression::copy() {
    TanExpression *copy =
        new TanExpression( mArgument->copy() );
    
    return copy;
    }        


    
#endif
