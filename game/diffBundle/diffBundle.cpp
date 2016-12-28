

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



static void bundleFileList( File **inFiles, int inNumFiles,
                            SimpleVector<unsigned char> *inFileDataBuffer ) {

    char *fileCount = autoSprintf( "%d ", inNumFiles );
    inFileDataBuffer->appendArray( (unsigned char*)fileCount, 
                                strlen( fileCount ) );
    
    delete [] fileCount;
    

    for( int i=0; i<inNumFiles; i++ ) {
        char *fileName = inFiles[i]->getFullFileName();

        char *fileSubdirName = getSubdirPath( fileName );
        
        char *header = autoSprintf( "%d %s ",
                                    strlen( fileSubdirName ),
                                    fileSubdirName );
        inFileDataBuffer->appendArray( (unsigned char*)header, 
                                       strlen( header ) );
        delete [] header;

        delete [] fileName;
        delete [] fileSubdirName;
        }
    
    }




static void bundleFiles( File **inFilesToRemove, int inNumFilesToRemove,
                         File **inDirsToRemove, int inNumDirsToRemove,
                         File **inDirs, int inNumDirs,
                         File **inFiles, int inNumFiles,
                         char *inDBZTargetFile ) {

    SimpleVector<unsigned char> fileDataBuffer;
    

    printf( "Bundling file removals...\n" );
    bundleFileList( inFilesToRemove, inNumFilesToRemove, &fileDataBuffer );

    printf( "Bundling dir removals...\n" );
    bundleFileList( inDirsToRemove, inNumDirsToRemove, &fileDataBuffer );

    printf( "Bundling dirs...\n" );
    bundleFileList( inDirs, inNumDirs, &fileDataBuffer );
    


    printf( "Bundling files...\n" );
    
    char *fileCount = autoSprintf( "%d ", inNumFiles );
    fileDataBuffer.appendArray( (unsigned char*)fileCount, 
                                strlen( fileCount ) );
    
    delete [] fileCount;
    

    for( int i=0; i<inNumFiles; i++ ) {
        char *fileName = inFiles[i]->getFullFileName();

        char *fileSubdirName = getSubdirPath( fileName );
        int size = inFiles[i]->getLength();
        
        
        // use # as separator before file data instead of space
        // because when parsing later, sscanf will scan multiple spaces
        // (for example, spaces at the start of the file data itself) as 
        // a single space, thus potentially eating part of the file data.
        // But even if the file data starts with '#', we'll be okay here,
        // because we can sscanf just a single # after the file size number.
        char *header = autoSprintf( "%d %s %d#",
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
            printf( "Wrote %d bytes to file %s\n", (int)ftell( outFile ),
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




static void printFileList( const char *inDescription,
                           SimpleVector<File *> *inList ) {

    if( inList->size() == 0 ) {
        printf( "\nNo %s.\n\n", inDescription );
        return;
        }

    printf( "%d %s:\n", inList->size(), inDescription );

    for( int i=0; i<inList->size(); i++ ) {
        char *fileName = 
            (* inList->getElement( i ) )->getFullFileName();
        
        char *fileSubdirName = getSubdirPath( fileName );

        printf( "  %s\n", fileSubdirName );

        delete [] fileName;
        delete [] fileSubdirName;
        }
    }




// NOTE:
// this app has a default recursion depth limit of 100 when entering
// sub directories
int main( int inNumArgs, char **inArgs ) {
    
    if( inNumArgs != 5 && inNumArgs != 4 ) {        
		printf( "\nUsage:  diffBundle  dirOld dirNew "
                "outIncremental.dbz [outFull.dbz]\n\n" );
        printf( "If outFull.dbz not supplied, only the incremental bundle is "
                "generated.\n\n" );
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

    SimpleVector<File*> newDirs;

    
    if( inNumArgs == 5 ) {
        

        // first, generate the full DBZ
        printf( "Making full bundle...\n" );
    
        // exclude all dirs
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
        
        bundleFiles( NULL, 0,
                     NULL, 0, 
                     newDirsArray, numNewDirs,
                     newNonDirsArray, numNewNonDirs, inArgs[4] );
        
        delete [] newDirsArray;
        delete [] newNonDirsArray;
        }
    

    printf( "\n\nMaking incremental bundle...\n" );
    
    printf( "Scanning files...\n" );

    SimpleVector<File *> removedFiles;
    SimpleVector<File *> removedDirs;


    // don't worry about directory vs non-directory collisions here
    // we exit in that case when scanning for differences below
    // we can assume that that kind of change will never happen
    for( int i=0; i<numOldChild; i++ ) {

        char *oldFileName = oldChild[i]->getFullFileName();
        char *oldFileSubdirName = getSubdirPath( oldFileName );

        char foundNewFile = false;
        
        for( int j=0; j<numNewChild && ! foundNewFile; j++ ) {
            char *newFileName = newChild[j]->getFullFileName();
            
            char *newFileSubdirName = getSubdirPath( newFileName );

            if( strcmp( oldFileSubdirName, newFileSubdirName ) == 0 ) {
                foundNewFile = true;
                }
            delete [] newFileName;
            delete [] newFileSubdirName;
            }
        

        if( !foundNewFile ) {
            
            if( oldChild[i]->isDirectory() ) {
                removedDirs.push_back( oldChild[i] );
                }
            else {
                removedFiles.push_back( oldChild[i] );
                }
            }
        
        delete [] oldFileName;
        delete [] oldFileSubdirName;
        }
    

    printFileList( "removed files", &removedFiles );

    printFileList( "removed directories", &removedDirs );

    
    
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

    
    printFileList( "new directories", &newDirs );
    



    printFileList( "changed files", &changedFiles );
    
    

    int numRemovedFiles = removedFiles.size();
    File **removedFilesArray = removedFiles.getElementArray();
    int numRemovedDirs = removedDirs.size();
    File **removedDirsArray = removedDirs.getElementArray();
    
    
    int numChanged = changedFiles.size();
    File **changedFilesArray = changedFiles.getElementArray();
    
    int numNewDirs = newDirs.size();
    File **newDirsArray = newDirs.getElementArray();

    bundleFiles( removedFilesArray, numRemovedFiles,
                 removedDirsArray, numRemovedDirs, 
                 newDirsArray, numNewDirs,
                 changedFilesArray, numChanged, inArgs[3] );

    delete [] removedFilesArray;
    delete [] removedDirsArray;
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
