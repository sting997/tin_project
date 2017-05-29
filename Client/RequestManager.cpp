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

RequestManager::RequestManager() {
}

void RequestManager::getUserInput() {
    bzero(buf, 1024);
    //fflush(stdin);
    fgets(buf, 1024, stdin);
}

bool RequestManager::checkIfLastMsg() {
    return msgEndPosition() != std::string::npos;
}

unsigned long RequestManager::msgEndPosition() {
    std::string bufs(buf);

    return bufs.find(msgEndIndicator);
}

void RequestManager::RequestTCPEcho() {
    prepareSocket(PORT_TCP_ECHO, SOCK_STREAM, inet_addr("127.0.0.1"));

    ssize_t rval;

    //keep communicating with server
    while (true) {
        printf("To ECHO: ");
        getUserInput();

        sendMessage(sock, 1, buf);

        if (checkIfLastMsg())
            break;
    }

    while (true) {
        if ((rval = receiveMessage()) == -1) {
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
    prepareSocket(PORT_TCP_TIME, SOCK_STREAM, inet_addr("127.0.0.1"));
    ssize_t rval;

    sendMessage(sock, 2, "PLS TCP TIME");

    if ((rval = receiveMessage()) == -1) {
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

void RequestManager::sendMessage(int sock, char code, std::string message) {
    std::string response = code + message;
    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &name, sizeof name);
}

void RequestManager::prepareBroadcastSocket(int port) {
    prepareSocket(port, SOCK_DGRAM, INADDR_ANY);

    int val = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
}

void RequestManager::prepareSocket(unsigned short port, int type, in_addr_t netlong) {
    sock = socket(PF_INET, type, 0);

    if (sock == -1) {
        puts("opening datagram socket");
        return;
    }

    setTimeout(sock, 3, 0);

    memset(&name, 0, sizeof(name));
    fillSockaddr_in(name, AF_INET, netlong, port);

    if (type == SOCK_STREAM && connect(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
        perror("Connect failed:");
    }
}

ssize_t RequestManager::receiveMessage() {
    ssize_t n;

    bzero(buf, 1024);
    len = sizeof(remote);

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);

    return n;
}

void RequestManager::RequestIP() {
    prepareBroadcastSocket(TS_PORT);

    sendMessage(sock, TS_REQ_IP, "");
    if (receiveMessage() < 0) {
        puts("Error: receiving data");
        close(sock);
        return;
    }

    if (buf[0] == TS_IP) {
        printf("Received package from TS: %s\n", inet_ntoa(remote.sin_addr));
    } else {
        printf("Received roaming package, didn't want it though!\n");
    }

    close(sock);
}

void RequestManager::RequestTicket() {
    /* Create socket on which to send. */
    prepareSocket(TS_PORT, SOCK_DGRAM, remote.sin_addr.s_addr);

    sendMessage(sock, TS_REQ_TICKET, "1;1;admin;admin");

    if (receiveMessage() < 0) {
        puts("Error: receiving data");
        close(sock);
        return;
    }

    if (buf[0] == TS_GRANTED) {
        printf("I just received my ticket, whoooaaa!\n buf: %s\n", buf);
    } else if (buf[0] == TS_REFUSED)
        printf("TS didn't give me a ticket, what a bitch!!!\n");
    else {
        printf("Received roaming package, didn't want it though!\n");
    }
    close(sock);
}

void RequestManager::RequestUDPEcho() {

    prepareSocket(PORT_UDP_ECHO, SOCK_DGRAM, inet_addr("127.0.0.1"));

    sendMessage(sock, 1, "UDPEcho: Lorem Ipsum");

    if (receiveMessage() < 0) {
        puts("Error: receiving data");
        close(sock);
        return;
    }


    if (buf[0] == SERVICE_GRANTED) {
        printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
        printf("%s\n", (buf + 1));
    } else if (buf[0] == SERVICE_REFUSED) {
        printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
        printf("%s\n", (buf + 1));
    } else
        printf("Received roaming package, didn't want it though!\n");

    close(sock);
}

void RequestManager::RequestUDPTime() {

    prepareSocket(PORT_UDP_TIME, SOCK_DGRAM, inet_addr("127.0.0.1"));

    sendMessage(sock, 1, "");

    if (receiveMessage() < 0) {
        puts("Error: receiving data");
        close(sock);
        return;
    }

    if (buf[0] == SERVICE_GRANTED) {
        printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
        printf("%s\n", std::asctime(std::localtime(reinterpret_cast<time_t *>(buf + 1))));
    } else if (buf[0] == SERVICE_REFUSED) {
        printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
        printf("%s\n", (buf + 1));
    } else
        printf("Received roaming package, didn't want it though!\n");

    close(sock);
}
