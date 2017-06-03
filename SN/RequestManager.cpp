//
// Created by monspid on 06.05.17.
//

#include "RequestManager.h"

void RequestManager::UDPEcho() {
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int ticket_correctness;

    recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);

    std::vector<std::string> splitBuffer = getSplitData(buf);

    TicketCorrectnessTester tester;
    if ((ticket_correctness = tester.checkTicket(splitBuffer[0], inet_ntoa(cliaddr.sin_addr), "1", "1")) == TICKET_CORRECT) {
        prepareBuffer(SERVICE_GRANTED, splitBuffer[1]);

    } else
        prepareRefuseBuffer(ticket_correctness);

    sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &cliaddr, len);
}

void RequestManager::UDPTime() {
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int ticket_correctness;

    recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);

    if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) == TICKET_CORRECT) {
        time_t current_time = time(nullptr);
        prepareBuffer(SERVICE_GRANTED, ctime(&current_time));
    } else
        prepareRefuseBuffer(ticket_correctness);

    sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &cliaddr, len);
}

void RequestManager::sendMessage(int sock, std::string message) {
    sendto(sock, message.c_str(), strlen(message.c_str()), 0, (struct sockaddr *) &name, sizeof name);
}

void RequestManager::TCPEcho() {
    int ticket_correctness;

    acceptConnection();

    if (fork() == 0) {
        close(sock);

        if (readOnTCP() <= 0)
            return;

        if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) != TICKET_CORRECT) {
            prepareRefuseBuffer(ticket_correctness);

            sendMessage(connfd, buf);

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
    bzero(buf, BUFFER_SIZE);

    if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1) {
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

    if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) == TICKET_CORRECT) {
        time_t current_time = time(nullptr);
        prepareBuffer(SERVICE_GRANTED, ctime(&current_time));
    } else
        prepareRefuseBuffer(ticket_correctness);

    write(connfd, buf, strlen(buf));
    close(connfd);
}

unsigned long RequestManager::msgEndPosition() {
    std::string bufs(buf);

    return bufs.find(msgEndIndicator);
}

bool RequestManager::checkIfLastMsg() {
    return msgEndPosition() != std::string::npos;
}

void RequestManager::prepareBuffer(char flag, std::string message) {
    bzero(buf, BUFFER_SIZE);
    string preparedBuffer = flag + message;
    memcpy(buf, preparedBuffer.c_str(), strlen(preparedBuffer.c_str()));
}

void RequestManager::prepareRefuseBuffer(int errNum) {
    std::string message;

    switch (errNum) {
        case 1:
            message = "Invalid Ticket";
            break;
        case 2:
            message = "Invalid IP address";
            break;
        case 3:
            message = "Ticket has expired";
            break;
        default:
            message = "Invalid data format";
            break;
    }
    prepareBuffer(SERVICE_REFUSED, message);
}

void RequestManager::acceptConnection() {
    if ((connfd = accept(sock, (struct sockaddr *) &remote, &len)) == -1)
        log.error("Socket was not accepted");
}

void RequestManager::writeTCPEchoToFile() {
    FILE *pFile = fopen(fileName, "w+");
    fwrite(buf + 1, sizeof(char), std::min(strlen(buf), msgEndPosition()) - 1, pFile);

    while (!checkIfLastMsg()){
        bzero(buf, 1024);
        if (readOnTCP() <= 0) {
            fclose(pFile);
            remove(fileName);

            _exit(0);
        }
        if (checkIfLastMsg()) {
            fwrite(buf + 1, sizeof(char), msgEndPosition() - 1, pFile);
            break;
        } else
            fwrite(buf + 1, sizeof(char), strlen(buf) - 1, pFile);
    };
    fclose(pFile);
}

void RequestManager::sendTCPEchoFromFile() {
    FILE *pFile = fopen(fileName, "r");
    char temp[1024];
    size_t charsread;
    do {
        charsread = fread(temp, sizeof(char), BUFFER_SIZE, pFile);
        fprintf(stderr,"11%s",buf);
        prepareBuffer(SERVICE_GRANTED, temp);
        fprintf(stderr,"22%s",buf);
        sendMessage(connfd, buf);
    } while (charsread == BUFFER_SIZE);
    fclose(pFile);
}

void RequestManager::generateFileName() {
    sprintf(fileName, "%d", getpid());
}

void RequestManager::requestEcho() {
    if (type == SOCK_DGRAM)
        UDPEcho();
    else
        TCPEcho();
}

void RequestManager::requestTime() {
    if (type == SOCK_DGRAM)
        UDPTime();
    else
        TCPTime();
}

RequestManager::RequestManager(int socket, int connectionType = SOCK_DGRAM) {
    sock = socket;
    type = connectionType;
    bzero(buf, BUFFER_SIZE);
}

std::vector<std::string> RequestManager::getSplitData(std::string data) {
    std::vector<std::string> split_data;

    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, DELIMITER))
        split_data.push_back(token);

    return split_data;
}