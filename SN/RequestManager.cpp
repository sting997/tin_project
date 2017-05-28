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
    unsigned long endPos;
    ssize_t rval;
    size_t charsread;

    acceptConnection();

    if (fork() == 0) {
        close(sock);
        bzero(buf, BUFFER_SIZE);

        if (read(connfd, buf, BUFFER_SIZE) == -1)
            perror("Reading stream message");

        if ((ticket_correctness = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
            buf[0] = SERVICE_GRANTED;
        } else {
            prepareRefuseBuffer(ticket_correctness);
            write(connfd, buf, strlen(buf));
            _exit(0);
        }
        if ((endPos = checkIfEnd("END")) != std::string::npos) {
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
                } else if ((endPos = checkIfEnd("END")) != std::string::npos) {
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

void RequestManager::TCPTime() {
    int ticket_correctness;

    acceptConnection();

    bzero(buf, BUFFER_SIZE);

    if (read(connfd, buf, BUFFER_SIZE) == -1) {
        perror("Reading stream message");

        close(connfd);
        return;
    }

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

unsigned long RequestManager::checkIfEnd(char const *seq) {
    std::string bufs(buf);
    std::string subs(seq);

    return bufs.find(seq);
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

RequestManager::RequestManager(int socket, int connectionType) {
    sock = socket;
    type = connectionType;
}