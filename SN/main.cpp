//
// Created by riuoku on 27.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <algorithm>
#include <ctime>
#include "../protocol_codes.h"
#include "TicketCorrectnessTester.h"

#define LISTENQ 5
#define MAXLINE 1024

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name);
void fillSockaddr_in(struct sockaddr_in &name, short sin_family, unsigned long s_addr, unsigned short sin_port);

int maxFd1(int tcpfd, int tcpfd2, int udpfd, int udpfd2);
void UDPEcho(int &udpfd, char* buf);
void UDPTime(int &udpfd, char* buf);

int main()
{
    int tcpfd, tcpfd2, udpfd, udpfd2, nready, maxfdp1;
    fd_set rset;
    struct sockaddr_in servaddr;
    char buf[1024];

    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 5000);
    prepareSocket(tcpfd, AF_INET, SOCK_STREAM, 0, servaddr);
    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 5001);
    prepareSocket(tcpfd2, AF_INET, SOCK_STREAM, 0, servaddr);

    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 6000);
    prepareSocket(udpfd, AF_INET, SOCK_DGRAM, 0, servaddr);
    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 6001);
    prepareSocket(udpfd2, AF_INET, SOCK_DGRAM, 0, servaddr);

    FD_ZERO(&rset);
    maxfdp1 = maxFd1(tcpfd, tcpfd2, udpfd, udpfd2);

    while(true) {
        FD_SET(tcpfd, &rset);
        FD_SET(tcpfd2, &rset);
        FD_SET(udpfd, &rset);
        FD_SET(udpfd2, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
            perror("Something bad happened with select");

        //if (FD_ISSET(tcpfd, &rset))
            //TCPEcho(udpfd2);

        //if (FD_ISSET(tcpfd2, &rset))
            //TCPTime(udpfd);

        if (FD_ISSET(udpfd, &rset))
            UDPEcho(udpfd, &buf[0]);

        if (FD_ISSET(udpfd2, &rset))
            UDPTime(udpfd2, &buf[0]);
    }

    exit(0);
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
        perror("opening socket");
        exit(1);
    }

    if (bind(fd,(struct sockaddr *)&name, sizeof name) == -1) {
        perror("binding socket");
        exit(1);
    }

    if (protocol == SOCK_STREAM && (listen(fd, LISTENQ) == -1)) {
        perror("listen on socket");
        exit(1);
    }
}

int maxFd1(int tcpfd, int tcpfd2, int udpfd, int udpfd2) {
    int max = tcpfd;

    if (max < tcpfd2)
        max = tcpfd2;

    if (max < udpfd)
        max = udpfd;

    if (max < udpfd2)
        max = udpfd2;

    return max + 1;
}

void UDPEcho(int &udpfd, char* buf) {
    int n, k;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    n = recvfrom(udpfd, buf, MAXLINE, 0,(struct sockaddr *) &cliaddr, &len);

    if((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        buf[0] = SERVICE_GRANTED;
    } else {
        bzero(buf, MAXLINE);
        buf[0] = SERVICE_REFUSED;

        switch(k) {
            case 1:
                memcpy(buf+1, "Invalid Ticket", sizeof("Invalid Ticket"));
                break;
            case 2:
                memcpy(buf+1, "Invalid IP address", sizeof("Invalid IP address"));
                break;
            case 3:
                memcpy(buf+1, "Ticket has expired", sizeof("Ticket has expired"));
                break;
            case 4:
                memcpy(buf+1, "Invalid data format", sizeof("Invalid data format"));
                break;
        }
    }
    sendto(udpfd, buf, n, 0, (struct sockaddr *) &cliaddr, len);
}

void UDPTime(int &udpfd, char* buf) {
    int n, k;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    n = recvfrom(udpfd, buf, MAXLINE, 0,(struct sockaddr *) &cliaddr, &len);
    bzero(buf, MAXLINE);

    if((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        buf[0] = SERVICE_GRANTED;

        //Sends a time in seconds since the Epoch
        std::time_t result = std::time(nullptr);
        memcpy(buf+1, &result, sizeof(&result));
    } else {
        buf[0] = SERVICE_REFUSED;

        switch(k) {
            case 1:
                memcpy(buf+1, "Invalid Ticket", sizeof("Invalid Ticket"));
                break;
            case 2:
                memcpy(buf+1, "Invalid IP address", sizeof("Invalid IP address"));
                break;
            case 3:
                memcpy(buf+1, "Ticket has expired", sizeof("Ticket has expired"));
                break;
            case 4:
                memcpy(buf+1, "Invalid data format", sizeof("Invalid data format"));
                break;
        }
    }
    sendto(udpfd, buf, n, 0, (struct sockaddr *) &cliaddr, len);
}