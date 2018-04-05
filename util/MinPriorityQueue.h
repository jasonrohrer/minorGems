#ifndef MIN_PRIORITY_QUEUE_INCLUDED
#define MIN_PRIORITY_QUEUE_INCLUDED



#include "minorGems/util/SimpleVector.h"

#include <float.h>



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
        

        void clear() {
            mStorage.deleteAll();
            mPriority.deleteAll();
            }
        
        

        void insert( Type inValue, double inPriority ) {
            
            // stick it at the bottom of the heap
            mStorage.push_back( inValue );
            mPriority.push_back( inPriority );
            
            // repair the minheap property
            bubbleUp( mStorage.size() - 1 );
            }

        
        double checkMinPriority() {
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
        
        
        void printHeap( int inNextParentIndex = 0, int depth = 0,
                        char inSecondChild = false,
                        char *inSkipVertLineMap = NULL ) {
            
            #define MAX_PRINT_HEAP_DEPTH 9999
            
            char skipVertLineMap[ MAX_PRINT_HEAP_DEPTH ];
            
            if( inSkipVertLineMap != NULL ) {
                memcpy( skipVertLineMap, inSkipVertLineMap, 
                        MAX_PRINT_HEAP_DEPTH );
                }
            else {
                memset( skipVertLineMap, false, MAX_PRINT_HEAP_DEPTH );
                }


            if( inNextParentIndex < mPriority.size() ) {
                
                if( inSecondChild ) {    
                    for( int s=0; s<1; s++ ) {
                        
                        for( int i=0; i<depth-1; i++ ) {
                            if( skipVertLineMap[i] ) {
                                printf( "      " );
                                }
                            else {
                                printf( " |    " );
                                }
                            }
                        
                        printf( " |\n" );
                        }
                    }
                
                
                for( int i=0; i<depth-1; i++ ) {
                    if( skipVertLineMap[i] ) {
                        printf( "      " );
                        }
                    else {
                        printf( " |    " );
                        }
                    }
                
                if( inNextParentIndex != 0 ) {
                    if( inSecondChild ) {
                        printf( " \\----" );
                        }
                    else {
                        printf( " |----" );
                        }
                    }
                
                printf( "(%f)\n", 
                        mPriority.getElementDirect( inNextParentIndex ) );
                
                int childA = 2 * inNextParentIndex + 1;
                int childB = 2 * inNextParentIndex + 2;
                
                printHeap( childA, depth + 1, false, skipVertLineMap );
                
                skipVertLineMap[ depth ] = true;
                printHeap( childB, depth + 1, true, skipVertLineMap );
                }
            
            }

        

    protected:


        void swap( int inIndexA, int inIndexB ) {
            double *aP = mPriority.getElement( inIndexA );
            double *bP = mPriority.getElement( inIndexB );

            Type *aV = mStorage.getElement( inIndexA );
            Type *bV = mStorage.getElement( inIndexB );


            double tempP = *aP;
            *aP = *bP;
            *bP = tempP;

            Type tempV = *aV;
            *aV = *bV;
            *bV = tempV;
            }
        


        void bubbleUp( int inIndex ) {
            int parent = (inIndex - 1 ) / 2;
            
            double thisP = mPriority.getElementDirect( inIndex );
            double parentP = mPriority.getElementDirect( parent );

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
            double smallestChildP = DBL_MAX;
            
            if( childA < num ) {
                double p = mPriority.getElementDirect( childA );
                
                if( p < smallestChildP ) {
                    smallestChild = childA;
                    smallestChildP = p;
                    }
                }

            if( childB < num ) {
                double p = mPriority.getElementDirect( childB );
                
                if( p < smallestChildP ) {
                    smallestChild = childB;
                    smallestChildP = p;
                    }
                }
            
            if( smallestChild != -1 ) {
                
                if( smallestChildP < mPriority.getElementDirect( inIndex ) ) {
                    // parent out of heap order with children
                    
                    // swap down
                    swap( inIndex, smallestChild );
                    
                    // recurse down
                    bubbleDown( smallestChild );
                    }
                }
            }
        

        


        SimpleVector<Type> mStorage;
        SimpleVector<double> mPriority;
    };



#endif
