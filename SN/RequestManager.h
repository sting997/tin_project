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
#include <string>
#include <sstream>
#include <log4cpp/Category.hh>

#define BUFFER_SIZE 1024

class RequestManager {
    log4cpp::Category &log = log4cpp::Category::getInstance(LOGGER_NAME);
    int sock, connfd;
    int type;
    const std::string serverID = "1";
    std::string _message;
    std::string fileName;
    const std::string msgEndIndicator = "END";
    struct sockaddr_in remote, name;
    socklen_t len = sizeof(remote);

    void TCPEcho();

    void TCPTime();

    void UDPEcho();

    void UDPTime();

    void acceptConnection();

    ssize_t readOnTCP();

    void sendMessage(int sock, std::string message);

    void prepareRefuseBuffer(int errNum);

    void generateFileName();

    void writeTCPEchoToFile();

    void sendTCPEchoFromFile();

    unsigned long msgEndPosition();

    bool checkIfLastMsg();

    std::vector<std::string> getSplitData(std::string data);

    void prepareBuffer(char flag, std::string message);

    ssize_t receiveMessage();

public:
    void requestEcho();

    void requestTime();

    RequestManager(int socket, int connectionType);

};


#endif //SN_REQUESTMANAGER_H
