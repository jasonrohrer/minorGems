/*
 * Modification History
 *
 * 2002-August-1  Jason Rohrer
 * Created.
 *
 * 2004-May-9  Jason Rohrer
 * Added function for getting data as a byte array.
 */



#include "minorGems/util/StringBufferOutputStream.h"



StringBufferOutputStream::StringBufferOutputStream()
    : mCharacterVector( new SimpleVector<unsigned char>() ) {

    }



StringBufferOutputStream::~StringBufferOutputStream() {

    delete mCharacterVector;
    }



char *StringBufferOutputStream::getString() {

    int numChars = mCharacterVector->size();

    char *returnArray = new char[ numChars + 1 ];

    for( int i=0; i<numChars; i++ ) {
        returnArray[i] = (char)( *( mCharacterVector->getElement( i ) ) );
        }
    returnArray[ numChars ] = '\0';

    return returnArray;
    }



unsigned char *StringBufferOutputStream::getBytes( int *outNumBytes ) {
    *outNumBytes = mCharacterVector->size();

    return mCharacterVector->getElementArray();
    }



long StringBufferOutputStream::write( unsigned char *inBuffer,
                                      long inNumBytes ) {

    for( int i=0; i<inNumBytes; i++ ) {
        mCharacterVector->push_back( inBuffer[ i ] );
        }
    
    return inNumBytes;
    }

