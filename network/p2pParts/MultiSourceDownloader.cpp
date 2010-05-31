/*
 * Modification History
 *
 * 2004-November-21   Jason Rohrer
 * Created.
 *
 * 2004-November-23   Jason Rohrer
 * Fixed compile errors caused by multiple definitions.
 *
 * 2004-December-4   Jason Rohrer
 * Fixed bug in source indexing.
 */



#include "MultiSourceDownloader.h"


#include <stdio.h>


int MULTISOURCE_DOWNLOAD_IN_PROGRESS = 0;
int MULTISOURCE_DOWNLOAD_FAILED = 1;
int MULTISOURCE_DOWNLOAD_CANCELED = 2;



void multiSourceGetFile( void *inFileDescriptor,
                         unsigned long inFileSize,
                         unsigned long inChunkSize,
                         int inNumSources,
                         void **inFileSources,
                         unsigned char * (*inChunkGetter)(
                             void *, void *, unsigned long, unsigned long ),
                         char (*inDownloadProgressHandler)(
                             int, unsigned long, void * ),
                         void *inProgressHandlerExtraArgument,
                         char *inDestinationPath ) {

    FILE *outputFile = fopen( inDestinationPath, "wb" );

    if( outputFile == NULL ) {

        inDownloadProgressHandler( MULTISOURCE_DOWNLOAD_FAILED,
                                   0, inProgressHandlerExtraArgument );
        return;
        }

    
    // for now, just fetch chunks in linear order, using each file source
    // until it fails
    char failed = false;
    char canceled = false;

    unsigned long chunksSoFar = 0;
    unsigned long numChunks = inFileSize / inChunkSize;
    if( inFileSize % inChunkSize != 0 ) {
        // extra partial chunk
        numChunks++;
        }

    unsigned long bytesSoFar = 0;

    int sourceIndex = 0;

    while( sourceIndex < inNumSources &&
           !failed &&
           !canceled &&
           chunksSoFar < numChunks ) {
        
        void *source = inFileSources[ sourceIndex ];

        unsigned long chunkSize = inChunkSize;
        
        if( chunksSoFar * inChunkSize + chunkSize > inFileSize ) {
            // partial chunk
            chunkSize = inFileSize - chunksSoFar * inChunkSize;
            }

        unsigned char *chunkData =
            inChunkGetter(
                source, inFileDescriptor, chunksSoFar, chunkSize );

        if( chunkData != NULL ) {
            chunksSoFar += 1;
            
            bytesSoFar += chunkSize;
            unsigned long numWritten =
                fwrite( chunkData, 1, chunkSize, outputFile );

            if( numWritten == chunkSize ) {
                char shouldContinue =
                    inDownloadProgressHandler(
                        MULTISOURCE_DOWNLOAD_IN_PROGRESS,
                        bytesSoFar,
                        inProgressHandlerExtraArgument );

                if( !shouldContinue ) {
                    canceled = true;

                    // call handler last time
                    inDownloadProgressHandler(
                        MULTISOURCE_DOWNLOAD_CANCELED,
                        bytesSoFar,
                        inProgressHandlerExtraArgument );
                    }
                }
            else {
                // failed to write to file, so download cannot continue
                inDownloadProgressHandler( MULTISOURCE_DOWNLOAD_FAILED,
                                           bytesSoFar,
                                           inProgressHandlerExtraArgument );
                failed = true;
                }
            delete [] chunkData;
            }
        else {
            // else go on to the next host
            sourceIndex ++;
            }
        }

    // check if we got here because we ran out of hosts (another type of
    // failure
    if( !failed && !canceled &&
        chunksSoFar < numChunks ) {

        inDownloadProgressHandler( MULTISOURCE_DOWNLOAD_FAILED,
                                   bytesSoFar,
                                   inProgressHandlerExtraArgument );
        }
    
    fclose( outputFile );    
    }


