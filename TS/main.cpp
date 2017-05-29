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
    int udp;
    fd_set rset;
    struct sockaddr_in name;

    fillSockaddr_in(name, AF_INET, INADDR_ANY, TS_PORT);
    prepareSocket(udp, AF_INET, SOCK_DGRAM, 0, name);

    RequestManager requestManager = RequestManager(udp);
    
    requestManager.listenForRequests();
}

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port) {
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name) {
    socklen_t len = sizeof(name);
    fd = socket(domain, type, protocol);

    if (fd == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    if (bind(fd, (struct sockaddr *) &name, len) == -1) {
        perror("binding datagram socket");
        exit(1);
    }

    if (getsockname(fd, (struct sockaddr *) &name, &len) == -1) {
        perror("getting socket name");
        exit(1);
    }

    printf("TS listens on port %d\n", ntohs(name.sin_port));
}