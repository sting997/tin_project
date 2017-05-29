//
// Created by monspid on 06.05.17.
//
#include "RequestManager.h"
#include "DBManager.h"

char RequestManager::getRequestCode() {
    return strlen(buf) > 0 ? buf[0] : ERROR;
}

void RequestManager::requestIP() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    sendMessage(TS_IP, "");
}

void RequestManager::requestTicket() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    std::string auth_data = getAuthData(buf);
    std::vector<std::string> split_auth_data = getSplitData(auth_data);

    PrivilegeManager privilege_manager = PrivilegeManager();

    int privilege_status = privilege_manager.getPrivilegeInfo(inet_ntoa(remote.sin_addr), split_auth_data);
    char grant_status;
    std::string message;

    if (privilege_status == 3) {
        grant_status = TS_GRANTED;
        int ticket_time_validity = getTicketTimeValidity(split_auth_data); // add it to time.now()

        Ticket ticket;
        message = ticket.createTicket("oto jest ticket");//change the argument to a string based on privilege info
    } else {
        grant_status = TS_REFUSED;
        message = std::to_string(privilege_status);
    }

    sendMessage(grant_status, message);
}

int RequestManager::getTicketTimeValidity(std::vector<std::string> split_auth_data) {
    DBManager dbManager = DBManager();

    std::string ticket_time_validity_line = dbManager.getTicketTimeValidityLine(split_auth_data[0], split_auth_data[1]);
    std::vector<std::string> split_ticket_time_validity_line = getSplitData(ticket_time_validity_line);

    return split_ticket_time_validity_line.size() == 3 ? stoi(split_ticket_time_validity_line[2]) : 0;
}

void RequestManager::sendMessage(char code, std::string message) {
    std::string response = code + message;
    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &remote, len);
}

std::string RequestManager::getAuthData(std::string buf) {
    buf.erase(0, 1);

    return buf;
}

RequestManager::RequestManager(int socket) {
    sock = socket;
}

void RequestManager::listenForRequests() {
    while (true) {
        bzero(buf, 1024);

        if (recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len) < 0)
            return;

        switch (getRequestCode()) {
            case TS_REQ_IP:
                requestIP();
                break;

            case TS_REQ_TICKET:
                requestTicket();
                break;

            default:
                sendMessage(ERROR, "Invalid code format.");
        };
    }
}

std::vector<std::string> RequestManager::getSplitData(std::string data) {
    std::vector<std::string> split_data;

    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, DELIMITER))
        split_data.push_back(token);

    return split_data;
}