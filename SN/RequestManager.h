//
// Created by monspid on 06.05.17.
//

#ifndef SN_REQUESTMANAGER_H
#define SN_REQUESTMANAGER_H

#include <cstring>
#include <iostream>
#include "../protocol_codes.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <arpa/inet.h>
#include <iostream>
#include "TicketCorrectnessTester.h"
#include "TicketDecryptor.h"

#define BUFFER_SIZE 1024

class RequestManager {
    int sock, connfd;
    char buf[BUFFER_SIZE];
    struct sockaddr_in remote;
    socklen_t len = sizeof(remote);
    ssize_t n;

    bool shouldPerform(char code);

    void sendMessage(int sock, char code, std::string message);

    std::string getAuthData(std::string buf);

    char getRequestCode(char *message);

    void acceptConnection();

    void prepareRefuseBuffer(int errNum);

    int checkIfEnd(char const *seq);

public:
    void requestTCPEcho();

    void requestTCPTime();

    void requestUDPEcho();

    void requestUDPTime();

    RequestManager(int socket);
};


#endif //SN_REQUESTMANAGER_H
