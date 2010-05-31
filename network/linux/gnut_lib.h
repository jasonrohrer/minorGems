/* Josh Pieper, (c) 2000 */

/* This file is distributed under the GPL, see file COPYING for details */

#ifndef GNUT_LIB_H
#define GNUT_LIB_H 0

#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#ifndef MIN
	#define MIN(a,b) (((a < b)) ? (a) : (b))
	#define MAX(a,b) (((a > b)) ? (a) : (b))
#endif

#ifndef __bswap_16
  #define __bswap_16(x) \
    ((((x) >> 8) & 0xff) | (((x) &0xff) <<8))
#endif
      
#ifndef __bswap_32
  #define __bswap_32(x) \
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | \
    (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))
#endif
                                                
#ifndef GUINT16_TO_LE
#ifdef WORDS_BIGENDIAN
  #define GUINT16_TO_LE(x)  __bswap_16(x)
  #define GUINT32_TO_LE(x)  __bswap_32(x)
  #define GUINT16_FROM_LE(x)  __bswap_16(x)
  #define GUINT32_FROM_LE(x)  __bswap_32(x)
#else
  #define GUINT16_TO_LE(x)  (x)
  #define GUINT32_TO_LE(x)  (x)
  #define GUINT16_FROM_LE(x)  (x)
  #define GUINT32_FROM_LE(x)  (x)
#endif
#endif

typedef unsigned int 	uint32;
typedef unsigned short 	uint16;
typedef unsigned char	uchar;
#ifndef WIN32
typedef unsigned long long uint64;

//#define g_debug(num, format, args... ) 
#define g_debug(num, format, args... ) _g_debug(__FUNCTION__,__LINE__, num, format, ##args)
void _g_debug(char *,int,int num, char *,...);
#endif

extern int gnut_lib_debug;

int gnut_mprobe(void *);

char *gnut_strdelimit(char *string, char *delimeters, char new_delim);

char *expand_path(char *);

#ifndef xmalloc
char *xmalloc(int size);
#endif

#ifdef WIN32
#include <windows.h>

#define VERSION "0.3.22/win32"

void g_debug(int, char *, ...);

typedef unsigned __int64 uint64;
typedef unsigned int uint;

#define sleep Sleep
char *strtok_r(char *, const char *, char **);
int getopt(int argc, char *argv[], char *);
extern char *optarg;
extern int optind;

#define strncasecmp strnicmp

#define inet_aton(string, ip) (ip)->s_addr = inet_addr(string)

#define write(sock, buf, len) send(sock, buf, len, 0)
#define read(sock, buf, len) recv(sock, buf, len, 0)
#define close(sock) closesocket(sock)

#define F_SETFL 1
#define O_NONBLOCK 1
int fcntl(int sock, int, uint);

#define flock(a,b) 0
#endif


#endif
