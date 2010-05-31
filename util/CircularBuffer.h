/*
 * Modification History
 *
 * 2001-Janary-11	Jason Rohrer
 * Created.
 *
 * 2001-Janary-12	Jason Rohrer
 * Added canRead and canWrite functions.  
 *
 * 2001-February-24		Jason Rohrer
 * Fixed incorrect delete usage.  
 */

#include "minorGems/common.h"


 
#ifndef CIRCULAR_BUFFER_INCLUDED
#define CIRCULAR_BUFFER_INCLUDED 


#include "minorGems/system/Semaphore.h"


/**
 * Thread-safe circular buffer.
 *
 * @author Jason Rohrer 
 */
class CircularBuffer { 
	
	public:
		
		/**
		 * Constructs a CircularBuffer.
		 *
		 * @param inSize the number of objects in this buffer.
		 */
		CircularBuffer( unsigned long inSize );
		
		
		~CircularBuffer();
		
		
		/**
		 * Writes an object into the next free position in the buffer.
		 * Blocks if no free positions are available.
		 *
		 * @param inObject the object pointer to write.
		 */
		void writeObject( void *inObject );
		
		
		/**
		 * Reads the next available object from the buffer.
		 * Blocks if now objects are available.
		 *
		 * @return the object pointer read.
		 */
		void *readNextObject();
		
		
		/**
		 * Returns true if an object can be read from this buffer
		 * without blocking.
		 */
		char canRead();
		
		
		/**
		 * Returns true if an object can be written to this buffer
		 * without blocking.
		 */
		char canWrite();
		
		
	private:
		unsigned long mBufferSize;
		
		void **mObjects;
		
		unsigned long mReadIndex, mWriteIndex;
		
		// initialized to 0
		Semaphore *mReadSemaphore;
		
		// initialized to mBufferSize;
		Semaphore *mWriteSemaphore;
		
		MutexLock *mLock;
	};



inline CircularBuffer::CircularBuffer( unsigned long inSize ) 
	: mBufferSize( inSize ), mObjects( new void*[inSize] ),
	mReadIndex( 0 ), mWriteIndex( 0 ), 
	mReadSemaphore( new Semaphore( 0 ) ), 
	mWriteSemaphore( new Semaphore( inSize ) ),
	mLock( new MutexLock() ) {
	
	}
		

		
inline CircularBuffer::~CircularBuffer() {
	delete [] mObjects;
	delete mReadSemaphore;
	delete mWriteSemaphore;
	delete mLock;
	}



// note that in this implementation, no mutex is needed, since
// reader and writer are never accessing the same data members
// simultaneously

inline void CircularBuffer::writeObject( void *inObject ) {
	// wait to for a space to write into
	mWriteSemaphore->wait();
	
	// write, and increment our write location
	mObjects[ mWriteIndex ] = inObject;
	mWriteIndex++;
	mWriteIndex = mWriteIndex % mBufferSize;
	
	// signal the reader that an new object is ready
	mReadSemaphore->signal();
	}


inline void *CircularBuffer::readNextObject() {
	void *returnObject;
	
	// wait for an object to read
	mReadSemaphore->wait();
	
	// read the object, and increment our read location
	returnObject = mObjects[ mReadIndex ];
	mReadIndex++;
	mReadIndex = mReadIndex % mBufferSize;
	
	// signal the writer
	mWriteSemaphore->signal();
	
	// now return the object that we retrieved from the buffer
	return returnObject;
	}



inline char CircularBuffer::canRead() {
	// can read if the read semaphore won't block
	return ! mReadSemaphore->willBlock();
	}


		
inline char CircularBuffer::canWrite() {
	// can write if the write semaphore won't block
	return ! mWriteSemaphore->willBlock();
	}



#endif
