#include <stdio.h>
#include <stdlib.h>

#include "fortify.h"


int main() {
    //Fortify_EnterScope();

    int *a = new int[ 10 ];
  
    int i;
    for( i=0; i<10; i++ ) {

        a[i] = i;
        
        }
    
    for( i=0; i<10; i++ ) {

        printf( "a[i] = %d\n", a[i] );
        
        }


    delete [] a;
    
    for( i=0; i<10; i++ ) {

        printf( "a[i] = %d\n", a[i] );
        
        }
    //Fortify_CheckAllMemory();
    
    for( i=0; i<10; i++ ) {

        a[i] = i;
        
        }
    for( i=0; i<10; i++ ) {

        printf( "a[i] = %d\n", a[i] );
        
        }

    //Fortify_CheckAllMemory();
    
    int *b = new int[5];
    
    delete [] b;

    delete [] a;
    
    //Fortify_LeaveScope();
    
    return 0;
    }
