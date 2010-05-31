/*
 * Modification History 
 *
 * 2001-February-10		Jason Rohrer
 * Created.  
 *
 * 2001-September-4		Jason Rohrer
 * Changed the destructor to virtual to fix a memory leak.
 *
 * 2001-September-9		Jason Rohrer
 * Changed setArgument to be more intelligent about duplicated calls.
 * Added an extractArgument() implemenation.
 */
 
 
#ifndef BINARY_OPERATION_EXPRESSION_INCLUDED
#define BINARY_OPERATION_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
 
/**
 * Abstract base for an binary operation expression object. 
 *
 * @author Jason Rohrer 
 */
class BinaryOperationExpression : public Expression { 
	
	public:
		
		/**
		 * Constructs a binary operation expression.
		 *
		 * Both arguments are destroyed when the class is destroyed.
		 *
		 * @param inArgumentA the first argument.  Defaults to NULL.
		 * @param inArgumentA the first argument.  Defaults to NULL.
		 */
		BinaryOperationExpression( Expression *inArgumentA = NULL, 
			Expression *inArgumentB = NULL );
		
		virtual ~BinaryOperationExpression();
		
		/**
		 * Sets the first argument for the operation.
		 *
		 * @param inArgument the first argument.  Is destroyed
		 *   when the class is destroyed, or by another
		 *   call to setArgumentA.
		 */
		void setArgumentA( Expression *inArgument );
		
		
		/**
		 * Sets the second argument for the operation.
		 *
		 * @param inArgument the second argument.  Is destroyed
		 *   when the class is destroyed, or by another
		 *   call to setArgumentB.
		 */
		void setArgumentB( Expression *inArgument );
		
		
		/**
		 * Gets the first argument for the operation.
		 *
		 * @return the first argument.  Is destroyed
		 *   when the class is destroyed, or by another
		 *   call to setArgumentB.  Returns NULL if the
		 *   argument was never set.
		 */
		Expression *getArgumentA();
		
		
		
		/**
		 * Gets the second argument for the operation.
		 *
		 * @return the second argument.  Is destroyed
		 *   when the class is destroyed, or by another
		 *   call to setArgumentB.  Returns NULL if the
		 *   argument was never set.
		 */
		Expression *getArgumentB();
		
		
		// implements the Expression interface
		virtual long getNumArguments();
		virtual Expression *getArgument( long inArgumentNumber );
		virtual void setArgument( long inArgumentNumber, 
			Expression *inArgument );
		virtual Expression *extractArgument( long inArgumentNumber );
	
	protected:
		Expression *mArgumentA;
		Expression *mArgumentB;		
	};



inline BinaryOperationExpression::BinaryOperationExpression(
	Expression *inArgumentA, Expression *inArgumentB )
	: mArgumentA( inArgumentA ), mArgumentB( inArgumentB ) {
	
	}


		
inline BinaryOperationExpression::~BinaryOperationExpression() {
	if( mArgumentA != NULL ) {
		delete mArgumentA;
		}
	if( mArgumentB != NULL ) {
		delete mArgumentB;
		}
	}



inline void BinaryOperationExpression::setArgumentA( Expression *inArgument ) {
	if( mArgumentA != NULL && inArgument != mArgumentA ) {
		delete mArgumentA;
		}
	mArgumentA = inArgument;
	}


	
inline void BinaryOperationExpression::setArgumentB( Expression *inArgument ) {
	if( mArgumentB != NULL && inArgument != mArgumentB ) {
		delete mArgumentB;
		}
	mArgumentB = inArgument;
	}



inline Expression *BinaryOperationExpression::getArgumentA() {
	return mArgumentA;
	}


	
inline Expression *BinaryOperationExpression::getArgumentB() {
	return mArgumentB;
	}



inline long BinaryOperationExpression::getNumArguments() {
	return 2;
	}



inline Expression *BinaryOperationExpression::getArgument( 
	long inArgumentNumber ) {
	switch( inArgumentNumber ) {
		case 0:
			return getArgumentA();
			break;
		case 1:
			return getArgumentB();
			break;
		default:
			return NULL;
			break;
		}
	}


	
inline void BinaryOperationExpression::setArgument( long inArgumentNumber, 
	Expression *inArgument ) {
	
	switch( inArgumentNumber ) {
		case 0:
			setArgumentA( inArgument );
			return;
			break;
		case 1:
			setArgumentB( inArgument );
			return;
			break;
		default:
			return;
			break;
		}
	}



inline Expression *BinaryOperationExpression::extractArgument( 
	long inArgumentNumber ) {

	Expression *returnArg;
	
	switch( inArgumentNumber ) {
		case 0:
			returnArg = mArgumentA;
			mArgumentA = NULL;
			return returnArg;
			break;
		case 1:
			returnArg = mArgumentB;
			mArgumentB = NULL;
			return returnArg;
			break;
		default:
			return NULL;
			break;
		}
	}



#endif
