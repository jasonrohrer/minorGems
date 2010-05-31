/*
 * Modification History
 *
 * 2002-April-20    Jason Rohrer
 * Created.
 *
 * 2002-April-22    Jason Rohrer
 * Fixed a bug with the mime type string.
 *
 * 2002-April-30    Jason Rohrer
 * Removed an unused variable.
 *
 * 2002-September-17    Jason Rohrer
 * Moved mime.ini into settings directory.
 *
 * 2002-October-7    Jason Rohrer
 * Added a function for getting mime types from file names.
 *
 * 2003-September-1   Jason Rohrer
 * Copied into minorGems from the konspire2b project.
 */



#include "MimeTyper.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"


#include <string.h>
#include <stdio.h>



MimeTyper::MimeTyper( char *inConfigFileName ) {

    File *configFile;

    char **pathSteps = new char*[1];
    pathSteps[0] = "settings";
    
    
    if( inConfigFileName == NULL ) {
        configFile = new File( new Path( pathSteps, 1, false ), "mime.ini" );
        }
    else {
        configFile = new File( new Path( pathSteps, 1, false ),
                               inConfigFileName );
        }

    delete [] pathSteps;
    
    
    if( !configFile->exists() ) {
        char *name = configFile->getFileName();
        
        printf( "Error:  MIME config file %s does not exist.\n",
                name );
        delete [] name;

        mMimeTypesString = new char[ 1 ];
        mMimeTypesString[0] = '\0';
        }
    else {
        int length = configFile->getLength();
        
        mMimeTypesString = new char[ length + 1 ];

        char *name = configFile->getFileName();

        FileInputStream *inStream = new FileInputStream( configFile );

        int numRead = inStream->read(
            (unsigned char *)mMimeTypesString, length );

        if( numRead != length ) {
            printf( "Error reading from MIME config file %s.\n",
                    name );
            delete [] mMimeTypesString;
            mMimeTypesString = new char[ 1 ];
            mMimeTypesString[0] = '\0';
            }
        else {
            mMimeTypesString[ length ] = '\0';
            }

        delete [] name;

        delete inStream;
        }

    delete configFile;
    }



MimeTyper::~MimeTyper() {
    if( mMimeTypesString != NULL ) {
        delete [] mMimeTypesString;
        }
    }



char *MimeTyper::getMimeType( char *inFileExtension ) {

    char *extensionMatch = strstr( mMimeTypesString, inFileExtension );

    if( extensionMatch ) {

        // we should be able to scan two strings from
        // the position of the match
        // the first string should be the extension
        // the second string should be the mime type

        // make sure buffer is big enough
        char *buffer = new char[ strlen( mMimeTypesString )  + 1 ];
        
        int numRead = sscanf( extensionMatch, "%s", buffer );

        char *returnString = NULL;
        
        if( numRead == 1 ) {
            int numToSkip = strlen( buffer );

            // skip first string token (extension)
            // and read next string token (mime type)
            numRead = sscanf( &( extensionMatch[ numToSkip ] ),
                              "%s", buffer );
            if( numRead == 1 ) {
                returnString = stringDuplicate( buffer );
                }
            }
        delete [] buffer;
            
        return returnString;
        }
    else {
        return NULL;
        }
    }



char *MimeTyper::getFileNameMimeType( char *inFileName ) {

    int fileNameLength = strlen( inFileName );
    
    int lastPeriodIndex = -1;
            
    for( int i=0; i<fileNameLength; i++ ) {
        if( inFileName[i] == '.' ) {
            lastPeriodIndex = i;
            }
        }

    char *mimeType = NULL;
            
    if( lastPeriodIndex != -1 ) {
        char *exension = &( inFileName[lastPeriodIndex] );
        
        mimeType = getMimeType( exension );
        }
    
    return mimeType;
    }
