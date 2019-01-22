#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include<sys/prctl.h>


#include "minorGems/util/stringUtils.h"


static void usage() {
    printf( "Usage:\n\n"
            "wallClockProfiler samples_per_sec ./myProgram\n\n" );
    exit( 1 );
    }


int inPipe;
int outPipe;



char sendBuff[1024];


static void sendCommand( const char *inCommand ) {
    sprintf( sendBuff, "%s\n", inCommand );
    write( outPipe, sendBuff, strlen( sendBuff ) );
    }


char readBuff[4096];

char programExited = false;


static int fillBufferWithResponse() {
    int readSoFar = 0;
    while( true ) {
        int numRead = 
            read( inPipe, &( readBuff[readSoFar] ), 4095 - readSoFar );
        
        if( numRead > 0 ) {
            
            readSoFar += numRead;
            
            readBuff[ readSoFar ] = '\0';
        
            if( strstr( readBuff, "(gdb)" ) != NULL ) {
                // read full response
                return readSoFar;
                }
            }
        }
    }




static void checkProgramExited() {
    if( strstr( readBuff, "exited-normally" ) != NULL ) {
        programExited = true;
        }
    }


static void printGDBResponse() {
    int numRead = fillBufferWithResponse();
    
    if( numRead > 0 ) {
        checkProgramExited();
        printf( "\n\nRead from GDB:  %s", readBuff );
        }
    }



static void printGDBResponseToFile( FILE *inFile ) {
    int numRead = fillBufferWithResponse();
    
    if( numRead > 0 ) {
        checkProgramExited();
        fprintf( inFile, "\n\nRead from GDB:  %s", readBuff );
        }
    }


static void skipGDBResponse() {
    int numRead = fillBufferWithResponse();
    checkProgramExited();
    }



int main( int inNumArgs, char **inArgs ) {
    printf( "%d args\n", inNumArgs );
    
    if( inNumArgs != 3 ) {
        usage();
        }
    
    int samplesPerSecond = 100;
    
    sscanf( inArgs[1], "%d", &samplesPerSecond );
    


    int readPipe[2];
    int writePipe[2];
    
    pipe( readPipe );
    pipe( writePipe );
    
    

    int childPID = fork();
    
    if( childPID == -1 ) {
        printf( "Failed to fork\n" );
        return 1;
        }
    else if( childPID == 0 ) {
        // child
        dup2( writePipe[0], STDIN_FILENO );
        dup2( readPipe[1], STDOUT_FILENO );
        dup2( readPipe[1], STDERR_FILENO );

        while( false && true ) {
            printf( "test\n" );
            }
        
        //ask kernel to deliver SIGTERM in case the parent dies
        prctl( PR_SET_PDEATHSIG, SIGTERM );

        execlp( "gdb", "gdb", "-nx", "--interpreter=mi", inArgs[2], NULL );

        exit( 0 );
        }
    
    // else parent
    printf( "Forked GDB child on PID=%d\n", childPID );
    
    
    //close unused pipe ends
    close( writePipe[0] );
    close( readPipe[1] );
    
    inPipe = readPipe[0];
    outPipe = writePipe[1];

    fcntl( inPipe, F_SETFL, O_NONBLOCK );

    skipGDBResponse();
    

    printf( "\n\nStarting gdb program with run\n" );

    sendCommand( "-exec-run" );

    usleep( 100000 );

    skipGDBResponse();
    
    printf( "Debugging program '%s'\n", inArgs[2] );

    char rawProgramName[100];
    
    char *endOfPath = strrchr( inArgs[2], '/' );

    char *progName = inArgs[2];
    
    if( endOfPath != NULL ) {
        progName = &( endOfPath[1] );
        }
    
    char *pidCall = autoSprintf( "pidof %s", progName );\

    FILE *pidPipe = popen( pidCall, "r" );

    if( pidPipe == NULL ) {
        printf( "Failed to open pipe to pidof to get debugged app pid\n" );
        return 1;
        }

    int pid = -1;
    
    // if there are multiple GDP procs, they are printed in newest-first order
    // this will get the pid of the latest one (our GDB child)
    int numRead = fscanf( pidPipe, "%d", &pid );
    
    pclose( pidPipe );

    if( numRead != 1 ) {
        printf( "Failed to read PID of debugged app\n" );
        return 1;
        }
    
    printf( "PID of debugged process = %d\n", pid );
    

    FILE *profFile = fopen( "wcProf.out", "w" );

    printf( "Sampling stack while program runs...\n" );

    
    int numSamples = 0;
    

    int usPerSample = 1000000 / samplesPerSecond;
    

    printf( "Sampling %d times per second, for %d usec between samples\n",
            samplesPerSecond, usPerSample );
    

    while( !programExited ) {
        usleep( usPerSample );
    
        // interrupt
        kill( pid, SIGINT );
        numSamples++;
        
        skipGDBResponse();
        

        // sample stack
        sendCommand( "-stack-list-frames" );
        printGDBResponseToFile( profFile );
        
        
        // continue running
        
        sendCommand( "-exec-continue" );
        skipGDBResponse();
        }
    
    printf( "Program exited normally\n" );
    
    printf( "%d stack samples taken\n", numSamples );

    
    return 0;
    }
