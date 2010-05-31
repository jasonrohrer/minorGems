/*
 * Modification History 
 *
 * 2003-May-2   Jason Rohrer
 * Created.
 */


 
#ifndef COMPARISON_EXPRESSION_INCLUDED
#define COMPARISON_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "BinaryOperationExpression.h" 



/**
 * Expression implementation of a binary comparison operation.
 *
 * Evaluating this expression returns 1 if the comparisson is true
 * and 0 if it is false.
 *
 * @author Jason Rohrer 
 */
class ComparisonExpression : public BinaryOperationExpression { 
    
    public:

        static const int GREATER_THAN = 0;
        static const int LESS_THAN = 1;
        static const int GREATER_THAN_OR_EQUAL_TO = 2;
        static const int LESS_THAN_OR_EQUAL_TO = 3;
        static const int EQUAL_TO = 4;
        static const int NOT_EQUAL_TO = 5;
        
        /**
         * Constructs a binary comparison operation expression.
         *
         * Both arguments are destroyed when the class is destroyed.
         *
         * @param inComparison the comparison to make, one of
         *   GREATER_THAN, LESS_THAN, GREATER_THAN_OR_EQUAL_TO,
         *   LESS_THAN_OR_EQUAL_TO, EQUAL_TO, NOT_EQUAL_TO.
         * @param inArgumentA the first argument.
         * @param inArgumentB the second argument.
         */
        ComparisonExpression( int inComparison,
                              Expression *inArgumentA, 
                              Expression *inArgumentB );

        

        /**
         * Gets the comparison being made.
         *
         * @return one of GREATER_THAN, LESS_THAN, GREATER_THAN_OR_EQUAL_TO,
         *   LESS_THAN_OR_EQUAL_TO, EQUAL_TO, NOT_EQUAL_TO.
         */
        int getComparison();



        /**
         * Sets the comparison to make.
         *
         * @param inComparison the comparison to make, one of
         *   GREATER_THAN, LESS_THAN, GREATER_THAN_OR_EQUAL_TO,
         *   LESS_THAN_OR_EQUAL_TO, EQUAL_TO, NOT_EQUAL_TO.
         */
        void setComparison( int inComparison );


        
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

        int mComparison;
        
    };



// static init
long ComparisonExpression::mID = 15;



inline ComparisonExpression::ComparisonExpression(
    int inComparison,
    Expression *inArgumentA, 
    Expression *inArgumentB ) 
    : BinaryOperationExpression( inArgumentA, inArgumentB ),
      mComparison( inComparison ) {
    
    }



inline int ComparisonExpression::getComparison() {
    return mComparison;
    }



inline void ComparisonExpression::setComparison( int inComparison ) {
    mComparison = inComparison;
    }



inline double ComparisonExpression::evaluate() {
    if( mComparison == GREATER_THAN ) {
        if( mArgumentA->evaluate() > mArgumentB->evaluate() ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mComparison == LESS_THAN ) {
        if( mArgumentA->evaluate() < mArgumentB->evaluate() ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mComparison == GREATER_THAN_OR_EQUAL_TO ) {
        if( mArgumentA->evaluate() >= mArgumentB->evaluate() ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mComparison == LESS_THAN_OR_EQUAL_TO ) {
        if( mArgumentA->evaluate() <= mArgumentB->evaluate() ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mComparison == EQUAL_TO ) {
        if( mArgumentA->evaluate() == mArgumentB->evaluate() ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mComparison == NOT_EQUAL_TO ) {
        if( mArgumentA->evaluate() != mArgumentB->evaluate() ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else {
        // unknown comparison type
        // default to false
        return 0;
        }
    }

    

inline long ComparisonExpression::getID() {    
    return mID;
    }



inline long ComparisonExpression::staticGetID() {
    return mID;
    }



inline void ComparisonExpression::print() {
    printf( "( " );
    
    mArgumentA->print();

    if( mComparison == GREATER_THAN ) {
        printf( " > " );
        }
    else if( mComparison == LESS_THAN ) {
        printf( " < " );
        }
    else if( mComparison == GREATER_THAN_OR_EQUAL_TO ) {
        printf( " >= " );
        }
    else if( mComparison == LESS_THAN_OR_EQUAL_TO ) {
        printf( " <= " );
        }
    else if( mComparison == EQUAL_TO ) {
        printf( " == " );
        }
    else if( mComparison == NOT_EQUAL_TO ) {
        printf( " != " );
        }
    else {
        printf( " UNKNOWN_COMPARISON " );
        }

    mArgumentB->print();
    printf( " )" );
    }
            


inline Expression *ComparisonExpression::copy() {
    ComparisonExpression *copy =
        new ComparisonExpression( mComparison,
                                  mArgumentA->copy(),
                                  mArgumentB->copy() );
    return copy;
    }



#endif
