/*
 * Modification History
 *
 * 2003-January-10   Jason Rohrer
 * Created.
 */



#include "minorGems/common.h"



#ifndef LAUNCHER_INCLUDED
#define LAUNCHER_INCLUDED 



/**
 * Interface for launching processes.
 *
 * @author Jason Rohrer
 */
class Launcher {
    public:


        
        /**
         * Launches a command in a new process.
         *
         * @param inCommandName the name of the command to launch.
         *   Must be destroyed by caller if non-const.
         * @param inArguments an array of argument strings for the command.
         *   This array must be terminated by a NULL pointer.
         *   Note that by convention, the first argument should be the
         *   command name.
         *   Must be destroyed by caller.
         */
        static void launchCommand( char *inCommandName,
                                   char **inArguments );

        
        
    };



#endif
 

