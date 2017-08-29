/*
 * Modification History
 *
 * 2003-August-22   Jason Rohrer
 * Created.
 *
 * 2003-September-22   Jason Rohrer
 * Added base64 encoding.
 *
 * 2004-March-21   Jason Rohrer
 * Fixed a variable scoping and redefinition bug pointed out by Benjamin Meyer.
 */


#include "encodingUtils.h"


#include "minorGems/util/SimpleVector.h"


#include <stdio.h>
#include <string.h>



char fourBitIntToHex( int inInt ) {
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



// returns -1 if inHex is not a valid hex character
int hexToFourBitInt( char inHex ) {
    int returnInt;

    switch( inHex ) {
        case '0':
            returnInt = 0;
            break;
        case '1':
            returnInt = 1;
            break;
        case '2':
            returnInt = 2;
            break;
        case '3':
            returnInt = 3;
            break;
        case '4':
            returnInt = 4;
            break;
        case '5':
            returnInt = 5;
            break;
        case '6':
            returnInt = 6;
            break;
        case '7':
            returnInt = 7;
            break;
        case '8':
            returnInt = 8;
            break;
        case '9':
            returnInt = 9;
            break;
        case 'A':
        case 'a':
            returnInt = 10;
            break;
        case 'B':
        case 'b':
            returnInt = 11;
            break;
        case 'C':
        case 'c':
            returnInt = 12;
            break;
        case 'D':
        case 'd':
            returnInt = 13;
            break;
        case 'E':
        case 'e':
            returnInt = 14;
            break;
        case 'F':
        case 'f':
            returnInt = 15;
            break;
        default:
            returnInt = -1;
            break;
        }

    return returnInt;
    }



char *hexEncode( unsigned char *inData, int inDataLength ) {

    char *resultHexString = new char[ inDataLength * 2 + 1 ];
    int hexStringIndex = 0;
    
    for( int i=0; i<inDataLength; i++ ) {

        unsigned char currentByte = inData[ i ];

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



unsigned char *hexDecode( char *inHexString ) {

    int hexLength = strlen( inHexString );
    
    if( hexLength % 2 != 0 ) {
        // hex strings must be even in length
        return NULL;
        }

    int dataLength = hexLength / 2;
    
    unsigned char *rawData = new unsigned char[ dataLength ];


    for( int i=0; i<dataLength; i++ ) {

        int highBits = hexToFourBitInt( inHexString[ 2 * i ] );
        int lowBits = hexToFourBitInt( inHexString[ 2 * i + 1 ] );

        if( highBits == -1 || lowBits == -1 ) {
            delete [] rawData;
            return NULL;
            }
        
        rawData[i] = (unsigned char)( highBits << 4 | lowBits );
        }

    return rawData;
    }



/*
 * These tables were taken from the GNU Privacy Guard source code.
 *
 * Wow... writing base64 functions would have been much more difficult
 * without these tables, especially the reverse table.
 */



// The base-64 character list
// Maps base64 binary numbers to ascii characters
static const char *binaryToAscii =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// The reverse base-64 list
// Maps ascii characters to base64 binary numbers
static unsigned char asciiToBinary[256] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff };


char *base64Encode( unsigned char *inData, int inDataLength,
                    char inBreakLines ) {

    SimpleVector<char> *encodingVector = new SimpleVector<char>();

    int numInLine = 0;
    
    // take groups of 3 data bytes and map them to 4 base64 digits
    for( int i=0; i<inDataLength; i=i+3 ) {

        if( i+2 < inDataLength ) {
            // not at end yet

            unsigned int block =
                inData[i]   << 16 |
                inData[i+1] << 8 |
                inData[i+2];

            // base64 digits, with digitA at left
            unsigned int digitA = 0x3F & ( block >> 18 );
            unsigned int digitB = 0x3F & ( block >> 12 );
            unsigned int digitC = 0x3F & ( block >> 6 );
            unsigned int digitD = 0x3F & ( block );

            encodingVector->push_back( binaryToAscii[ digitA ] );
            encodingVector->push_back( binaryToAscii[ digitB ] );
            encodingVector->push_back( binaryToAscii[ digitC ] );
            encodingVector->push_back( binaryToAscii[ digitD ] );
            numInLine += 4;

            if( inBreakLines && numInLine == 76 ) {
                // break the line
                encodingVector->push_back( '\r' );
                encodingVector->push_back( '\n' );
                numInLine = 0;
                }
            
            }
        else {
            // at end
            int numLeft = inDataLength - i;

            switch( numLeft ) {
                case 0:
                    // no padding
                    break;
                case 1: {
                    // two digits, two pads
                    unsigned int block =
                        inData[i]   << 16 |
                        0;
                    unsigned int digitA = 0x3F & ( block >> 18 );
                    unsigned int digitB = 0x3F & ( block >> 12 );
            
                    encodingVector->push_back( binaryToAscii[ digitA ] );
                    encodingVector->push_back( binaryToAscii[ digitB ] );

                    encodingVector->push_back( '=' );
                    encodingVector->push_back( '=' );
                    break;
                    }
                case 2: {
                    // three digits, one pad
                    unsigned int block =
                        inData[i]   << 16 |
                        inData[i+1] << 8 |
                        0;

                    // base64 digits, with digitA at left
                    unsigned int digitA = 0x3F & ( block >> 18 );
                    unsigned int digitB = 0x3F & ( block >> 12 );
                    unsigned int digitC = 0x3F & ( block >> 6 );
                    
                    encodingVector->push_back( binaryToAscii[ digitA ] );
                    encodingVector->push_back( binaryToAscii[ digitB ] );
                    encodingVector->push_back( binaryToAscii[ digitC ] );
                    encodingVector->push_back( '=' );
                    break;
                    }
                default:
                    break;
                }
            // done with all data
            i = inDataLength;
            }
        }

    char *returnString = encodingVector->getElementString();

    delete encodingVector;

    return returnString;
    }



unsigned char *base64Decode( char *inBase64String,
                             int *outDataLength ) {

    SimpleVector<unsigned char> *decodedVector =
        new SimpleVector<unsigned char>();


    
    int encodingLength = strlen( inBase64String );

    SimpleVector<unsigned char> *binaryEncodingVector =
        new SimpleVector<unsigned char>();

    int i;
    for( i=0; i<encodingLength; i++ ) {
        unsigned char currentChar = (unsigned char)( inBase64String[i] );

        unsigned char currentBinary = asciiToBinary[ currentChar ]; 
        
        if( currentBinary != 0xFF ) {
            // in range
            binaryEncodingVector->push_back( currentBinary );
            }
        }

    int binaryEncodingLength = binaryEncodingVector->size();

    unsigned char *binaryEncoding = binaryEncodingVector->getElementArray();
    delete binaryEncodingVector;

    int blockCount = binaryEncodingLength / 4;

    if( binaryEncodingLength % 4 != 0 ) {
        // extra, 0-padded block
        blockCount += 1;
        }



    // take groups of 4 encoded digits and map them to 3 data bytes
    for( i=0; i<binaryEncodingLength; i=i+4 ) {

        if( i+3 < binaryEncodingLength ) {
            // not at end yet

            unsigned int block =
                binaryEncoding[i]   << 18 |
                binaryEncoding[i+1] << 12 |
                binaryEncoding[i+2] << 6 |
                binaryEncoding[i+3];

            // data byte digits, with digitA at left
            unsigned int digitA = 0xFF & ( block >> 16 );
            unsigned int digitB = 0xFF & ( block >> 8 );
            unsigned int digitC = 0xFF & ( block );
            
            decodedVector->push_back( digitA );
            decodedVector->push_back( digitB );
            decodedVector->push_back( digitC );            
            }
        else {
            // at end
            int numLeft = binaryEncodingLength - i;

            switch( numLeft ) {
                case 0:
                    // no padding
                    break;
                case 1: {
                    // impossible
                    break;
                    }
                case 2: {
                    // two base64 digits, one data byte
                    unsigned int block =
                        binaryEncoding[i]   << 18 |
                        binaryEncoding[i+1] << 12 |
                        0;
                    
                    // data byte digits, with digitA at left
                    unsigned int digitA = 0xFF & ( block >> 16 );
                    
                    decodedVector->push_back( digitA );
                    break;
                    }
                case 3: {
                    // three base64 digits, two data bytes
                    unsigned int block =
                        binaryEncoding[i]   << 18 |
                        binaryEncoding[i+1] << 12 |
                        binaryEncoding[i+2] << 6 |
                        0;

                    // data byte digits, with digitA at left
                    unsigned int digitA = 0xFF & ( block >> 16 );
                    unsigned int digitB = 0xFF & ( block >> 8 );
                    
            
                    decodedVector->push_back( digitA );
                    decodedVector->push_back( digitB );
                    break;
                    }
                default:
                    break;
                }
            // done with all data
            i = binaryEncodingLength;
            }
        }    

    delete [] binaryEncoding;
    

    *outDataLength = decodedVector->size();
    unsigned char* returnData = decodedVector->getElementArray();

    delete decodedVector;

    return returnData;
    }




#include "miniz.h"
#include "miniz.c"


unsigned char *zipCompress( unsigned char *inData, int inDataLength,
                            int *outCompressedDataLength ) {
    
    int maxCompLength = compressBound( inDataLength );
    
    unsigned char *compressBuffer = new unsigned char[ maxCompLength ];
    
    mz_ulong compLength = maxCompLength;
    
    int cmp_status = compress( compressBuffer, &compLength, 
                               inData, inDataLength );
    if( cmp_status != Z_OK ) {
        printf( "zipCompress failed\n" );
        delete [] compressBuffer;
        return NULL;
        }
    
    printf( "Compressed %d bytes down to %d\n", 
            inDataLength, (int)compLength );
    
    unsigned char *shortBuffer = new unsigned char[ compLength ];
    
    memcpy( shortBuffer, compressBuffer, compLength );
    delete [] compressBuffer;
    
    *outCompressedDataLength = compLength;
    
    return shortBuffer;
    }



unsigned char *zipDecompress( unsigned char *inCompressedData, 
                              int inCompressedDataLength,
                              int inExpectedResultDataLength ) {

    unsigned char *dataBuffer = 
        new unsigned char[ inExpectedResultDataLength ];
    
    mz_ulong actualDataLength = inExpectedResultDataLength;

    int cmp_status = uncompress( dataBuffer, &actualDataLength, 
                                 inCompressedData, inCompressedDataLength );

    if( cmp_status != Z_OK ) {
        printf( "zipDecompress failed\n" );
        delete [] dataBuffer;
        return NULL;
        }
    if( (int)actualDataLength != inExpectedResultDataLength ) {
        printf( "zipDecompress expecting %d result bytes, got %d\n",
                inExpectedResultDataLength, (int)actualDataLength );
        delete [] dataBuffer;
        return NULL;
        }

    return dataBuffer;
    }





 
