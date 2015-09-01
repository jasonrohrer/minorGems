#ifndef MIN_PRIORITY_QUEUE_INCLUDED
#define MIN_PRIORITY_QUEUE_INCLUDED



#include "minorGems/util/SimpleVector.h"

#include <limits.h>



// Dynamically-sized priority queue that can pop the element with 
// the minimum priority value

// Implemented with a binary heap, giving the following worst case 
// running times for a queue with n elements:

// Insert:  O(log n)
// Check min:  O(1)
// remove min: O(log n)

// The heap structure is stored in a dynamically-sized array with no
// pointers, using index jumps described in the Wikipedia Binary_heap article.
template <class Type>
class MinPriorityQueue {
	public:
        
        int size() {
            return mStorage.size();
            }
        

        void insert( Type inValue, unsigned int inPriority ) {
            
            // stick it at the bottom of the heap
            mStorage.push_back( inValue );
            mPriority.push_back( inPriority );
            
            // repair the minheap property
            bubbleUp( mStorage.size() - 1 );
            }

        
        unsigned int checkMinPriority() {
            if( mStorage.size() > 0 ) {
                return mPriority.getElementDirect( 0 );
                }
            else {
                return 0;
                }
            }


        Type removeMin() {
            if( mStorage.size() == 0 ) {
                Type t = Type();
                return t;
                }
            
            Type *root = mStorage.getElement( 0 );
            unsigned int *rootP = mPriority.getElement( 0 );
            
            Type returnValue = mStorage.getElementDirect( 0 );
            

            // swap root with last element
            int lastIndex = mStorage.size() - 1;
            
            swap( 0, lastIndex );
            
            // remove last element (now root value)            
            mStorage.deleteElement( lastIndex );
            mPriority.deleteElement( lastIndex );

            // repair min heap property
            bubbleDown( 0 );
            
            
            return returnValue;
            }
        
        
        

    protected:


        void swap( int inIndexA, int inIndexB ) {
            unsigned int *aP = mPriority.getElement( inIndexA );
            unsigned int *bP = mPriority.getElement( inIndexB );

            Type *aV = mStorage.getElement( inIndexA );
            Type *bV = mStorage.getElement( inIndexB );


            unsigned int tempP = *aP;
            *aP = *bP;
            *bP = tempP;

            Type tempV = *aV;
            *aV = *bV;
            *bV = tempV;
            }
        


        void bubbleUp( int inIndex ) {
            int parent = (inIndex - 1 ) / 2;
            
            unsigned int thisP = mPriority.getElementDirect( inIndex );
            unsigned int parentP = mPriority.getElementDirect( parent );

            if( thisP < parentP ) {

                // swap up
                
                swap( inIndex, parent );
                
                // recurse up
                bubbleUp( parent );
                }
            }
        

        
        void bubbleDown( int inIndex ) {
            int childA = 2 * inIndex + 1;
            int childB = 2 * inIndex + 2;

            int num = mStorage.size();
            
            int smallestChild = -1;
            unsigned int smallestChildP = UINT_MAX;
            
            if( childA < num ) {
                unsigned int p = mPriority.getElementDirect( childA );
                
                if( p < smallestChildP ) {
                    smallestChild = childA;
                    smallestChildP = p;
                    }
                }

            if( childB < num ) {
                unsigned int p = mPriority.getElementDirect( childB );
                
                if( p < smallestChildP ) {
                    smallestChild = childB;
                    smallestChildP = p;
                    }
                }
            
            if( smallestChild != -1 ) {
                // swap down
                swap( inIndex, smallestChild );
                
                // recurse down
                bubbleDown( smallestChild );
                }
            }
        

        SimpleVector<Type> mStorage;
        SimpleVector<unsigned int> mPriority;
    };



#endif
