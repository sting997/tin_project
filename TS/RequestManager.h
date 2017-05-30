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
#include "DBManager.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <arpa/inet.h>
#include <iostream>
#include <string>

using namespace std;

class RequestManager {
    int sock;
    char buf[1024];
    struct sockaddr_in remote;
    socklen_t len = sizeof(remote);

    void sendMessage(char code, string message);

    string getAuthData(string buf);

    char getRequestCode();

    void requestIP();

    void requestTicket();

    vector<string> getSplitData(string data);

    int getTicketTimeValidity(vector<string> split_auth_data);

    string prepareTicketToEncryption(string ip, string serverNr, string serviceNr, int ticketValidityTime);

public:

    void listenForRequests();

    RequestManager(int socket);
};


#endif //TIN_REQUESTMANAGER_H
