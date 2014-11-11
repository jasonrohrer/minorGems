

#include "minorGems/io/file/File.h"
#include "minorGems/formats/encodingUtils.h"
#include "minorGems/crypto/hashes/sha1.h"

#include <stdlib.h>


// strips off the first step of a path
// converts dirA/dirB/test.txt to dirB/test.txt
static char *getSubdirPath( char *inFullFilePath ) {
    char delim = Path::getDelimeter();
    
    // skip very first, to avoid stopping at root
    int firstDelimPosition = 1;
    
    int found = false;
    
    int pathLen = strlen( inFullFilePath );
    
    for( int i=0; i<pathLen; i++ ) {
        if( inFullFilePath[i] == delim ) {
            firstDelimPosition = i;
            found = true;
            break;
            }
        }

    if( !found ) {
        printf( "getSubdirPath didn't find delimeters %c in %s\n",
                delim, inFullFilePath );
        
        return stringDuplicate( inFullFilePath );
        }
    return stringDuplicate( &( inFullFilePath[ firstDelimPosition + 1 ] ) );
    }




static void bundleFiles( File **inDirs, int inNumDirs,
                         File **inFiles, int inNumFiles,
                         char *inDBZTargetFile ) {

    SimpleVector<unsigned char> fileDataBuffer;
    

    printf( "Bundling dirs...\n" );
    
    char *dirCount = autoSprintf( "%d ", inNumDirs );
    fileDataBuffer.appendArray( (unsigned char*)dirCount, strlen( dirCount ) );
    
    delete [] dirCount;
    

    for( int i=0; i<inNumDirs; i++ ) {
        char *fileName = inDirs[i]->getFullFileName();

        char *fileSubdirName = getSubdirPath( fileName );
        
        char *header = autoSprintf( "%d %s ",
                                    strlen( fileSubdirName ),
                                    fileSubdirName );
        fileDataBuffer.appendArray( (unsigned char*)header, strlen( header ) );
        delete [] header;
        }
    


    printf( "Bundling files...\n" );
    
    char *fileCount = autoSprintf( "%d ", inNumFiles );
    fileDataBuffer.appendArray( (unsigned char*)fileCount, 
                                strlen( fileCount ) );
    
    delete [] fileCount;
    

    for( int i=0; i<inNumFiles; i++ ) {
        char *fileName = inFiles[i]->getFullFileName();

        char *fileSubdirName = getSubdirPath( fileName );
        int size = inFiles[i]->getLength();
        
        
        char *header = autoSprintf( "%d %s %d ",
                                    strlen( fileSubdirName ),
                                    fileSubdirName,
                                    size );
        fileDataBuffer.appendArray( (unsigned char*)header, strlen( header ) );
        delete [] header;
        
        int contentLength;
        unsigned char *contents = 
            inFiles[i]->readFileContents( &contentLength );
        
        if( contents != NULL && contentLength == size ) {
            fileDataBuffer.appendArray( contents, contentLength );
            }
        else {
            printf( "Reading file contents of %s failed, or expected size "
                    "%d did not match actual size %d\n",
                    fileName, size, contentLength );
            }
        
        if( contents != NULL ) {
            delete [] contents;
            }
        

                
        delete [] fileName;
        
        delete [] fileSubdirName;
        }
    
    int totalSize = fileDataBuffer.size();
    
    unsigned char *data = fileDataBuffer.getElementArray();
    

    /*
    // for testing, output uncompressed data too
    char *rawFileName = autoSprintf( "%s.raw", inDBZTargetFile );
    
    FILE *outFile = fopen( rawFileName, "wb" );

    delete [] rawFileName;
    
    int numWritten = fwrite( data, 1, totalSize, outFile );

    fclose( outFile );
    */

    printf( "Compressing bundle...\n" );

    int compSize;
    unsigned char *compData = zipCompress( data, totalSize, &compSize );
    

    char *hash = computeSHA1Digest( compData, compSize );
    
    printf( "Compressed data has SHA1 = %s\n", hash );
    
    delete [] hash;


    if( compData != NULL ) {
        
        FILE *outFile = fopen( inDBZTargetFile, "wb" );
        
        if( outFile != NULL ) {
            
            printf( "Writing file %s\n", inDBZTargetFile );
            
            fprintf( outFile, "%d %d ", totalSize, compSize );
            
            int numWritten = fwrite( compData, 1, compSize, outFile );
            if( numWritten != compSize ) {
                printf( "Tried to write %d to file %s, "
                        "but wrote %d instead\n",
                        compSize, inDBZTargetFile, numWritten );
                }
            printf( "Wrote %d bytes to file %s\n", ftell( outFile ),
                    inDBZTargetFile );
            fclose( outFile );
            }
        else {
            printf( "Failed to open %s for writing\n", inDBZTargetFile );
            }

        delete [] compData;
        }
    

    delete [] data;
    }






// NOTE:
// this app has a default recursion depth limit of 100 when entering
// sub directories
int main( int inNumArgs, char **inArgs ) {
    
    if( inNumArgs != 5 ) {
		printf( "Usage:  diffBundle  dirOld dirNew "
                "outIncremental.dbz outFull.dbz\n" );
		return 1;
		}
    
    File dirOld( NULL, inArgs[1] );
    
    if( ! dirOld.exists() || ! dirOld.isDirectory() ) {
        printf( "Directory %s not found\n", inArgs[1] );
        
        return 0;
        }
    
    File dirNew( NULL, inArgs[2] );

    if( ! dirNew.exists() || ! dirNew.isDirectory() ) {
        printf( "Directory %s not found\n", inArgs[2] );
        
        return 0;
        }

    //File outIncFile( NULL, inArgs[3] );
    //File outFullFile( NULL, inArgs[3] );
    
    
    int numOldChild;
    File **oldChild = dirOld.getChildFilesRecursive( 100, &numOldChild );
    
    int numNewChild;
    File **newChild = dirNew.getChildFilesRecursive( 100, &numNewChild );
    
    // bundle is additive, ignore files in old that are no longer in new
    

    // first, generate the full DBZ
    printf( "Making full bundle...\n" );
    
    // exclude all dirs
    SimpleVector<File*> newDirs;
    SimpleVector<File*> newNonDirs;
    for( int i=0; i<numNewChild; i++ ) {
        if( newChild[i]->isDirectory() ) {
            newDirs.push_back( newChild[i] );
            }
        else {
            newNonDirs.push_back( newChild[i] );
            }
        }
    int numNewNonDirs = newNonDirs.size();
    File **newNonDirsArray = newNonDirs.getElementArray();

    int numNewDirs = newDirs.size();
    File **newDirsArray = newDirs.getElementArray();

    
    printf( "%d new directories, %d files to bundle\n", 
            numNewDirs, numNewNonDirs );

    bundleFiles( newDirsArray, numNewDirs,
                 newNonDirsArray, numNewNonDirs, inArgs[4] );
    
    delete [] newDirsArray;
    delete [] newNonDirsArray;


    printf( "\n\nMaking incremental bundle...\n" );
    
    printf( "Scanning files...\n" );
    
    
    // directories in new that don't exist in old
    newDirs.deleteAll();

    SimpleVector<File*> changedFiles;
    
    for( int i=0; i<numNewChild; i++ ) {        
        
        char *newFileName = newChild[i]->getFullFileName();
        
        char *newFileSubdirName = getSubdirPath( newFileName );

        int newFileLength;
        unsigned char *newFileContents = 
            newChild[i]->readFileContents( &newFileLength );
        
        char foundOldFile = false;
        for( int j=0; j<numOldChild && ! foundOldFile; j++ ) {
            char *oldFileName = oldChild[j]->getFullFileName();
            
            char *oldFileSubdirName = getSubdirPath( oldFileName );

            if( strcmp( oldFileSubdirName, newFileSubdirName ) == 0 ) {
                foundOldFile = true;
                
                char doneComparing = false;
                
                if( newChild[i]->isDirectory() ) {
                    if( oldChild[j]->isDirectory() ) {
                        // both are dirs, no data contents
                        doneComparing = true;
                        }
                    else {
                        printf( "%s is dir in %s, but non-dir in %s\n",
                                newFileSubdirName, inArgs[2], inArgs[1] );
                        
                        exit( 1 );
                        }
                    }
                else if( oldChild[j]->isDirectory() ) {
                    printf( "%s is non-dir in %s, but dir in %s\n",
                            newFileSubdirName, inArgs[2], inArgs[1] );
                    
                    exit( 1 );
                    }
                
                
                if( !doneComparing ) {
                
                    int oldFileLength;
                    unsigned char *oldFileContents = 
                        oldChild[j]->readFileContents( &oldFileLength );
                
                    if( oldFileLength != newFileLength ) {
                        changedFiles.push_back( newChild[i] );
                        }
                    else {
                        for( int b=0; b<newFileLength; b++ ) {
                            if( oldFileContents[b] != newFileContents[b] ) {
                                changedFiles.push_back( newChild[i] );
                                break;
                                }
                            }
                        }

                    delete [] oldFileContents;
                    }
                }
            
            delete [] oldFileName;
            delete [] oldFileSubdirName;
            }
        
        if( !foundOldFile ) {
            if( newChild[i]->isDirectory() ) {
                newDirs.push_back( newChild[i] );
                }
            else {
                changedFiles.push_back( newChild[i] );
                }
            }

        delete [] newFileName;
        delete [] newFileSubdirName;
        delete [] newFileContents;
        }


    printf( "%d new directories:\n", newDirs.size() );

    for( int i=0; i<newDirs.size(); i++ ) {
        char *fileName = 
            (* newDirs.getElement( i ) )->getFullFileName();
        
        char *fileSubdirName = getSubdirPath( fileName );

        printf( "  %s\n", fileSubdirName );

        delete [] fileName;
        delete [] fileSubdirName;
        }


    
    printf( "%d changed files:\n", changedFiles.size() );

    for( int i=0; i<changedFiles.size(); i++ ) {
        char *fileName = 
            (* changedFiles.getElement( i ) )->getFullFileName();
        
        char *fileSubdirName = getSubdirPath( fileName );

        printf( "  %s\n", fileSubdirName );

        delete [] fileName;
        delete [] fileSubdirName;
        }
    
    
    int numChanged = changedFiles.size();
    File **changedFilesArray = changedFiles.getElementArray();
    
    numNewDirs = newDirs.size();
    newDirsArray = newDirs.getElementArray();

    bundleFiles( newDirsArray, numNewDirs,
                 changedFilesArray, numChanged, inArgs[3] );

    delete [] changedFilesArray;
    delete [] newDirsArray;
    
    
    for( int i=0; i<numOldChild; i++ ) {
        delete oldChild[i];
        }
    delete [] oldChild;

    
    for( int i=0; i<numNewChild; i++ ) {
        delete newChild[i];
        }
    delete [] newChild;

    

    
    return 1;
    }
