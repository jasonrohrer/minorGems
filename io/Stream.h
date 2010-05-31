/*
 * Modification History
 *
 * 2001-February-12		Jason Rohrer
 * Created. 
 *
 * 2002-March-29    Jason Rohrer
 * Added Fortify inclusion.
 *
 * 2002-March-31    Jason Rohrer
 * Made destructor virtual so it works with subclasses.
 * Fixed several bugs in deletion of mLastError.
 */

#include "minorGems/common.h"


#ifndef STREAM_CLASS_INCLUDED
#define STREAM_CLASS_INCLUDED


#include <string.h>



#ifdef FORTIFY
#include "minorGems/util/development/fortify/fortify.h"
#endif



/**
 * Base class for all streams.
 *
 * @author Jason Rohrer
 */ 
class Stream {

	public:
		
		/**
		 * Constructs a stream.
		 */
		Stream();
		
		virtual ~Stream();
		
		/**
		 * Gets the last error associated with this stream.
		 * Calling this function clears the error until 
		 * another error occurs.
		 *
		 * @return the last stream error in human-readable form.
		 *   Must be destroyed by caller.  Returns NULL if 
		 *   there is no error.  Note that this string is '\0' terminated.
		 */
		char *getLastError();

		
	protected:
		
		/**
		 * Called by subclasses to specify a new last error.  Useful
		 * when error doesn't contain information specifiable by
		 * a constant string.
		 *
		 * @param inString human-readable string representing the error.
		 *   Note that this string must be '\0' terminated.
         *   Will be destroyed by this class.
		 */
		void setNewLastError( char *inString );
		
		
		/**
		 * Called by subclasses to specify a new last error.  Useful
		 * when the error can be described by a constant string
		 *
		 * @param inString human-readable constant string representing 
		 *   the error.
		 *   Note that this string must be '\0' terminated.
		 */
		void setNewLastErrorConst( const char *inString ); 
		
		
	private:	
		// set to NULL when there is no error
		char *mLastError;	
	};		



inline Stream::Stream() 
	: mLastError( NULL ) {
	
	}



inline Stream::~Stream() {
	if( mLastError != NULL ) {
		delete [] mLastError;
		}
	}
	
	
	
inline char *Stream::getLastError() {
	
	char *returnString = mLastError;
	mLastError = NULL;
	
	return returnString;
	}



inline void Stream::setNewLastError( char *inString ) {
	if( mLastError != NULL ) {
		delete [] mLastError;
		}
	
	mLastError = inString;
	}
		
		

inline void Stream::setNewLastErrorConst( const char *inString ) {
	
	int length = 0;
	char lastChar = 'a';
	while( lastChar != '\0' ) {
		lastChar = inString[length];
		length++;
		}
	
	// add one more to length to accommodate '\0' terminination
	length++;
	
	if( mLastError != NULL ) {
		delete [] mLastError;
		}

	mLastError = new char[ length ];
	
	memcpy( mLastError, inString, length );	
	}



#endif
