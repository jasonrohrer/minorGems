#include "diffBundleClient.h"

#include "minorGems/game/game.h"

#include "minorGems/io/file/File.h"
#include "minorGems/formats/encodingUtils.h"
#include "minorGems/crypto/hashes/sha1.h"

#include "minorGems/util/log/AppLog.h"


#if defined(__mac__)
    #define PLATFORM_CODE "mac"
#elif defined(WIN_32)
    #define PLATFORM_CODE "win"
#elif defined(LINUX)
    #define PLATFORM_CODE "linux"
#else
    #define PLATFORM_CODE ""
#endif



#include <sys/stat.h>

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



static int webHandle;
static int updateSize = -1;

static int updateProgressCompleteSteps;

static char *updateServerURL = NULL;
static int oldVersionNumber;


// returns handle
char startUpdate( char *inUpdateServerURL, int inOldVersionNumber ) {
    File binaryFlagFile( NULL, "binary.txt" );
    
    if( ! binaryFlagFile.exists() ) {
        return false;
        }
    

    char *fullURL = autoSprintf( "%s?action=is_update_available"
                                 "&platform=%s&old_version=%d",
                                 inUpdateServerURL, PLATFORM_CODE,
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


// take another non-blocking step
// return 1 if request complete
// return -1 if request hit an error
// return 0 if request still in-progress
int stepUpdate() {
    int result = stepWebRequest( webHandle );

    if( result == 1 ) {
        if( updateSize == -1 ) {
            char *result = getWebResult( webHandle );
            
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
                                         updateServerURL, PLATFORM_CODE,
                                         oldVersionNumber );
    
            
            printf( "Downloading update from %s\n", fullURL );

            webHandle = startWebRequest( "GET", fullURL, NULL );
    
            delete [] fullURL;
            
            return 0;
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

            // process it, unzip, apply file changes, etc.
            
            int size;
            unsigned char *result = getWebResult( webHandle, &size );

            int rawSize, compSize;
            int bytesScanned;
            int numRead =
                sscanf( (char*)result, "%d %d %n", 
                        &rawSize, &compSize, &bytesScanned );
            
            // not sure whether %n counts as a read argument or not
            if( numRead == 2 || numRead == 3 ) {
                
                unsigned char *compData = & result[ bytesScanned ];
                
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

                int bytesUsed = 0;
                
                int numDirs;
                numRead =
                    sscanf( (char*)&rawData[bytesUsed], "%d %n", 
                            &numDirs, &bytesScanned );
                    
                if( numRead != 1 && numRead != 2 ) {
                    printf( "Failed to parse dir count from diff bundle\n" );
                    dumpRawDataToFile( rawData, rawSize );
                    delete [] rawData;
                    return -1;
                    }
                bytesUsed += bytesScanned;
                
                printf( "Creating %d new directories\n", numDirs );
                
                for( int d=0; d<numDirs; d++ ) {
                    
                    int fileNameLength;
                    
                    numRead =
                        sscanf( (char*)&rawData[bytesUsed], "%d %n", 
                                &fileNameLength, &bytesScanned );
                    
                    if( numRead != 1 && numRead != 2 ) {
                        printf( "Failed to parse directory name length "
                                "from diff bundle\n" );
                        dumpRawDataToFile( rawData, rawSize );
                        delete [] rawData;
                        return -1;
                        }
                    
                    bytesUsed += bytesScanned;
                    
                    char *fileName = new char[ fileNameLength + 1 ];
                    
                    memcpy( fileName, &rawData[bytesUsed], fileNameLength );
                    
                    fileName[ fileNameLength ] = '\0';
                    
                    printf( "   %s\n", fileName );
                    
                    bytesUsed += fileNameLength + 1;

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
                


                int numFiles;
                numRead =
                    sscanf( (char*)&rawData[bytesUsed], "%d %n", 
                            &numFiles, &bytesScanned );
                    
                if( numRead != 1 && numRead != 2 ) {
                    printf( "Failed to parse file count from diff bundle\n" );
                    
                    dumpRawDataToFile( rawData, rawSize );
                    delete [] rawData;
                    return -1;
                    }
                bytesUsed += bytesScanned;
                

                printf( "Updating %d files\n", numFiles );
                
                for( int f=0; f<numFiles; f++ ) {
                    
                    int fileNameLength;
                    
                    numRead =
                        sscanf( (char*)&rawData[bytesUsed], "%d %n", 
                                &fileNameLength, &bytesScanned );
                    
                    if( numRead != 1 && numRead != 2 ) {
                        printf( "Failed to parse file name length "
                                "from diff bundle\n" );
                        
                        dumpRawDataToFile( rawData, rawSize );
                        delete [] rawData;
                        return -1;
                        }
                    
                    bytesUsed += bytesScanned;
                    
                    char *fileName = new char[ fileNameLength + 1 ];
                    
                    memcpy( fileName, &rawData[bytesUsed], fileNameLength );
                    
                    fileName[ fileNameLength ] = '\0';
                    
                    printf( "   %s\n", fileName );

                    bytesUsed += fileNameLength + 1;

                    int fileSize;
                    
                    // single # separates the file size from the file data
                    numRead =
                        sscanf( (char*)&rawData[bytesUsed], "%d#%n", 
                                &fileSize, &bytesScanned );
                    
                    if( numRead != 1 && numRead != 2 ) {
                        printf( "Failed to parse file size "
                                "from diff bundle\n" );
                        delete [] fileName;
                        
                        dumpRawDataToFile( rawData, rawSize );
                        delete [] rawData;
                        return -1;
                        }
                    bytesUsed += bytesScanned;
                    

                    File targetFile( NULL, fileName );

                    char *backupName = NULL;
                    
                    if( targetFile.exists() ) {
                        backupName = autoSprintf( "%s.bak", fileName );
                        
                        rename( fileName, backupName );
                        
                        printf( "File %s exists, moving temporariliy to %s\n",
                                fileName, backupName );
                        }
                    

                    FILE *file = fopen( fileName, "wb" );
                    
                    
                    if( file == NULL ) {
                        printf( "Failed to open file %s for writing\n",
                                fileName );
                        }
                    else {
                        int numWritten = fwrite( &rawData[bytesUsed], 
                                                 1, fileSize, file );
                        if( numWritten != fileSize ) {
                            printf( "Failed to write %d bytes to file  %s\n",
                                    fileSize, fileName );
                            }
                        
                        fclose( file );
                        }
                    
                    
                    if( backupName != NULL ) {
                        copyPermissions( backupName, fileName );
                        
                        if( remove( backupName ) != 0 ) {
                            FILE *postRemoveListFile =
                                fopen( "postRemoveList.txt", "a" );
                            if( postRemoveListFile != NULL ) {    
                                fprintf( postRemoveListFile, 
                                         "%s\n", backupName );
                                fclose( postRemoveListFile );
                                }
                            }
                        
                        
                        delete [] backupName;
                        }
                    
                    delete [] fileName;
                    
                    bytesUsed += fileSize;
                    }
                
                delete [] rawData;
                
                printf( "Update complete\n" );
                }
            else {
                printf( "Failed to parse diff bundle\n" );
                delete [] result;
                return -1;
                }
            
            }
        }
    
    return result;
    }



float getUpdateProgress() {
    if( updateSize > 0 ) {
        float progress = getWebProgressSize( webHandle ) / (float)updateSize;
        
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




void clearUpdate() {
    clearWebRequest( webHandle );

    if( updateServerURL != NULL ) {
        delete [] updateServerURL;
        }
    
    updateServerURL = NULL;
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

