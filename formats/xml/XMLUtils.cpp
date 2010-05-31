/*
 * Modification History
 *
 * 2002-September-12    Jason Rohrer
 * Created.
 */



#include "XMLUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"

#include <string.h>



char *XMLUtils::escapeDisallowedCharacters( char *inString ) {

    SimpleVector<char> *returnStringVector = new SimpleVector<char>();


    int stringLength = strlen( inString );

    int i;
    for( i=0; i<stringLength; i++ ) {

        switch( inString[i] ) {
            case '&':
                returnStringVector->push_back( '&' );
                returnStringVector->push_back( 'a' );
                returnStringVector->push_back( 'm' );
                returnStringVector->push_back( 'p' );
                returnStringVector->push_back( ';' );
                break;
            case '<':
                returnStringVector->push_back( '&' );
                returnStringVector->push_back( 'l' );
                returnStringVector->push_back( 't' );
                returnStringVector->push_back( ';' );
                break;
            case '>':
                returnStringVector->push_back( '&' );
                returnStringVector->push_back( 'g' );
                returnStringVector->push_back( 't' );
                returnStringVector->push_back( ';' );
                break;
            case '\"':
                returnStringVector->push_back( '&' );
                returnStringVector->push_back( 'q' );
                returnStringVector->push_back( 'u' );
                returnStringVector->push_back( 'o' );
                returnStringVector->push_back( 't' );
                returnStringVector->push_back( ';' );
                break;
            case '\'':
                returnStringVector->push_back( '&' );
                returnStringVector->push_back( 'a' );
                returnStringVector->push_back( 'p' );
                returnStringVector->push_back( 'o' );
                returnStringVector->push_back( 's' );
                returnStringVector->push_back( ';' );
                break;
            default:
                returnStringVector->push_back( inString[i] );
                break;
            }
        }

    int numChars = returnStringVector->size();
    char *returnString = new char[ numChars + 1 ];

    for( i=0; i<numChars; i++ ) {
        returnString[i] = *( returnStringVector->getElement( i ) );
        }

    returnString[ numChars ] = '\0';

    delete returnStringVector;

    return returnString;
    }
