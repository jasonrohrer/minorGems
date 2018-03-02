/*
 * Modification History
 *
 * 2002-May-10    Jason Rohrer
 * Created.
 *
 * 2002-May-11    Jason Rohrer
 * Added functions for hex encoding and decoding.
 *
 * 2002-May-12    Jason Rohrer
 * Added conversion for #, &, and ?.
 * Added conversion for CR and LF.
 *
 * 2002-August-1    Jason Rohrer
 * Added conversion for /, \, ., and :.
 *
 * 2002-September-12    Jason Rohrer
 * Added missing breaks.
 *
 * 2002-September-25    Jason Rohrer
 * Fixed a bug with the way + characters are handled.
 * Changed to trim the returned buffer.
 *
 * 2002-October-8    Jason Rohrer
 * Added functions for extracting query arguments.
 *
 * 2014-January-13    Jason Rohrer
 * Fixed hexEncode for URLs to work with actual reserved list.
 */



#include "URLUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/formats/encodingUtils.h"

#include <string.h>



char *URLUtils::getRootServer( char *inURL ) {

    char *urlCopy = stringDuplicate( inURL );
    
    char *endOfURLType = strstr( urlCopy, "://" );

    char *returnString = NULL;
    
    if( endOfURLType != NULL ) {
        char *startOfRootServer = &( endOfURLType[ 3 ] );

        char *endOfRootServer = strstr( startOfRootServer, "/" );
        if( endOfRootServer != NULL ) {
            endOfRootServer[0] = '\0';
            }

        returnString = stringDuplicate( startOfRootServer );

        }

    delete [] urlCopy;
    
    return returnString;
    }



char *URLUtils::getRootRelativePath( char *inURL ) {
    char *urlCopy = stringDuplicate( inURL );
    
    char *endOfURLType = strstr( urlCopy, "://" );

    char *returnString = NULL;
    
    if( endOfURLType != NULL ) {

        char *startOfRootServer = &( endOfURLType[ 3 ] );

        char *endOfRootServer = strstr( startOfRootServer, "/" );

        if( endOfRootServer == NULL ) {
            returnString = stringDuplicate( "/" );
            }
        else {
            char *lastSlash = endOfRootServer;
            char *currentSlash = strstr( &( lastSlash[1] ), "/" );

            while( currentSlash != NULL ) {

                lastSlash = currentSlash;
                currentSlash = strstr( &( lastSlash[1] ), "/" );
                }

            // terminate string right after last slash
            lastSlash[1] = '\0';
            returnString = stringDuplicate( endOfRootServer );
            }

        }
    
    delete [] urlCopy;
    
    return returnString;
    }



char *URLUtils::urlDecode( char *inString ) {

    // first run through string and replace any + characters with spaces

    char *workingString = stringDuplicate( inString );
    
    char *plusLocation = strstr( workingString, "+" );
    while( plusLocation != NULL ) {
        plusLocation[0] = ' ';
        plusLocation = strstr( plusLocation, "+" );
        }

    
	int stringLength = strlen( workingString );

	char *returnString = new char[ stringLength + 1 ];

	int stringIndex = 0;
	int returnStringIndex = 0;

	while( stringIndex < stringLength + 1 ) {

		if( workingString[ stringIndex ] != '%' ) {
			// not a hex representation
			returnString[ returnStringIndex ] = workingString[ stringIndex ];
			stringIndex++;
			returnStringIndex++;
			}
		else {
			// the start of hex
			char twoChars[2];
			twoChars[0] = workingString[ stringIndex + 1 ];
			twoChars[1]= workingString[ stringIndex + 2 ];

			char summedChar = 0;

			for( int i=0; i<2; i++ ) {
				int shiftAmount = 4 * ( 1 - i );
				switch( twoChars[i] ) {
					case '0':
						summedChar += 0x0 << shiftAmount;
						break;
					case '1':
						summedChar += 0x1 << shiftAmount;
						break;
					case '2':
						summedChar += 0x2 << shiftAmount;
						break;
					case '3':
						summedChar += 0x3 << shiftAmount;
						break;
					case '4':
						summedChar += 0x4 << shiftAmount;
						break;
					case '5':
						summedChar += 0x5 << shiftAmount;
						break;
					case '6':
						summedChar += 0x6 << shiftAmount;
						break;
					case '7':
						summedChar += 0x7 << shiftAmount;
						break;
					case '8':
						summedChar += 0x8 << shiftAmount;
						break;
					case '9':
						summedChar += 0x9 << shiftAmount;
						break;
					case 'A':
						summedChar += 0xA << shiftAmount;
						break;
					case 'B':
						summedChar += 0xB << shiftAmount;
						break;
					case 'C':
						summedChar += 0xC << shiftAmount;
						break;
					case 'D':
						summedChar += 0xD << shiftAmount;
						break;
					case 'E':
						summedChar += 0xE << shiftAmount;
						break;
					case 'F':
						summedChar += 0xF << shiftAmount;
						break;
					default:
						break;
					}

				}

			returnString[ returnStringIndex ] = summedChar;
			stringIndex += 3;
			returnStringIndex++;			
			}
		}
    
    delete [] workingString;

    // trim the return string
    workingString = returnString;
    returnString = stringDuplicate( workingString );
    delete [] workingString;

    
	return returnString;
	}



char *URLUtils::urlEncode( char *inString ) {
    
    /*
      Unreserved characters from RFC 3986
      
      Everything else must be percent-encoded
      
      A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S
      T  U  V  W  X  Y  Z
      a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s
      t  u  v  w  x  y  z
      0  1  2  3  4  5  6  7  8  9  -  _  .  ~
      
      To match application/x-www-form-urlencoded, we also encode ~ and
      encode spaces as '+'
    */


    SimpleVector<char> *returnStringVector = new SimpleVector<char>();


    int stringLength = strlen( inString );

    int i;
    for( i=0; i<stringLength; i++ ) {

        char c = inString[i];

        if( ( c >= 'A' && c <= 'Z' )
            ||
            ( c >= 'a' && c <= 'z' )
            ||
            ( c >= '0' && c <= '9' )
            ||
            c == '-'
            ||
            c == '_'
            ||
            c == '~'
            ||
            c == '.' ) {
            // unreserved
            returnStringVector->push_back( c );
            }
        else if( c == ' ' ) {
            // historical web form encoding of space as +
            returnStringVector->push_back( '+' );
            }
        else {
            // reserved to be replaced with percent-encoding
            returnStringVector->push_back( '%' );

            char *hexEncoding = hexEncode( (unsigned char *)( &c ), 1 );
            
            returnStringVector->appendElementString( hexEncoding );
            delete [] hexEncoding;
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



char *URLUtils::extractArgument( char *inHaystack,
                                 char *inArgName ) {

    char *argNameWithEquals = new char[ strlen( inArgName ) + 2 ];

    sprintf( argNameWithEquals, "%s%s",
             inArgName, "=" );

    char *haystackCopy = stringDuplicate( inHaystack );
    
    char *pointerToArgStart = strstr( haystackCopy, argNameWithEquals );
    
    if( pointerToArgStart == NULL ) {
        delete [] haystackCopy;
        delete [] argNameWithEquals;
        return NULL;
        }
    else {
        char *pointerToArgEnd = strstr( pointerToArgStart, "&" );
        if( pointerToArgEnd != NULL ) {
            // terminate string at arg end
            pointerToArgEnd[0] = '\0';
            }
        // else entire remainder of string is argument

        char *pointerToArgValue =
            &( pointerToArgStart[ strlen( argNameWithEquals ) ] );


        // trim string
        char *returnString = stringDuplicate( pointerToArgValue );

        
        delete [] argNameWithEquals;
        delete [] haystackCopy;
        return returnString;
        }
    
    }



char *URLUtils::extractArgumentRemoveHex( char *inHaystack,
                                          char *inArgName ) {
    char *extractedArg = extractArgument( inHaystack, inArgName );

    if( extractedArg != NULL ) {

        char *convertedArg = URLUtils::urlDecode( extractedArg );

        delete [] extractedArg;

        return convertedArg;
        }
    else {
        return NULL;
        }
    }



