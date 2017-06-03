//
// Created by monspid on 06.05.17.
//

#include "RequestManager.h"

void RequestManager::UDPEcho() {
    int ticket_correctness;

    if (receiveMessage() <= 0) {
        perror("Read failed:");
        return;
    }

    std::vector<std::string> splitBuffer = getSplitData(_message);

    TicketCorrectnessTester tester;
    if ((ticket_correctness = tester.checkTicket(splitBuffer[0], inet_ntoa(remote.sin_addr), serverID, UDP_ECHO_SERVICE)) == TICKET_CORRECT)
        prepareBuffer(SERVICE_GRANTED, splitBuffer[1]);
    else
        prepareRefuseBuffer(ticket_correctness);

    sendMessage(sock, _message);
}

void RequestManager::UDPTime() {
    int ticket_correctness;

    if (receiveMessage() <= 0) {
        perror("Read failed:");
        return;
    }

    std::vector<std::string> splitBuffer = getSplitData(_message);

    TicketCorrectnessTester tester;
    if ((ticket_correctness = tester.checkTicket(splitBuffer[0], inet_ntoa(remote.sin_addr), serverID, UDP_TIME_SERVICE)) == TICKET_CORRECT){
        time_t current_time = time(nullptr);
        prepareBuffer(SERVICE_GRANTED, ctime(&current_time));
    } else
        prepareRefuseBuffer(ticket_correctness);

    sendMessage(sock, _message);
}

void RequestManager::sendMessage(int sock, std::string message) {
    if(sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr *) &remote, sizeof remote) <= 0 ){
        perror("Send Message:");
    }
}

void RequestManager::TCPEcho() {
    int ticket_correctness;

    acceptConnection();

    if (fork() == 0) {
        close(sock);

        if (readOnTCP() <= 0)
            return;
        std::cerr<<_message<<std::endl;


        std::vector<std::string> splitBuffer = getSplitData(_message);

        TicketCorrectnessTester tester;
        if ((ticket_correctness = tester.checkTicket(splitBuffer[0], inet_ntoa(remote.sin_addr), serverID, TCP_ECHO_SERVICE)) == TICKET_CORRECT)
            prepareBuffer(SERVICE_GRANTED, splitBuffer[1]);
        else {
            prepareRefuseBuffer(ticket_correctness);
            sendMessage(connfd, _message);

            _exit(0);
        }

        generateFileName();

        writeTCPEchoToFile();
        sendTCPEchoFromFile();

        //remove(fileName);
        _exit(0);
    }
    close(connfd);
}

ssize_t RequestManager::readOnTCP() {
    ssize_t rval;

    if ((rval = receiveMessage()) == -1) {
        log.error("Reading stream message:");
        close(connfd);
    } else if (rval == 0) {
        log.info("Client has disconnected. Closing TCP service.");
        close(connfd);
    }
    return rval;
}

void RequestManager::TCPTime() {
    int ticket_correctness;

    acceptConnection();

    if (readOnTCP() <= 0)
        return;

    std::vector<std::string> splitBuffer = getSplitData(_message);

    TicketCorrectnessTester tester;
    if ((ticket_correctness = tester.checkTicket(splitBuffer[0], inet_ntoa(remote.sin_addr), serverID, TCP_TIME_SERVICE)) == TICKET_CORRECT){
        time_t current_time = time(nullptr);
        prepareBuffer(SERVICE_GRANTED, ctime(&current_time));
    } else
        prepareRefuseBuffer(ticket_correctness);

    sendMessage(sock, _message);

    close(connfd);
}

unsigned long RequestManager::msgEndPosition() {
    return _message.find(msgEndIndicator);
}

bool RequestManager::checkIfLastMsg() {
    return msgEndPosition() != std::string::npos;
}

void RequestManager::prepareBuffer(char flag, std::string message) {
    _message = flag + message;
}

void RequestManager::prepareRefuseBuffer(int errNum) {
    switch (errNum) {
        case 1:
            _message = "Invalid Ticket";
            break;
        case 2:
            _message = "Invalid IP address";
            break;
        case 3:
            _message = "Ticket has expired";
            break;
        default:
            _message = "Invalid data format";
            break;
    }
    prepareBuffer(SERVICE_REFUSED, _message);
}

void RequestManager::acceptConnection() {
    if ((connfd = accept(sock, (struct sockaddr *) &remote, &len)) == -1)
        log.error("Socket was not accepted");
}

void RequestManager::writeTCPEchoToFile() {
    FILE *pFile = fopen(fileName.c_str(), "w+");
    if(checkIfLastMsg())
        fwrite(_message.substr(1).c_str(), sizeof(char), msgEndPosition() - 1, pFile);
    else
        fwrite(_message.substr(1).c_str(), sizeof(char), _message.length() - 1 , pFile);

    while (!checkIfLastMsg()){
        if (readOnTCP() <= 0) {
            fclose(pFile);
            remove(fileName.c_str());

            _exit(0);
        }
        if (checkIfLastMsg()) {
            fwrite(_message.substr(1).c_str(), sizeof(char), msgEndPosition() - 1, pFile);
            break;
        } else {
            fwrite(_message.substr(1).c_str(), sizeof(char), _message.length() - 1, pFile);
        }
    };
    fclose(pFile);
}

void RequestManager::sendTCPEchoFromFile() {
    FILE *pFile = fopen(fileName.c_str(), "r");
    char temp[1024];
    size_t charsread;
    do {
        charsread = fread(temp, sizeof(char), BUFFER_SIZE, pFile);
        prepareBuffer(SERVICE_GRANTED, temp);
        sendMessage(connfd, _message);
    } while (charsread == BUFFER_SIZE);
    fclose(pFile);
}

void RequestManager::requestEcho() {
    if (type == SOCK_DGRAM)
        UDPEcho();
    else
        TCPEcho();
}

void RequestManager::generateFileName() {
    fileName = getpid();
}

void RequestManager::requestTime() {
    if (type == SOCK_DGRAM)
        UDPTime();
    else
        TCPTime();
}

RequestManager::RequestManager(int socket, int connectionType = SOCK_DGRAM) {
    sock = socket;
    connfd = socket;
    type = connectionType;
}

std::vector<std::string> RequestManager::getSplitData(std::string data) {
    std::vector<std::string> split_data;

    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, DELIMITER))
        split_data.push_back(token);

    return split_data;
}

ssize_t RequestManager::receiveMessage() {
    ssize_t n;

    char temp[1024];
    bzero(temp, 1024);

    len = sizeof(remote);

    n = recvfrom(connfd, temp, 1024, 0, (struct sockaddr *) &remote, &len);

    _message.clear();
    _message = temp;

    return n;
}