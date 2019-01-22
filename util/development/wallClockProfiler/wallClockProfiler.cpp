#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include<sys/prctl.h>


#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"


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


typedef struct StackFrame{
        void *address;
        char *funcName;
        char *fileName;
        int lineNum;
        int sampleCount;
    } StackFrame;

SimpleVector<StackFrame> frameLog;

    

static void logFrame( char *inFrameString ) {
    
    char *openPos = strstr( inFrameString, "{" );
    
    if( openPos == NULL ) {
        return;
        }
    openPos = &( openPos[1] );
    
    char *closePos = strstr( openPos, "}" );
    
    if( closePos == NULL ) {
        return;
        }
    closePos[0] ='\0';
    
    int numVals;
    char **vals = split( openPos, ",", &numVals );
    
    void *address = NULL;
    
    for( int i=0; i<numVals; i++ ) {
        if( strstr( vals[i], "addr=" ) == vals[i] ) {
            sscanf( vals[i], "addr=\"%p\"", &address );
            break;
            }
        }
    
    StackFrame *f = NULL;
    
    for( int i=0; i<frameLog.size(); i++ ) {
        f = frameLog.getElement( i );
        
        if( f->address == address ) {
            break;
            }
        f = NULL;
        }

    if( f == NULL ) {
        StackFrame newF;
        newF.address = address;
        newF.sampleCount = 1;
        newF.lineNum = -1;
        newF.funcName = NULL;
        newF.fileName = NULL;

        for( int i=0; i<numVals; i++ ) {
            if( strstr( vals[i], "func=" ) == vals[i] ) {
                newF.funcName = new char[ 500 ];
                sscanf( vals[i], "func=\"%499s\"", newF.funcName );
                }
            else if( strstr( vals[i], "file=" ) == vals[i] ) {
                newF.fileName = new char[ 500 ];
                sscanf( vals[i], "file=\"%499s\"", newF.fileName );
                }
            else if( strstr( vals[i], "line=" ) == vals[i] ) {
                sscanf( vals[i], "line=\"%d\"", &newF.lineNum );
                }
            }

        if( newF.fileName == NULL ) {
            newF.fileName = stringDuplicate( "" );
            }
        if( newF.funcName == NULL ) {
            newF.funcName = stringDuplicate( "" );
            }
        
        char *quotePos = strstr( newF.fileName, "\"" );
        if( quotePos != NULL ) {
            quotePos[0] ='\0';
            }
        quotePos = strstr( newF.funcName, "\"" );
        if( quotePos != NULL ) {
            quotePos[0] ='\0';
            }
        
        
        frameLog.push_back( newF );
        }
    else {
        f->sampleCount++;
        }
    }



static void logGDBStackResponse() {
    int numRead = fillBufferWithResponse();
    
    if( numRead == 0 ) {
        return;
        }
    

    checkProgramExited();
        
    if( programExited ) {
        return;
        }
    
    const char *stackStartMarker = ",stack=[";
    
    char *stackStartPos = strstr( readBuff, ",stack=[" );
        
    if( stackStartPos == NULL ) {
        return;
        }
    
    char *stackStart = &( stackStartPos[ strlen( stackStartMarker ) ] );
    
    char *closeBracket = strstr( stackStart, "]" );
    
    if( closeBracket == NULL ) {
        return;
        }
    
    // terminate at close
    closeBracket[0] = '\0';
    
    const char *frameMarker = "frame=";
    
    if( strstr( stackStart, frameMarker ) != stackStart ) {
        return;
        }
    
    // skip first
    stackStart = &( stackStart[ strlen( frameMarker ) ] );
    
    int numFrames;
    char **frames = split( stackStart, frameMarker, &numFrames );
    
    for( int i=0; i<numFrames; i++ ) {
        logFrame( frames[i] );
        
        delete [] frames[i];
        }
    delete [] frames;
    
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
    

    printf( "\n\nStarting gdb program with 'run'\n" );

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
        //skipGDBResponse();
        //printGDBResponseToFile( profFile );
        logGDBStackResponse();
        
        
        // continue running
        
        sendCommand( "-exec-continue" );
        skipGDBResponse();
        }
    
    printf( "Program exited normally\n" );
    
    printf( "%d stack samples taken\n", numSamples );

    printf( "%d unique stack frames sampled\n", frameLog.size() );
    
    
    // simple insertion sort
    SimpleVector<StackFrame> sortedFrames;
    
    while( frameLog.size() > 0 ) {
        int max = 0;
        StackFrame maxFrame;
        int maxInd = -1;
        for( int i=0; i<frameLog.size(); i++ ) {
            StackFrame f = frameLog.getElementDirect( i );
            
            if( f.sampleCount > max ) {
                maxFrame = f;
                max = f.sampleCount;
                maxInd = i;
                }
            }  
        sortedFrames.push_back( maxFrame );
        frameLog.deleteElement( maxInd );
        }
    
    printf( "\n\n\nReport:\n\n" );
    
    for( int i=0; i<sortedFrames.size(); i++ ) {
        StackFrame f = sortedFrames.getElementDirect( i );
        printf( "%f%%: \t%s \t(%s:%d)\n\n", 
                100 * f.sampleCount / (float )numSamples,
                f.funcName, f.fileName, f.lineNum );
        
        delete [] f.funcName;
        delete [] f.fileName;
        }
    


    
    return 0;
    }
