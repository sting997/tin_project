//
// Created by riuoku on 27.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include "RequestManager.h"
#include "config.h"

#define LISTENQ 5

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name);

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port);

int main() {
    int tcpEcho, tcpTime, udpEcho, udpTime, nready, maxfdp;
    fd_set rset;
    struct sockaddr_in servaddr;

    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, PORT_TCP_ECHO);
    prepareSocket(tcpEcho, AF_INET, SOCK_STREAM, 0, servaddr);
    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, PORT_TCP_TIME);
    prepareSocket(tcpTime, AF_INET, SOCK_STREAM, 0, servaddr);

    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, PORT_UDP_ECHO);
    prepareSocket(udpEcho, AF_INET, SOCK_DGRAM, 0, servaddr);
    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, PORT_UDP_TIME);
    prepareSocket(udpTime, AF_INET, SOCK_DGRAM, 0, servaddr);

    maxfdp = std::max({tcpEcho, tcpTime, udpEcho, udpTime}) + 1;

    while (true) {
        FD_ZERO(&rset);
        FD_SET(tcpEcho, &rset);
        FD_SET(tcpTime, &rset);
        FD_SET(udpEcho, &rset);
        FD_SET(udpTime, &rset);

        if ((nready = select(maxfdp, &rset, NULL, NULL, NULL)) < 0) {
            perror("Something bad happened with select");
            exit(nready);
        }

        if (FD_ISSET(tcpEcho, &rset)) {
            RequestManager requestManager = RequestManager(tcpEcho, SOCK_STREAM);
            requestManager.requestEcho();
        }

        if (FD_ISSET(tcpTime, &rset)) {
            RequestManager requestManager = RequestManager(tcpTime, SOCK_STREAM);
            requestManager.requestTime();
        }

        if (FD_ISSET(udpEcho, &rset)) {
            RequestManager requestManager = RequestManager(udpEcho, SOCK_DGRAM);
            requestManager.requestEcho();
        }

        if (FD_ISSET(udpTime, &rset)) {
            RequestManager requestManager = RequestManager(udpTime, SOCK_DGRAM);
            requestManager.requestTime();
        }
    }
}

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port) {
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name) {
    int enable = 1;

    fd = socket(domain, type, protocol);

    if (fd == -1) {
        perror("opening socket");
        exit(1);
    }
    if ((type == SOCK_STREAM) && (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(1);
    }
    if (bind(fd, (struct sockaddr *) &name, sizeof name) == -1) {
        perror("binding socket");
        exit(1);
    }

    if (type == SOCK_STREAM && (listen(fd, LISTENQ) == -1)) {
        perror("listen on socket");
        exit(1);
    }
}
