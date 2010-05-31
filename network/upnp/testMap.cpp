#include "portMapping.h"

#include <stdio.h>


// false to unmap
char map = true;


int main() {
    
    if( map ) {
        
        char *externalIP;
    

        int result = mapPort( 7780, "test upnp",
                              2000, &externalIP );
    
        if( result == 1 ) {
            printf( "mapping success\n" );
            }
        else {
            printf( "mapping failure\n" );
            }
    

        if( externalIP != NULL ) {
            printf( "External IP = %s\n", externalIP );
        
            delete [] externalIP;
            }
        }
    else {
        int result = unmapPort( 7780, 2000 );
        if( result == 1 ) {
            printf( "unmapping success\n" );
            }
        else {
            printf( "unmapping failure\n" );
            }
        }
    
    return 0;
    }

    
