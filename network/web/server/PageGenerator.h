/*
 * Modification History
 *
 * 2001-November-14   Jason Rohrer
 * Created.
 * Added a missing comment.
 *
 * 2002-March-28    Jason Rohrer
 * Added Fortify include to widely included header so
 * that fortify will be included almost everywhere.
 *
 * 2002-March-29    Jason Rohrer
 * Changed to reflect new location of Fortify.
 * Fixed include order.
 *
 * 2002-April-20    Jason Rohrer
 * Added a function for getting mime types.
 *
 * 2002-August-7    Jason Rohrer
 * Removed use of fortify.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */



#ifndef PAGE_GENERATOR_INCLUDED
#define PAGE_GENERATOR_INCLUDED



#include "minorGems/io/OutputStream.h"



/**
 * An interface for an HTML page generator.
 *
 * @author Jason Rohrer
 */
class PageGenerator {

    public:
        

        
        /**
         * Virtual destructor to ensure proper destruction of derived classes.
         */
        virtual ~PageGenerator();



        /**
         * Generates a page according to a get request path.
         *
         * @param inGetRequestPath the path specified
         *   by the get request.
         *   Must be destroyed by caller if non-const.
         * @param inOutputStream the stream to write the generated page to.
         *   Must be destroyed by caller.
         */
        virtual void generatePage( char *inGetRequestPath,
                                   OutputStream *inOutputStream ) = 0;
        


        /**
         * Gets the mime type associated with a particular request path.
         *
         *
         * @param inGetRequestPath the path specified
         *   by the get request.
         *   Must be destroyed by caller if non-const.
         *
         * @return the mime type as a \0-terminated string.
         *   Must be destroyed by caller.
         */
        virtual char *getMimeType( char *inGetRequestPath ) = 0;
        

        
    };



inline PageGenerator::~PageGenerator() {
    // does nothing
    }



#endif
