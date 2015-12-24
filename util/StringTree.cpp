#include "StringTree.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"


#include <stdio.h>

// for platform-independent ptr -> unsigned int casting
#include <stdint.h>



typedef struct valueHolder {
        void *value;
        char mark;
    } valueHolder;


#define B 2000

class ValueHashTable {
    public:
        
        ~ValueHashTable();
        
        void insert( valueHolder *inHolder );
        
        void remove( valueHolder *inHolder );
        
        valueHolder *lookup( void *inValue );

    protected:
        
        SimpleVector<valueHolder *> mBins[B];
        
    };



ValueHashTable::~ValueHashTable() {
    for( int b=0; b<B; b++ ) {
        SimpleVector<valueHolder *> *v = &( mBins[b] );
        
        int numHits = v->size();
        
        for( int i=0; i<numHits; i++ ) {
            delete *( v->getElement( i ) );
            }
        }
    }

        
        
void ValueHashTable::insert( valueHolder *inHolder ) {
    int b = (uintptr_t)( inHolder->value ) % B;
    
    mBins[b].push_back( inHolder );
    }



void ValueHashTable::remove( valueHolder *inHolder ) {
    int b = (uintptr_t)( inHolder->value ) % B;
    
    mBins[b].deleteElementEqualTo( inHolder );
    }


        
valueHolder *ValueHashTable::lookup( void *inValue ) {
    int b = (uintptr_t)( inValue ) % B;
    
    SimpleVector<valueHolder *> *v = &( mBins[b] );
    
    int numHits = v->size();
    
    for( int i=0; i<numHits; i++ ) {
        valueHolder *holder = *( v->getElement( i ) );

        if( holder->value == inValue ) {
            return holder;
            }
        }

    return NULL;
    }







class StringTreeNode {
    public:
        StringTreeNode( char inChar, StringTreeNode *inParent );
        
        ~StringTreeNode();
        

        void insert( const char *inString, void *inValue,
                     ValueHashTable *inHashTable );
        

        // removes value at this node, and recursively deletes upward if
        // this node is now empty
        void remove( valueHolder *inHolder );
        

        StringTreeNode *search( const char *inString );


        // is this node empty
        char isEmpty();

        
        // use inDownOnly for root node of a match (to get everything below
        // that node, but not to left or right of that node)
        int countValuesBelow( char inDownOnly );
        
        

        // pointers for inNumToGet and inNumToSkip because they are 
        // used for tracking progress in recursion (modified during call)
        // returns num gotten
        void getValuesBelow( int *inNumToGet, int *inNumToSkip, 
                             SimpleVector<void *> *outValues,
                             char inDownOnly );
        
        
        void unmarkValuesBelow( char inDownOnly );
        

        void print();
        
        char mChar;
        SimpleVector<valueHolder*> mValues;
        
        StringTreeNode *mParent;
        
        StringTreeNode *mLeft;
        StringTreeNode *mDown;
        StringTreeNode *mRight;

    protected:
        
        void removeChild( StringTreeNode *inChild );
        
        
    };



StringTreeNode::StringTreeNode( char inChar, StringTreeNode *inParent )
        : mChar( inChar ),
          mParent( inParent ), mLeft( NULL ), mDown( NULL), mRight( NULL ) {
    }


        
StringTreeNode::~StringTreeNode() {
    if( mLeft != NULL ) {
        delete mLeft;
        }
    if( mDown != NULL ) {
        delete mDown;
        }
    if( mRight != NULL ) {
        delete mRight;
        }
    }



extern void printResourceRecord( void *inR );


void StringTreeNode::insert( const char *inString, void *inValue,
                             ValueHashTable *inHashTable ) {
    
    if( inString[0] == mChar ) {
        // match

        if( inString[1] == '\0' ) {

            // ends here
            valueHolder *v = inHashTable->lookup( inValue );
            
            if( v == NULL ) {
                v = new valueHolder;
                v->value = inValue;
                v->mark = false;
                inHashTable->insert( v );
                }
            
            mValues.push_back( v );
            }
        else {
            // pass down, skipping matched char
            
            if( mDown == NULL ) {
                // create it
                mDown = new StringTreeNode( inString[1], this );
                }
            
            mDown->insert( &( inString[1] ), inValue, inHashTable );
            }
        }
    else {
        // non-match
        
        if( inString[0] < mChar ) {
            // left    
            if( mLeft == NULL ) {
                mLeft = new StringTreeNode( inString[0], this );
                }
            
            mLeft->insert( inString, inValue, inHashTable );
            }
        else if( inString[0] > mChar ) {
            // right    
            if( mRight == NULL ) {
                mRight = new StringTreeNode( inString[0], this );
                }
            
            mRight->insert( inString, inValue, inHashTable );
            }
        }
    }



char StringTreeNode::isEmpty() {
    if( mValues.size() == 0
        && mLeft == NULL
        && mDown == NULL
        && mRight == NULL ) {

        return true;
        }
    return false;
    }




void StringTreeNode::remove( valueHolder *inHolder ) {
    
    mValues.deleteElementEqualTo( inHolder );
    

    if( isEmpty() ) {
        
        if( mParent != NULL ) {
            mParent->removeChild( this );
            }
        }
    }



void StringTreeNode::removeChild( StringTreeNode *inChild ) {

    if( mLeft == inChild ) {
        delete mLeft;
        mLeft = NULL;
        }
    if( mDown == inChild ) {
        delete mDown;
        mDown = NULL;
        }
    if( mRight == inChild ) {
        delete mRight;
        mRight = NULL;
        }
    

    if( isEmpty() ) {
        
        if( mParent != NULL ) {
            mParent->removeChild( this );
            }
        }
    }








StringTreeNode *StringTreeNode::search( const char *inString ) {
    if( inString[0] == mChar ) {
        // match

        if( inString[1] == '\0' ) {
            // ends here
            return this;
            }
        else {
            // pass down, skipping matched char
            
            if( mDown == NULL ) {
                // not found
                return NULL;
                }
            return mDown->search( &( inString[1] ) );
            }
        }
    else {
        // non-match
        
        if( inString[0] < mChar ) {
            // left    
            if( mLeft == NULL ) {
                return NULL;
                }
            
            return mLeft->search( inString );
            }
        else {
            // right    
            if( mRight == NULL ) {
                return NULL;
                }
            
            return mRight->search( inString );
            }
        }
    }



int StringTreeNode::countValuesBelow( char inDownOnly ) {
    
    int num = 0;
    
    if( !inDownOnly && mLeft != NULL ) {
        num += mLeft->countValuesBelow( false );
        }
    
    int numHere = mValues.size();
    
    for( int i=0; i<numHere; i++ ) {
        valueHolder *v = *( mValues.getElement( i ) );
        
        if( !v->mark ) {
            // mark it
            v->mark = true;
            
            // count it
            num++;
            }
        // else already counted... skip it
        }
    

    if( mDown != NULL ) {
        num += mDown->countValuesBelow( false );
        }

    if( !inDownOnly && mRight != NULL ) {
        num += mRight->countValuesBelow( false );
        }

    return num;
    }




void StringTreeNode::getValuesBelow( int *inNumToGet, int *inNumToSkip, 
                                     SimpleVector<void *> *outValues,
                                     char inDownOnly ) {

    if( !inDownOnly && mLeft != NULL ) {
        mLeft->getValuesBelow( inNumToGet, inNumToSkip,
                               outValues, false );
        }
    
    if( *inNumToGet == 0 ) {
        return;
        }
    
    // self
    int numHere = mValues.size();

    // newest values first
    for( int i=numHere-1; i>=0 && *inNumToGet > 0; i-- ) {
        
        valueHolder *v = *( mValues.getElement( i ) );
        
        if( !v->mark ) {
            // mark it
            v->mark = true;

            if( *inNumToSkip > 0 ) {
                (*inNumToSkip) --;
                }
            else {
                // done skipping

                // take value
                outValues->push_back( v->value );
            
                (*inNumToGet) --;
                }
            }
        }




    if( *inNumToGet == 0 ) {
        return;
        }
    

    if( mDown != NULL ) {
        mDown->getValuesBelow( inNumToGet, inNumToSkip,
                               outValues, false );
        }


    if( *inNumToGet == 0 ) {
        return;
        }
    

    if( !inDownOnly && mRight != NULL ) {
        mRight->getValuesBelow( inNumToGet, inNumToSkip,
                                outValues, false );
        }

    }



void StringTreeNode::unmarkValuesBelow( char inDownOnly ) {
    
    if( !inDownOnly && mLeft != NULL ) {
        mLeft->unmarkValuesBelow( false );
        }
    
    int numHere = mValues.size();
    
    for( int i=0; i<numHere; i++ ) {
        valueHolder *v = *( mValues.getElement( i ) );
        v->mark = false;
        }
    

    if( mDown != NULL ) {
        mDown->unmarkValuesBelow( false );
        }

    if( !inDownOnly && mRight != NULL ) {
        mRight->unmarkValuesBelow( false );
        }
    }



void StringTreeNode::print() {
    
    // Graphviz format

    char *valueString = stringDuplicate( "" );
    
    /*
    for( int i=0; i<mValues.size(); i++ ) {
        char *newString = 
            autoSprintf( "%s%s, ", valueString, 
                         (char*)( *( mValues.getElement( i ) ) )->value );
        
        delete [] valueString;
        valueString = newString;
        }
    */

    printf( "n%p; n%p [label = \"%c (%s) {%p}\"]; ",
            (void *)this, (void *)this, mChar, valueString, 
            (void *)this );
    
    delete [] valueString;

    if( mLeft != NULL ) {
        mLeft->print();
        printf( "n%p -> n%p [label = \"L\", weight=1]; ", 
                (void *)this, (void *)mLeft );
        }
    if( mDown != NULL ) {
        mDown->print();
        printf( "n%p -> n%p [label = \"D\", weight=4]; ", 
                (void *)this, (void *)mDown );
        }
    if( mRight != NULL ) {
        mRight->print();
        printf( "n%p -> n%p [label = \"R\", weight=1]; ", 
                (void *)this, (void *)mRight );
        }


    }
        
    







StringTree::StringTree()
        : mTreeRoot( NULL ),
          mHashTable( new ValueHashTable ) {
    }

        
StringTree::~StringTree() {
    if( mTreeRoot != NULL ) {
        delete mTreeRoot;
        }
    delete mHashTable;
    }

        

void StringTree::insert( const char *inString, void *inValue ) {
    if( mTreeRoot == NULL ) {
        mTreeRoot = new StringTreeNode( inString[0], NULL );
        }
    

    // insert all suffixes
    int numChars = strlen( inString );

    
    for( int i=0; i<numChars; i++ ) {    
        mTreeRoot->insert( &( inString[i] ), inValue, mHashTable );
        }
    
    /*
    printf( "After inseriting %s, tree is:\n", inString );
    mTreeRoot->print();
    
    printf( "\n\n\n" );
    */
    }


void StringTree::remove( const char *inString, void *inValue ) {
    if( mTreeRoot == NULL ) {
        return;
        }

    valueHolder *holder = mHashTable->lookup( inValue );
    
    if( holder != NULL ) {
        
    
        // search for all suffixes to find nodes that hold our value
        int numChars = strlen( inString );
        
        for( int i=0; i<numChars; i++ ) {  
            
            StringTreeNode *matchNode = mTreeRoot->search( &( inString[i] ) );
    
            if( matchNode != NULL ) {

                matchNode->remove( holder );
                }
            }
        mHashTable->remove( holder );

        delete holder;
        }

    if( mTreeRoot->isEmpty() ) {
        delete mTreeRoot;
        mTreeRoot = NULL;
        }
    
    /*
    printf( "After removing %s, tree is:\n", inString );
    if( mTreeRoot != NULL ) {
        mTreeRoot->print();
        }
    else {
        printf( "NULL" );
        }
    printf( "\n\n\n" );
    */
    }

        
int StringTree::countMatches( const char *inSearch ) {
    if( mTreeRoot == NULL ) {
        return 0;
        }
    
    
    StringTreeNode *matchNode;
    char downOnly;
    
    if( inSearch[0] == '\0' ) {
        // empty search, whole tree
        matchNode = mTreeRoot;
        downOnly = false;
        }
    else {
        matchNode = mTreeRoot->search( inSearch );
        downOnly = true;
        }

    
    if( matchNode != NULL ) {
        int numMatches = matchNode->countValuesBelow( downOnly );
        
        matchNode->unmarkValuesBelow( downOnly );

        return numMatches;
        }
    else {
        return 0;
        }
    }

        

int StringTree::getMatches( const char *inSearch, 
                            int inNumToSkip, int inNumToGet,
                            void **outValues ) {
    
    if( mTreeRoot == NULL ) {
        return 0;
        }


    StringTreeNode *matchNode;
    char downOnly;
    
    if( inSearch[0] == '\0' ) {
        // empty search, whole tree
        matchNode = mTreeRoot;
        downOnly = false;
        }
    else {
        matchNode = mTreeRoot->search( inSearch );
        downOnly = true;
        }

    if( matchNode == NULL ) {
        return 0;
        }
    

    SimpleVector<void *> results;
    
    // these are modified by call
    int numToGet = inNumToGet;
    int numToSkip = inNumToSkip;
    
    matchNode->getValuesBelow( &numToGet, &numToSkip, 
                               &results, downOnly );

    matchNode->unmarkValuesBelow( downOnly );


    int numResults = results.size();
    

    void **resultArray = results.getElementArray();
    
    int numToCopy = numResults;
    if( inNumToGet < numToCopy ) {
        numToCopy = inNumToGet;
        }
    

    memcpy( outValues, resultArray, numToCopy * sizeof( void * ) );

    delete [] resultArray;


    return numToCopy;
    }

        

void StringTree::print() {
    printf( "Printing StringTree:\n" );

    printf( "#  command:  dot -Tpng test.graph >graph.png\n\n" );
    

    printf( "digraph G{\n\n" );

    if( mTreeRoot != NULL ) {
        mTreeRoot->print();
        }
    else {
        //printf( "NULL" );
        }
    printf( "\n}\n\n" );
    }
