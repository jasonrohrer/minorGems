/*
 * Modification History
 *
 * 2004-January-4   Jason Rohrer
 * Created.
 */


/**
 * A test program for the various child file functions in File.h
 *
 * @author Jason Rohrer.
 */



#include "minorGems/io/file/File.h"



int main( int inNumArgs, char **inArgs ) {

    char *fileName = "linux";

    if( inNumArgs > 1 ) {
        fileName = inArgs[1];
        }

    File *testFile = new File( NULL, fileName );

    int numChildren;

    File **childFiles = testFile->getChildFiles( &numChildren );


    printf( "child files:\n" );
    for( int i=0; i<numChildren; i++ ) {

        char *fullName = childFiles[i]->getFullFileName();

        printf( "  %s\n", fullName );

        delete [] fullName;

        delete childFiles[i];
        }

    delete [] childFiles;



    childFiles = testFile->getChildFilesRecursive( 10, &numChildren );


    printf( "recursive child files:\n" );
    for( int i=0; i<numChildren; i++ ) {

        char *fullName = childFiles[i]->getFullFileName();

        printf( "  %s\n", fullName );

        delete [] fullName;

        delete childFiles[i];
        }

    delete [] childFiles;

    
    delete testFile;

    return 0;
    }
