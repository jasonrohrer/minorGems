/*
 * Modification History
 *
 * 2002-September-12    Jason Rohrer
 * Created.
 */



#ifndef HTML_UTILS_INCLUDED
#define HTML_UTILS_INCLUDED



/**
 * Utilities for processing HTML.
 *
 * @author Jason Rohrer
 */
class HTMLUtils {



    public:


        
        /**
         * Removes all HTML tags from an HTML string.
         *
         * @param the HTML data as a \0-terminated string.
         *   Must be destroyed by caller if non-const.
         *
         * @return data with all HTML tags removed as a  newly allocated
         *   \0-terminated string.
         *   Must be destroyed by caller.
         */
        static char *removeAllTags( char *inString );

        
        
    };



#endif
