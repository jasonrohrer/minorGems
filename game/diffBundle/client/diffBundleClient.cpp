#include "diffBundleClient.h"

#include "minorGems/game/game.h"

#include "minorGems/io/file/File.h"
#include "minorGems/formats/encodingUtils.h"
#include "minorGems/crypto/hashes/sha1.h"

#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/SimpleVector.h"


#include "minorGems/util/random/JenkinsRandomSource.h"




#if defined(__mac__)
    #define PLATFORM_CODE "mac"
    #define WINDOWS_LINE_ENDS 0
#elif defined(WIN_32)
    #define PLATFORM_CODE "win"
    #define WINDOWS_LINE_ENDS 1
#elif defined(LINUX)
    #define PLATFORM_CODE "linux"
    #define WINDOWS_LINE_ENDS 0
#else
    #define PLATFORM_CODE ""
    #define WINDOWS_LINE_ENDS 0
#endif



#include <sys/stat.h>
#include <stdlib.h>


static void copyPermissions( char *inSourceFile, char *inDestFile ) {
    struct stat sourceST;
    
    stat( inSourceFile, &sourceST );
    
    chmod( inDestFile, sourceST.st_mode );
    }




char isUpdatePlatformSupported() {
    if( strcmp( PLATFORM_CODE, "" ) == 0 ) {
        return false;
        }
    else {
        return true;
        }
    }



static int webHandle = -1;
static int updateSize = -1;

static int updateProgressCompleteSteps;

static char *updateServerURL = NULL;
static int oldVersionNumber;


static char batchMirrorUpdate = false;

static char batchStepsDone = 0;

// true if this update is via a mirror url with "all" for the platform name
// on windows, this means we need to do line end conversion on text files
static char currentUpdateUniversal = false;


typedef struct MirrorList {
        int version;
        int currentMirror;
        int size;
        SimpleVector<char*> mirrorURLS;
    } MirrorList;
    

static SimpleVector<MirrorList> mirrors;


static const char *platformCode = "";


static char writeError = false;

char wasUpdateWriteError() {
    return writeError;
    }



char startUpdate( char *inUpdateServerURL, int inOldVersionNumber ) {
    writeError = false;
    
    batchMirrorUpdate = false;
    currentUpdateUniversal = false;


    // make sure we're not being built from inside a working mercurial
    // checkout.  Don't want to download updates in this context
    File hgFileA( NULL, ".hg" );
    File hgFileB( NULL, "../.hg" );
    File hgFileC( NULL, "../../.hg" );
    File hgFileD( NULL, "../../../.hg" );


    if( hgFileA.exists() ||
        hgFileB.exists() ||
        hgFileC.exists() ||
        hgFileD.exists() ) {
    
        return false;
        }
    

    File binaryFlagFile( NULL, "binary.txt" );
    
    platformCode = PLATFORM_CODE;

    if( ! binaryFlagFile.exists() ) {
        // distribution compiled from source?
        // still try fetching platform-independent updates
        platformCode = "all";
        }
    

    char *fullURL = autoSprintf( "%s?action=is_update_available"
                                 "&platform=%s&old_version=%d",
                                 inUpdateServerURL, platformCode,
                                 inOldVersionNumber );
    
    printf( "Checking for latest update at %s\n", fullURL );
    
    webHandle = startWebRequest( "GET", fullURL, NULL );
    
    delete [] fullURL;
    
    if( updateServerURL != NULL ) {
        delete [] updateServerURL;
        }

    updateSize = -1;
    updateProgressCompleteSteps = 0;
    
    updateServerURL = stringDuplicate( inUpdateServerURL );
    oldVersionNumber = inOldVersionNumber;

    return true;
    }



static void dumpRawDataToFile( unsigned char *inRawBundleData,
                               int inNumBytes ) {
    FILE *dumpFile = fopen( "diffBundle_dump.raw", "wb" );
    
    if( dumpFile != NULL ) {
        fwrite( inRawBundleData, 1, inNumBytes, dumpFile );
        fclose( dumpFile );
        }
    }



// returns 1 on success, -1 on failure
static int applyUpdateFromWebResult() {
    // process it, unzip, apply file changes, etc.
            
    int size;
    unsigned char *result = getWebResult( webHandle, &size );

    char *nextRawScanPointer = (char*)result;
            
    // don't use sscanf here because it scans the entire buffer
    // (and this buffer has binary data at end)
    int rawSize = scanIntAndSkip( &nextRawScanPointer );
    int compSize = scanIntAndSkip( &nextRawScanPointer );

    if( rawSize > 0 && compSize > 0 ) {
                
        unsigned char *compData = (unsigned char*)nextRawScanPointer;
                
        char *hash = computeSHA1Digest( compData, compSize );

        AppLog::infoF( "Received compressed data with SHA1 = %s\n",
                       hash );
        delete [] hash;

        unsigned char *rawData = 
            zipDecompress( compData,
                           compSize,
                           rawSize );

        delete [] result;
                

        if( rawData == NULL ) {
            printf( "Failed to decompress diff bundle\n" );
            return -1;
            }

                                
        char *nextScanPointer = (char*)rawData;
        char success;




        
        int numRemovedFiles = scanIntAndSkip( &nextScanPointer, &success );
                    
        if( !success ) {
            printf( "Failed to parse removed file count from diff bundle\n" );
            dumpRawDataToFile( rawData, rawSize );
            delete [] rawData;
            return -1;
            }

        printf( "Removing %d files\n", numRemovedFiles );
                
        for( int d=0; d<numRemovedFiles; d++ ) {
                    
            int fileNameLength = 
                scanIntAndSkip( &nextScanPointer, &success );
                    
            if( !success ) {
                printf( "Failed to parse removed file name length "
                        "from diff bundle\n" );
                dumpRawDataToFile( rawData, rawSize );
                delete [] rawData;
                return -1;
                }
                    
            char *fileName = new char[ fileNameLength + 1 ];
                    
            memcpy( fileName, nextScanPointer, fileNameLength );
                    
            fileName[ fileNameLength ] = '\0';
                    
            printf( "   %s\n", fileName );
                    

            nextScanPointer = 
                &( nextScanPointer[ fileNameLength + 1 ] );
                    
            File fileToRemove( NULL, fileName );

            if( fileToRemove.exists() && ! fileToRemove.isDirectory() ) {
                
                fileToRemove.remove();
                }
                    
                    
            delete [] fileName;
            }



        int numRemovedDirs = scanIntAndSkip( &nextScanPointer, &success );
                    
        if( !success ) {
            printf( "Failed to parse removed file count from diff bundle\n" );
            dumpRawDataToFile( rawData, rawSize );
            delete [] rawData;
            return -1;
            }

        printf( "Removing %d dirs\n", numRemovedDirs );
                
        for( int d=0; d<numRemovedDirs; d++ ) {
                    
            int fileNameLength = 
                scanIntAndSkip( &nextScanPointer, &success );
                    
            if( !success ) {
                printf( "Failed to parse removed dir name length "
                        "from diff bundle\n" );
                dumpRawDataToFile( rawData, rawSize );
                delete [] rawData;
                return -1;
                }
                    
            char *fileName = new char[ fileNameLength + 1 ];
                    
            memcpy( fileName, nextScanPointer, fileNameLength );
                    
            fileName[ fileNameLength ] = '\0';
                    
            printf( "   %s\n", fileName );
                    

            nextScanPointer = 
                &( nextScanPointer[ fileNameLength + 1 ] );
                    
            File dirToRemove( NULL, fileName );

            if( dirToRemove.exists() && dirToRemove.isDirectory() ) {
                
                dirToRemove.remove();
                }
                    
                    
            delete [] fileName;
            }










                
        int numDirs = scanIntAndSkip( &nextScanPointer, &success );
                    
        if( !success ) {
            printf( "Failed to parse dir count from diff bundle\n" );
            dumpRawDataToFile( rawData, rawSize );
            delete [] rawData;
            return -1;
            }

        printf( "Creating %d new directories\n", numDirs );
                
        for( int d=0; d<numDirs; d++ ) {
                    
            int fileNameLength = 
                scanIntAndSkip( &nextScanPointer, &success );
                    
            if( !success ) {
                printf( "Failed to parse directory name length "
                        "from diff bundle\n" );
                dumpRawDataToFile( rawData, rawSize );
                delete [] rawData;
                return -1;
                }
                    
            char *fileName = new char[ fileNameLength + 1 ];
                    
            memcpy( fileName, nextScanPointer, fileNameLength );
                    
            fileName[ fileNameLength ] = '\0';
                    
            printf( "   %s\n", fileName );
                    

            nextScanPointer = 
                &( nextScanPointer[ fileNameLength + 1 ] );
                    
            File dirFile( NULL, fileName );

            if( dirFile.exists() ) {
                printf( "Directory exists %s\n",
                        fileName );
                }
            else {
                char made = Directory::makeDirectory( &dirFile );
                    
                if( !made ) {
                    printf( "Failed to make directory %s\n",
                            fileName );
                            
                    delete [] fileName;
                            
                    dumpRawDataToFile( rawData, rawSize );
                    delete [] rawData;
                    return -1;
                    }
                }
                    
                    
            delete [] fileName;
            }
                


        int numFiles = scanIntAndSkip( &nextScanPointer, &success );

        if( !success ) {
            printf( "Failed to parse file count from diff bundle\n" );
                    
            dumpRawDataToFile( rawData, rawSize );
            delete [] rawData;
            return -1;
            }                

        printf( "Updating %d files\n", numFiles );
        
        char fileCreationFailed = false;
        
        SimpleVector<char*> backupList;

        for( int f=0; f<numFiles; f++ ) {
                    
            int fileNameLength =
                scanIntAndSkip( &nextScanPointer, &success );
                    
            if( !success ) {
                printf( "Failed to parse file name length "
                        "from diff bundle\n" );
                        
                dumpRawDataToFile( rawData, rawSize );
                delete [] rawData;
                return -1;
                }
                    
            char *fileName = new char[ fileNameLength + 1 ];
                    
            memcpy( fileName, nextScanPointer, fileNameLength );
                    
            fileName[ fileNameLength ] = '\0';
                    
            printf( "   %s\n", fileName );

            nextScanPointer = 
                &( nextScanPointer[ fileNameLength + 1 ] );

            // single # separates the file size from the file data
            // this skips it
            int fileSize = 
                scanIntAndSkip( &nextScanPointer, &success );
                    
            if( !success ) {
                printf( "Failed to parse file size "
                        "from diff bundle\n" );
                delete [] fileName;
                        
                dumpRawDataToFile( rawData, rawSize );
                delete [] rawData;
                return -1;
                }
                    

            File targetFile( NULL, fileName );

            char *backupName = NULL;
                    
            if( targetFile.exists() ) {
                backupName = autoSprintf( "%s.bak", fileName );

                printf( "File %s exists, moving temporariliy to %s\n",
                        fileName, backupName );
                
                File backFile( NULL, backupName );
                
                if( backFile.exists() ) {
                    printf( "Backup file %s already exists, skipping move\n",
                            backupName );
                    }
                else {
                    int result = rename( fileName, backupName );
                
                    if( result != 0 ) {
                        printf( "Moving backup to %s failed\n",
                                backupName );
                        fileCreationFailed = true;
                        delete [] fileName;
                        delete [] backupName;
                        printf( "Ending update process\n" );
                        break;
                        }
                    else {
                        backupList.push_back( stringDuplicate( backupName ) );
                        }
                    }
                }
            else {
                if( strstr( fileName, "/" ) != NULL ) {
                    // file name contains a path
                    
                    // make sure the dir exists
                    char *dirName = stringDuplicate( fileName );
                    
                    // find last / and terminate there to get dir name
                    int len = strlen( dirName );
                    for( int i=len-1; i>=0; i-- ) {
                        if( dirName[i] == '/' ) {
                            dirName[i] = '\0';
                            break;
                            }
                        }
                    File dirFile( NULL, dirName );
                    
                    if( ! dirFile.exists() ) {
                        printf( "Making necessary directory %s for "
                                "new file %s\n",
                                dirName, fileName );
                        
                        char made = Directory::makeDirectory( &dirFile );
                    
                        if( !made ) {
                            printf( "Failed to make directory %s\n",
                                    dirName );
                            
                            delete [] dirName;
                            printf( "Ending update process\n" );
                            fileCreationFailed = true;
                            break;
                            }
                        }
                    delete [] dirName;
                    }
                
                }
            

            FILE *file = fopen( fileName, "wb" );
                    
                    
            if( file == NULL ) {
                printf( "Failed to open file %s for writing\n",
                        fileName );
                fileCreationFailed = true;
                delete [] fileName;
                if( backupName != NULL ) {
                    delete [] backupName;
                    }
                printf( "Ending update process\n" );
                break;
                }
            else {
                int numWritten = 
                    fwrite( (unsigned char *)nextScanPointer, 
                            1, fileSize, file );
                if( numWritten != fileSize ) {
                    printf( "Failed to write %d bytes to file  %s\n",
                            fileSize, fileName );
                    }
                        
                fclose( file );
                }
                    
                    
            if( backupName != NULL ) {
                copyPermissions( backupName, fileName );                        
                        
                delete [] backupName;
                }
            else {
                // try to set permissions manually on mac for main app exe
                if( strcmp( PLATFORM_CODE, "mac" ) == 0 ) {
                    if( strstr( fileName, "Contents/MacOS/" ) != NULL ) {
                        const char *mode = "0755";
                        int modeInt = strtol( mode, 0, 8 );
                        chmod( fileName, modeInt );
                        }
                    }
                }
            

            if( currentUpdateUniversal &&
                WINDOWS_LINE_ENDS &&
                strstr( fileName, ".txt" ) != NULL ) {
                char *contents = targetFile.readFileContents();
                
                if( contents != NULL ) {
                    
                    if( strstr( contents, "\n" ) != NULL &&
                        strstr( contents, "\r\n" ) == NULL ) {
                        // contains at least one unix-style line ending
                        // and no \r, which is part of windows \r\n
                        // and other platforms, or ill-formed, line endings

                        char found;
                        char *convertedContents =
                            replaceAll( contents, "\n", "\r\n", &found );
                        
                        if( convertedContents != NULL ) {
                            
                            targetFile.writeToFile( convertedContents );

                            delete [] convertedContents;
                            }
                        }
                    delete [] contents;
                    }
                }

            delete [] fileName;

                
            nextScanPointer = &( nextScanPointer[ fileSize ] );
            }
        
        if( fileCreationFailed ) {
            writeError = true;
            
            // restore from backups if possible
            
            
            for( int i=0; i<backupList.size(); i++ ) {
                char *backName = backupList.getElementDirect( i );
                char *origName = stringDuplicate( backName );
                
                char *bakStart = strstr( origName, ".bak" );
                
                if( bakStart != NULL ) {
                    bakStart[0] = '\0';
                    }
                
                printf( "Trying to restore %s from %s\n",
                        origName, backName );
                       
                if( remove( origName ) != 0 ) {
                    printf( "    Failed to remove %s\n", origName );
                    }
                if( rename( backName, origName ) != 0 ) {
                    printf( "    Failed to move %s to %s\n", 
                            backName, origName );
                    }
                delete [] origName;
                }

            backupList.deallocateStringElements();
            
            return -1;
            }
        

        // success
        // remove backup files if we can
        
        for( int i=0; i<backupList.size(); i++ ) {
            char *backName = backupList.getElementDirect( i );
            
            if( remove( backName ) != 0 ) {
                // can't remove
                // save on list to remove later if postUpdate called
                // (if postUpdate not call, just leave them)
                FILE *postRemoveListFile =
                    fopen( "postRemoveList.txt", "a" );
                if( postRemoveListFile != NULL ) {    
                    fprintf( postRemoveListFile, 
                             "%s\n", backName );
                    fclose( postRemoveListFile );
                    }
                }
            }
        
        backupList.deallocateStringElements();


                
        delete [] rawData;
        printf( "Update complete\n" );
        return 1;
        }
    else {
        printf( "Failed to parse diff bundle\n" );
        delete [] result;
        return -1;
        }

    }

    


static int batchMirrorStep() {

    if( batchStepsDone < mirrors.size() ) {
        
        if( webHandle != -1 ) {
            
            int result = stepWebRequest( webHandle );

            if( result == 1 ) {
                result = applyUpdateFromWebResult();
                
                clearWebRequest( webHandle );
                webHandle = -1;
            
                if( result == 1 ) {
                    // start next step on next step() call
                    batchStepsDone ++;
                    return 0;
                    }
                }
            
            if( result == -1 ) {
                if( writeError ) {
                    // stop immediately, don't try another mirror
                    clearWebRequest( webHandle );
                    webHandle = -1;
                    return -1;
                    }
                MirrorList *list = mirrors.getElement( batchStepsDone );
            
                if( list->currentMirror < list->mirrorURLS.size() - 1 ) {
                    
                    list->currentMirror ++;
                    
                    clearWebRequest( webHandle );
                    webHandle = -1;
                    // start request next step
                    return 0;
                    }
                else {
                    // exhausted mirror list
                    return result;
                    }
                }                
            return result;
            }
        else {
            MirrorList *list = mirrors.getElement( batchStepsDone );
            
            if( list->currentMirror < list->mirrorURLS.size() ) {

                if( strstr( list->mirrorURLS.getElementDirect( 
                                list->currentMirror ),
                            "_all.dbz" ) != NULL ) {
                    
                    currentUpdateUniversal = true;
                    }
                else {
                    currentUpdateUniversal = false;
                    }
                         

                // start a request
                printf( "Trying to fetch:  %s\n", 
                        list->mirrorURLS.getElementDirect( 
                            list->currentMirror ) );
                
                webHandle = 
                    startWebRequest( 
                        "GET", 
                        list->mirrorURLS.getElementDirect( 
                        list->currentMirror ), 
                        NULL );
                
                updateSize = list->size;
                return 0;
                }
            else {
                // ran out of mirrors
                return -1;
                }
            }
        }
    else {
        // done
        return 1;
        }

    }



// take another non-blocking step
// return 1 if request complete
// return -1 if request hit an error
// return 0 if request still in-progress
int stepUpdate() {
    
    if( batchMirrorUpdate ) {
        return batchMirrorStep();
        }

    int result = stepWebRequest( webHandle );

    if( result == 1 ) {
        if( updateSize == -1 ) {
            char *result = getWebResult( webHandle );
            

            if( strstr( result, "URLS" ) == result ) {
                
                // init rand source once here
                // if we init it over and over in the loop, we're likely
                // going to be seeding with the same time repeatedly.
                JenkinsRandomSource shuffleRand;

                // starts with URLS
                // batch update from remote mirrors
                printf( "Receiving URL list: \n%s\n\n", result );
            
                int numParts;
                char **parts = split( result, "#", &numParts );
                
                if( numParts > 0 ) {
                    // "URLS\n"
                    delete [] parts[0];
                    }

                for( int i=1; i<numParts; i++ ) {

                    char *trimmedPart = trimWhitespace( parts[i] );

                    int numLines;
                    char **lines = split( trimmedPart, "\n", &numLines );
                    
                    delete [] trimmedPart;

                    if( numLines > 0 ) {
                        // "UPDATE A->B"

                        int a, b, size;
                        sscanf( lines[0], "UPDATE %d->%d %d", &a, &b, &size );

                        delete [] lines[0];
                        
                        MirrorList list;
                        
                        list.version = b;
                        list.currentMirror = 0;
                        list.size = size;
                        
                        for( int j=1; j<numLines; j++ ) {
                            list.mirrorURLS.push_back( lines[j] );
                            }

                        int numMirrors = list.mirrorURLS.size();
                        
                        // shuffle them
                        // https://en.wikipedia.org/wiki/
                        //     Fisher%E2%80%93Yates_shuffle
                        
                        for( int j=numMirrors - 1; j >= 1; j-- ) {
                            int k = shuffleRand.getRandomBoundedInt( 0, j );
                            list.mirrorURLS.swap( k, j );
                            }
                        
                        printf( "After shuffling, URL list=\n" );
                        for( int j=0; j<numMirrors; j++ ) {
                            printf( "%s\n",
                                    list.mirrorURLS.getElementDirect( j ) );
                            }
                        printf( "\n" );

                        mirrors.push_back( list );
                        }
                    
                    delete [] lines;
                    
                    delete [] parts[i];
                    }
                delete [] parts;

                batchMirrorUpdate = true;
                batchStepsDone = 0;
                
                clearWebRequest( webHandle );
                webHandle = -1;
                return 0;
                }
            else {
                // direct update from this server
                int numRead = sscanf( result, "%d", &updateSize );
            
                delete [] result;
                if( numRead != 1 ) {
                    updateSize = 0;
                    return -1;
                    }
                
                if( updateSize <= 0 ) {
                    // no update available for this platform
                    return -1;
                    }
                
                // have an update size
                
                printf( "Found an update with %d bytes\n",
                        updateSize );

                // start next request for update itself
                clearWebRequest( webHandle );
                
                char *fullURL = autoSprintf( "%s?action=get_update"
                                             "&platform=%s&old_version=%d",
                                             updateServerURL, platformCode,
                                             oldVersionNumber );
                
                
                printf( "Downloading update from %s\n", fullURL );
                
                webHandle = startWebRequest( "GET", fullURL, NULL );
                
                delete [] fullURL;
            
                return 0;
                }
            }
        else if( updateProgressCompleteSteps < 1 ) {
            // don't process data this step, wait until next step
            updateProgressCompleteSteps++;
            return 0;
            }
        else {
            // have update itself AND we've let one step go
            // by for our final update progress to post

            
            printf( "Update download complete\n" );
            
            return applyUpdateFromWebResult();
            }
        }
    
    return result;
    }



float getUpdateProgress() {
    if( batchMirrorUpdate ) {
        
        float globalProgress = batchStepsDone / (float)mirrors.size();
        

        if( updateSize > 0 ) {
            float progress = 0;
            
            if( webHandle != -1 ) {
                progress = getWebProgressSize( webHandle ) / (float)updateSize;
                }
            
            if( progress > 1 ) {
                // getWebProgressSize includes headers
                progress = 1;
                }
            
            globalProgress += progress * 1.0f / mirrors.size();
            }
        
        return globalProgress;
        }
    else {
        
        if( updateSize > 0 ) {
            float progress = 
                getWebProgressSize( webHandle ) / (float)updateSize;
        
            if( progress > 1 ) {
                // getWebProgressSize includes headers
                progress = 1;
                }
            
            return progress;
            }
        else {
            return 0;
            }
        }
    }




void clearUpdate() {
    clearWebRequest( webHandle );

    if( updateServerURL != NULL ) {
        delete [] updateServerURL;
        }
    
    updateServerURL = NULL;


    for( int i=0; i<mirrors.size(); i++ ) {
        MirrorList *l = mirrors.getElement( i );
        
        
        for( int j=0; j<l->mirrorURLS.size(); j++ ) {
            delete [] l->mirrorURLS.getElementDirect( j );
            }        
        }
    mirrors.deleteAll();
    }



void postUpdate() {

    File postRemoveListFile( NULL, "postRemoveList.txt" );
    
    if( postRemoveListFile.exists() ) {
        char *contents = postRemoveListFile.readFileContents();
        
        char allRemoved = true;
        
        if( contents != NULL ) {
            
            int numFiles;
            char **fileNames = split( contents, "\n", &numFiles );

            for( int i=0; i<numFiles; i++ ) {
                File thisFile( NULL, fileNames[i] );
                
                if( thisFile.exists() ) {
                    if( remove( fileNames[i] ) != 0 ) {
                        allRemoved = false;
                        }
                    }
                
                delete [] fileNames[i];
                }
            delete [] fileNames;
            
            
            delete [] contents;
            }
        else {
            allRemoved = false;
            }
        
        if( allRemoved ) {
            postRemoveListFile.remove();
            }
        }
    }

