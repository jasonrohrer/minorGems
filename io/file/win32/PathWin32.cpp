/*
 * Modification History
 *
 * 2001-February-12		Jason Rohrer
 * Created.
 *
 * 2001-March-4		Jason Rohrer
 * Fixed delimeter constants.  
 *
 * 2001-August-1   Jason Rohrer
 * Added missing length return value.
 *
 * 2003-June-2   Jason Rohrer
 * Added support for new path checking functions.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */

#include "minorGems/io/file/Path.h"
#include "minorGems/util/stringUtils.h"


/*
 * Windows-specific path implementation.
 */ 



char Path::getDelimeter() {
	return '\\';
	}
	
		
		
char *Path::getAbsoluteRoot( int *outLength) {
	// C:\ is the only root we can generically return
	
	char *returnString = new char[3];
	
	returnString[0] = 'C';
	returnString[1] = ':';
	returnString[2] = '\\';

    *outLength = 3;
    
	return returnString;
	}



char Path::isAbsolute( const char *inPathString ) {
    // ignore first character, which will be drive letter
    if( inPathString[1] == ':' && inPathString[2] == '\\' ) {
        return true;
        }
    else {
        return false;
        }
    }



char *Path::extractRoot( const char *inPathString ) {
    if( isAbsolute( inPathString )  ){
        // copy path, then trim to only three characters

        char *pathCopy = stringDuplicate( inPathString );
        pathCopy[ 3 ] = '\0';

        char *trimmedCopy = stringDuplicate( pathCopy );
        delete [] pathCopy;
        
        return trimmedCopy;
        }
    else {
        return NULL;
        }
    }



char Path::isRoot( const char *inPathString ) {
    // must be of form "c:\"
    if( strlen( inPathString ) == 3 &&
        inPathString[1] == ':' &&
        inPathString[2] == '\\' ) {
        return true;
        }
    else {
        return false;
        }
    }


