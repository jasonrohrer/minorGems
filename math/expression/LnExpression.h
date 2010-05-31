/*
 * Modification History 
 *
 * 2003-April-28   Jason Rohrer
 * Created.
 */
 

#ifndef LN_EXPRESSION_INCLUDED
#define LN_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 

#include <math.h>
 
/**
 * Expression implementation of a unary ln (natural log) operation. 
 *
 * @author Jason Rohrer 
 */
class LnExpression : public UnaryOperationExpression { 
    
    public:
        
        /**
         * Constructs a unary ln operation expression.
         *
         * Argument is destroyed when the class is destroyed.
         *
         * @param inArgument the argument.
         */
        LnExpression( Expression *inArgument );
        
        
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
long LnExpression::mID = 10;



inline LnExpression::LnExpression( Expression *inArgument ) 
    : UnaryOperationExpression( inArgument ) {
    
    }



inline double LnExpression::evaluate() {
    return log( mArgument->evaluate() );
    }



inline long LnExpression::getID() {    
    return mID;
    }    



inline long LnExpression::staticGetID() {
    return mID;
    }



inline void LnExpression::print() {
    printf( "( ln" );
    
    mArgument->print();
    printf( " )" );
    }
            


inline Expression *LnExpression::copy() {
    LnExpression *copy =
        new LnExpression( mArgument->copy() );
    
    return copy;
    }        


    
#endif
