/*
 * Modification History
 *
 * 2002-May-25    Jason Rohrer
 * Created.
 * Changed to remove extra zeros when subtracting.
 */



#include "BigInt.h"
#include "minorGems/util/stringUtils.h"


#include <string.h>
#include <stdio.h>



BigInt::BigInt( int inSign, int inNumBytes, unsigned char *inBytes )
    : mSign( inSign ), mNumBytes( inNumBytes ),
      mBytes( new unsigned char[ inNumBytes ] ) {
          
    memcpy( mBytes, inBytes, mNumBytes );
    }



BigInt::BigInt( int inInt ) {
    if( inInt > 0 ) {
        mSign = 1;
        }
    else if( inInt == 0 ) {
        mSign = 0;
        }
    else {
        mSign = -1;

        // flip sign so conversion works
        inInt *= -1;
        }

    mNumBytes = 4;
    int bytesToSkip = 0;
    
    
    int baseFactor = 1;

    mBytes = new unsigned char[ 4 ];
    
    int i;
    for( i=mNumBytes-1; i>=mNumBytes-4; i-- ) {
        mBytes[i] = 0xFF & ( inInt / baseFactor );        
        
        baseFactor *= 256;        
        }

    if( mBytes[0] == 0 ) {
        bytesToSkip++;
        if( mBytes[1] == 0 ) {
            bytesToSkip++;
            if( mBytes[2] == 0 ) {
                bytesToSkip++;
                if( mBytes[3] == 0 ) {
                    bytesToSkip++;
                    }
                }
            }
        }

    mNumBytes -= bytesToSkip;

    unsigned char *temp = new unsigned char[ mNumBytes ];
    
    memcpy( temp, &( mBytes[ bytesToSkip ] ), mNumBytes );

    delete [] mBytes;

    mBytes = temp;
    }



BigInt::~BigInt() {
    delete [] mBytes;
    }



BigInt *BigInt::add( BigInt *inOtherInt ) {
    //char *stringA = convertToHexString();
    //char *stringB = inOtherInt->convertToHexString();
    //printf( "Adding %s to %s\n", stringA, stringB );
    //delete [] stringA;
    //delete [] stringB;
    
    if( mSign == 0 && inOtherInt->mSign == 0 ) {
        return getZero();
        }
    else if( mSign == 0 && inOtherInt->mSign != 0 ) {
        return inOtherInt->copy();
        }
    else if( mSign != 0 && inOtherInt->mSign == 0 ) {
        return copy();
        }
    else if( mSign == -1 && inOtherInt->mSign == -1 ) {
        // adding two negatives

        // add them as positives, then flip sign of result

        BigInt *copyA = copy();
        BigInt *copyB = inOtherInt->copy();

        copyA->mSign = 1;
        copyB->mSign = 1;

        BigInt *result = copyA->add( copyB );

        delete copyA;
        delete copyB;

        result->mSign = -1;
        
        return result;        
        }
    else if( mSign == 1 && inOtherInt->mSign == -1 ) {

        // flip other sign and subtract
        BigInt *copy = inOtherInt->copy();
        copy->mSign = 1;

        BigInt *result = subtract( copy );

        delete copy;

        return result;
        }
    else if( mSign == -1  && inOtherInt->mSign == 1 ) {
        // flip our sign and subtract
        BigInt *copy = this->copy();
        copy->mSign = 1;

        
        BigInt *result = inOtherInt->subtract( copy );

        delete copy;

        return result;        
        }
    else {
        // both this int and arg int are positive

        
        int maxLength = mNumBytes;
        if( maxLength < inOtherInt->mNumBytes ) {
            maxLength = inOtherInt->mNumBytes;
            }

        // leave room for carry
        unsigned char *result = new unsigned char[ maxLength + 1 ];

        // flip to little-endian order to make adding easier
        BigInt *intA = flipByteOrder();
        BigInt *intB = inOtherInt->flipByteOrder();
        
        int sum = 0;
        
        for( int i=0; i<maxLength; i++ ) {

            int byteA = 0;
            int byteB = 0;

            if( i < intA->mNumBytes ) {
                byteA = intA->mBytes[i];
                }
            if( i < intB->mNumBytes ) {
                byteB = intB->mBytes[i];
                }

            // add in high-order bits from last sum
            sum = byteA + byteB + ( sum >> 8 );

            // truncate to low-order bits
            result[i] = (unsigned char)sum;
            }

        // stick remaining carry into last byte
        result[maxLength] = (unsigned char)( sum >> 8 );

        BigInt *resultInt;
        
        if( result[maxLength] == 0 ) {
            // result array will be copied skipping carry byte 
            resultInt = new BigInt( 1, maxLength, result );
            }
        else {
            // keep carry byte
            resultInt = new BigInt( 1, maxLength + 1, result );
            }

        BigInt *flippedResult = resultInt->flipByteOrder();

        
        delete intA;
        delete intB;
        delete resultInt;
        delete [] result;

        
        return flippedResult;
        }

    }



BigInt *BigInt::subtract( BigInt *inOtherInt ) {
    //char *stringA = convertToHexString();
    //char *stringB = inOtherInt->convertToHexString();
    //printf( "Subtracting %s from %s\n", stringB, stringA );
    //delete [] stringA;
    //delete [] stringB;

    if( mSign == 0 && inOtherInt->mSign == 0 ) {
        return getZero();
        }
    else if( mSign != 0 && inOtherInt->mSign == 0 ) {
        return copy();
        }
    else if( mSign == 0 && inOtherInt->mSign != 0 ) {
        BigInt *copy = inOtherInt->copy();
        // flip sign
        copy->mSign = -( copy->mSign );
        return copy;
        }
    else if( isEqualTo( inOtherInt ) ) {
        return getZero();
        }
    else if( inOtherInt->mSign == -1 ) {
        // flip sign and add
        BigInt *copy = inOtherInt->copy();
        copy->mSign = 1;

        BigInt *result = add( copy );

        delete copy;
        
        return result;
        }
    else if( mSign == -1 ) {

        // flip our sign and add
        BigInt *copy = this->copy();
        copy->mSign = 1;

        BigInt *result = inOtherInt->add( copy );

        delete copy;

        // flip sign of result
        result->mSign = -1;

        return result;
        }
    else if( isLessThan( inOtherInt ) ) {
        // trying to subtract a larger number
        
        // flip subtraction order, then flip sign of result

        BigInt *result = inOtherInt->subtract( this );

        result->mSign = -( result->mSign );

        return result;
        }
    else {
        // we're subtracting a smaller positive number from
        // a larger positive number

        int maxLength = mNumBytes;
        if( maxLength < inOtherInt->mNumBytes ) {
            maxLength = inOtherInt->mNumBytes;
            }

        // result cannot be longer than the larger number
        unsigned char *result = new unsigned char[ maxLength ];

        // flip to little-endian order to make subtracting easier
        BigInt *intA = this->flipByteOrder();
        BigInt *intB = inOtherInt->flipByteOrder();
        
        
        int borrow = 0;

        for( int i=0; i<maxLength; i++ ) {

            int byteA = 0;
            int byteB = 0;

            if( i < intA->mNumBytes ) {
                byteA = intA->mBytes[i];
                }
            if( i < intB->mNumBytes ) {
                byteB = intB->mBytes[i];
                }
            
            int diff = byteA - byteB - borrow;

            // compute next borrow
            borrow = 0;
            while( diff < 0 ) {
                borrow++;

                diff += 256;
                }
            result[i] = (unsigned char)diff;
            }

        BigInt *resultInt;
        
        if( borrow == 0 ) {

            // skip any trailing 0 bytes
            int bytesToSkip = 0;
            int index = maxLength - 1;
            while( index >= 0 && result[index] == 0 ) {
                bytesToSkip ++;
                index--;
                }

            // result array will be copied skipping borrow byte
            // and skipping any trailing 0 bytes
            resultInt = new BigInt( true, maxLength - bytesToSkip, result );
            }
        else {
            printf( "Error:  final borrow not zero:  %d\n", borrow );
            resultInt = getZero();
            }

        BigInt *flippedResult = resultInt->flipByteOrder();

        
        delete intA;
        delete intB;
        delete resultInt;
        delete [] result;

        
        return flippedResult;
        }
    }



char BigInt::isLessThan( BigInt *inOtherInt ) {
    if( mSign > inOtherInt->mSign ) {
        return false;
        }
    else if( mSign < inOtherInt->mSign ) {
        return true;
        }
    else {
        // signs are equal

        if( mSign == 0 ) {
            return false;
            }
        else if( mSign == -1 && mNumBytes > inOtherInt->mNumBytes ) {
            return true;
            }
        else if( mSign == 1 && mNumBytes > inOtherInt->mNumBytes ) {
            return false;
            }
        else if( mSign == 1 && mNumBytes < inOtherInt->mNumBytes ) {
            return true;
            }
        else if( mSign == -1 && mNumBytes < inOtherInt->mNumBytes ) {
            return false;
            }
        else if( isEqualTo( inOtherInt ) ) {
            return false;
            }
        else {
            // numbers are the same length but are not equal

            // start with high-order bytes
            for( int i=0; i<mNumBytes; i++ ) {
                if( mBytes[i] > inOtherInt->mBytes[i] ) {
                    if( mSign == 1 ) {
                        return false;
                        }
                    else {
                        return true;
                        }
                    }
                else if ( mBytes[i] < inOtherInt->mBytes[i] ) {
                    if( mSign == 1 ) {
                        return true;
                        }
                    else {
                        return false;
                        }
                    }                
                }

            // if we made it here, the numbers are equal...
            // but we tested for equality above:  must be an error
            printf( "Error:  equality test in isLessThan failed\n" );

            return false;
            }
        
        }
    }



char BigInt::isEqualTo( BigInt *inOtherInt ) {

    if( mSign != inOtherInt->mSign ) {
        return false;
        }
    else if( mNumBytes != inOtherInt->mNumBytes ) {
        return false;
        }
    else {
        // same length, same sign
        
        for( int i=0; i<mNumBytes; i++ ) {
            if( mBytes[i] != inOtherInt->mBytes[i] ) {
                // mismatch found
                return false;
                }
            
            }

        // no mismatch found
        return true;
        }

    }



BigInt *BigInt::copy() {
    return new BigInt( mSign, mNumBytes, mBytes );
    }



BigInt *BigInt::getZero() {
    unsigned char bytes[0];
    
    return new BigInt( 0, 0, bytes );
    }


/*
  THIS DOES NOT WORK!
char *BigInt::convertToDecimalString() {

    char *tempString = new char[4];

    
    char *resultString = new char[ mNumBytes * 3 + 1 ];

    int stringIndex = 0;

    for( int i=0; i<mNumBytes; i++ ) {

        unsigned char currentByte = mBytes[i];

        // pad with zeros
        
        char *firstZero = "";
        char *secondZero = "";            
        
        if( currentByte < 100 ) {
            firstZero = "0";
            
            if( currentByte < 10 ) {
                secondZero = "0";
                }
            }
        
        sprintf( tempString, "%s%s%d", firstZero, secondZero, currentByte );

        memcpy( &( resultString[ stringIndex ] ), tempString, 3 );        

        stringIndex += 3;
        }

    resultString[ mNumBytes * 3 ] = '\0';
    
    delete [] tempString;

    return resultString;
    }
*/



char *BigInt::convertToHexString() {
    if( mSign == 0 ) {
        return stringDuplicate( "0" );
        }
    else {
        char *resultHexString = new char[ mNumBytes * 2 + 1 + 1 ];
        int hexStringIndex = 0;

        if( mSign == -1 ) {
            resultHexString[0] = '-';
            hexStringIndex++;
            }
    
        for( int i=0; i<mNumBytes; i++ ) {

            unsigned char currentByte = mBytes[ i ];

            int highBits = 0xF & ( currentByte >> 4 );
            int lowBits = 0xF & ( currentByte );

            resultHexString[ hexStringIndex ] = fourBitIntToHex( highBits );
            hexStringIndex++;

            resultHexString[ hexStringIndex ] = fourBitIntToHex( lowBits );
            hexStringIndex++;
            }

        resultHexString[ hexStringIndex ] = '\0';
    
        return resultHexString;
        }
    }



int BigInt::convertToInt() {

    int result = 0;

    int baseFactor = 1;
    
    for( int i=mNumBytes-1; i>=mNumBytes-4; i-- ) {

        if( i >=0 ) {
            result += mBytes[i] * baseFactor;            
            }
        
        baseFactor *= 256;        
        }

    return mSign * result;    
    }



BigInt *BigInt::flipByteOrder() {
    BigInt *returnInt = copy();

    unsigned char *newBytes = returnInt->mBytes;

    int newIndex = mNumBytes - 1;
    
    for( int i=0; i<mNumBytes; i++ ) {
        newBytes[ newIndex ] = mBytes[ i ];

        newIndex--;
        }

    return returnInt;
    }



char BigInt::fourBitIntToHex( int inInt ) {
    char outChar[2];

    if( inInt < 10 ) {
        sprintf( outChar, "%d", inInt );
        }
    else {
        switch( inInt ) {
            case 10:
                outChar[0] = 'A';
                break;
            case 11:
                outChar[0] = 'B';
                break;
            case 12:
                outChar[0] = 'C';
                break;
            case 13:
                outChar[0] = 'D';
                break;
            case 14:
                outChar[0] = 'E';
                break;
            case 15:
                outChar[0] = 'F';
                break;
            default:
                outChar[0] = '0';
                break;
            }
        }

    return outChar[0];
    }

