/*
 * Modification History 
 *
 * 2003-April-28   Jason Rohrer
 * Created.
 */
 

#ifndef COS_EXPRESSION_INCLUDED
#define COS_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 

#include <math.h>
 
/**
 * Expression implementation of a unary cos operation. 
 *
 * @author Jason Rohrer 
 */
class CosExpression : public UnaryOperationExpression { 
    
    public:
        
        /**
         * Constructs a unary cos operation expression.
         *
         * Argument is destroyed when the class is destroyed.
         *
         * @param inArgument the argument.
         */
        CosExpression( Expression *inArgument );
        
        
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
long CosExpression::mID = 11;



inline CosExpression::CosExpression( Expression *inArgument ) 
    : UnaryOperationExpression( inArgument ) {
    
    }



inline double CosExpression::evaluate() {
    return cos( mArgument->evaluate() );
    }



inline long CosExpression::getID() {    
    return mID;
    }    



inline long CosExpression::staticGetID() {
    return mID;
    }



inline void CosExpression::print() {
    printf( "( cos" );
    
    mArgument->print();
    printf( " )" );
    }
            


inline Expression *CosExpression::copy() {
    CosExpression *copy =
        new CosExpression( mArgument->copy() );
    
    return copy;
    }        


    
#endif
