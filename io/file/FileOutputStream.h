/*
 * Modification History
 *
 * 2001-February-11		Jason Rohrer
 * Created.
 *
 * 2001-April-12   Jason Rohrer
 * Changed so that File is not destroyed when this stream is destroyed.
 *
 * 2001-April-29   Jason Rohrer
 * Fixed a bug in the use of fwrite
 * (num elements and element size swapped).
 * Fixed a memory leak in the error message handling.
 *
 * 2006-August-22   Jason Rohrer
 * Fixed include order bug.
 *
 * 2010-April-6   Jason Rohrer
 * Fixed memory leak.
 */

#include "minorGems/common.h"


#include "File.h"


#ifndef FILE_OUTPUT_STREAM_CLASS_INCLUDED
#define FILE_OUTPUT_STREAM_CLASS_INCLUDED

#include "minorGems/io/OutputStream.h"

#include <stdio.h>

/**
 * File implementation of an OutputStream.
 *
 * @author Jason Rohrer
 */ 
class FileOutputStream : public OutputStream {

	public:
		
		/**
		 * Constructs an output stream.
		 *
		 * @param inFile the file to open for writing.
		 *   If the file does not exist, it will be created.
		 *   inFile is NOT destroyed when this class is destroyed.
		 * @param inAppend set to true to append to file.  If
		 *   file does not exist, file is still created.  Defaults
		 *   to false.
		 */
		FileOutputStream( File *inFile, char inAppend = false );
		
		
		/**
		 * Destroys this stream and closes the file.
		 */
		~FileOutputStream();
		
		
		/**
		 * Gets the file attached to this stream.
		 *
		 * @return the file used by this stream.
		 *   Should not be modified or destroyed by caller until after
		 *   this class is destroyed.
		 */
		File *getFile();
		
		
		// implementst OutputStream interface
		virtual long write( unsigned char *inBuffer, long inNumBytes );
		
	private:
		File *mFile;
		
		FILE *mUnderlyingFile;
	};		




inline FileOutputStream::FileOutputStream( File *inFile, 
	char inAppend ) 
	: mFile( inFile ) {
	
	int fileNameLength;
	
	char *fileName = mFile->getFullFileName( &fileNameLength );
	
	if( inAppend ) {
		mUnderlyingFile = fopen( fileName, "ab" );
		}
	else {
		mUnderlyingFile = fopen( fileName, "wb" );
		}
		
	if( mUnderlyingFile == NULL ) {
		// file open failed.
		
		char *stringBuffer = new char[ fileNameLength + 50 ];
		sprintf( stringBuffer, "Opening file %s failed.", fileName );
		setNewLastError( stringBuffer );
		}
	
	delete [] fileName;	
	}
	


inline FileOutputStream::~FileOutputStream() {
	if( mUnderlyingFile != NULL ) {
		fclose( mUnderlyingFile );
		}	
	}


	
inline File *FileOutputStream::getFile() {
	return mFile;
	}
	
	
	
inline long FileOutputStream::write( 
	unsigned char *inBuffer, long inNumBytes ) {
	
	if( mUnderlyingFile != NULL ) {
	
		long numWritten =
			fwrite( inBuffer, 1, inNumBytes, mUnderlyingFile );

		if( numWritten < inNumBytes ) {
			int fileNameLength;
			char *fileName = mFile->getFullFileName( &fileNameLength );

			// some other kind of error occured
			char *stringBuffer = new char[ fileNameLength + 50 ];
			sprintf( stringBuffer, "Writing to file %s failed.", 
				fileName );
			setNewLastError( stringBuffer );

			delete [] fileName;
			
			if( numWritten == 0 ) {
				// a complete write failure
				return -1;
				}
			}

		return numWritten;
		}
	else {
		// file was not opened properly
		
		int fileNameLength;
		char *fileName = mFile->getFullFileName( &fileNameLength );
		char *stringBuffer = new char[ fileNameLength + 50 ];
		sprintf( stringBuffer, 
			"File %s was not opened properly before writing.", 
			fileName );
        
        delete [] fileName;

		setNewLastError( stringBuffer );
		
		return -1;
		}
	}

	
	
#endif
