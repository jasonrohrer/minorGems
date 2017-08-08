/*
 * Modification History
 *
 * 2001-February-11		Jason Rohrer
 * Created. 
 *
 * 2001-February-25		Jason Rohrer
 * Fixed file name bugs in length and existence functions.
 *
 * 2001-May-11   Jason Rohrer
 * Added a missing include.
 *
 * 2001-November-3   Jason Rohrer
 * Added a function for checking if a file is a directory.
 * Added a function for getting the child files of a directory.
 * Added a function for getting a pathless file name.
 *
 * 2001-November-13   Jason Rohrer
 * Made name length parameter optional in constructor.
 * Made return length parameter optional in name getting functions.
 * 
 * 2001-November-17   Jason Rohrer
 * Added a functions for removing a file and for copying a file.
 *
 * 2002-March-11   Jason Rohrer
 * Added destruction comment to getFullFileName().
 *
 * 2002-March-13   Jason Rohrer
 * Changed mName to be \0-terminated to fix interaction bugs with Path.
 * Fixed a missing delete.
 * Added a function for creating a directory.
 *
 * 2002-March-31   Jason Rohrer
 * Fixed some bad syntax.
 *
 * 2002-April-6    Jason Rohrer
 * Replaced use of strdup.
 *
 * 2002-April-8    Jason Rohrer
 * Fixed fopen bug.
 *
 * 2002-April-11    Jason Rohrer
 * Fixed a memory leak.
 * Fixed a casting error.
 *
 * 2002-June-28    Jason Rohrer
 * Added a function for copying a file class.
 *
 * 2002-August-3    Jason Rohrer
 * Added a function for getting the parent file.
 *
 * 2002-August-5    Jason Rohrer
 * Used an unused error variable.
 *
 * 2002-September-11   Jason Rohrer
 * Added return value to remove.
 *
 * 2003-January-27   Jason Rohrer
 * Added a function for reading file contents.
 *
 * 2003-February-3   Jason Rohrer
 * Added a function for writing a string to a file.
 *
 * 2003-March-13   Jason Rohrer
 * Added a function for getting a child file from a directory.
 *
 * 2003-June-2   Jason Rohrer
 * Fixed parent directory behavior when current file is root directory.
 * Fixed a bug in getting child files of root directory.
 *
 * 2003-November-6   Jason Rohrer
 * Added function for getting last modification time.
 *
 * 2003-November-10   Jason Rohrer
 * Changed to use platform-dependent makeDirectory function.
 *
 * 2004-January-4   Jason Rohrer
 * Added recursive child file functions.
 *
 * 2005-August-29   Jason Rohrer
 * Fixed an uninitialized variable warning.
 *
 * 2010-March-6   Jason Rohrer
 * Added versions of writeToFile readFileContents for binary data.
 *
 * 2010-April-23   Jason Rohrer
 * Fixed a string length bug when line ends are Windows.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2017-March-13    Jason Rohrer
 * Functions for reading/writing an int from/to a file.
 * Function for comparing files.
 *
 * 2017-August-8    Jason Rohrer
 * Function for getting alphabetically sorted child files.
 */



#include "minorGems/common.h"



#ifndef FILE_CLASS_INCLUDED
#define FILE_CLASS_INCLUDED

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dirent.h>

#include "Path.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/system/Time.h"



/**
 * File interface.  Provides access to information about a
 * file.
 *
 * @author Jason Rohrer
 */ 
class File {

	public:
		
		/**
		 * Constructs a file.
		 *
		 * @param inPath the path for this file.
		 *   Is destroyed when this class is destroyed. 
		 *   Pass in NULL to specify
		 *   no path (the current working directory).
		 * @param inName the name of the file to open.
		 *   Must be destroyed by caller if not const.
		 *   Copied internally.
		 * @param inNameLength length of the name in chars,
		 *   or -1 to use the c-string length of inName
		 *   (assuming that inName is \0-terminated).
		 *   Defaults to -1.
		 */
		File( Path *inPath, const char *inName, int inNameLength = -1 );
		
		
		~File();



		/**
		 * Gets whether this file is a directory.
		 *
		 * @return true iff this file is a directory.
		 */
		char isDirectory();
		


        /**
         * Makes a directory in the location of this file.
         *
         * Can only succeed if exists() is false.
         *
         * @return true iff directory creation succeeded.
         */
        char makeDirectory();

        

		/**
		 * Gets the files contained in this file if it is a directory.
         *
		 * @param outNumFiles pointer to where the number of
		 *   files will be returned.
		 *
		 * @return an array of files, or NULL if this
		 *    file is not a directory, is an empty directory, or doesn't exist.
		 *    Must be destroyed by caller if non-NULL.
		 */
		File **getChildFiles( int *outNumFiles );
        
        // sorted in alphabetical order
        File **getChildFilesSorted( int *outNumFiles );



        /**
		 * Gets the files contained in this file if it is a directory and
         * recursively in subdirectories of this file.
		 *
		 * @param inDepthLimit the maximum subdirectory depth to recurse into.
         *   If inDepthLimit is 0, then only child files in this directory
         *   will be returned.
		 * @param outNumFiles pointer to where the number of
		 *   files will be returned.
		 *
		 * @return an array of files, or NULL if this
		 *    file is not a directory, is an empty directory (or a directory
         *    containing empty subdirectories), or doesn't exist.
		 *    Must be destroyed by caller if non-NULL.
		 */
		File **getChildFilesRecursive( int inDepthLimit, int *outNumFiles );
        
        

        /**
         * Gets a child of this directory.
         *
         * @param inChildFileName the name of the child file.
         *   Must be destroyed by caller if non-const.
         *
         * @return the child file (even if it does not exist), or NULL if
         *   this file is not a directory.
         *   Must be destroyed by caller if non-NULL.
         */
        File *getChildFile( const char *inChildFileName );
        

        
        /**
         * Gets the parent directory of this file.
         *
         * @return the parent directory of this file.
         *    Must be destroyed by caller.
         */
        File *getParentDirectory();


        
		/**
		 * Gets the length of this file.
		 *
		 * @return the length of this file in bytes.  Returns
		 *   0 if the file does not exist.
		 */
		long getLength();
		
		
		/**
		 * Gets whether a file exists.
		 * 
		 * @return true if the file exists.
		 */
		char exists();


        
		/**
		 * Gets the last modification time of this file.
		 *
		 * @return the modification time in seconds since the epoch.
         *
         * Returns 0 if the file does not exist.
		 */
		timeSec_t getModificationTime();


		
		/**
		 * Removes this file from the disk, if it exists.
         *
         * @return true iff the remove succeeded, false if the removal
         *   fails or the file does not exist.
		 */
		char remove();



        /**
         * Copies this file object (does not copy the file described by
         * this object).
         *
         * @return a deep copy of this file object.
         */
        File *copy();

        

		/**
		 * Copies the contents of this file into another file.
		 *
		 * @param inDestination the file to copy this file into.
		 *   If it exists, it will be overwritten.
		 *   If it does not exist, it will be created.
		 *   Must be destroyed by caller.
		 * @param inBlockSize the block size to use when copying.
		 *   Defaults to blocks of 5000 bytes.
		 */
		void copy( File *inDestination, long inBlockSize = 5000 );
        

        // returns true if file contents match, false otherwise
        char contentsMatches( File *inOtherFile );
        
		
		
		/**
		 * Gets the full-path file name sufficient
		 * to access this file from the current working
		 * directory.
		 *
		 * @param outLength pointer to where the name length, in
		 *   characters, will be returned.  Set to NULL to ignore
		 *   the output length.  Defaults to NULL.
		 *
		 * @return the full path file name for this file,
		 *   in platform-specific form.  Must be destroyed by caller.
		 *   The returned string is '\0' terminated, but this
		 *   extra character is not included in the length.
		 *   Must be destroyed by caller.
		 */
		char *getFullFileName( int *outLength = NULL );
	


		/**
		 * Gets the pathless name of this file.
		 *
		 * @param outLength pointer to where the name length, in
		 *   characters, will be returned.  Set to NULL to ignore
		 *   the output length.  Defaults to NULL.
		 *
		 * @return the name of this file.  Must be destroyed by caller.
		 */
		char *getFileName( int *outLength = NULL );



        /**
         * Reads the contents of this file.
         *
         * @return a \0-terminated string containing the file contents,
         *   or NULL if reading the file into memory failed.
         *   Must be destroyed by caller.
         */
        char *readFileContents();
        
        // read a single int from this file
        // inDefaultValue returned if reading fails
        int readFileIntContents( int inDefaultValue );
        


        /**
         * Reads the contents of this file.
         *
         * @param outLength pointer to where the return array length should
         *   be returned.
         * @param inTextMode true to open the file as text, false as binary.
         *   Defaults to false.
         *
         * @return an array containing the binary file contents,
         *   or NULL if reading the file into memory failed.
         *   Must be destroyed by caller.
         */
        unsigned char *readFileContents( int *outLength, 
                                         char inTextMode = false );



        /**
         * Writes a string to this file.
         *
         * @param inString the \0-terminated string to write.
         *   Must be destroyed by caller if non-const.
         *
         * @return true if the file was written to successfully, or
         *   false otherwise.
         */
        char writeToFile( const char *inString );


        /**
         * Writes a binary data to this file.
         *
         * @param inData the data to write.
         *   Must be destroyed by caller if non-const.
         * @param inLength length of inData.
         *
         * @return true if the file was written to successfully, or
         *   false otherwise.
         */
        char writeToFile( unsigned char *inData, int inLength );

        

        // write an int to a file
        // @return true if the file was written to successfully, or
        //   false otherwise.
        char writeToFile( int inInt );
        
        
		
	private:
		Path *mPath;
		char *mName;
		int mNameLength;



        /**
		 * Gets the files contained in this file if it is a directory and
         * recursively in subdirectories of this file.
		 *
		 * @param inDepthLimit the maximum subdirectory depth to recurse into.
         *   If inDepthLimit is 0, then only child files in this directory
         *   will be returned.
		 * @param inResultVector vector to add the discovered files to.
         *   Must be destroyed by caller.
		 */
		void getChildFilesRecursive( int inDepthLimit,
                                     SimpleVector<File *> *inResultVector );


        
	};		



inline File::File( Path *inPath, const char *inName, int inNameLength  )
	: mPath( inPath ), mNameLength( inNameLength ) {

	if( inNameLength == -1 ) {
		inNameLength = strlen( inName );
		mNameLength = inNameLength;
		}
	
	// copy name internally
	mName = stringDuplicate( inName );
	
	}



inline File::~File() {
	delete [] mName;
	
	if( mPath != NULL ) {	
		delete mPath;
		}
	}	
	


inline long File::getLength() {
	struct stat fileInfo;
	
	// get full file name
	int length;
	char *stringName = getFullFileName( &length );
	
	int statError = stat( stringName, &fileInfo );
	
	delete [] stringName;
	
	if( statError == 0 ) {
		return fileInfo.st_size;
		}
	else {
		// file does not exist
		return 0;
		}
	}



inline char File::isDirectory() {
	struct stat fileInfo;
	
	// get full file name
	int length;
	char *stringName = getFullFileName( &length );
	
	int statError = stat( stringName, &fileInfo );
	
	delete [] stringName;

        if( statError == -1 ) {
            return false;
            }
	else {
            return S_ISDIR( fileInfo.st_mode );
            }
	}



inline File **File::getChildFiles( int *outNumFiles ) {

	int length;
	char *stringName = getFullFileName( &length );
	
	DIR *directory = opendir( stringName );
	
	if( directory != NULL ) {

		SimpleVector< File* > *fileVector = new SimpleVector< File* >();
		
		struct dirent *entry = readdir( directory );

		if( entry == NULL ) {
			delete fileVector;

			closedir( directory );
			
			delete [] stringName;

			*outNumFiles = 0;
			return NULL;
			}

		
		while( entry != NULL ) {
			// skip parentdir and thisdir files, if they occur
			if( strcmp( entry->d_name, "." ) &&
				strcmp( entry->d_name, ".." ) ) {
				
				Path *newPath;

				if( mPath != NULL ) {
					newPath = mPath->append( mName );
					}
				else {

                    if( Path::isRoot( mName ) ) {
                        // use name as a string path
                        newPath = new Path( mName );
                        }
                    else {
                        char **folderPathArray = new char*[1];
                        folderPathArray[0] = mName;
				
                        // a non-absolute path to this directory's contents
                        int numSteps = 1;
                        char absolute = false;
                        newPath =
                            new Path( folderPathArray, numSteps,
                                      absolute );
                        
                        delete [] folderPathArray;
                        }
                    }

				// safe to pass d_name in directly because it is copied
				// internally by the constructor
							
				fileVector->push_back(
					new File( newPath,
							  entry->d_name,
							  strlen( entry->d_name ) ) );
				}
			
			entry = readdir( directory );
			}

		// now we have a vector full of this directory's files
		int vectorSize = fileVector->size();

		*outNumFiles = vectorSize;
		
		if( vectorSize == 0 ) {
			delete fileVector;
		
			closedir( directory );

			delete [] stringName;
		
			return NULL;
			}
		else {
			File **returnFiles = new File *[vectorSize];
			for( int i=0; i<vectorSize; i++ ) {
				returnFiles[i] = *( fileVector->getElement( i ) );
				}
		
			delete fileVector;
		
			closedir( directory );

			delete [] stringName;
		
			return returnFiles;
			}
		}
	else {
		delete [] stringName;

		*outNumFiles = 0;
		return NULL;
		}
	

	
	}




static int fileNameCompare(const void *inA, const void * inB ) {
    
    File *a = *( (File**)inA );
    File *b = *( (File**)inB );

    char *nameA = a->getFileName();
    char *nameB = b->getFileName();
    
    int result = strcmp( nameA, nameB );

    delete [] nameA;
    delete [] nameB;
    
    return result;
    }




inline File **File::getChildFilesSorted( int *outNumFiles ) {
    File **result = getChildFiles( outNumFiles );
    

    qsort( result, *outNumFiles, sizeof(File*), fileNameCompare );

    return result;
    }





inline File **File::getChildFilesRecursive( int inDepthLimit,
                                            int *outNumFiles ) {

    // create a vector for results
    SimpleVector<File *> *resultVector = new SimpleVector<File *>();

    // call the recursive function
    getChildFilesRecursive( inDepthLimit, resultVector );


    // extract results from vector
    File **resultArray = NULL;

    int numResults = resultVector->size();
    
    if( numResults > 0 ) {
        resultArray = resultVector->getElementArray();
        }

    delete resultVector;

    
    
    *outNumFiles = numResults;
    return resultArray;
    }



inline void File::getChildFilesRecursive(
    int inDepthLimit,
    SimpleVector<File *> *inResultVector ) {

    // get our child files
    int numChildren;
    File **childFiles = getChildFiles( &numChildren );

    if( childFiles != NULL ) {

        // for each child, add it to vector and
        // recurse into it if it is a directory

        for( int i=0; i<numChildren; i++ ) {

            File *child = childFiles[i];

            // add it to results vector
            inResultVector->push_back( child );
            
            if( child->isDirectory() ) {
                // skip recursion if we have hit our depth limit
                if( inDepthLimit > 0 ) {
                    // recurse into this subdirectory
                    child->getChildFilesRecursive( inDepthLimit - 1,
                                                   inResultVector );
                    }
                }
            }

        delete [] childFiles;
        }
    }



inline File *File::getChildFile( const char *inChildFileName ) {
    // make sure we are a directory
    if( !isDirectory() ) {
        return NULL;
        }

    // get a path to this directory
    Path *newPath;

    if( mPath != NULL ) {
        newPath = mPath->append( mName );
        }
    else {
				
        char **folderPathArray = new char*[1];
        folderPathArray[0] = mName;
				
        // a non-absolute path to this directory's contents
        int numSteps = 1;
        char absolute = false;
        newPath =
            new Path( folderPathArray, numSteps,
                      absolute );
        
        delete [] folderPathArray;
        }

    return new File( newPath, inChildFileName );
    }



inline File *File::getParentDirectory() {

    if( mPath != NULL ) {
        
        char *parentName;

        Path *parentPath;
        
        if( strcmp( mName, ".." ) == 0 ) {
            // already a parent dir reference
            // append one more parent dir reference with parentName below
            parentPath = mPath->append( ".." );

            parentName = stringDuplicate( ".." );
            }
        else {
            // not a parent dir reference, so we can truncate
            parentPath = mPath->truncate();

            parentName = mPath->getLastStep();
            }

        File *parentFile = new File( parentPath, parentName );

        delete [] parentName;

        return parentFile;
        }
    else {
        if( Path::isRoot( mName ) ) {
            // we are already at the root
            return new File( NULL, mName );
            }
        else {
            // append parent dir symbol to path
            char **parentPathSteps = new char*[1];
            parentPathSteps[0] = mName;

            Path *parentPath = new Path( parentPathSteps, 1, false );

            const char *parentName = "..";

            File *parentFile = new File( parentPath, parentName );

            delete [] parentPathSteps;

            return parentFile;
            }
        }
    }


		
inline char File::exists() {
	struct stat fileInfo;
	
	// get full file name
	int length;
	char *stringName = getFullFileName( &length );
	
	int statError = stat( stringName, &fileInfo );
	
	delete [] stringName;
	
	if( statError == 0 ) {
		return true;
		}
	else {
		// file does not exist
		return false;
		}
	}



inline timeSec_t File::getModificationTime() {
    struct stat fileInfo;
	
	// get full file name
	int length;
	char *stringName = getFullFileName( &length );
	
	int statError = stat( stringName, &fileInfo );
	
	delete [] stringName;
	
	if( statError == 0 ) {
		return Time::normalize( fileInfo.st_mtime );
		}
	else {
		// file does not exist
		return 0;
		}
    }



inline char File::remove() {
    char returnVal = false;
    
    if( exists() ) {
		char *stringName = getFullFileName(); 

		int error = ::remove( stringName );

        if( error == 0 ) {
            returnVal = true;
            }
            
		delete [] stringName;
		}

    return returnVal;
	}



inline File *File::copy() {
    Path *pathCopy = NULL;
    
    if( mPath != NULL ) {
        pathCopy = mPath->copy();
        }

    return new File( pathCopy, mName );
    }



inline void File::copy( File *inDestination, long inBlockSize ) {
	char *thisFileName = getFullFileName();
	char *destinationFileName = inDestination->getFullFileName();

	FILE *thisFile = fopen( thisFileName, "rb" );
	FILE *destinationFile = fopen( destinationFileName, "wb" );

	long length = getLength();

	long bytesCopied = 0;

	char *buffer = new char[ inBlockSize ];
	
	while( bytesCopied < length ) {

		long bytesToCopy = inBlockSize;

		// end of file case
		if( length - bytesCopied < bytesToCopy ) {
			bytesToCopy = length - bytesCopied;
			}

		fread( buffer, 1, bytesToCopy, thisFile );
		fwrite( buffer, 1, bytesToCopy, destinationFile );
		
		bytesCopied += bytesToCopy;
		}

	fclose( thisFile );
	fclose( destinationFile );

	delete [] buffer;
	delete [] thisFileName;
	delete [] destinationFileName;	
	}



inline char File::contentsMatches( File *inOtherFile ) {
    if( !exists() || ! inOtherFile->exists() ) {
        return false;
        }
    
    if( isDirectory() || inOtherFile->isDirectory() ) {
        return false;
        }
    
    if( getLength() != inOtherFile->getLength() ) {
        return false;
        }
    
    int lenA;
    unsigned char *contA = readFileContents( &lenA );

    int lenB;
    unsigned char *contB = inOtherFile->readFileContents( &lenB );

    
    char match = false;
    
    if( contA != NULL && contB != NULL && lenA == lenB ) {
        
        match = true;
        
        for( int i=0; i<lenA; i++ ) {
            if( contA[i] != contB[i] ) {
                match = false;
                break;
                }
            }
        }

    if( contA != NULL ) {
        delete [] contA;
        }
    if( contB != NULL ) {
        delete [] contB;
        }
    

    return match;
    }




inline char *File::getFileName( int *outLength ) {
	char *returnName = stringDuplicate( mName );
    
	if( outLength != NULL ) {
		*outLength = mNameLength;
		}
	
	return returnName;
	}


		
inline char *File::getFullFileName( int *outLength ) {
	int length = mNameLength;

    int pathLength = 0;
	char *path = NULL;
	if( mPath != NULL ) {
		path = mPath->getPathString( &pathLength );
	
		length += pathLength;
		}
		
	// extra character for '\0' termination	
	char *returnString = new char[ length + 1 ];
	
	if( path != NULL ) {
		memcpy( returnString, path, pathLength );
		memcpy( &( returnString[pathLength] ), mName, mNameLength );
		
		delete [] path;
		}
	else {
		// no path, so copy the name directly in
		memcpy( returnString, mName, mNameLength );
		}
	
	// terminate the string
	returnString[ length ] = '\0';

        
	if( outLength != NULL ) {
		*outLength = length;
		}
	
	return returnString;
	}



#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/io/file/FileOutputStream.h"



inline char *File::readFileContents() {

    int length;
    // text mode!
    unsigned char *data = readFileContents( &length, true );

    if( data == NULL ) {
        return NULL;
        }
    
    char *dataString = new char[ length + 1 ];
    
    memcpy( dataString, data, length );
    dataString[ length ] = '\0';
    
    delete [] data;
    return dataString;
    }



inline int File::readFileIntContents( int inDefaultValue ) {
    char *cont = readFileContents();
    
    if( cont == NULL ) {
        return inDefaultValue;
        }
    
    int val;
    
    int numRead = sscanf( cont, "%d", &val );
    
    delete [] cont;

    if( numRead != 1 ) {
        return inDefaultValue;
        }

    return val;
    }




inline unsigned char *File::readFileContents( int *outLength, 
                                              char inTextMode ) {

    if( exists() ) {
        int length = getLength();

        unsigned char *returnData = new unsigned char[ length ];

        if( returnData != NULL ) {
            FileInputStream *input = new FileInputStream( this, inTextMode );
            int numRead = input->read( returnData, length );

            delete input;

            // in text mode, read length might not equal binary file length,
            // due to line end conversion
            if( numRead == length ||
                ( inTextMode && numRead >= 0 ) ) {
                *outLength = numRead;
                return returnData;
                }
            else {
                delete [] returnData;
                return NULL;
                }
            }
        else {
            // failed to allocate this much memory
            return NULL;
            }
        }
    else {
        return NULL;
        }

    }



inline char File::writeToFile( const char *inString ) {
    return writeToFile( (unsigned char *)inString, strlen( inString ) );    
    }



inline char File::writeToFile( int inInt ) {
    char *stringVal = autoSprintf( "%d", inInt );
    
    char returnVal = writeToFile( stringVal );

    delete [] stringVal;

    return returnVal;
    }



inline char File::writeToFile( unsigned char *inData, int inLength ) {
    FileOutputStream *output = new FileOutputStream( this );

    long numWritten = output->write( inData, inLength );

    delete output;

    if( inLength == numWritten ) {
        return true;
        }
    else {
        return false;
        }

    }



#include "Directory.h"



inline char File::makeDirectory() {
    if( exists() ) {
        return false;
        }
    else {
        return Directory::makeDirectory( this );
        }
    }



#endif
