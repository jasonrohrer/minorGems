#include <winsock.h>
#include <windows.h>

#include <sys/stat.h>
#include <stdio.h>


int main() {

    WORD wVersionRequested;
    WSADATA wsaData;
    
    int err; 
    wVersionRequested = MAKEWORD( 1, 0 ); 
    err = WSAStartup( wVersionRequested, &wsaData );
    
    if ( err != 0 ) {
        // no usable DLL found  
        printf( "WinSock DLL version 1.0 or higher not found.\n" );  
        
        return -1;
        }

    int error = WSAGetLastError();

    printf( "WSA Error before stat call = %d\n", error );

    struct stat fileInfo;
    int statError = stat( "DoesNotExist", &fileInfo );

    printf( "stat error = %d\n", statError );

    error = WSAGetLastError();

    printf( "WSA Error after stat call = %d\n", error );

    return 0;
    }



