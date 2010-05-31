/* Josh Pieper, (c) 2000 */

/* This file is distributed under the GPL, see file COPYING for details */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef MCHECK
#include <mcheck.h>
#endif

#include "gnut_lib.h"
#include "gnut_threads.h"

int gnut_mprobe(void *d)
{
#ifdef MCHECK
	return mprobe(d);
#else
	return 0;
#endif
}

#ifndef HAVE_XMALLOC
#ifndef xmalloc
char *xmalloc(int size)
{
	char *ptr;
	ptr=malloc(size);
	if (!ptr) { g_debug(0,"malloc(%d) failed!\n", size); exit(1); }
	return ptr;
}
#endif
#endif

char *expand_path(char *a)
{
	char *b;
	if (strncmp(a,"~/", 2)==0) {
#ifndef WIN32
		char *c;
		c=getenv("HOME");
#else
		char c[MAX_PATH];
		GetWindowsDirectory(c, MAX_PATH);
#endif
		b=(char *)xmalloc(strlen(a)+strlen(c)+1);
		strcpy(b, c);
		strcat(b, &a[1]);
		return b;
	}
			   	   	   	   	   	    
	b=(char *) xmalloc(strlen(a)+1);
	strcpy(b,a);
	return b;   
}			   	   	   	   	   	                

int gnut_lib_debug=0;

#ifndef PTHREADS_DRAFT4
pthread_mutex_t _g_debug_mutex=PTHREAD_MUTEX_INITIALIZER;
#else
pthread_mutex_t _g_debug_mutex;
#endif

#ifndef WIN32
void _g_debug(char *file, int line, int num, char *format, ...)
{
   	va_list argp; 
	   	
	if (gnut_lib_debug>=num) {
		pthread_mutex_lock(&_g_debug_mutex);
		fprintf(stderr,"%i %s:%i > ",
			getpid(),file,line);

		fflush(stdout);
        va_start(argp,format);
        vfprintf(stderr,format,argp);
        va_end(argp);
        pthread_mutex_unlock(&_g_debug_mutex);
    }
}
#else
void g_debug(int a, char *format, ...)
{
	va_list argp;
	if (gnut_lib_debug>=a) {
		pthread_mutex_lock(&_g_debug_mutex);
		fprintf(stderr,"%li > ", (long) pthread_self());
		va_start(argp,format);
		vfprintf(stderr,format,argp);
		va_end(argp);
		pthread_mutex_unlock(&_g_debug_mutex);
	}
}
#endif

char *gnut_strdelimit(char *string, char *delim, char new_delim)
{
	char *c;
	
	for (c=string;*c;c++) {
		if (strchr(delim,*c)) *c=new_delim;
	}
	
	return string;
}


                       