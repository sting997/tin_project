//
// Created by monspid on 06.05.17.
//

#ifndef TIN_REQUESTMANAGER_H
#define TIN_REQUESTMANAGER_H

#include <cstring>
#include <iostream>
#include "../protocol_codes.h"
#include "Ticket.h"
#include "PrivilegeManager.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <arpa/inet.h>
#include <iostream>

class RequestManager {
    int sock;
    char buf[1024];
    struct sockaddr_in remote;
    socklen_t len = sizeof(remote);
    ssize_t n;

    bool shouldPerform(char code);

    void sendMessage(int sock, char code, std::string message);

    std::string getAuthData(std::string buf);

    char getRequestCode(char *message);

public:
    void requestIP();

    void requestTicket();

    RequestManager(int socket);
};


#endif //TIN_REQUESTMANAGER_H
