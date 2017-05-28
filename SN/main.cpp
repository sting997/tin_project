//
// Created by riuoku on 27.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <unistd.h>
#include <string>
#include "../protocol_codes.h"
#include "TicketCorrectnessTester.h"

#define LISTENQ 5
#define BUFFER_SIZE 1024

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name);
void fillSockaddr_in(struct sockaddr_in &name, short sin_family, unsigned long s_addr, unsigned short sin_port);

void prepareRefuseBuffer(char * buf, int errNum);
int checkIfEnd(char const* buf, char const* seq);

int maxFd1(int fd, int fd2, int fd3, int fd4);
void UDPEcho(int fd, char* buf);
void UDPTime(int fd, char* buf);
void TCPEcho(int fd, char* buf);
void TCPTime(int fd, char* buf);

int main()
{
    int tcpfd, tcpfd2, udpfd, udpfd2, nready, maxfdp1;
    fd_set rset;
    struct sockaddr_in servaddr;
    char buf[BUFFER_SIZE];

    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 5000);
    prepareSocket(tcpfd, AF_INET, SOCK_STREAM, 0, servaddr);
    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 5001);
    prepareSocket(tcpfd2, AF_INET, SOCK_STREAM, 0, servaddr);

    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 6000);
    prepareSocket(udpfd, AF_INET, SOCK_DGRAM, 0, servaddr);
    fillSockaddr_in(servaddr, AF_INET, INADDR_ANY, 6001);
    prepareSocket(udpfd2, AF_INET, SOCK_DGRAM, 0, servaddr);

    maxfdp1 = maxFd1(tcpfd, tcpfd2, udpfd, udpfd2);
    while(true) {
        FD_ZERO(&rset);
        FD_SET(tcpfd, &rset);
        FD_SET(tcpfd2, &rset);
        FD_SET(udpfd, &rset);
        FD_SET(udpfd2, &rset);

        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            perror("Something bad happened with select");
            exit(0);
        }

        if (FD_ISSET(tcpfd, &rset))
            TCPEcho(tcpfd, &buf[0]);

        if (FD_ISSET(tcpfd2, &rset))
            TCPTime(tcpfd2, &buf[0]);

        if (FD_ISSET(udpfd, &rset))
            UDPEcho(udpfd, &buf[0]);

        if (FD_ISSET(udpfd2, &rset))
            UDPTime(udpfd2, &buf[0]);
    }
}

void fillSockaddr_in(struct sockaddr_in &name, short sin_family, unsigned long s_addr, unsigned short sin_port){
    /* Create name with wildcards. */
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void prepareSocket(int &fd, int domain, int type, int protocol, struct sockaddr_in name){
    socklen_t len;
    fd = socket(domain, type, protocol);

    if (fd == -1) {
        perror("opening socket");
        exit(1);
    }

    if (bind(fd,(struct sockaddr *)&name, sizeof name) == -1) {
        perror("binding socket");
        exit(1);
    }

    if (type == SOCK_STREAM && (listen(fd, LISTENQ) == -1)) {
        perror("listen on socket");
        exit(1);
    }
}

int maxFd1(int fd, int fd2, int fd3, int fd4) {
    int max = fd;

    if (max < fd2)
        max = fd2;

    if (max < fd3)
        max = fd3;

    if (max < fd4)
        max = fd4;

    return max + 1;
}

void UDPEcho(int fd, char* buf) {
    int n, k;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    n = recvfrom(fd, buf, BUFFER_SIZE, 0,(struct sockaddr *) &cliaddr, &len);

    if((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        buf[0] = SERVICE_GRANTED;
    }
    else {
        prepareRefuseBuffer(buf, k);
    }
    sendto(fd, buf, n, 0, (struct sockaddr *) &cliaddr, len);
}

void UDPTime(int fd, char* buf) {
    int n, k;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    n = recvfrom(fd, buf, BUFFER_SIZE, 0,(struct sockaddr *) &cliaddr, &len);

    if((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        bzero(buf, BUFFER_SIZE);
        buf[0] = SERVICE_GRANTED;
        //Sends a time in seconds since the Epoch
        std::time_t result = std::time(nullptr);
        memcpy(buf+1, &result, sizeof(&result));
    }
    else {
        prepareRefuseBuffer(buf, k);
    }
    sendto(fd, buf, n, 0, (struct sockaddr *) &cliaddr, len);
}

void TCPEcho(int fd, char* buf) {
    int connfd, k;
    int endPos;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int rval, charsread;

    if ((connfd = accept(fd, (struct sockaddr *) &cliaddr, &len)) == -1)
        perror("accept");

    if (fork() == 0) {
        /* child process */
        close(fd);
        bzero(buf, BUFFER_SIZE);

        if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1)
            perror("Reading stream message");
        if((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
            buf[0] = SERVICE_GRANTED;
        }
        else {
            prepareRefuseBuffer(buf, k);
            write(connfd, buf, strlen(buf));
            _exit(0);
        }
        if((endPos = checkIfEnd(buf,"END")) != std::string::npos) {
            write(connfd, buf, endPos);
        }
        else {
            FILE* pFile;
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
                if(rval == 0) {
                    printf("Premature end of echo connection. Closing service.\n");

                    fclose(pFile);
                    remove(fileName);

                    _exit(0);
                }
                else if((endPos = checkIfEnd(buf,"END")) != std::string::npos) {
                    fwrite(buf, sizeof(char), endPos, pFile);
                    break;
                }
                else
                    fwrite(buf, sizeof(char), strlen(buf), pFile);
            }while(true);

            rewind(pFile);
            bzero(buf, sizeof buf);

            do{
                charsread = fread(buf, sizeof(char), BUFFER_SIZE, pFile);
                write(connfd, buf, charsread);
            }while(charsread == 1024);

            fclose(pFile);
            remove(fileName);
        }
        _exit(0);
    }
    close(connfd);
}

void TCPTime(int fd, char* buf) {
    int connfd, k;
    int endPos;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int rval, charsread;

    if ((connfd = accept(fd, (struct sockaddr *) &cliaddr, &len)) == -1) {
        perror("accept");
        return;
    }

    bzero(buf, BUFFER_SIZE);

    if ((rval = read(connfd, buf, BUFFER_SIZE)) == -1) {
        perror("Reading stream message");
        close(connfd);
        return;
    }
    if((k = TicketCorrectnessTester::CheckTicket(buf)) == 0) {
        bzero(buf, BUFFER_SIZE);
        buf[0] = SERVICE_GRANTED;
        std::time_t result = std::time(nullptr);
        memcpy(buf+1, &result, sizeof(&result));
    }
    else {
        prepareRefuseBuffer(buf, k);
    }
    write(connfd, buf, strlen(buf));
    close(connfd);
}

int checkIfEnd(char const* buf, char const* seq) {
    std::string bufs(buf);
    std::string subs(seq);

    auto pos = bufs.find(seq);
    return pos;
}

void prepareRefuseBuffer(char * buf, int errNum) {
    bzero(buf, BUFFER_SIZE);
    buf[0] = SERVICE_REFUSED;
    switch(errNum) {
        case 1:
            memcpy(buf+1, "Invalid Ticket", sizeof("Invalid Ticket"));
            break;
        case 2:
            memcpy(buf+1, "Invalid IP address", sizeof("Invalid IP address"));
            break;
        case 3:
            memcpy(buf+1, "Ticket has expired", sizeof("Ticket has expired"));
            break;
        case 4:
            memcpy(buf+1, "Invalid data format", sizeof("Invalid data format"));
            break;
    }
}