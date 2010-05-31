/*
 * Modification History 
 *
 * 2003-April-29   Jason Rohrer
 * Created.
 *
 * 2003-May-9   Jason Rohrer
 * Added support for replacing a variable.
 */
 

#ifndef VARIABLE_EXPRESSION_INCLUDED
#define VARIABLE_EXPRESSION_INCLUDED 
 
#include "Expression.h" 
#include "Variable.h" 



/**
 * Expression that contains a single variable.
 *
 * @author Jason Rohrer 
 */
class VariableExpression : public Expression { 
    
    public:
        
        /**
         * Constructs a variable expression.
         *
         * @param inVariable the variable.
         *   Must be destroyed by caller after this class is destroyed.
         */
        VariableExpression( Variable *inVariable );



        /**
         * Gets this expression's variable.
         *
         * @return the variable.
         *   Must not be destroyed by caller.
         */
        Variable *getVariable();


        
        /**
         * A static version of getID().
         */
        static long staticGetID();

        
        
        // implements the Expression interface
        virtual double evaluate();
        virtual long getID();
        virtual void print();
        virtual Expression *copy();

        virtual long getNumArguments();
        virtual Expression *getArgument( long inArgumentNumber );
        virtual void setArgument( long inArgumentNumber, 
            Expression *inArgument );
        virtual Expression *extractArgument( long inArgumentNumber );

        

        // overrides the default implementation
        virtual void replaceVariable( Variable *inTarget,
                                      Variable *inReplacement );

        
        
    protected:
        static long mID;

        Variable *mVariable;
    };



// static init
long VariableExpression::mID = 14;



inline VariableExpression::VariableExpression( Variable *inVariable ) 
    : mVariable( inVariable ) {
    
    }



inline Variable *VariableExpression::getVariable() {
    return mVariable;
    }



inline double VariableExpression::evaluate() {
    return mVariable->getValue();
    }



inline long VariableExpression::getID() {    
    return mID;
    }    



inline long VariableExpression::staticGetID() {
    return mID;
    }



inline void VariableExpression::print() {
    char *varName = mVariable->getName();
    
    printf( " %s ", varName );

    delete [] varName;
    }
            


inline Expression *VariableExpression::copy() {
    // don't copy our variable
    VariableExpression *copy =
        new VariableExpression( mVariable );
    
    return copy;
    }        



long VariableExpression::getNumArguments() {
    return 0;
    }   



inline Expression *VariableExpression::getArgument( 
    long inArgumentNumber ) {
    
    return NULL;
    }


    
inline void VariableExpression::setArgument( long inArgumentNumber, 
    Expression *inArgument ) {
    
    // do nothing
    return;
    }



inline Expression *VariableExpression::extractArgument( 
    long inArgumentNumber ) {
    
    return NULL;
    }



inline void VariableExpression::replaceVariable( Variable *inTarget,
                                                 Variable *inReplacement ) {
    if( mVariable == inTarget ) {
        mVariable = inReplacement;
        }
    }


    
#endif
