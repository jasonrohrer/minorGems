/*
 * Modification History
 *
 * 2001-January-10		Jason Rohrer
 * Created.
 * Added untility functions for converting integers to and from byte arrays.
 *
 * 2001-January-15		Jason Rohrer
 * Made utility functions static.
 * Fixed a major bug in the longToBytes function.
 *
 * 2001-February-3		Jason Rohrer
 * Removed the long io functions, which are now contained in the
 * input- and outputStream classes.
 *
 * 2005-November-21		Jason Rohrer
 * Fixed a warning by adding a virtual destructor. 
 */

#include "minorGems/common.h"


#ifndef SERIALIZABLE_CLASS_INCLUDED
#define SERIALIZABLE_CLASS_INCLUDED

#include "InputStream.h"
#include "OutputStream.h"

/**
 * Interface for an object that can be serialized to and deserialized
 * from a stream.
 *
 * Note that the deserialize function is meant to be called from an already
 * constructed object (to set object parameters using data read from the
 * stream), and is not a method of obtaining an object.
 *
 * All multi-byte data members should be encoded in using a big endian format.
 *
 * @author Jason Rohrer
 */ 
class Serializable {

	public:
		
		/**
		 * Writes this object out to a stream.
		 *
		 * @param inOutputStream the stream to write to.
		 *
		 * @return the number of bytes written successfully,
		 *   or -1 for a stream error.
		 */
		virtual int serialize( OutputStream *inOutputStream ) = 0;
		
		
		/**
		 * Reads this object in from a stream.
		 *
		 * @param inInputStream the stream to read from.
		 *
		 * @return the number of bytes read successfully,
		 *   or -1 for a stream error.
		 */
		virtual int deserialize( InputStream *inInputStream ) = 0;
		

        
        virtual ~Serializable();
        
	};		



inline Serializable::~Serializable() {
    // does nothing
    // exists to ensure that subclass destructors are called
    }
	


#endif
