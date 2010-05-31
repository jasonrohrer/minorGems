/*
 * Modification History
 *
 * 2001-February-19		Jason Rohrer
 * Created.
 *
 * 2001-February-20		Jason Rohrer
 * Added a missing return value. 
 */

#include "minorGems/common.h"



#ifndef PIPED_STREAM_CLASS_INCLUDED
#define PIPED_STREAM_CLASS_INCLUDED

#include "minorGems/io/InputStream.h"
#include "minorGems/io/OutputStream.h"

#include "minorGems/util/SimpleVector.h"

#include <string.h>

/**
 * An input/output stream that can server as a pipe between
 * two components that read from and write to streams.
 *
 * Buffered internally to prevent blocking, so is compatible
 * with non-threaded components.  Note, however, that
 * ever buffer written to the stream is copied internally, 
 * so max memory usage is doubled.
 *
 * IS NOT THREAD-SAFE!
 *
 * @author Jason Rohrer
 */ 
class PipedStream : public InputStream, public OutputStream {

	public:
		
		/**
		 * Constructs a PipedStream.
		 */
		PipedStream();
		
		/**
		 * Destroys any pending unread buffers.
		 */
		~PipedStream();
		
		
		
		
		// implements the InputStream interface
		long read( unsigned char *inBuffer, long inNumBytes );
		
		
		// implements the OutputStream interface
		long write( unsigned char *inBuffer, long inNumBytes );
		
		
	protected:
		SimpleVector<unsigned char *> *mBuffers;	
		SimpleVector<long> *mBufferSizes;
		
		long mCurrentBufferIndex;
		
	};		



inline PipedStream::PipedStream()
	: mBuffers( new SimpleVector<unsigned char*>() ),
	mBufferSizes( new SimpleVector<long>() ),
	mCurrentBufferIndex( 0 ) {
	
	}

		

inline PipedStream::~PipedStream() {
	int numRemaining = mBuffers->size();
	for( int i=0; i<numRemaining; i++ ) {
		unsigned char *buffer = *( mBuffers->getElement( i ) );
		
		delete [] buffer;
		}
	delete mBuffers;
	delete mBufferSizes;
	}
		
		

inline long PipedStream::read( unsigned char *inBuffer, long inNumBytes ) {
	if( mBuffers->size() == 0 ) {
		// none read, since no buffers available
		InputStream::
			setNewLastErrorConst( "No data available on piped stream read." );
		return 0;
		}
	else {
		
		unsigned char *currentBuffer = *( mBuffers->getElement( 0 ) );
		long currentBufferSize = *( mBufferSizes->getElement( 0 ) ); 

		long outputIndex = 0;

		while( outputIndex < inNumBytes ) {
			long bytesRemaining = inNumBytes - outputIndex;

			long bytesRemainingInCurrent = currentBufferSize - mCurrentBufferIndex;

			// if current buffer isn't big enough to fill output
			if( bytesRemaining >= bytesRemainingInCurrent ) {
				// copy all of current buffer into inBuffer
				memcpy( &( inBuffer[outputIndex] ), 
					&( currentBuffer[mCurrentBufferIndex] ), 
					bytesRemainingInCurrent );

				outputIndex += bytesRemainingInCurrent;

				// delete the current buffer
				mBuffers->deleteElement( 0 );
				mBufferSizes->deleteElement( 0 );
				delete [] currentBuffer;
				
				mCurrentBufferIndex = 0;
				
				if( outputIndex != inNumBytes && mBuffers->size() == 0 ) {
					// partial read, since no more buffers available
					InputStream::setNewLastErrorConst( 
						"Partial data available on piped stream read." );
					return outputIndex;
					}
				if( mBuffers->size() != 0 ) {
					// get the new current buffer 
					currentBuffer = *( mBuffers->getElement( 0 ) );
					currentBufferSize = *( mBufferSizes->getElement( 0 ) );
					}
				}
			else {
				// current buffer is bigger
				memcpy( &( inBuffer[outputIndex] ), 
					&( currentBuffer[mCurrentBufferIndex] ), 
					bytesRemaining );
				
				mCurrentBufferIndex += bytesRemaining;
				outputIndex += bytesRemaining;
				}

			} // end while
		
		// if we made it out of the while loop, we read all bytes
		return inNumBytes;
		
		} // end else
	}
	
		
		
inline long PipedStream::write( unsigned char *inBuffer, long inNumBytes ) {
	// add a copy of the buffer to the vector of buffers
	unsigned char *copy = new unsigned char[ inNumBytes ];
	
	memcpy( copy, inBuffer, inNumBytes );
	
	mBuffers->push_back( copy );
	mBufferSizes->push_back( inNumBytes );
	
	return inNumBytes;
	}
	
	
	
#endif
