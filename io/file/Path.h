/*
 * Modification History
 *
 * 2001-February-12		Jason Rohrer
 * Created.
 *
 * 2001-May-11    Jason Rohrer
 * Added a version of getPathString that
 * returns a '\0' terminated string.
 *
 * 2001-September-21    Jason Rohrer
 * Added a missing include.
 *
 * 2001-September-23    Jason Rohrer
 * Added a copy function.
 * Made some comments more explicit.
 * Changed the constructor to allow for const path step strings.
 *
 * 2001-November-3    Jason Rohrer
 * Added a function for appending a string to a path.
 * Changed the interface to the main constructor.
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-April-11   Jason Rohrer
 * Fixed a variable scoping bug.
 *
 * 2002-July-2   Jason Rohrer
 * Fixed a major memory leak in copy().
 *
 * 2002-August-1   Jason Rohrer
 * Added support for path truncation.
 * Added support for parsing platform-dependent path strings.
 *
 * 2003-May-29   Jason Rohrer
 * Fixed a bug when an extra delimeters are at the end of the path.
 * Fixed a bug when string path consists only of root.
 *
 * 2003-June-2   Jason Rohrer
 * Fixed a bug in absolute path detection.
 * Added platform-specific functions for root and absolute path detection.
 * Fixed a memory bug when string path contains root only.
 * Fixed a path step bug when path is root.
 * Fixed bugs in truncate and append when non-default root string is used.
 *
 * 2005-August-29   Jason Rohrer
 * Fixed an uninitialized variable warning.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */

#include "minorGems/common.h"



#ifndef PATH_CLASS_INCLUDED
#define PATH_CLASS_INCLUDED


#include <string.h>



#include "minorGems/util/stringUtils.h"



#ifdef FORTIFY
#include "minorGems/util/development/fortify/fortify.h"
#endif



/**
 * Platform-independent file path interface.  Contains
 * all of path except for file name.  Thus, appending
 * a file name to the path will produce a complete file path.
 *
 * E.g., on Linux, file path: 
 *   temp/files/
 * file name:
 *   test.txt
 * full path:
 *   temp/files/test.txt
 *
 * @author Jason Rohrer
 */ 
class Path {

	public:
		
		/**
		 * Constructs a path.
		 *
		 * @param inPathSteps an array of c-strings representing
		 *   each step in the path, with no delimeters.
		 *   For example, { "temp", "files" } to represent
		 *   the linux path temp/files.
		 *   Must be destroyed by caller since copied internally.
		 * @param inNumSteps the number of strings in the path.
		 * @param inAbsolute set to true to make this an absolute
		 *   path.  For example, in Linux, an absolute path
		 *   is one that starts with '/', as in /usr/include/.
		 *   The effects of inAbsolute vary by platform.
         * @param inRootString the root string for this path if it
         *   is absolute, or NULL to specify a default root.
         *   Defaults to NULL.
         *   Must be destroyed by caller if non-NULL.
		 */
		Path( char **inPathSteps, int inNumSteps, char inAbsolute,
              char *inRootString = NULL );



        /**
         * Constructs a path by parsing a platform-dependent path string.
         *
         * @param inPathSteps a \0-terminated string representing the path.
         *   Must be destroyed by caller.
         */
        Path( const char *inPathString );

        
		
		~Path();


		
		/**
		 * Returns a complete, platform-dependent string path.
		 *
		 * @param outLength pointer to where the path length, in
		 *   characters, will be returned.
		 *
		 * @return a new char array containing the path.  Note
		 *   that the string is not terminated by '\0'.  Must
		 *   be destroyed by the caller.
		 */
		char *getPathString( int *outLength );

		
		/**
		 * Returns a complete, platform-dependent string path, terminated
		 * bye '\0'.
		 *
		 * @return a new char array containing the path.  Note
		 *   that the string IS terminated by '\0'.  Must
		 *   be destroyed by the caller.
		 */
		char *getPathStringTerminated();
		
		
		/**
		 * Gets the platform-specific path delimeter.
		 *
		 * Note that this function is implemented separately for
		 * each supported platform.
		 *
		 * @return the path delimeter.
		 */
		static char getDelimeter();
	
		
		/**
		 * Gets start characters for an absolute path.
		 *
		 * Note that this function is implemented separately for
		 * each supported platform.
		 *
		 * @param outLength pointer to where the string length, in
		 *   characters, will be returned.
		 *
		 * @return the absolute path start string characters.  For
		 *   example, on Linux, this would be the string "/".
		 *   Must be destroyed by the caller.
		 */
		static char *getAbsoluteRoot( int *outLength );



        /**
         * Gets whether a path string is absolute.
		 *
		 * Note that this function is implemented separately for
		 * each supported platform.
		 *
         * @param inPathString the string to check.
         *   Must be destroyed by caller if non-const.
         *
         * @return true if the string is absolute, or false otherwise.
         */
        static char isAbsolute( const char *inPathString );



        /**
         * Extracts the root string from a path string.
         *
		 *
         * @param inPathString the string to check.
         *   Must be destroyed by caller if non-const.
         *
         * @return the root string, or NULL if inPathString is not
         *   absolute.  Must be destroyed by caller if non-NULL.
         */
        static char *extractRoot( const char *inPathString );


        
        /**
         * Gets whether a path string is a root path.
		 *
		 * Note that this function is implemented separately for
		 * each supported platform.  For example, on Unix, only "/"
         * is the root path, while on Windows, both "c:\" and "d:\" might
         * be root paths.
		 *
         * @param inPathString the string to check.
         *   Must be destroyed by caller if non-const.
         *
         * @return true if the string is a root string, or false otherwise.
         */
        static char isRoot( const char *inPathString );

        

        /**
		 * Gets start string for an absolute path.
		 *
		 * @return the absolute path start string in \0-terminated form.
         *   Must be destroyed by the caller.
		 */
		static char *getAbsoluteRootString();

        

		/**
		 * Copies this path.
		 *
		 * @return a new path that is a deep copy of this path.
		 */
		Path *copy();


		
		/**
		 * Constructs a new path by appending an additional
		 * step onto this path.
		 *
		 * @param inStepString the step to add to this path.
		 *   Must be destroyed by caller if non-const.
         *
         * @return a new path with the extra step.
         *   Must be destroyed by caller.
		 */
		Path *append( const char *inStepString );

        
        
        /**
         * Constructs a new path by removing the last step from this path.
		 *
         * @return a new path, or NULL if there is only one step in this path.
         *   Must be destroyed by caller.
		 */
		Path *truncate();



        /**
         * Gets the last step in this path.
         *
         * @return the last step.  Must be destroyed by caller.
         */
        char *getLastStep();


        
	private:
		char **mPathSteps;
		int mNumSteps;
		int *mStepLength;
		char mAbsolute;

        // the root string of this path, if it is absolute
        char *mRootString;
	};		



inline Path::Path( char **inPathSteps, int inNumSteps,
				   char inAbsolute, char *inRootString ) 
	: mNumSteps( inNumSteps ), mAbsolute( inAbsolute ),
      mRootString( NULL ) {

    if( inRootString != NULL ) {
        mRootString = stringDuplicate( inRootString );
        }
    
	// copy the path steps
	
	mPathSteps = new char*[ mNumSteps ];
	mStepLength = new int[ mNumSteps ];
	
	for( int i=0; i<mNumSteps; i++ ) {
		int stepLength = strlen( inPathSteps[i] );
		
		mPathSteps[i] = new char[ stepLength + 1 ];
		memcpy( mPathSteps[i], inPathSteps[i], stepLength + 1 );

		mStepLength[i] = stepLength;
		}
	
	}



inline Path::Path( const char *inPathString ) {
    mAbsolute = isAbsolute( inPathString );

    
    char *pathStringCopy = stringDuplicate( inPathString );
    
    char delimeter = getDelimeter();

    char *delimString = new char[ 2 ];
    delimString[0] = delimeter;
    delimString[1] = '\0';
        

    char *pathRootSkipped;
    
    if( !mAbsolute ) {
        mRootString = NULL;
        pathRootSkipped = pathStringCopy;
        }
    else {
        // root occurs at start of path string

        mRootString = extractRoot( inPathString );
        pathRootSkipped = &( pathStringCopy[ strlen( mRootString ) ] );
        }

    
    // remove any trailing delimeters, if they exist
    while( pathRootSkipped[ strlen( pathRootSkipped ) - 1 ] == delimeter ) {
        pathRootSkipped[ strlen( pathRootSkipped ) - 1 ] = '\0';
        }
    
    char *currentDelimPointer = strstr( pathRootSkipped, delimString );

    if( currentDelimPointer != NULL ) {

        // first, count the delimeters
        
        int delimCount = 0;
        while( currentDelimPointer != NULL ) {
            if( strlen( currentDelimPointer ) > 1 ) {
                // don't count tail end delimeters
                delimCount++;
                }
            currentDelimPointer = strstr( &( currentDelimPointer[1] ),
                                          delimString );
            }

        
        // no delimeter at end of path
        mNumSteps = delimCount + 1;
       
        mPathSteps = new char*[ mNumSteps ];
        mStepLength = new int[ mNumSteps ];
        
        
        // now extract the chars between delimeters as path steps
        currentDelimPointer = strstr( pathRootSkipped, delimString );

        int stepIndex = 0;

        currentDelimPointer[0] = '\0';

        mPathSteps[ stepIndex ] = stringDuplicate( pathRootSkipped );
        mStepLength[ stepIndex ] = strlen( mPathSteps[ stepIndex ] );
        stepIndex++;
        
        while( currentDelimPointer != NULL ) {

            char *nextDelimPointer = strstr( &( currentDelimPointer[1] ),
                                             delimString );
            
            if( nextDelimPointer != NULL ) {
                nextDelimPointer[0] = '\0';
                }
            
            mPathSteps[ stepIndex ] =
                stringDuplicate( &( currentDelimPointer[1] ) );
            mStepLength[ stepIndex ] = strlen( mPathSteps[ stepIndex ] );
            
            stepIndex++;
            
            currentDelimPointer = nextDelimPointer;
            }
        
        }
    else {
        // no delimeters
        
        if( strlen( pathRootSkipped ) > 0 ) {
            mNumSteps = 1;
            mPathSteps = new char*[1];
            mPathSteps[0] = stringDuplicate( pathRootSkipped );
            
            mStepLength = new int[1];
            
            mStepLength[0] = strlen( mPathSteps[0] );
            }
        else {
            // path with root only
            mNumSteps = 0;
            mPathSteps = new char*[0];
            mStepLength = new int[0];
            }
        }

    delete [] delimString;

    delete [] pathStringCopy;
    }


		
inline Path::~Path() {
	// delete each step
	for( int i=0; i<mNumSteps; i++ ) {
		delete [] mPathSteps[i];
		}
	delete [] mPathSteps;

	delete [] mStepLength;

    if( mRootString != NULL ) {
        delete [] mRootString;
        }
	}
	


inline char *Path::getPathString( int *outLength ) {
	int length = 0;
	// length = sum( length each step string + 1 )
	// ( + 1 is for the delimeter that occurs after each step string )
    int i;
    for( i=0; i<mNumSteps; i++ ) {
        length += mStepLength[i] + 1;
		}
	// if absolute, we need to add in the length of the root
	char *rootString = NULL;
	int rootLength = 0;
	if( mAbsolute ) {
        if( mRootString != NULL ) {
            rootString = stringDuplicate( mRootString );
            rootLength = strlen( mRootString );
            }
        else {
            rootString = getAbsoluteRoot( &rootLength );
            }
		length += rootLength;
		}

	char *returnString = new char[ length ];
	int index = 0;
	
	if( rootString != NULL ) {
		// write root into string
		memcpy( &( returnString[index] ), rootString, rootLength );
		index += rootLength;
		
		delete [] rootString;
		}
	
	char delimeter = getDelimeter();
	
	// write each step into the string
	for( i=0; i<mNumSteps; i++ ) {
		memcpy( &( returnString[index] ), mPathSteps[i], mStepLength[i] );
		index += mStepLength[i];
		returnString[ index ] = delimeter;
		index++;
		}
		
	*outLength = length;
	
	return returnString;
	}
	


inline char *Path::getPathStringTerminated() {
	int length;

	char *pathString = getPathString( &length );

	char *delimitedPathString = new char[ length + 1 ];
	memcpy( delimitedPathString, pathString, length );

	delimitedPathString[ length ] = '\0';


	delete [] pathString;

	return delimitedPathString;
	}



inline Path *Path::copy() {

	// the steps will be copied internally
	return new Path( mPathSteps, mNumSteps, mAbsolute, mRootString );
	}



inline Path *Path::append( const char *inStepString ) {
	char **newPathSteps = new char*[ mNumSteps + 1 ];

	// shallow copy, since the strings themselves
	// are copied in the Path constructor below
	for( int i=0; i<mNumSteps; i++ ) {
		newPathSteps[i] = mPathSteps[i];
		}

	// append final step
	newPathSteps[ mNumSteps ] = (char*)inStepString;

	Path *newPath = new Path( newPathSteps, mNumSteps + 1, mAbsolute,
                              mRootString );

	// shallow delete, because of shallow copy above
	delete [] newPathSteps;

	
	return newPath;
	}



inline Path *Path::truncate() {

    if( mNumSteps < 2 && !mAbsolute ) {
        return NULL;
        }
    else if( mNumSteps < 1 ) {
        return NULL;
        }
    
    char **newPathSteps = new char*[ mNumSteps ];

	// shallow copy, since the strings themselves
	// are copied in the Path constructor below
	for( int i=0; i<mNumSteps-1; i++ ) {
		newPathSteps[i] = mPathSteps[i];
		}

	Path *newPath = new Path( newPathSteps, mNumSteps - 1, mAbsolute,
                              mRootString );

	// shallow delete, because of shallow copy above
	delete [] newPathSteps;

	
	return newPath;
	}



inline char *Path::getLastStep() {
    if( mNumSteps >= 1 ) {
        return stringDuplicate( mPathSteps[ mNumSteps - 1 ] );
        }
    else {
        if( mAbsolute ) {
            if( mRootString != NULL ) {
                return stringDuplicate( mRootString );
                }
            else {
                return getAbsoluteRootString();
                }
            }
        else {
            // no path steps and not absolute...
            return stringDuplicate( "" );
            }
        }
    }



inline char *Path::getAbsoluteRootString() {
    int rootLength;
    char *root = getAbsoluteRoot( &rootLength );
    
    char *rootString = new char[ rootLength + 1 ];
    strncpy( rootString, root, rootLength );
    // strncopy won't add termination if length limit reached
    rootString[ rootLength ] = '\0';
    
    delete [] root;
    return rootString;
    }



#endif
