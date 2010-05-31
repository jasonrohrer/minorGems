/*
 * Modification History 
 *
 * 2001-February-10		Jason Rohrer
 * Created. 
 *
 * 2001-March-7		Jason Rohrer
 * Added a copy() function interface.
 *
 * 2001-September-4		Jason Rohrer
 * Added a virtual destructor to fix a major memory leak.
 *
 * 2001-September-9		Jason Rohrer
 * Added an extractArgument() function to work around some
 * object destruction issues.
 *
 * 2003-May-9   Jason Rohrer
 * Added support for replacing a variable.
 */
 
 
#ifndef EXPRESSION_INCLUDED
#define EXPRESSION_INCLUDED 

#include <stdio.h>

#include "Variable.h"


 
/**
 * Interface for an expression object. 
 *
 * @author Jason Rohrer 
 */
class Expression { 
	
	public:

		
		virtual ~Expression();

				
		/**
		 * Evaluates this expression.
		 *
		 * @return the value of this expression.
		 */
		virtual double evaluate() = 0;
		
		
		/**
		 * Gets the unique ID of this expression subtype.
		 *
		 * @return the ID of this subtype.
		 */
		virtual long getID() = 0;
		
		
		/**
		 * Gets the number of arguments taken by this expression subtype.
		 *
		 * @return the number of arguments for this subtype.
		 */
		virtual long getNumArguments() = 0;
		
		
		/**
		 * Gets a specified argument for this expression.
		 *
		 * @param inArgumentNumber the index of this argument.
		 *
		 * @return the specified argument.  Will be destroyed
		 *   when this class is destroyed.  Returns NULL if 
		 *   the argument has not been set.
		 */
		virtual Expression *getArgument( long inArgumentNumber ) = 0;
		
		
		/**
		 * Sets a specified argument for this expression.
		 *
		 * @param inArgumentNumber the index of this argument.
		 * @param inArgument the specified argument.  Will be destroyed
		 *   when this class is destroyed, or by another call to 
		 *   setArgument.
		 */
		virtual void setArgument( long inArgumentNumber, 
			Expression *inArgument ) = 0;



		/**
		 * Extracts an argument from this expression.
		 *
		 * This is similar to getArgument(), except that internally
		 * this expression's argument is set to NULL.  In other words,
		 * this allows you to setArgument( NULL ) without destroying
		 * the argument.
		 *
		 * @param inArgumentNumber the argument to get.  Must be destroyed
		 *   by caller.  Returns NULL if the argument has not been set.
		 */
		virtual Expression *extractArgument( long inArgumentNumber ) = 0;

		
		
		/**
		 * Prints this expression to standard out.
		 */
		virtual void print() = 0;
		
		
		/**
		 * Makes a copy of this expression recursively.
		 *
		 * @return a copy of this expression.
		 */
		virtual Expression *copy() = 0;



        /**
         * Recursively replace a variable in this expression.
         *
         * Default implementation calls replacement recursively on each
         * argument.  Expressions that actually deal with variables
         * should override this implementation.
         *
         * @param inTarget the target to replace.
         *   Must be destroyed by caller.
         * @param inReplacement the variable to replace the target with.
         *   Must be destroyed by caller after this expression is destroyed.
         */
        virtual void replaceVariable( Variable *inTarget,
                                      Variable *inReplacement );

        
		
	};



inline Expression::~Expression() {
	// do nothing
	}



inline void Expression::replaceVariable( Variable *inTarget,
                                         Variable *inReplacement ) {

    // call recursively on each of our arguments

    int numArgs = getNumArguments();
    for( int i=0; i<numArgs; i++ ) {
        Expression *arg = getArgument( i );
        arg->replaceVariable( inTarget, inReplacement );
        }
    }


	
#endif
