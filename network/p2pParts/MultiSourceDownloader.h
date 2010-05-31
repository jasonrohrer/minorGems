/*
 * Modification History
 *
 * 2004-November-21   Jason Rohrer
 * Created.
 *
 * 2004-November-23   Jason Rohrer
 * Fixed compile errors caused by multiple definitions.
 */



#ifndef MULTISOURCE_DOWNLOADER_INCLUDED
#define MULTISOURCE_DOWNLOADER_INCLUDED



/**
 * Abstract API for multi-source downloads.
 *
 * @author Jason Rohrer.
 */



extern int MULTISOURCE_DOWNLOAD_IN_PROGRESS;
extern int MULTISOURCE_DOWNLOAD_FAILED;
extern int MULTISOURCE_DOWNLOAD_CANCELED;



/**
 * Gets a file from multiple sources.
 *
 * @param inFileDescriptor abstract pointer to a file descriptor that can
 *   be used by inChunkGetter to identify a file.
 *   Must be destroyed by caller.
 * @param inFileSize the size of the file in bytes.
 * @param inChunkSize the size of a download chunk in bytes.
 * @param inNumSources the number of file sources.
 * @param inFileSources an array of abstract pointers to file sources that
 *   can be used by inChunkSize to connect to sources.
 *   Array and elements must be destroyed by caller.
 * @param inChunkGetter pointer to the function that can be used to get a
 *   chunk.
 *   The function must return an array of chunk bytes (or NULL on failure)
 *   and take the following arguments:
 *   ( void *inFileSource, void *inFileDescriptor,
 *     unsigned long inChunkNumber, unsigned long inChunkSize ).
 * @praram inDownloadProgressHandler pointer to the handler function for
 *   download progress events.
 *   This function must return true to continue the download (or false
 *   to cancel) and take the following arguments:
 *   (int inResultCode, unsigned long inTotalBytesReceived,
 *    void *inExtraArgument ).
 *   inResultCode will be set to one of MULTISOURCE_DOWNLOAD_IN_PROGRESS,
 *   MULTISOURCE_DOWNLOAD_FAILED, or MULTISOURCE_DOWNLOAD_CANCELED.
 *   Once a FAILED or CANCELED result code is passed to the handler, the
 *   handler will never be called again, so it should destroy its extra
 *   argument, if necessary.
 * @param inProgressHandlerExtraArgument pointer to an extra argument to be
 *   passed in to the handler function each time it is called.
 *   Must be destroyed by caller.
 * @param inDestinationPath the path to save the file to.
 *   Must be destroyed by caller.
 */
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
                         char *inDestinationPath );



#endif



