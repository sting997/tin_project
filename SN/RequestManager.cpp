//
// Created by monspid on 06.05.17.
//

#include "RequestManager.h"


void RequestManager::requestUDPEcho() {
    int n, k;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    n = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);

    if ((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        buf[0] = SERVICE_GRANTED;
    } else {
        RequestManager::prepareRefuseBuffer(k);
    }
    sendto(sock, buf, n, 0, (struct sockaddr *) &cliaddr, len);

}

void RequestManager::requestUDPTime() {
    int n, k;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    n = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);

    if ((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        bzero(buf, BUFFER_SIZE);
        buf[0] = SERVICE_GRANTED;
        //Sends a time in seconds since the Epoch
        std::time_t result = std::time(nullptr);
        memcpy(buf + 1, &result, sizeof(&result));
    } else {
        prepareRefuseBuffer(k);
    }
    sendto(sock, buf, n, 0, (struct sockaddr *) &cliaddr, len);
}

void RequestManager::requestTCPEcho() {
    int k;
    int endPos;
    int rval, charsread;

    RequestManager::acceptConnection();

    if (fork() == 0) {
        close(sock);
        bzero(buf, BUFFER_SIZE);

        if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1)
            perror("Reading stream message");

        if ((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
            buf[0] = SERVICE_GRANTED;
        } else {
            RequestManager::prepareRefuseBuffer(k);
            write(connfd, buf, strlen(buf));
            _exit(0);
        }
        if ((endPos = RequestManager::checkIfEnd("END")) != std::string::npos) {
            write(connfd, buf, endPos);
        } else {
            FILE *pFile;
            char fileName[64];
            sprintf(fileName, "%d", getpid());
            pFile = fopen(fileName, "w+");

            fwrite(buf, sizeof(char), strlen(buf), pFile);
            do {
                bzero(buf, sizeof buf);
                if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1) {
                    perror("Reading stream message");

                    fclose(pFile);
                    remove(fileName);

                    _exit(0);
                }
                if (rval == 0) {
                    printf("Premature end of TCP-ECHO REQUEST connection. Closing service.\n");

                    fclose(pFile);
                    remove(fileName);

                    _exit(0);
                } else if ((endPos = RequestManager::checkIfEnd("END")) != std::string::npos) {
                    fwrite(buf, sizeof(char), endPos, pFile);
                    break;
                } else
                    fwrite(buf, sizeof(char), strlen(buf), pFile);
            } while (true);

            rewind(pFile);
            bzero(buf, sizeof buf);

            do {
                charsread = fread(buf, sizeof(char), BUFFER_SIZE, pFile);
                write(connfd, buf, charsread);
            } while (charsread == 1024);

            fclose(pFile);
            remove(fileName);
        }
        _exit(0);
    }
    close(connfd);
}

void RequestManager::requestTCPTime() {
    int k, rval;

    RequestManager::acceptConnection();

    bzero(buf, BUFFER_SIZE);

    if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1) {
        perror("Reading stream message");
        close(connfd);
        return;
    }

    if ((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        bzero(buf, BUFFER_SIZE);
        buf[0] = SERVICE_GRANTED;
        std::time_t result = std::time(nullptr);
        memcpy(buf + 1, &result, sizeof(&result));
    } else {
        RequestManager::prepareRefuseBuffer(k);
    }

    write(connfd, buf, strlen(buf));
    close(connfd);
}

int RequestManager::checkIfEnd(char const *seq) {
    std::string bufs(buf);
    std::string subs(seq);

    auto pos = bufs.find(seq);
    return pos;
}

void RequestManager::prepareRefuseBuffer(int errNum) {
    bzero(buf, BUFFER_SIZE);
    buf[0] = SERVICE_REFUSED;
    switch (errNum) {
        case 1:
            memcpy(buf + 1, "Invalid Ticket", sizeof("Invalid Ticket"));
            break;
        case 2:
            memcpy(buf + 1, "Invalid IP address", sizeof("Invalid IP address"));
            break;
        case 3:
            memcpy(buf + 1, "Ticket has expired", sizeof("Ticket has expired"));
            break;
        case 4:
            memcpy(buf + 1, "Invalid data format", sizeof("Invalid data format"));
            break;
    }
}

void RequestManager::acceptConnection() {
    if ((connfd = accept(sock, (struct sockaddr *) &remote, &len)) == -1)
        perror("accept");
}

RequestManager::RequestManager(int socket) {
    sock = socket;
}