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

void returnIP( int sock ) {
    ssize_t n;
    socklen_t len;
    char buf[1024];
    struct sockaddr_in remote;

    /* need to know how big address struct is, len must be set before the call to recvfrom!!! */
    len = sizeof(remote);

    /* read a datagram from the socket (put result in bufin) */
    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);

    /* print out the address of the sender */
    printf("TS got a datagram from %s port %d\n",
           inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    if (n < 0) {
        perror("Error receiving data");
    } else {
        printf("TS got %d bytes\n", (int) n);
        /* Got something, just send it back */
        sendto(sock, buf, (size_t) n, 0,(struct sockaddr *) &remote, len);
    }
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

int main()
{
    int udpfd, nready, maxfdp1;
    socklen_t len;
    fd_set rset;
    struct sockaddr_in name;
    char buf[1024];

    udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpfd == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    /* Create name with wildcards. */
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons(9000);

    if (bind(udpfd,(struct sockaddr *)&name, sizeof name) == -1) {
        perror("binding datagram socket");
        exit(1);
    }

    //print port number on console
    len = sizeof(name);

    if (getsockname(udpfd,(struct sockaddr *) &name, &len) == -1) {
        perror("getting socket name");
        exit(1);
    }

    printf("TS listens on port %d\n", ntohs(name.sin_port));

    FD_ZERO(&rset);
    maxfdp1 = udpfd + 1;

    while(true) {
        FD_SET(udpfd, &rset);
        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) 
            perror("Something bad happened with select");
        
        if (FD_ISSET(udpfd, &rset)) 
            returnIP(udpfd);
    }

    exit(0);
}
