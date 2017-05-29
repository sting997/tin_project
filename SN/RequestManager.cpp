//
// Created by monspid on 06.05.17.
//

#include "RequestManager.h"


void RequestManager::UDPEcho() {
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    ssize_t n = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
    int ticket_correctness;

    if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) == 0)
        buf[0] = SERVICE_GRANTED;
    else
        prepareRefuseBuffer(ticket_correctness);

    sendto(sock, buf, (size_t) n, 0, (struct sockaddr *) &cliaddr, len);
}

void RequestManager::UDPTime() {
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    ssize_t n = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
    int ticket_correctness;

    if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) == 0)
        prepareTimeBuffer();
    else
        prepareRefuseBuffer(ticket_correctness);

    sendto(sock, buf, size_t(n), 0, (struct sockaddr *) &cliaddr, len);
}

void RequestManager::TCPEcho() {
    int ticket_correctness;

    acceptConnection();

    if (fork() == 0) {
        close(sock);

        if(readOnTCP() <= 0)
            return;

        if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) != 0) {
            prepareRefuseBuffer(ticket_correctness);

            write(connfd, buf, strlen(buf));
            _exit(0);
        }

        buf[0] = SERVICE_GRANTED;

        if (checkIfLastMsg()) {
            write(connfd, buf, msgEndPos);
        } else {

            prepareFileName();

            saveTCPEcho();
            sendTCPEcho();

            remove(fileName);
        }
        _exit(0);
    }
    close(connfd);
}

ssize_t RequestManager::readOnTCP() {
    ssize_t rval;
    bzero(buf, BUFFER_SIZE);

    if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1) {
        perror("Reading stream message:");
        close(connfd);
    } else if (rval == 0){
        printf("Client has disconnected. Closing TCP service.\n");
        close(connfd);
    }
    msgEndPos = msgEndPosition();

    return rval;
}

void RequestManager::TCPTime() {
    int ticket_correctness;

    acceptConnection();

    if(readOnTCP() <= 0)
        return;

    if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) == 0)
        prepareTimeBuffer();
    else
        prepareRefuseBuffer(ticket_correctness);

    write(connfd, buf, strlen(buf));
    close(connfd);
}

void RequestManager::prepareTimeBuffer() {
    bzero(buf, BUFFER_SIZE);
    buf[0] = SERVICE_GRANTED;

    time_t result = time(nullptr);
    memcpy(buf + 1, &result, sizeof(&result));
}

unsigned long RequestManager::msgEndPosition() {
    std::string bufs(buf);

    return bufs.find(msgEndIndicator);
}

bool RequestManager::checkIfLastMsg() {
    return msgEndPos != std::string::npos;
}

void RequestManager::prepareRefuseBuffer(int errNum) {
    char * message;
    bzero(buf, BUFFER_SIZE);
    buf[0] = SERVICE_REFUSED;

    switch (errNum) {
        case 1:
            message = (char *) "Invalid Ticket";
            break;
        case 2:
            message = (char *) "Invalid IP address";
            break;
        case 3:
            message = (char *) "Ticket has expired";
            break;
        default:
            message = (char *) "Invalid data format";
            break;
    }

    memcpy(buf + 1, message, sizeof(message));
}

void RequestManager::acceptConnection() {
    if ((connfd = accept(sock, (struct sockaddr *) &remote, &len)) == -1)
        perror("accept");
}

void RequestManager::saveTCPEcho() {
    FILE* pFile = fopen(fileName, "w+");
    fwrite(buf, sizeof(char), strlen(buf), pFile);
    do {
        bzero(buf, sizeof buf);
        if(readOnTCP() <= 0) {
            fclose(pFile);
            remove(fileName);

            _exit(0);
        }

        if (checkIfLastMsg()) {
            fwrite(buf, sizeof(char), msgEndPos, pFile);
            break;
        } else
            fwrite(buf, sizeof(char), strlen(buf), pFile);
    } while (true);

    fclose(pFile);
}

void RequestManager::sendTCPEcho() {
    FILE* pFile = fopen(fileName, "r");
    size_t charsread;

    do {
        charsread = fread(buf, sizeof(char), BUFFER_SIZE, pFile);
        write(connfd, buf, charsread);
    } while (charsread == 1024);

    fclose(pFile);
}

void RequestManager::requestEcho() {
    if (type == SOCK_DGRAM)
        UDPEcho();
    else
        TCPEcho();
}

void RequestManager::prepareFileName() {
    sprintf(fileName, "%d", getpid());
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
}