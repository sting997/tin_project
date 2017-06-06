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

    std::cout << "Input your password: ";
    passwd.clear();
    std::cin >> passwd;
	passwd = calculateMD5(passwd);
}

void RequestManager::getUserInput() {
    buf.clear();
    while(std::getline(std::cin, buf) && buf.length() == 0) {}
}

bool RequestManager::checkIfLastMsg() {
    return msgEndPosition() != std::string::npos;
}

unsigned long RequestManager::msgEndPosition() {
    return buf.find(msgEndIndicator);
}

void RequestManager::RequestTCPEcho() {
    if (!userServAddrInput()) {
        std::cout << "Invalid input." << std::endl;
        return;
    }

    prepareSocket(PORT_TCP_ECHO, SOCK_STREAM, inet_addr(serverIP.c_str()));

    ssize_t rval;

    std::pair<std::string, std::string> ticketKey;
    ticketKey.first = serverID;
    ticketKey.second = "3";

    if (ticketManager.contains(ticketKey)) {
        std::string ticket = ticketManager.getTicket(ticketKey);

        log.info("To ECHO: ");

        getUserInput();
        sendTicketAndMessage(sock, ticket, buf);

        while (!checkIfLastMsg()) {
            getUserInput();
            sendTCPEchoMessage();
        }

        while (true) {
            if ((rval = receiveMessage()) == -1) {
                log.error("Read failed:");
                exit(1);
            }
            if (rval == 0) {
                log.info("Ending connection.");
                break;
            } else
                PrintMessage();
        }
    } else
        std::cout << "You do not possess a valid ticket!\nGet one and try again.\n";
    while (true) {
        if ((rval = receiveMessage()) == -1) {
            log.error("Read failed:");
            exit(1);
        }
        if (rval == 0) {
            log.info("Ending connection.");
            break;
        } else
            PrintMessage();
    }
    close(sock);
}

void RequestManager::sendTCPEchoMessage() {
    if(checkIfLastMsg())
        sendMessage(sock, 1, buf.substr(0, msgEndPosition() + msgEndIndicator.length()));
    else
        sendMessage(sock, 1, buf);
}

void RequestManager::RequestTCPTime() {
    if (!userServAddrInput()) {
        log.warn("Invalid input.");
        return;
    }

    prepareSocket(PORT_TCP_TIME, SOCK_STREAM, inet_addr(serverIP.c_str()));

    ssize_t rval;

    std::pair<std::string, std::string> ticketKey;
    ticketKey.first = serverID;
    ticketKey.second = "4";

    if (ticketManager.contains(ticketKey)) {
        std::string ticket = ticketManager.getTicket(ticketKey);

        sendTicketAndMessage(sock, ticket, "PL");

        if ((rval = receiveMessage()) == -1) {
            log.error("Read failed:");
            exit(1);
        }
        if (rval == 0) {
            log.info("Service server disconnected.");
        } else {
            PrintMessage();
        }
    } else {
        std::cout << "You do not possess a valid ticket!\nGet one and try again.\n";
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
    unsigned long ticket_size = ticket.size();
    std::string response = std::to_string(ticket_size) + DELIMITER + ticket + message;
    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &name, sizeof name);
}

void RequestManager::prepareBroadcastSocket(unsigned short port) {
//    prepareSocket(port, SOCK_DGRAM, INADDR_BROADCAST);
     prepareSocket(port, SOCK_DGRAM, INADDR_ANY);

    int val = 1;

    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
}

void RequestManager::prepareSocket(unsigned short port, int type, in_addr_t netlong) {
    sock = socket(PF_INET, type, 0);

    if (sock == -1) {
        log.error("Opening datagram socket");
        return;
    }

    setTimeout(sock, 3, 0);

    memset(&name, 0, sizeof(name));
    fillSockaddr_in(name, AF_INET, netlong, port);

    if (type == SOCK_STREAM && connect(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
        log.error("Connect failed:");
    }
}

ssize_t RequestManager::receiveMessage() {
    char temp[1024];
    bzero(temp, 1024);

    len = sizeof(remote);

    ssize_t n = recvfrom(sock, temp, 1024, 0, (struct sockaddr *) &remote, &len);

    buf.clear();
    buf = temp;

    return n;
}

bool RequestManager::RequestIP() {
    prepareBroadcastSocket(TS_PORT);

    sendMessage(sock, TS_REQ_IP, "");
    if (receiveMessage() < 0) {
        log.error("Receiving IP from TS server.");
        close(sock);
        return false;
    }

    if (buf[0] == TS_IP) {
        log.info("Received package from TS: %s", inet_ntoa(remote.sin_addr));
    } else {
        log.error("receiving IP from TS server.");
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

    if (!isNumeric(userInput))
        return false;
    serverID = userInput;

    userInput.clear();

    std::cout << "Input service ID: ";
    std::cin >> userInput;

    if (!isNumeric(userInput))
        return false;
    serviceID = userInput;

    return true;
}

void RequestManager::RequestTicket() {
    /* Create socket on which to send. */
    if (!userTicketInput()) {
        log.warn("Invalid input.");
        return;
    }

    prepareSocket(TS_PORT, SOCK_DGRAM, remote.sin_addr.s_addr);

    std::string message = serverID + DELIMITER + serviceID + DELIMITER + login + DELIMITER + passwd;

    sendMessage(sock, TS_REQ_TICKET, message);

    if (receiveMessage() < 0) {
        log.error("Receiving data");
        close(sock);
        return;
    }

    if (buf[0] == TS_GRANTED) {
        std::pair<std::string, std::string> key(serverID, serviceID);
        ticketManager.addTicket(key, GetTicketData());

        log.info("Received a ticket %s", buf);

    } else if (buf[0] == TS_REFUSED)
        log.warn("TS didn't give me a ticket!!!");
    else {
        log.warn("Received roaming package, didn't want it though!");
    }
    close(sock);
}

std::string RequestManager::GetTicketData() {
    return buf.substr(1);
}

void RequestManager::userEchoInput() {
    std::string userInput;

    std::cout << "Input data to be echoed: ";

    echoData.clear();
    while(echoData.length() == 0)
        std::getline(std::cin, echoData);
}

bool RequestManager::userServAddrInput() {
    std::string userInput;

    std::cout << "Input server IP: ";
    std::cin >> userInput;

    if (!isIPAddr(userInput))
        return false;
    serverIP = userInput;

    userInput.clear();

    std::cout << "Input server ID: ";
    std::cin >> userInput;

    if (!isNumeric(userInput))
        return false;
    serverID = userInput;

    return true;
}

bool RequestManager::isIPAddr(std::string ipAddr) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ipAddr.c_str(), &(sa.sin_addr)) == 1;
}

void RequestManager::RequestUDPService(int serviceType) {

    if (!userServAddrInput()) {
        log.warn("Invalid input.");
        return;
    }

    std::pair<std::string, std::string> ticketKey;
    ticketKey.first = serverID;

    if (serviceType == UDP_ECHO_SERVICE) {
        userEchoInput();
        prepareSocket(PORT_UDP_ECHO, SOCK_DGRAM, inet_addr(serverIP.c_str()));
        ticketKey.second = "1";
    }
    if (serviceType == UDP_TIME_SERVICE) {
        prepareSocket(PORT_UDP_TIME, SOCK_DGRAM, inet_addr(serverIP.c_str()));
        ticketKey.second = "2";
    }

    if (ticketManager.contains(ticketKey)) {
        std::string ticket = ticketManager.getTicket(ticketKey);

        if (serviceType == UDP_ECHO_SERVICE)
            sendTicketAndMessage(sock, ticket, echoData);
        if (serviceType == UDP_TIME_SERVICE)
            sendTicketAndMessage(sock, ticket, "");

        if (receiveMessage() < 0) {
            log.error("Receiving data");
            close(sock);
            return;
        }

        if (buf[0] == SERVICE_GRANTED) {
            log.info("Ticket Accepted.");
            PrintMessage();
        } else if (buf[0] == SERVICE_REFUSED) {
            log.warn("Ticket Refused.");
            PrintMessage();
        } else
            log.warn("Received roaming package, didn't want it though!");
    } else {
        log.warn("You do not have a valid ticket! Get one and try again.");
    }

    close(sock);
}

void RequestManager::PrintMessage() {
    std::cout << buf.substr(1) << std::endl;
}

void RequestManager::RequestUDPEcho() {
    RequestUDPService(UDP_ECHO_SERVICE);
}

void RequestManager::RequestUDPTime() {
    RequestUDPService(UDP_TIME_SERVICE);
}

std::string RequestManager::calculateMD5(std::string message){
	CryptoPP::MD5 hash;
	byte digest[ CryptoPP::MD5::DIGESTSIZE ];

	hash.CalculateDigest( digest, (byte*) message.c_str(), message.length() );

	CryptoPP::HexEncoder encoder;
	std::string output;
	encoder.Attach( new CryptoPP::StringSink( output ) );
	encoder.Put( digest, sizeof(digest) );
	encoder.MessageEnd();
	return output;
}
