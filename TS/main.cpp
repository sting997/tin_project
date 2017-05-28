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
    int udpfd_IP, udpfd_ticket, nready, maxfdp;
    fd_set rset;
    struct sockaddr_in name;

    fillSockaddr_in(name, AF_INET, INADDR_ANY, PORT_IP_REQUEST);
    prepareSocket(udpfd_IP, AF_INET, SOCK_DGRAM, 0, name);
    fillSockaddr_in(name, AF_INET, INADDR_ANY, PORT_TICKET_REQUEST);
    prepareSocket(udpfd_ticket, AF_INET, SOCK_DGRAM, 0, name);

    FD_ZERO(&rset);
    maxfdp = udpfd_ticket > udpfd_IP ? udpfd_ticket + 1 : udpfd_IP + 1;

    while (true) {
        FD_SET(udpfd_IP, &rset);
        FD_SET(udpfd_ticket, &rset);

        if ((nready = select(maxfdp, &rset, NULL, NULL, NULL)) < 0) {
            perror("Something bad happened with select");
            return nready;
        }

        if (FD_ISSET(udpfd_IP, &rset)) {
            RequestManager requestManager = RequestManager(udpfd_IP);
            requestManager.requestIP();
        } else if (FD_ISSET(udpfd_ticket, &rset)) {
            RequestManager requestManager = RequestManager(udpfd_ticket);
            requestManager.requestTicket();
        }
    }
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