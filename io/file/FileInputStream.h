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
 * Fixed a bug in the use of fread
 * (num elements and element size swapped).
 * Fixed a memory leak in the error message handling.
 *
 * 2006-November-23   Jason Rohrer
 * Fixed a memory leak in error message handling.
 *
 * 2010-April-22   Jason Rohrer
 * Added support for text mode.
 */

#include "minorGems/common.h"



#ifndef FILE_INPUT_STREAM_CLASS_INCLUDED
#define FILE_INPUT_STREAM_CLASS_INCLUDED

#include "minorGems/io/InputStream.h"
#include "File.h"

#include <stdio.h>

/**
 * File implementation of an InputStream.
 *
 * @author Jason Rohrer
 */ 
class FileInputStream : public InputStream {

	public:
		
		/**
		 * Constructs an input stream.
		 *
		 * @param inFile the file to open for reading.
		 *   If the file does not exist, all calls to read will fail.
		 *   inFile is NOT destroyed when this class is destroyed.
         * @param inTextMode true to open the file as text, false as binary.
         *   Defaults to false.
		 */
		FileInputStream( File *inFile, char inTextMode = false );
		
		
		/**
		 * Destroys this stream and closes the file.
		 */
		~FileInputStream();
		
		
		/**
		 * Gets the file attached to this stream.
		 *
		 * @return the file used by this stream.
		 *   Should not be modified or destroyed by caller until after
		 *   this class is destroyed.
		 */
		File *getFile();
		
		
		// implementst InputStream interface
		virtual long read( unsigned char *inBuffer, long inNumBytes );
		
	private:
		File *mFile;
		
		FILE *mUnderlyingFile;
	};		




inline FileInputStream::FileInputStream( File *inFile, char inTextMode ) 
	: mFile( inFile ) {
	
	int fileNameLength;
	
	char *fileName = mFile->getFullFileName( &fileNameLength );
	
    if( inTextMode ) {
        mUnderlyingFile = fopen( fileName, "r" );
        }
    else {
        mUnderlyingFile = fopen( fileName, "rb" );
        }
    
	
	if( mUnderlyingFile == NULL ) {
		// file open failed.
		
		char *stringBuffer = new char[ fileNameLength + 50 ];
		sprintf( stringBuffer, "Opening file %s failed.", fileName );
		setNewLastError( stringBuffer );
		}
		
	delete [] fileName;	
	}
	


inline FileInputStream::~FileInputStream() {
	if( mUnderlyingFile != NULL ) {
		fclose( mUnderlyingFile );
		}
	}


	
inline File *FileInputStream::getFile() {
	return mFile;
	}
	
	
	
inline long FileInputStream::read( 
	unsigned char *inBuffer, long inNumBytes ) {
	
	if( mUnderlyingFile != NULL ) {
	
		long numRead = fread( inBuffer, 1, inNumBytes, mUnderlyingFile );

		if( numRead < inNumBytes ) {

			int fileNameLength;
			char *fileName = mFile->getFullFileName( &fileNameLength );

			if( feof( mUnderlyingFile ) ) {
				// we reached the end of the file.
				char *stringBuffer = new char[ fileNameLength + 50 ];
				sprintf( stringBuffer, "Reached end of file %s on read.", 
					fileName );
				setNewLastError( stringBuffer );

				delete [] fileName;
				}
			else {
				// some other kind of error occured
				char *stringBuffer = new char[ fileNameLength + 50 ];
				sprintf( stringBuffer, "Reading from file %s failed.", 
					fileName );
				setNewLastError( stringBuffer );

				delete [] fileName;
				
				if( numRead == 0 ) {
					// a complete read failure
					return -1;
					}
				}
			}

		return numRead;
		}
	else {
		// file was not opened properly
		
		int fileNameLength;
		char *fileName = mFile->getFullFileName( &fileNameLength );
		char *stringBuffer = new char[ fileNameLength + 50 ];
		sprintf( stringBuffer, 
			"File %s was not opened properly before reading.", 
			fileName );
        delete [] fileName;
        
		setNewLastError( stringBuffer );
		
		return -1;
		}
	}

	
	
#endif
