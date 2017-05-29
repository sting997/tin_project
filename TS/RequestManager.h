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
#include <string>

class RequestManager {
    int sock;
    char buf[1024];
    struct sockaddr_in remote;
    socklen_t len = sizeof(remote);

    void sendMessage(char code, std::string message);

    std::string getAuthData(std::string buf);

    char getRequestCode();

    void requestIP();

    void requestTicket();

    std::vector<std::string> getSplitData(std::string data);

    int getTicketTimeValidity(std::vector<std::string> split_auth_data);

public:

    void listenForRequests();

    RequestManager(int socket);
};


#endif //TIN_REQUESTMANAGER_H
