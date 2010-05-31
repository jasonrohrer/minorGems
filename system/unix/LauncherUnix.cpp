/*
 * Modification History
 *
 * 2003-January-10   Jason Rohrer
 * Created.
 */



#include "minorGems/system/Launcher.h"

#include <unistd.h>
#include <stdarg.h>


void Launcher::launchCommand( char *inCommandName,
                              char **inArguments ) {

    
    int forkValue = fork();

    if( forkValue == 0 ) {
        // we're in child process, so exec command
        
        execvp( inCommandName, inArguments );

        // we'll never return from this call
        }
    
    }



