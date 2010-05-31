/*
 * Modification History
 *
 * 2002-September-12    Jason Rohrer
 * Created.
 */



#ifndef XML_UTILS_INCLUDED
#define XML_UTILS_INCLUDED



/**
 * Utilities for processing XML.
 *
 * @author Jason Rohrer
 */
class XMLUtils {



    public:


        
        /**
         * Escapes characters disallowed in XML character data.
         *
         * @param the string to escape as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         *
         * @return string with characters escaped as a  newly allocated
         *   \0-terminated string.
         *   Must be destroyed by caller.
         */
        static char *escapeDisallowedCharacters( char *inString );

        
        
    };



#endif
