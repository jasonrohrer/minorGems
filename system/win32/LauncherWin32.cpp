/*
 * Modification History
 *
 * 2003-January-10   Jason Rohrer
 * Created.
 *
 * 2003-March-24   Jason Rohrer
 * Fixed a syntax typo.
 */



#include "minorGems/system/Launcher.h"

#include <windows.h>
#include <process.h>



void Launcher::launchCommand( char *inCommandName,
                              char **inArguments ) {

    _spawnvp( _P_NOWAIT,
              inCommandName,
              inArguments );
    }



