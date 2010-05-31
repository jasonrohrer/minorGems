/* Josh Pieper, (c) 2000 */

/* This file is distributed under the GPL, see file COPYING for details */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifndef WIN32
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <netdb.h>
#endif

#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>

#include "gnut_lib.h"
#include "gnut_threads.h"
#include "gnut_if.h"
#include "protocol.h"

uchar local_ip[4];

// uint32 net_local_ip()
//
// returns the local ip address
//
uchar *net_local_ip()
{
	return local_ip;
}

int gnut_net_host()
{
	char host[256];
	struct hostent *he;
	int ret;
	
	g_debug(2,"entering\n");
	
	ret=gethostname(host,sizeof(host));
	
	if (ret<0) {
		perror("gnut_net_host, gethostname");
		return ret;
	}
	
	he=gethostbyname(host);
	if (he==NULL) {
		perror("gnut_net_host, gethostbyname");
		return ret;
	}
	
	memcpy(local_ip,he->h_addr_list[0],4);
	
	return 0;
}

// int net_init(char *iface)
//
// Does misc. net init stuff.  Mostly just finds our local IP address
//
int net_init(char *iface)
{
	struct sockaddr_in *sinptr=NULL;
	
	g_debug(2,"entering\n");
	
	if (iface!=NULL) {
		if (inet_aton(iface,(struct in_addr *) local_ip)) {
		g_debug(1,"local was set by command to: %i.%i.%i.%i\n",local_ip[0],local_ip[1],
			local_ip[2],local_ip[3]);
			
			return 0;
		}
	}
	if ((sinptr = get_if_addr(iface))==NULL) {
		g_debug(1,"Can't get local IP address through interface, trying host name...\n");
		gnut_net_host();	
	} else {
		memcpy(local_ip,&(sinptr->sin_addr),sizeof(local_ip));
	}
	
	g_debug(1,"local ip is: %i.%i.%i.%i\n",local_ip[0],local_ip[1],
		local_ip[2],local_ip[3]);
	

	return 0;	
}

// exactly like the real read, except that it returns -2
// if no data was read before the timeout occurred...
int timed_read(int sock, char *buf, int len, int secs)
{
	fd_set fsr;
	struct timeval tv;
	int ret;
	
	ret=fcntl(sock,F_SETFL,O_NONBLOCK);
	
	FD_ZERO(&fsr);
	FD_SET(sock,&fsr);
	
	tv.tv_sec=secs;
	tv.tv_usec=0;
	
	ret=select(sock+1,&fsr,NULL,NULL,&tv);
	
	if (ret==0) {
		return -2;
	}
	
	if (ret<0) return ret;
	
	ret=read(sock,buf,len);
	
	fcntl(sock,F_SETFL,0);
	
	return ret;
}

// int timed_connect(int sock, struct sockaddr *sa,int addrlen,int secs)
//
// just like connect except that it times out after time secs
// returns -2 on timeout, otherwise same as connect
//
int timed_connect(int sockfd, struct sockaddr *sa, int addrlen,int secs)
{
	int ret;
	fd_set fsr;
	struct timeval tv;
	int val,len;
	
	g_debug(1,"entering sock=%i secs=%i\n",sockfd,secs);
	
	ret=fcntl(sockfd,F_SETFL,O_NONBLOCK);
	g_debug(5,"fcntl returned %i\n",ret);
	
	if (ret<0) return ret;
	
	ret=connect(sockfd,sa,addrlen);
	g_debug(5,"connect returned %i\n",ret);
	
	if (ret==0) {
		g_debug(0,"immediate connection!\n");
		// wooah!  immediate connection
		return -2;
	}
	
//	if (errno != EINPROGRESS) {
//		perror("timed_connect, connect");
//		return ret;
//	}
	

	FD_ZERO(&fsr);
	FD_SET(sockfd,&fsr);

	tv.tv_sec=secs;
	tv.tv_usec=0;

	ret=select(sockfd+1,NULL,&fsr,NULL,&tv);
	g_debug(5,"select returned %i\n",ret);

	if (ret==0) {
		// timeout
		g_debug(1,"timeout\n");
		fcntl(sockfd,F_SETFL,0);
		return -2;
	}

	len=4;
	ret=getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&val,&len);
	g_debug(5,"getsockopt returned %i val=%i\n",ret,val);
	
	if (ret<0) {
		g_debug(1,"getsockopt returned: %i\n",ret);
		return ret;
	}

	if (val!=0) {
		g_debug(3,"returning failure!\n");
		return -1;
	}
	
	ret=fcntl(sockfd,F_SETFL,0);
	 
	g_debug(1,"fcntl: %i\n",ret);
	
	g_debug(3,"returning success val=%i\n",val);
	return 0;
}

// int create_listen_socket(int * port)
//
// attempts to create a socket to listen on port
// returns the actual port bound to in port or <0 if not
// successfull
//
int create_listen_socket(int *port)
{
	struct sockaddr_in sin;
	int sock;
	int ret,i;
	int val;
	
	g_debug(3,"entering\n");
	
	sock=socket(AF_INET,SOCK_STREAM,6);
	if (sock<0) {
		perror("create_listen_socket, socket");
		return sock;
	}
	
	val=1;
	ret=setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
	
	i=0;
	ret=-1;
	while (ret<0) {
		sin.sin_addr.s_addr=INADDR_ANY;
		sin.sin_port=htons(*port);
		sin.sin_family=AF_INET;
		ret=bind(sock,(struct sockaddr *) &sin,sizeof(struct sockaddr_in));
		if (++i>50) {
			(*port)++;
			i=0;
		}
	}
	if (ret<0) {
		perror("create_listen_socket, bind");
		return ret;
	}
	
	ret=listen(sock,5);
	if (ret<0) {
		perror("create_listen_socket, listen");
		return ret;
	}
	
	g_debug(1,"bound to port %i\n",*port);
	
	printf("Bound to port: %i\n",*port);
	
	ret=fcntl(sock,F_SETFL,O_NONBLOCK);
	if (ret<0) {
		perror("create_listen_socket, fcntl");
		return ret;
	}
	
	return sock;
}
			
// int read_line(int sock, char *buf, int len)
//
// reads a line from sock into buf, not exceeding len chars
// returns 0 on EOF, -1 on error, or num of characters read
// and -2 if buffer length exceeded
//
int read_line(int sock, char *buf, int len)
{
	int i,ret;
	char c;
	char *ptr;
	
	ptr=buf;
	g_debug(3,"entering\n");
	fcntl(sock,F_SETFL,0);
	
	buf[0]=0;
	
	for (i=1;i<len;i++) {
		try_again:
		
		ret=timed_read(sock,&c,1,30);
		
		if (ret==1) {
			*ptr++=c;
			if (c=='\n') break;
		} else if (ret==0) {
			if (i==1) {
				return 0;
			} else break;
		} else {
			if (errno==EAGAIN)
				goto try_again;
			if (errno==EINTR)
				goto try_again;
			
			if (gnut_lib_debug>=1) perror("read_line, read");
			return -1;
		}
	}
	buf[i]=0;
	
	g_debug(3,"returning success\n");
	return i;
}		

#ifndef PTHREADS_DRAFT4
pthread_mutex_t make_connection_mutex=PTHREAD_MUTEX_INITIALIZER;
#else
pthread_mutex_t make_connection_mutex;
#endif

struct hostent *gnut_hostent_copy(struct hostent *he)
{
	struct hostent *n;
	int i,len;
	
	if (!he) return NULL;
	
	n=(struct hostent *) calloc(sizeof(struct hostent),1);
	
	memcpy(n,he,sizeof(struct hostent));
	
	if (he->h_name) n->h_name=strdup(he->h_name);
	
	for (len=0;he->h_addr_list[len];len++);
	n->h_addr_list=(char **) calloc(sizeof(char *) * (len+1),1);
	for (i=0;i<len;i++) {
		n->h_addr_list[i]=xmalloc(4);
		memcpy(n->h_addr_list[i],he->h_addr_list[i],4);
	}
	
	return n;
}

void gnut_hostent_delete(struct hostent *he)
{
	int i,len;
	g_debug(1,"entering\n");

	for (len=0;he->h_addr_list[len];len++);
	
	g_debug(1,"len=%i\n",len);
	for (i=0;i<len;i++) {
		g_debug(1,"deleting i=%i\n",i);
		if (he->h_addr_list[i]) free(he->h_addr_list[i]);
	}
	
	g_debug(1,"freeing h_name\n");
	if (he->h_name) free(he->h_name);
	
	g_debug(1,"freeing h_addr_list\n");
	free(he->h_addr_list);
	
	g_debug(1,"freeing he\n");
	free(he);
	g_debug(1,"returning\n");
}
	

// int make_connection(char *host, uint port, uchar ip[4])
//
// creates a socket, and attempts to make a connection to the host
// named.  it tries multiple resolved ip addresses if necessary,
// returning the socket on success and storing the good ip number in ip
// otherwise it returns <0
//
// Notes: due to the fact that gethostbyname is not thread safe
// we need to protect this function with a mutex
//
int make_connection(uchar *host, uint port, uchar ip[4])
{
	struct hostent *he;
	struct sockaddr_in sin;
	int i,sock=-1,ret;
	
	g_debug(1,"entering\n");
	
	pthread_mutex_lock(&make_connection_mutex);
	
	he=gnut_hostent_copy(gethostbyname(host));
	if (he==NULL) {
		pthread_mutex_unlock(&make_connection_mutex);
		return -1;
	}
	
	pthread_mutex_unlock(&make_connection_mutex);
	
	for (i=0,ret=-1;he->h_addr_list[i]!=NULL && ret<0;i++) {
		g_debug(1,"trying host %i.%i.%i.%i:%i\n",
			(uchar) he->h_addr_list[i][0],(uchar) he->h_addr_list[i][1],
			(uchar) he->h_addr_list[i][2],(uchar) he->h_addr_list[i][3],port);
		sock=socket(AF_INET,SOCK_STREAM,6);
		if (sock<0) {
			perror("make_connection, socket");
			gnut_hostent_delete(he);
			return sock;
		}
		
		sin.sin_family=AF_INET;
		sin.sin_port=htons(port);
		memcpy(&sin.sin_addr.s_addr,he->h_addr_list[i],4);
		
		ret=timed_connect(sock,(struct sockaddr *) &sin, sizeof(struct sockaddr_in),10);
		g_debug(5,"timed_connect returned: %i\n",ret);
		if (ret<0) {
			g_debug(1,"host bad, closing\n");
			close(sock);
		} else {
			break;
		}
	}
	if (ret<0 || he->h_addr_list[i]==NULL) {
		g_debug(1,"returning failure\n");
		gnut_hostent_delete(he);
		return -2;
	}
	g_debug(5,"about to copy ip from slot %i\n",i);
	
	memcpy(ip,he->h_addr_list[i],4);
	
	g_debug(4,"trying to unlock mutex\n");
	
	gnut_hostent_delete(he);
	
	g_debug(1,"returning success\n");
	
	return sock;
}

// int send_packet (int sock, gnutella_packet *gpa)
//
// sends the packet described by gpa over socket sock
// return 0 on success or <0 for error
//
int send_packet(int sock, gnutella_packet *gpa)
{
	int ret;
	int t;
	
	g_debug(3,"entering\n");
	
	ret=write(sock,(char *) &gpa->gh,sizeof(gnutella_hdr));
	if (ret<0) {
		if (gnut_lib_debug>3) perror("send_packet, write header");
		return ret;
	}
	
	memcpy(&t,gpa->gh.dlen,4);
	t=GUINT32_FROM_LE(t);
	
	if (t>0) {
		ret=write(sock,gpa->data,t);
		if (ret<0) {
			if (gnut_lib_debug>3) 
				perror("send_packet, write data");
			return ret;
		}
	}
	g_debug(3,"returning success\n");
	
	return 23+t;
}

// int read_packet(int sock, gnutella_packet *gpa)
//
// reads a packet from the socket sock into the packet
// structure of gpa.
// returns 0 on success or <0 on error
//
int read_packet(int sock, gnutella_packet *gpa)
{
	int ret;
	char *ptr;
	int left;
	uint dlen;
	int i;
	
	g_debug(3,"entering\n");
	
	ptr=(char *) &gpa->gh;
	for (left=sizeof(gnutella_hdr);left>0;) {
		ret=timed_read(sock,ptr,left,10);
		g_debug(6,"timed_read returned: %i\n",ret);
		if (ret==-2) return 0;
		if (ret==0) {
			return -2;
		} else if (ret<0) {
			if (errno!=EINTR) {
				if (gnut_lib_debug>3) perror("read_packet, header");
				return ret;
			} else ret=0;
		}
		
		ptr+=ret;
		left-=ret;
	}
	
	assert(left==0);
	memcpy(&dlen,gpa->gh.dlen,4);
	dlen=GUINT32_FROM_LE(dlen);
	
	while (dlen>32767 || (gpa->gh.func==129 && dlen>65536)) {
		// We're out of sync!  I'm going to do large reads until
		// one returns 23, which is probably a ping packet....
		g_debug(2,"out of sync! func=%i dlen=%i\n",gpa->gh.func,dlen);
		ptr=(char *) xmalloc(100);
		
		ret=1;
		i=0;
		while (ret>0 && ret!=23 && ret!=49) {
			ret=timed_read(sock,ptr,60,2);
			g_debug(6,"timed_read returned: %i\n",ret);
			if (ret==-2 || (++i>5)) {
				free(ptr);
				return 0;
			}
		}
		
		if  (ret<=0) {
			free(ptr);
			return ret;
		}
		
		free(ptr);
		memcpy(&gpa->gh,ptr,23);
		memcpy(&dlen,gpa->gh.dlen,4);
		dlen=GUINT32_FROM_LE(dlen);
	}
	
	if (dlen>0) {
		gpa->data=(char *) calloc(dlen,1);
		ptr=gpa->data;
		for (left=dlen;left>0;) {
			ret=timed_read(sock,ptr,left,10);
			g_debug(6,"timed_read returned: %i\n",ret);
			if (ret==-2) return 0;
			if (ret==0) return -2;
			if (ret<0) {
				if (gnut_lib_debug>3) perror("read_packet, data");
				return ret;
			}
			
			ptr+=ret;
			left-=ret;
		}
	}
	
	if (dlen>3000) g_debug(2,"OVERSIZE packet! size: %i\n",dlen);
	g_debug(3,"returning success\n");
	return 23+dlen;
}

	
	
	
	