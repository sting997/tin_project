//
// Created by monspid on 13.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "../protocol_codes.h"

void returnIP(int sock);
void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name);
void fillSockaddr_in(struct sockaddr_in &name, short sin_family, unsigned long s_addr, unsigned short sin_port);
void returnTicket(int sock);

int main()
{
    int udpfd, udpfd2, nready, maxfdp1, maxfdp2;
    fd_set rset, rset2;
    struct sockaddr_in name, name2;
    char buf[1024];

    fillSockaddr_in(name, AF_INET, INADDR_ANY, 9000);
	prepareSocket(udpfd, AF_INET, SOCK_DGRAM, 0, name);
    fillSockaddr_in(name, AF_INET, INADDR_ANY, 8000);
	prepareSocket(udpfd2, AF_INET, SOCK_DGRAM, 0, name);

    FD_ZERO(&rset);
    maxfdp1 = udpfd2 > udpfd ? udpfd2 + 1 : udpfd +1;
    while(true) {
        FD_SET(udpfd, &rset);
		FD_SET(udpfd2, &rset);
        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) 
            perror("Something bad happened with select");
        
        if (FD_ISSET(udpfd, &rset)) 
            returnIP(udpfd);
		
		if (FD_ISSET(udpfd2, &rset)) 
            returnTicket(udpfd2);
    }

    exit(0);
}

void returnIP( int sock ) {
    int n;
    socklen_t len;
    char buf[1024];
    struct sockaddr_in remote;

    /* need to know how big address struct is, len must be set before the
       call to recvfrom!!! */

    len = sizeof(remote);

    /* read a datagram from the socket (put result in bufin) */
    n=recvfrom(sock,buf,1024,0,(struct sockaddr *) &remote, &len);

    /* print out the address of the sender */
    printf("Got a datagram from %s port %d\n",
           inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    if (n<0) {
        perror("Error receiving data");
    } 
	else 
        if(buf[0] == TS_REQ_IP){
			char msg[1];
			msg[0] = TS_IP;
			sendto(sock,msg,n,0,(struct sockaddr *) &remote, len);
		}
}

void fillSockaddr_in(struct sockaddr_in &name, short sin_family, unsigned long s_addr, unsigned short sin_port){
	/* Create name with wildcards. */
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name){
	socklen_t len;
	fd = socket(domain, type, protocol);

    if (fd == -1) {
        perror("opening datagram socket");
        exit(1);
    }
    
    if (bind(fd,(struct sockaddr *)&name, sizeof name) == -1) {
        perror("binding datagram socket");
        exit(1);
    }

    //print port number on console
    len = sizeof(name);

    if (getsockname(fd,(struct sockaddr *) &name, &len) == -1) {
        perror("getting socket name");
        exit(1);
    }

    printf("TS listens on port %d\n", ntohs(name.sin_port));
}

void returnTicket(int sock){
	int n;
    socklen_t len;
    char buf[1024];
    struct sockaddr_in remote;


    len = sizeof(remote);

    /* read a datagram from the socket (put result in bufin) */
    n=recvfrom(sock,buf,1024,0,(struct sockaddr *) &remote, &len);

    if (n<0) {
        perror("Error receiving data");
    } 
	else 
        if(buf[0] == TS_REQ_TICKET){
			char msg[1];
			msg[0] = TS_GRANTED;
			sendto(sock,msg,n,0,(struct sockaddr *) &remote, len);
		}
}


