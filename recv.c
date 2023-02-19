/*
 * mbeacon.c - Sends/Receivers Multicast Packets on a group, payload format
 *             is is u_long (64bits) incrementing sequentialy
 *
 * Steve Rubin <ser@tch.org> 8/3/99
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

#define GROUP "233.31.185.1"
#define PORT 32767
#define UINTERVAL 10000

int recv_open(long group, short port);
int send_open(long group, short port);
void mcast_beacon(int s);


void mcast_beacon(int s) {
	unsigned long buf=0L;
	
	for(;;) {
		printf("%d\n",buf);
		if(send(s,&buf,sizeof(buf),0) < 0) {
			printf("Error sending on sock %d, pid %d exiting\n",
				s,getpid());
			perror("");
			exit(1);
		}
		++buf;
		usleep(UINTERVAL);
	}
}

int send_open(long group, short port) {
	struct sockaddr_in addr;
	int s;
	const int one = 1;	

	s=socket(AF_INET,SOCK_DGRAM,0);
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(PORT);
	bind(s,(struct sockaddr *) &addr, sizeof(addr)); 
	printf("socket is %d\n",s);

	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=group;
 	addr.sin_port = htons(PORT);	
	printf("connect = %d\n",connect(s,(struct sockaddr *) &addr,sizeof(addr)));
	return(s);
	
}


int recv_open(long group, short port) {
        struct sockaddr_in addr;
        int fd;
        struct ip_mreq mreq;
        const int one = 1;
        
        fd=socket(AF_INET,SOCK_DGRAM,0);
        if (fd<0) {
                perror("socket");
                exit(1);
        }
        bzero(&addr,sizeof(addr));
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr=htonl(INADDR_ANY);
        addr.sin_port=htons(port);
 
        if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one))<0) {
                perror("SO_REUSEADDR");                                
                exit(1);                                               
        }                                                              
        if(setsockopt(fd,SOL_SOCKET,SO_REUSEPORT,&one,sizeof(one))<0) {
                perror("SO_REUSEPORT");                                
        } 
        if(bind(fd,(struct sockaddr *) &addr, sizeof(addr)) <0) {
                perror("bind");
                exit(1);
        }                

        mreq.imr_multiaddr.s_addr=group;
        mreq.imr_interface.s_addr=htonl(INADDR_ANY);
        if(setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP, &mreq, 
          sizeof(mreq)) <0) {   
                perror("setsockopt");
                exit(1);
        }
        return(fd);
}

void mcast_rbeacon(int s) {
	long buf;

	for(;;) {
		recv(s,&buf,sizeof(buf),MSG_WAITALL);
		printf("%d\n",buf);
	}
}



int main() {
	int recvfd,sendfd;

	recvfd=recv_open(inet_addr(GROUP),PORT);
	sendfd=send_open(inet_addr(GROUP),PORT);
	mcast_rbeacon(recvfd);
}

