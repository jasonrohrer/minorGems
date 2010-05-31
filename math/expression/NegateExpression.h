/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created.
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function.
 */
 
 
#ifndef NEGATE_EXPRESSION_INCLUDED
#define NEGATE_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "UnaryOperationExpression.h" 
 
/**
 * Expression implementation of a unary negate operation. 
 *
 * @author Jason Rohrer 
 */
class NegateExpression : public UnaryOperationExpression { 
	
	public:
		
		/**
		 * Constructs a unary negate operation expression.
		 *
		 * Argument is destroyed when the class is destroyed.
		 *
		 * @param inArgument the argument.
		 */
		NegateExpression( Expression *inArgument );
		
		
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
long NegateExpression::mID = 3;



inline NegateExpression::NegateExpression( Expression *inArgument ) 
	: UnaryOperationExpression( inArgument ) {
	
	}



inline double NegateExpression::evaluate() {
	return -( mArgument->evaluate() );
	}

	

inline long NegateExpression::getID() {	
	return mID;
	}



inline long NegateExpression::staticGetID() {
	return mID;
	}



inline void NegateExpression::print() {
	printf( "( -" );
	mArgument->print();
	printf( " )" );
	}



inline Expression *NegateExpression::copy() {
	NegateExpression *copy = new NegateExpression( mArgument->copy() );
	return copy;
	}



#endif
