//
// Created by monspid on 06.05.17.
//

#include "RequestManager.h"

RequestManager::RequestManager() {
}

void RequestManager::RequestNewData() {
    userDataInput();
}

void RequestManager::userDataInput() {
    std::cout << "Input your ID: ";
    login.clear();
    std::cin >> login;

    std::cout << std::endl;

    std::cout << "Input your password: ";
    passwd.clear();
    std::cin >> passwd;

    std::cout << std::endl;
}

void RequestManager::getUserInput() {
    bzero(buf, 1024);
    //fflush(stdin);
    fgets(buf, 1024, stdin);
}

bool RequestManager::checkIfLastMsg() {
    return msgEndPosition() != std::string::npos;
}

unsigned long RequestManager::msgEndPosition() {
    std::string bufs(buf);

    return bufs.find(msgEndIndicator);
}

void RequestManager::RequestTCPEcho() {
    prepareSocket(PORT_TCP_ECHO, SOCK_STREAM, inet_addr("127.0.0.1"));

    ssize_t rval;

    //keep communicating with server
    while (true) {
        printf("To ECHO: ");
        getUserInput();

        sendMessage(sock, 1, buf);

        if (checkIfLastMsg())
            break;
    }

    while (true) {
        if ((rval = receiveMessage()) == -1) {
            perror("Read failed:");
            exit(1);
        }
        if (rval == 0) {
            printf("Ending connection.\n");
            break;
        } else
            printf("%s\n", buf);
    }
    close(sock);
}

void RequestManager::RequestTCPTime() {
    prepareSocket(PORT_TCP_TIME, SOCK_STREAM, inet_addr("127.0.0.1"));
    ssize_t rval;

    sendMessage(sock, 2, "PLS TCP TIME");

    if ((rval = receiveMessage()) == -1) {
        perror("Read failed:");
        exit(1);
    }
    if (rval == 0) {
        printf("Service server disconnected.\n");
    } else {
        printf("%s\n", std::asctime(std::localtime(reinterpret_cast<time_t *>(buf + 1))));
    }
    close(sock);
}

void RequestManager::setTimeout(int socket, time_t tv_sec, long int tv_usec) {
    struct timeval tv;
    tv.tv_sec = tv_sec;
    tv.tv_usec = tv_usec;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval));
}

void RequestManager::fillSockaddr_in(struct sockaddr_in &name, sa_family_t sin_family, in_addr_t s_addr,
                                     unsigned short sin_port) {
    name.sin_family = sin_family;
    name.sin_addr.s_addr = s_addr;
    name.sin_port = htons(sin_port);
}

void RequestManager::sendMessage(int sock, char code, std::string message) {
    std::string response = code + message;
    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &name, sizeof name);
}

void RequestManager::sendTicketAndMessage(int sock, std::string ticket, std::string message) {
    std::string response = ticket + ";" + message;
    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &name, sizeof name);
}

void RequestManager::prepareBroadcastSocket(int port) {
    prepareSocket(port, SOCK_DGRAM, INADDR_ANY);

    int val = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
}

void RequestManager::prepareSocket(unsigned short port, int type, in_addr_t netlong) {
    sock = socket(PF_INET, type, 0);

    if (sock == -1) {
        puts("Opening datagram socket");
        return;
    }

    setTimeout(sock, 3, 0);

    memset(&name, 0, sizeof(name));
    fillSockaddr_in(name, AF_INET, netlong, port);

    if (type == SOCK_STREAM && connect(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
        perror("Connect failed:");
    }
}

ssize_t RequestManager::receiveMessage() {
    ssize_t n;

    bzero(buf, 1024);
    len = sizeof(remote);

    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);

    return n;
}

bool RequestManager::RequestIP() {
    prepareBroadcastSocket(TS_PORT);

    sendMessage(sock, TS_REQ_IP, "");
    if (receiveMessage() < 0) {
        puts("Error: receiving IP from TS server.");
        close(sock);
        return false;
    }

    if (buf[0] == TS_IP) {
        printf("Received package from TS: %s\n", inet_ntoa(remote.sin_addr));
    } else {
        printf("Error: receiving IP from TS server.\n");
        close(sock);
        return false;
    }

    close(sock);
    return true;
}

bool isNumeric(std::string input) {
    size_t index = input.find_first_not_of("0123456789", 0);

    return !input.empty() && index == std::string::npos;
}

bool RequestManager::userTicketInput() {
    std::string serverId, serviceId;
    std::string userInput;

    std::cout << "Input server ID: ";
    std::cin >> userInput;
    std::cout << std::endl;

    if(!isNumeric(userInput))
        return false;
    serverID = userInput;

    userInput.clear();

    std::cout << "Input service ID: ";
    std::cin >> userInput;
    std::cout << std::endl;

    if(!isNumeric(userInput))
        return false;
    serviceID = userInput;

    return true;
}

void RequestManager::RequestTicket() {
    /* Create socket on which to send. */
    if(!userTicketInput()) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    prepareSocket(TS_PORT, SOCK_DGRAM, remote.sin_addr.s_addr);

    std::string message = serverID + ";" + serviceID + ";" + login + ";" + passwd;

    sendMessage(sock, TS_REQ_TICKET, message);

    if (receiveMessage() < 0) {
        puts("Error: receiving data");
        close(sock);
        return;
    }

    if (buf[0] == TS_GRANTED) {
        std::pair<std::string, std::string> key(serverID, serviceID);
        ticketManager.addTicket(key, GetTicketData());

        printf("I just received my ticket, whoooaaa!\n buf: %s\n", buf);

    } else if (buf[0] == TS_REFUSED)
        printf("TS didn't give me a ticket!!!\n");
    else {
        printf("Received roaming package, didn't want it though!\n");
    }
    close(sock);
}

std::string RequestManager::GetTicketData() {
    return buf+1;
}

void RequestManager::userEchoInput() {
    std::string userInput;

    std::cout << "Input data to be echoed: ";
    std::cin >> userInput;
    std::cout << std::endl;

    echoData = userInput;
}


bool RequestManager::userServAddrInput() {
    std::string userInput;

    std::cout << "Input server IP: ";
    std::cin >> userInput;
    std::cout << std::endl;

    if (!isIPAddr(userInput))
        return false;
    serverIP = userInput;

    userInput.clear();

    std::cout << "Input server ID: ";
    std::cin >> userInput;
    std::cout << std::endl;

    if (!isNumeric(userInput))
        return false;
    serverID = userInput;

    return true;
}

bool RequestManager::isIPAddr(std::string ipAddr) {
        struct sockaddr_in sa;
        return inet_pton(AF_INET, ipAddr.c_str(), &(sa.sin_addr)) == 1;
}

void RequestManager::RequestUDPEcho() {

    if(!userServAddrInput()) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    userEchoInput();

    prepareSocket(PORT_UDP_ECHO, SOCK_DGRAM, inet_addr(serverIP.c_str()));
	std::pair<std::string, std::string> ticketKey(serverID, "1"); //second parameter "1", because
																//this is udp echo service id
    if (ticketManager.contains(ticketKey)){
        std::string ticket = ticketManager.getTicket(ticketKey);
        sendTicketAndMessage(sock, ticket, echoData);

        if (receiveMessage() < 0) {
            puts("Error: receiving data");
            close(sock);
            return;
        }

        if (buf[0] == SERVICE_GRANTED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else if (buf[0] == SERVICE_REFUSED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else
            printf("Received roaming package, didn't want it though!\n");
    }
    else{
        std::cout<<"You do not possess a valid ticket!\nGet one and try again.\n";
    }

    close(sock);
}

void RequestManager::RequestUDPTime() {

    if(!userServAddrInput()) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    prepareSocket(PORT_UDP_TIME, SOCK_DGRAM, inet_addr(serverIP.c_str()));

    std::pair<std::string, std::string> ticketKey(serverID, "2");

    if (ticketManager.contains(ticketKey)) {
        std::string ticket = ticketManager.getTicket(ticketKey);

        sendTicketAndMessage(sock, ticket, echoData);
        if (receiveMessage() < 0) {
            puts("Error: receiving data");
            close(sock);
            return;
        }

        if (buf[0] == SERVICE_GRANTED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", std::asctime(std::localtime(reinterpret_cast<time_t *>(buf + 1))));
        } else if (buf[0] == SERVICE_REFUSED) {
            printf("Received package from service server: %s\n", inet_ntoa(remote.sin_addr));
            printf("%s\n", (buf + 1));
        } else
            printf("Received roaming package, didn't want it though!\n");
    }
    else{
        std::cout<<"You do not possess a valid ticket!\nGet one and try again.\n";
    }

    close(sock);
}
