/*
 * Modification History
 *
 * 2003-August-12   Jason Rohrer
 * Created.
 *
 * 2003-August-22   Jason Rohrer
 * Added function for getting a token after reading.
 */



#ifndef P2P_PROTOCOL_UTILS_INCLUDED
#define P2P_PROTOCOL_UTILS_INCLUDED



#include "minorGems/io/InputStream.h"
#include "minorGems/io/OutputStream.h"



/**
 * A collection of common protocol processing functions.
 *
 * @author Jason Rohrer
 */



/**
 * Reads from a stream up to (and including) the
 * first occurence of a tag.
 *
 * @param inInputStream the stream to read from.
 *   Must be destroyed by caller.
 * @param inTag the tag to look for.
 *   Must be destroyed by caller if non-const.
 * @param inMaxCharsToRead the maximum number of characters to read.
 *
 * @return the read string, or NULL if reading fails.
 *   NULL is also returned if the character count limit is
 *   reached.
 *   Must be destroyed by caller if non-NULL.
 */
char *readStreamUpToTag( InputStream *inInputStream,
                         char *inTag, int inMaxCharsToRead );




/**
 * Reads from a stream up to (and including) the
 * first occurence of a tag and gets a specific token from the read data.
 *
 * Tokens are split by whitespace.
 *
 * @param inInputStream the stream to read from.
 *   Must be destroyed by caller.
 * @param inTag the tag to look for.
 *   Must be destroyed by caller if non-const.
 * @param inMaxCharsToRead the maximum number of characters to read.
 * @param inTokenNumber the token to get after reading up to inTag and
 *   tokenizing the read data.  0 specifies the first token.
 *
 * @return the read token string, or NULL if reading or token extraction fails.
 *   NULL is also returned if the character count limit is
 *   reached.
 *   Must be destroyed by caller if non-NULL.
 */
char *readStreamUpToTagAndGetToken( InputStream *inInputStream,
                                    char *inTag, int inMaxCharsToRead,
                                    int inTokenNumber );



#endif
