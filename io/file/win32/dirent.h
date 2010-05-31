/*
 * Modification History
 *
 * 2002-April-7   Jason Rohrer
 * Added a mkdir wrapper for CodeWarrior.
 *
 * 2002-April-11   Jason Rohrer
 * Changed type of mode parameter to work with Visual C++.
 * Added missing macros.
 *
 * 2002-July-22   Jason Rohrer
 * Commented out mkdir replacement function to work with new MSL.
 *
 * 2002-October-13   Jason Rohrer
 * Re-added mkdir wrapper function, since both CW4 and VC++ need it.
 */

#include "minorGems/common.h"



/*

    Declaration of POSIX directory browsing functions and types for Win32.

    Kevlin Henney (mailto:kevlin@acm.org), March 1997.

    Copyright Kevlin Henney, 1997. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives, and that no charge may be made for the software and its
    documentation except to cover cost of distribution.
    
*/

#ifndef DIRENT_INCLUDED
#define DIRENT_INCLUDED

typedef struct DIR DIR;

struct dirent
{
    char *d_name;
};

DIR           *opendir(const char *);
int           closedir(DIR *);
struct dirent *readdir(DIR *);
void          rewinddir(DIR *);



#include <sys/stat.h>
/**
 * The Metrowerks Standard Library seems
 * to have only a 1-parameter mkdir command in sys/stat.h.
 */
int mkdir( const char *pathname, unsigned int mode );


// make sure our needed macros are defined
// S_IFMT and S_IFDIR seem to be defined everywhere

#ifndef __S_ISTYPE
#define __S_ISTYPE(mode, mask)  (((mode) & S_IFMT) == (mask))
#endif


#ifndef S_ISDIR
#define S_ISDIR(mode)    __S_ISTYPE((mode), S_IFDIR)
#endif    



#endif
