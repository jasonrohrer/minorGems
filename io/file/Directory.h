/*
 * Modification History
 *
 * 2003-January-23   Jason Rohrer
 * Created.
 *
 *
 * 2003-November-10   Jason Rohrer
 * Added makeDirectory function.
 */



#include "minorGems/common.h"
#include "minorGems/io/file/File.h"



#ifndef DIRECTORY_INCLUDED
#define DIRECTORY_INCLUDED



/**
 * Class of static directory functions.
 *
 * This class exists because most directory operations are
 * platform-dependent, and a large body of existing code
 * depends on a platform-independent File.h.
 *
 * @author Jason Rohrer.
 */
class Directory {

    public:



        /**
         * Removes a directory.
         *
         * The directory must be empty for this call to succeed.
         *
         * @param inFile the file representing the directory.
         *   Must be destroyed by caller.
         *
         * @return true if the directory is removed successfully, or
         *   false otherwise (for example, if the directory is not empy).
         */
        static char removeDirectory( File *inFile );



        /**
         * Makes a directory.
         *
         * @param inFile the file representing the directory.
         *   Must be destroyed by caller.
         *
         * @return true if the directory is removed successfully, or
         *   false otherwise (for example, if the directory is not empy).
         */
        static char makeDirectory( File *inFile );

        
        
    };



#endif


