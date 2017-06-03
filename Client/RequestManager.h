//
// Created by monspid on 06.05.17.
//

#ifndef TIN_REQUESTMANAGER_H
#define TIN_REQUESTMANAGER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include <iostream>
#include "../protocol_codes.h"
#include "RequestManager.h"
#include "config.h"
#include "TicketManager.h"



class RequestManager {
    int sock;
    struct sockaddr_in remote, name;
    const std::string msgEndIndicator = "END";
	std::string serverIP, serverID, serviceID;
	std::string echoData;
    std::string buf;
    socklen_t len;
	TicketManager ticketManager;
	std::string login = "admin";
	std::string passwd = "admin";

    void sendMessage(int sock, char code, std::string message);

    ssize_t receiveMessage();

    void setTimeout(int socket, time_t tv_sec, long int tv_usec);

    void fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr, unsigned short sin_port);

    void prepareBroadcastSocket(unsigned short port);

    void prepareSocket(unsigned short  port, int type, uint32_t netlong);

    bool checkIfLastMsg();

    void getUserInput();

    unsigned long msgEndPosition();

    std::string GetTicketData();

	bool userTicketInput();

	void userEchoInput();

	bool userServAddrInput();

	void userDataInput();

	bool isIPAddr(std::string ipAddr);

	void RequestUDPService(int serviceType);

	void sendTicketAndMessage(int sock, std::string ticket, std::string message);

	void PrintMessage();

	void sendTCPEchoMessage();

public:
    RequestManager();

    bool RequestIP();

    void RequestTicket();

    void RequestUDPEcho();

    void RequestUDPTime();

    void RequestTCPEcho();

    void RequestTCPTime();

    void RequestNewData();
};

#endif //TIN_REQUESTMANAGER_H
