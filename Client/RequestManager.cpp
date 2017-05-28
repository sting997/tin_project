//
// Created by monspid on 06.05.17.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include "RequestManager.h"
#include "config.h"

void RequestManager::tcpEchoTest() {
    int sock = startTcpCon("127.0.0.1", PORT_TCP_ECHO);
    char buf[1024];
    int rval, endpos;

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

void RequestManager::tcpTimeTest() {
    int sock = startTcpCon("127.0.0.1", PORT_TCP_TIME);
    char buf[1024];
    int rval;

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

int RequestManager::startTcpCon(char const *ip, int port) {

    struct sockaddr_in service;
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

int RequestManager::checkIfEnd(char const *buf, char const *seq) {
    std::string bufs(buf);
    std::string subs(seq);

    return bufs.find(seq);
}
