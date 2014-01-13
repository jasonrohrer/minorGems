/*
 * Modification History
 *
 * 2002-May-10    Jason Rohrer
 * Created.
 *
 * 2002-May-11    Jason Rohrer
 * Added functions for hex encoding and decoding.
 *
 * 2002-October-8    Jason Rohrer
 * Added functions for extracting query arguments.
 *
 * 2014-January-13    Jason Rohrer
 * Changed url-encoding function names to not conflict with hexEncode/decode.
 */

#include "minorGems/common.h"


#ifndef URL_UTILS_INCLUDED
#define URL_UTILS_INCLUDED



/**
 * Utilities for handling URLS.
 *
 * @author Jason Rohrer
 */
class URLUtils {



    public:

        

        /**
         * Gets the root server from a URL.
         *
         * For example, if the URL is http://www.yahoo.com/test.html
         * the root server is www.yahoo.com
         *
         * @param inURL the URL as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         *
         * @return the root server as a  newly allocated
         *   \0-terminated string.
         *   Must be destroyed by caller.
         */
        static char *getRootServer( char *inURL );

        

        /**
         * Gets the root-relative path from a URL.
         *
         * For example, if the URL is http://www.yahoo.com/temp/test/test.html
         * the root-relative path is /temp/test/
         *
         * @param inURL the URL as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         *
         * @return the root-relative path as a newly allocated
         *   \0-terminated string.
         *   Must be destroyed by caller.
         */
        static char *getRootRelativePath( char *inURL );



        /**
         * Removes explicit hex encoding from a string.
         *
         * For example
         *    http%3A%2F%2Fservera.com%2Ftesta.html
         * would be converted to
         *    http://servera.com/testa.html
         *
         * @param inString the string to convert in \0-terminated form.
         *    Must be destroyed by caller if non-const.
         *
         * @return a newly allocated converted string in \0-terminated form.
         *    Must be destroyed by caller.
         */
        static char *urlDecode( char *inString );



        /**
         * Encodes a string in a browser-safe hex encoding
         * (including adding '+' for each space).
         *
         * @param inString the string to encode.
         *   Must be destroyed by caller.
         *
         * @return an encoded string.  Must be destroyed by caller.
         */
        static char *urlEncode( char *inString );



        /**
         * Extracts the value from an argument of the form:
         * name=value&   or
         * name=value[string_end]
         *
         * Note that if name is the suffix of an argument with a longer name
         * the longer-named argument's value may be returned.  Thus,
         * argument names should not be suffixes of eachother.
         *
         * All parameters must be destroyed by caller.
         *
         * @param inHaystack the string to extract the argument from.
         * @param inArgName the argument name (without ?, &, or =) to search
         *   for in inHaystack.
         *
         * @return the value of the argument, or NULL if the argument is
         *   not found.  Must be destroyed by caller if non-NULL.
         */
        static char *extractArgument( char *inHaystack,
                                      char *inArgName );



        /**
         * The same as extractArgument, except that explicit
         * hex representations are translated to plain ascii
         * before the argument value is returned.
         */
        static char *extractArgumentRemoveHex( char *inHaystack,
                                               char *inArgName );

        
        
    };



#endif
