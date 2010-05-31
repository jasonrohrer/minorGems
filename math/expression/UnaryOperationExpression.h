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
 
 
#ifndef UNARY_OPERATION_EXPRESSION_INCLUDED
#define UNARY_OPERATION_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
 
/**
 * Abstract base for an unary operation expression object. 
 *
 * @author Jason Rohrer 
 */
class UnaryOperationExpression : public Expression { 
	
	public:
		
		/**
		 * Constructs a unary operation expression.
		 *
		 * Argument is destroyed when the class is destroyed.
		 *
		 * @param inArgument the argument.  Defaults to NULL.
		 */
		UnaryOperationExpression( Expression *inArgument = NULL );
		
		virtual ~UnaryOperationExpression();
		
		/**
		 * Sets the argument for the operation.
		 *
		 * @param inArgument the argument.  Is destroyed
		 *   when the class is destroyed, or by another
		 *   call to setArgument.
		 */
		void setArgument( Expression *inArgument );

		
		/**
		 * Gets the argument for the operation.
		 *
		 * @return the argument.  Is destroyed
		 *   when the class is destroyed, or by another
		 *   call to setArgumentB.  Returns NULL if the
		 *   argument was never set.
		 */
		Expression *getArgument();
		
		
		// implements the Expression interface
		virtual long getNumArguments();
		virtual Expression *getArgument( long inArgumentNumber );
		virtual void setArgument( long inArgumentNumber, 
			Expression *inArgument );
		virtual Expression *extractArgument( long inArgumentNumber );
		
	protected:
		Expression *mArgument;		
	};



inline UnaryOperationExpression::UnaryOperationExpression(
	Expression *inArgument )
	: mArgument( inArgument ) {
	
	}


		
inline UnaryOperationExpression::~UnaryOperationExpression() {
	if( mArgument != NULL ) {
		delete mArgument;
		}
	}



inline void UnaryOperationExpression::setArgument( Expression *inArgument ) {
	if( mArgument != NULL && inArgument != mArgument ) {
		delete mArgument;
		}
	mArgument = inArgument;
	}



inline Expression *UnaryOperationExpression::getArgument() {
	return mArgument;
	}



long UnaryOperationExpression::getNumArguments() {
	return 1;
	}



inline Expression *UnaryOperationExpression::getArgument( 
	long inArgumentNumber ) {
	if( inArgumentNumber == 0 ) {
		return getArgument();
		}
	else {	
		return NULL;
		}
	}


	
inline void UnaryOperationExpression::setArgument( long inArgumentNumber, 
	Expression *inArgument ) {
	
	if( inArgumentNumber == 0 ) {
		setArgument( inArgument );
		return;
		}
	else {	
		return;
		}
	}



inline Expression *UnaryOperationExpression::extractArgument( 
	long inArgumentNumber ) {
	Expression *returnArg = mArgument;
	mArgument = NULL;
	return returnArg;
	}


	
#endif

