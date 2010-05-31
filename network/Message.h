/*
 * Modification History
 *
 * 2001-January-15		Jason Rohrer
 * Created.
 *
 * 2001-February-3		Jason Rohrer
 * Updated serialization code to use new interfaces.  
 */

#include "minorGems/common.h"


#ifndef MESSAGE_CLASS_INCLUDED
#define MESSAGE_CLASS_INCLUDED

#include "minorGems/io/Serializable.h"

/**
 * A simple message that carries a 4 byte opcode.  Useful for sending a 
 * simple service request.
 *
 * @author Jason Rohrer
 */ 
class Message : public Serializable {

	public:
		
		long mOpcode;
		
		// implement the Serializable interface
		virtual int serialize( OutputStream *inOutputStream );
		virtual int deserialize( InputStream *inInputStream );		
	};		



inline int Message::serialize( OutputStream *inOutputStream ) {
	return inOutputStream->writeLong( mOpcode );
	}
	


inline int Message::deserialize( InputStream *inInputStream ) {
	return inInputStream->readLong( &mOpcode );
	}


#endif
