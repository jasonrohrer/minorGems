/*
 * Modification History
 *
 * 2002-September-12    Jason Rohrer
 * Created.
 */



#include "HTMLUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"

#include <string.h>



char *HTMLUtils::removeAllTags( char *inString ) {

    SimpleVector<char> *returnStringVector = new SimpleVector<char>();


    int stringLength = strlen( inString );

    int i = 0;

    while( i < stringLength ) {
        if( inString[i] == '<' ) {
            // the start of a tag

            // skip all until (and including) close of tag
            while( i < stringLength && inString[i] != '>' ) {
                // do nothing
                i++;
                }
            }
        else {
            returnStringVector->push_back( inString[i] );
            }
        i++;
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
