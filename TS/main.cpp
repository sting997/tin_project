//
// Created by monspid on 13.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include "RequestManager.h"

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name);

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port);

int main() {
    int udpfd, udpfd2, nready, maxfdp1;
    fd_set rset;
    struct sockaddr_in name;
    char buf[1024];

    fillSockaddr_in(name, AF_INET, INADDR_ANY, 9000);
    prepareSocket(udpfd, AF_INET, SOCK_DGRAM, 0, name);
    fillSockaddr_in(name, AF_INET, INADDR_ANY, 8000);
    prepareSocket(udpfd2, AF_INET, SOCK_DGRAM, 0, name);

    FD_ZERO(&rset);
    maxfdp1 = udpfd2 > udpfd ? udpfd2 + 1 : udpfd + 1;
    while (true) {
        FD_SET(udpfd, &rset);
        FD_SET(udpfd2, &rset);
        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
            perror("Something bad happened with select");

        if (FD_ISSET(udpfd, &rset)) {
            RequestManager requestManager = RequestManager(udpfd);
            requestManager.requestIP();
        }

        else if (FD_ISSET(udpfd2, &rset)) {
            RequestManager requestManager = RequestManager(udpfd2);
            requestManager.requestTicket();
        }
    }

    exit(0);
}

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port) {
    /* Create name with wildcards. */
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name) {
    socklen_t len;
    fd = socket(domain, type, protocol);

    if (fd == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    if (bind(fd, (struct sockaddr *) &name, sizeof name) == -1) {
        perror("binding datagram socket");
        exit(1);
    }

    //print port number on console
    len = sizeof(name);

    if (getsockname(fd, (struct sockaddr *) &name, &len) == -1) {
        perror("getting socket name");
        exit(1);
    }

    printf("TS listens on port %d\n", ntohs(name.sin_port));
}
