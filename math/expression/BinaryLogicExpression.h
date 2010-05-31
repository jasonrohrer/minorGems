/*
 * Modification History 
 *
 * 2003-May-2   Jason Rohrer
 * Created.
 *
 * 2003-May-3   Jason Rohrer
 * Fixed a few compile-time errors.
 */


 
#ifndef BINARY_LOGIC_EXPRESSION_INCLUDED
#define BINARY_LOGIC_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "BinaryOperationExpression.h" 



/**
 * Expression implementation of a binary logic operation.
 *
 * Evaluating this expression returns 1 if the logic operation is true
 * and 0 if it is false.
 *
 * During evaluation, positive values are treated as logical true,
 * while all other values (zero and negative) are treated as false.
 *
 * @author Jason Rohrer 
 */
class BinaryLogicExpression : public BinaryOperationExpression { 
    
    public:

        static const int LOGIC_AND = 0;
        static const int LOGIC_OR = 1;
        static const int LOGIC_XOR = 2;
        
        /**
         * Constructs a binary logic operation expression.
         *
         * Both arguments are destroyed when the class is destroyed.
         *
         * @param inLogicOperation the logical operation, one of
         *   LOGIC_AND, LOGIC_OR, LOGIC_XOR.
         * @param inArgumentA the first argument.
         * @param inArgumentB the second argument.
         */
        BinaryLogicExpression( int inLogicOperation,
                               Expression *inArgumentA, 
                               Expression *inArgumentB );

        

        /**
         * Gets the logical operation.
         *
         * @return one of LOGIC_AND, LOGIC_OR, LOGIC_XOR.
         */
        int getLogicOperation();



        /**
         * Sets the logical operation
         *
         * @param inLogicOperation LOGIC_AND, LOGIC_OR, LOGIC_XOR.
         */
        void setLogicOperation( int inLogicOperation );


        
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

        int mLogicOperation;
        
    };



// static init
long BinaryLogicExpression::mID = 16;



inline BinaryLogicExpression::BinaryLogicExpression(
    int inLogicOperation,
    Expression *inArgumentA, 
    Expression *inArgumentB ) 
    : BinaryOperationExpression( inArgumentA, inArgumentB ),
      mLogicOperation( inLogicOperation ) {
    
    }



inline int BinaryLogicExpression::getLogicOperation() {
    return mLogicOperation;
    }



inline void BinaryLogicExpression::setLogicOperation( int inLogicOperation ) {
    mLogicOperation = inLogicOperation;
    }



inline double BinaryLogicExpression::evaluate() {
    if( mLogicOperation == LOGIC_AND ) {
        if( mArgumentA->evaluate() > 0 && mArgumentB->evaluate() > 0 ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mLogicOperation == LOGIC_OR ) {
        if( mArgumentA->evaluate() > 0 || mArgumentB->evaluate() > 0 ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else if( mLogicOperation == LOGIC_XOR ) {
        double valA = mArgumentA->evaluate();
        double valB = mArgumentB->evaluate();
        if( ( valA > 0 && valB <= 0 ) || ( valA <= 0 && valB > 0 ) ) {
            return 1;
            }
        else {
            return 0;
            }
        }
    else {
        // unknown operation type
        // default to false
        return 0;
        }
    }

    

inline long BinaryLogicExpression::getID() {    
    return mID;
    }



inline long BinaryLogicExpression::staticGetID() {
    return mID;
    }



inline void BinaryLogicExpression::print() {
    printf( "( " );
    
    mArgumentA->print();

    if( mLogicOperation == LOGIC_AND ) {
        printf( " and " );
        }
    else if( mLogicOperation == LOGIC_OR ) {
        printf( " or " );
        }
    else if( mLogicOperation == LOGIC_XOR ) {
        printf( " xor " );
        }
    else {
        printf( " UNKNOWN_LOGIC_OPERATION " );
        }

    mArgumentB->print();
    printf( " )" );
    }
            


inline Expression *BinaryLogicExpression::copy() {
    BinaryLogicExpression *copy =
        new BinaryLogicExpression( mLogicOperation,
                                   mArgumentA->copy(),
                                   mArgumentB->copy() );
    return copy;
    }



#endif
