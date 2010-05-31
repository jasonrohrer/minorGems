/*
 * Modification History
 *
 * 2003-August-22   Jason Rohrer
 * Created.
 *
 * 2003-September-22   Jason Rohrer
 * Added base64 encoding.
 */



#ifndef ENCODING_UTILS_INCLUDED
#define ENCODING_UTILS_INCLUDED



/**
 * A collection of functions for representing data in various encoding formats.
 *
 * @author Jason Rohrer
 */



/**
 * Encodes data as a ASCII hexidecimal string.
 *
 * @param inData the data to encode.
 *   Must be destroyed by caller.
 * @param inDataLength the length of inData in bytes.
 *
 * @return a \0-terminated ASCII hexidecimal string containing
 *   characters in the range [0-9] and [A-F].
 *   Will be twice as long as inData.
 *   Must be destroyed by caller.
 */
char *hexEncode( unsigned char *inData, int inDataLength );



/**
 * Decodes raw data from an ASCII hexidecimal string.
 *
 * @param inHexString a \0-terminated hexidecimal string.
 *   Must be destroyed by caller.
 *
 * @return an array of raw data, or NULL if decoding fails.
 *   Will be half as long as inHexString.
 *   Must be destroyed by caller if non-NULL.
 */
unsigned char *hexDecode( char *inHexString );




/**
 * Encodes data as a ASCII base64 string.
 *
 * @param inData the data to encode.
 *   Must be destroyed by caller.
 * @param inDataLength the length of inData in bytes.
 * @param inBreakLines set to true to break lines every 76 characters,
 *   or false to produce an unbroken base64 string.
 *
 * @return a \0-terminated ASCII base64 string containing
 *   characters in the range [0-9], [A-Z], [a-z], and [+,/,=].
 *   Must be destroyed by caller.
 */
char *base64Encode( unsigned char *inData, int inDataLength,
                    char inBreakLines = true );



/**
 * Decodes raw data from an ASCII base64 string.
 *
 * @param inBase64String a \0-terminated base64 string.  Can optionally contain
 *   linebreaks.
 *   Must be destroyed by caller.
 * @param outDataLength pointer to where the length of the decoded data
 *   should be returned.
 *
 * @return an array of raw data, or NULL if decoding fails.
 *   Must be destroyed by caller if non-NULL.
 */
unsigned char *base64Decode( char *inBase64String,
                             int *outDataLength );


 
#endif
