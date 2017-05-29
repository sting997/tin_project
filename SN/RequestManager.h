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
    int type;
    char buf[BUFFER_SIZE];
    char fileName[8];
    static constexpr char * msgEndIndicator = (char *) "END";
    size_t msgEndPos;
    struct sockaddr_in remote;
    socklen_t len = sizeof(remote);

    void TCPEcho();

    void TCPTime();

    void UDPEcho();

    void UDPTime();

    void acceptConnection();

    ssize_t readOnTCP();

    void prepareRefuseBuffer(int errNum);

    void prepareTimeBuffer();

    void prepareFileName();

    void saveTCPEcho();

    void sendTCPEcho();

    unsigned long msgEndPosition();

    bool checkIfLastMsg();

public:
    void requestEcho();

    void requestTime();

    RequestManager(int socket, int connectionType);

};


#endif //SN_REQUESTMANAGER_H
