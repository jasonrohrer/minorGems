

#include "SimpleVector.h"


#include <stdio.h>


class A {
    public:
        A() {
            mV.push_back( 50 );
            mV.push_back( 60 );
            }

        ~A() {
            printf( "A's destructor invoked\n" );
            }
        
        
        SimpleVector<int> mV;

        void print() {
            printf( "A{ " );
            
            for( int i=0; i<mV.size(); i++ ) {
                printf( "%d, ", *( mV.getElement( i ) ) );
                }
            printf( "}\n" );
            
            }
        
                
    };



int main() {
    
    SimpleVector<A> v;

    A a;
    

    v.push_back( a );
    v.push_back( a );
    
    // push more back to force vector expansion
    for( int i=0; i<10; i++ ) {    
        v.push_back( a );
        }
    


    /*
    printf( "making array\n" );
    

    A *array = new A[100];
    

    printf( "deleting array\n" );
    
    delete [] array;
    


    printf( "About to exit\n" );
    */
    printf( "making w\n" );
    SimpleVector<A> w;
    
    w.push_back( a );
    w.push_back( a );

    // push more back to force vector expansion
    for( int i=0; i<10; i++ ) {    
        w.push_back( a );
        }

    printf( "deleting from w\n" );

    // delete a few to test for leak
    w.deleteElement( 0 );
    w.deleteElement( 0 );
    
        

    
    {
        
        // copy constructor
        SimpleVector<A> clone = v;
    
        printf( "clone = " );
        
        for( int i=0; i<clone.size(); i++ ) {
            ( clone.getElement(i) )->print();
            }
        printf( "\n" );


        // assignment operator
        clone = w;
        
        printf( "clone = " );
        
        for( int i=0; i<clone.size(); i++ ) {
            ( clone.getElement(i) )->print();
            }
        printf( "\n" );

        // clone deleted here
        }
    
    
    // v still okay?
    printf( "v = " );
    
    for( int i=0; i<v.size(); i++ ) {
        ( v.getElement( i ) )->print();
        }
    printf( "\n" );

    // w still okay?
    printf( "w = " );
    
    for( int i=0; i<w.size(); i++ ) {
        ( w.getElement( i ) )->print();
        }
    printf( "\n" );
    
    }

    
