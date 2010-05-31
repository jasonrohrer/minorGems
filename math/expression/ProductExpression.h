/*
 * Modification History 
 *
 * 2001-February-11		Jason Rohrer
 * Created.
 *
 * 2001-March-10   Jason Rohrer
 * Added implementation of copy function. 
 */
 
 
#ifndef PRODUCT_EXPRESSION_INCLUDED
#define PRODUCT_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "BinaryOperationExpression.h" 
 
/**
 * Expression implementation of a binary product operation. 
 *
 * @author Jason Rohrer 
 */
class ProductExpression : public BinaryOperationExpression { 
	
	public:
		
		/**
		 * Constructs a binary product operation expression.
		 *
		 * Both arguments are destroyed when the class is destroyed.
		 *
		 * @param inArgumentA the first argument.
		 * @param inArgumentB the second argument.
		 */
		ProductExpression( Expression *inArgumentA, 
			Expression *inArgumentB );
		
		
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
long ProductExpression::mID = 5;



inline ProductExpression::ProductExpression( Expression *inArgumentA, 
	Expression *inArgumentB ) 
	: BinaryOperationExpression( inArgumentA, inArgumentB ) {
	
	}



inline double ProductExpression::evaluate() {
	return mArgumentA->evaluate() * mArgumentB->evaluate();
	}

	

inline long ProductExpression::getID() {	
	return mID;
	}



inline long ProductExpression::staticGetID() {
	return mID;
	}



inline void ProductExpression::print() {
	printf( "( " );
	
	mArgumentA->print();
	printf( " * " );
	mArgumentB->print();
	printf( " )" );
	}
			


inline Expression *ProductExpression::copy() {
	ProductExpression *copy =
		new ProductExpression( mArgumentA->copy(),
							   mArgumentB->copy() );
	return copy;
	}



#endif
