//
// Created by monspid on 06.05.17.
//

#ifndef TIN_REQUESTMANAGER_H
#define TIN_REQUESTMANAGER_H

#include <iostream>

class RequestManager {
    int sock;
    struct sockaddr_in broadcastAddr;
    struct sockaddr_in remote, service;
    char buf[1024];
    char req[2];
    socklen_t len;
    ssize_t n;

    int startTcpCon(char const *ip, uint16_t port);

    void setTimeout(int socket, time_t tv_sec, long int tv_usec);

    void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port);

    unsigned long checkIfEnd(char const *buf, char const *seq);

public:
    RequestManager();

    void RequestIP();

    void RequestTicket();

    void RequestUDPEcho();

    void RequestUDPTime();

    void RequestTCPEcho();

    void RequestTCPTime();

    void RequestNewData() {};
};


#endif //TIN_REQUESTMANAGER_H
