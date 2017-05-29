//
// Created by monspid on 06.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include <iostream>
#include "../protocol_codes.h"
#include "RequestManager.h"
#include "config.h"

RequestManager::RequestManager() {
}

void RequestManager::RequestTCPEcho() {
    sock = startTcpCon("127.0.0.1", PORT_TCP_ECHO);
    ssize_t rval;
    unsigned long endpos;

    bzero(buf, 1024);

    //keep communicating with server
    while (true) {
        printf("To ECHO: ");
        fgets(buf, 1024, stdin);

        //Send some data
        if (send(sock, buf, strlen(buf), 0) < 0) {
            perror("Send failed:");
            exit(1);
        }
        endpos = checkIfEnd(buf, "END");
        if (endpos != std::string::npos)
            break;
    }
    while (true) {
        if ((rval = read(sock, buf, 1024)) == -1) {
            perror("Read failed:");
            exit(1);
        }
        if (rval == 0) {
            printf("Ending connection.\n");
            break;
        } else
            printf("%s\n", buf);
    }
    close(sock);
}

void RequestManager::RequestTCPTime() {
    sock = startTcpCon("127.0.0.1", PORT_TCP_TIME);
    ssize_t rval;

    bzero(buf, 1024);

    memcpy(buf, "TCP TIME PLS", sizeof("TCP TIME PLS"));

    if (send(sock, buf, strlen(buf), 0) < 0) {
        perror("Send failed:");
        exit(1);
    }

    bzero(buf, 1024);
    if ((rval = read(sock, buf, 1024)) == -1) {
        perror("Read failed:");
        exit(1);
    }
    if (rval == 0) {
        printf("Service server disconnected.\n");
    } else {
        printf("%s\n", std::asctime(std::localtime(reinterpret_cast<time_t *>(buf + 1))));
    }
    close(sock);
}

int RequestManager::startTcpCon(char const *ip, uint16_t port) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("Opening datagram socket:");
        exit(1);
    }

    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(port);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *) &service, sizeof(service)) < 0) {
        perror("Connect failed:");
        return -1;
    }

    return sock;
}

unsigned long RequestManager::checkIfEnd(char const *buf, char const *seq) {
    std::string bufs(buf);
    std::string subs(seq);

    return bufs.find(seq);
}

void RequestManager::setTimeout(int socket, time_t tv_sec, long int tv_usec) {
    struct timeval tv;
    tv.tv_sec = tv_sec;
    tv.tv_usec = tv_usec;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval));
}

void RequestManager::fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr,
                                     unsigned short sin_port) {
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void RequestManager::RequestIP() {
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }

    setTimeout(sock, 3, 0);

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    fillSockaddr_in(broadcastAddr, AF_INET, htonl(INADDR_ANY), TS_PORT);


    req[0] = TS_REQ_IP;
    req[1] = 0;
    /* Send message. */
    if (sendto(sock, req, sizeof req, 0, (struct sockaddr *) &broadcastAddr, sizeof broadcastAddr) == -1)
        puts("sending datagram message");
    //receive package from server
    len = sizeof(remote);
    while (true) {
        bzero(buf, 1024);
        n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);
        if (n < 0) {
            puts("Error: receiving data from TS");
            break;
        } else {
            if (buf[0] == TS_IP) {
                printf("Received package from TS: %s\n", inet_ntoa(remote.sin_addr));
                break;
            } else
                printf("Received roaming package, didn't want it though!\n");
        }
    }
    close(sock);
}

void RequestManager::RequestTicket() {
    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }
    setTimeout(sock, 3, 0);

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    fillSockaddr_in(broadcastAddr, AF_INET, remote.sin_addr.s_addr, TS_PORT);

    //create simple request for server
    req[0] = TS_REQ_TICKET;

    char *auth_data = (char *) "1;1;admin;admin";
    char temp[1024];
    strcpy(temp, req);
    strcat(temp, auth_data);

    /* Send message. */
    if (sendto(sock, temp, sizeof temp, 0, (struct sockaddr *) &broadcastAddr, sizeof broadcastAddr) == -1)
        puts("sending datagram message");

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);
    if (n < 0)
        puts("Error: receiving data from TS");
    else {
        if (buf[0] == TS_GRANTED) {
            printf("I just received my ticket, whoooaaa!\n buf: %s\n", buf);
        } else if (buf[0] == TS_REFUSED)
            printf("TS didn't give me a ticket, what a bitch!!!\n");
        else {
            printf("Received roaming package, didn't want it though!\n");
        }
    }
    close(sock);
}

void RequestManager::RequestUDPEcho() {
    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }
    setTimeout(sock, 3, 0);

    memset(&service, 0, sizeof(service));
    fillSockaddr_in(service, AF_INET, inet_addr("127.0.0.1"), PORT_UDP_ECHO);

    bzero(buf, 1024);
    memcpy(buf + 1, "UDPEcho: Lorem Ipsum", sizeof("UDPEcho: Lorem Ipsum"));
    if (sendto(sock, buf, sizeof buf, 0, (struct sockaddr *) &service, sizeof service) == -1)
        puts("sending datagram message");
    //receive package from server
    len = sizeof(remote);

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);

    if (n < 0)
        puts("Error: receiving data from Sn");
    else {
        if (buf[0] == SERVICE_GRANTED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else if (buf[0] == SERVICE_REFUSED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else
            printf("Received roaming package, didn't want it though!\n");
    }

    close(sock);
}

void RequestManager::RequestUDPTime() {

    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }
    setTimeout(sock, 3, 0);

    memset(&service, 0, sizeof(service));
    fillSockaddr_in(service, AF_INET, inet_addr("127.0.0.1"), PORT_UDP_TIME);

    bzero(buf, 1024);
    memcpy(buf + 1, "UDPEcho: Lorem Ipsum", sizeof("UDPEcho: Lorem Ipsum"));
    if (sendto(sock, buf, sizeof buf, 0, (struct sockaddr *) &service, sizeof service) == -1)
        puts("sending datagram message");
    //receive package from server
    socklen_t len = sizeof(remote);
    ssize_t n;

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);

    if (n < 0)
        puts("Error: receiving data from Sn");
    else {
        if (buf[0] == SERVICE_GRANTED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", std::asctime(std::localtime(reinterpret_cast<time_t *>(buf + 1))));
        } else if (buf[0] == SERVICE_REFUSED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else
            printf("Received roaming package, didn't want it though!\n");
    }

    close(sock);
}
