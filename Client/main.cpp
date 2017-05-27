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
#include "../TS/RequestManager.h"

#define DATA "The sea is calm, the tide is full . . ."
#define PORT 9000
#define UDP_ECHO_PORT 6000
#define UDP_TIME_PORT 6001


//TODO
//refactor this code, divide into functions
void communicationTest(int port);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in broadcastAddr;
    struct sockaddr_in remote;
    char buf[1024];


    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    broadcastAddr.sin_port = htons(PORT);

    //create simple request for server
    char req[2];
    RequestManager::createRequest(req, TS_REQ_IP);
    req[1] = 0;
    /* Send message. */
    if (sendto(sock, req, sizeof req, 0, (struct sockaddr *) &broadcastAddr, sizeof broadcastAddr) == -1)
        perror("sending datagram message");
    //receive package from server
    socklen_t len = sizeof(remote);
    int n;
    while (true) {
        bzero(buf, 1024);
        n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);
        if (n < 0)
            perror("Error receiving data");
        else {
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
        perror("opening datagram socket");
        exit(1);
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = remote.sin_addr.s_addr;
    broadcastAddr.sin_port = htons(8000);

    //create simple request for server
    RequestManager::createRequest(req, TS_REQ_TICKET);

    char *auth_data = "1;1;admin;admin";
    char temp[1024];
    strcpy(temp, req);
    strcat(temp, auth_data);

    /* Send message. */
    if (sendto(sock, temp, sizeof temp, 0, (struct sockaddr *) &broadcastAddr, sizeof broadcastAddr) == -1)
        perror("sending datagram message");

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);
    if (n < 0)
        perror("Error receiving data");
    else {
        if (buf[0] == TS_GRANTED) {
            printf("I just received my ticket, whoooaaa!\n%s\n", buf);
        } else
            printf("Received roaming package, didn't want it though!\n");
    }

    communicationTest(UDP_TIME_PORT);
    communicationTest(UDP_ECHO_PORT);

    close(sock);
    exit(0);
}

void communicationTest(int port) {
    int sock;
    char buf[1024];
    struct sockaddr_in remote;
    struct sockaddr_in service;

    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(port);

    bzero(buf, 1024);
    memcpy(buf + 1, "Echo: Lorem Ipsum", sizeof("Echo: Lorem Ipsum"));

    if (sendto(sock, buf, sizeof buf, 0, (struct sockaddr *) &service, sizeof service) == -1)
        perror("sending datagram message");
    //receive package from server
    socklen_t len = sizeof(remote);
    int n;

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);
    if (n < 0)
        perror("Error receiving data");
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