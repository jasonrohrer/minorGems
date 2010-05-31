/*
 * Modification History 
 *
 * 2003-April-28   Jason Rohrer
 * Created.
 */
 

#ifndef SQRT_EXPRESSION_INCLUDED
#define SQRT_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 

#include <math.h>
 
/**
 * Expression implementation of a unary sqrt operation. 
 *
 * @author Jason Rohrer 
 */
class SqrtExpression : public UnaryOperationExpression { 
    
    public:
        
        /**
         * Constructs a unary sqrt operation expression.
         *
         * Argument is destroyed when the class is destroyed.
         *
         * @param inArgument the argument.
         */
        SqrtExpression( Expression *inArgument );
        
        
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
long SqrtExpression::mID = 13;



inline SqrtExpression::SqrtExpression( Expression *inArgument ) 
    : UnaryOperationExpression( inArgument ) {
    
    }



inline double SqrtExpression::evaluate() {
    return sqrt( mArgument->evaluate() );
    }



inline long SqrtExpression::getID() {    
    return mID;
    }    



inline long SqrtExpression::staticGetID() {
    return mID;
    }



inline void SqrtExpression::print() {
    printf( "( sqrt" );
    
    mArgument->print();
    printf( " )" );
    }
            


inline Expression *SqrtExpression::copy() {
    SqrtExpression *copy =
        new SqrtExpression( mArgument->copy() );
    
    return copy;
    }        


    
#endif
