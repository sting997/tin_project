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
#include "../SN/TicketDecryptor.h"//only for debug


#define PORT 9000
#define PORT2 8000
#define UDP_ECHO_PORT 6000
#define UDP_TIME_PORT 6001


void communicationTest(unsigned short  port);

void setTimeout(int socket, time_t tv_sec, long int tv_usec);

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in broadcastAddr;
    struct sockaddr_in remote;
    char buf[1024];


    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }

    setTimeout(sock, 3, 0);

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    fillSockaddr_in(broadcastAddr, AF_INET, htonl(INADDR_ANY), PORT);

    //create simple request for server
    char req[2];
    req[0] = TS_REQ_IP;
    req[1] = 0;
    /* Send message. */
    if (sendto(sock, req, sizeof req, 0, (struct sockaddr *) &broadcastAddr, sizeof broadcastAddr) == -1)
        puts("sending datagram message");
    //receive package from server
    socklen_t len = sizeof(remote);
    ssize_t n;
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

    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }
    setTimeout(sock, 3, 0);

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    fillSockaddr_in(broadcastAddr, AF_INET, remote.sin_addr.s_addr, PORT2);

    //create simple request for server
    req[0] = TS_REQ_TICKET;

    char *auth_data =  (char *) "1;1;admin;admin";
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
        } else
            printf("Received roaming package, didn't want it though!\n");
    }

    communicationTest(UDP_TIME_PORT);
    communicationTest(UDP_ECHO_PORT);

    close(sock);
    exit(0);
}

void communicationTest(unsigned short port) {
    int sock;
    char buf[1024];
    struct sockaddr_in remote;
    struct sockaddr_in service;

    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        puts("opening datagram socket");
        exit(1);
    }
    setTimeout(sock, 3, 0);

    memset(&service, 0, sizeof(service));
    fillSockaddr_in(service, AF_INET, inet_addr("127.0.0.1"), port);

    bzero(buf, 1024);
    memcpy(buf + 1, "Echo: Lorem Ipsum", sizeof("Echo: Lorem Ipsum"));

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
            if (port == UDP_TIME_PORT)
                printf("%s\n", std::asctime(std::localtime(reinterpret_cast<time_t *>(buf + 1))));
            else
                printf("%s\n", (buf + 1));
        } else if (buf[0] == SERVICE_REFUSED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else
            printf("Received roaming package, didn't want it though!\n");
    }

    close(sock);
}

void setTimeout(int socket, time_t tv_sec, long int tv_usec) {
    struct timeval tv;
    tv.tv_sec = tv_sec;
    tv.tv_usec = tv_usec;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval));
}

void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port) {
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}