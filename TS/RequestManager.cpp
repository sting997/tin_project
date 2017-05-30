//
// Created by monspid on 06.05.17.
//
#include "RequestManager.h"

char RequestManager::getRequestCode() {
    return strlen(buf) > 0 ? buf[0] : ERROR;
}

void RequestManager::requestIP() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    sendMessage(TS_IP, "");
}

void RequestManager::requestTicket() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    string auth_data = getAuthData(buf);
    vector<string> split_auth_data = getSplitData(auth_data);

    PrivilegeManager privilege_manager = PrivilegeManager();

    int privilege_status = privilege_manager.getPrivilegeInfo(inet_ntoa(remote.sin_addr), split_auth_data);
    char grant_status;
    string message;

    if (privilege_status == 3) {
        grant_status = TS_GRANTED;
		time_t now = time(nullptr);
        int ticket_time_validity = getTicketTimeValidity(split_auth_data) + now;

		string unencryptedTicket = prepareTicketToEncryption(inet_ntoa(remote.sin_addr),
										split_auth_data[0], split_auth_data[1], ticket_time_validity);
        Ticket ticket;
        message = ticket.createTicket(unencryptedTicket);
    } else {
        grant_status = TS_REFUSED;
        message = to_string(privilege_status);
    }

    sendMessage(grant_status, message);
}

int RequestManager::getTicketTimeValidity(vector<string> split_auth_data) {
    DBManager dbManager = DBManager();

    string ticket_time_validity_line = dbManager.getTicketTimeValidityLine(split_auth_data[0], split_auth_data[1]);
    vector<string> split_ticket_time_validity_line = getSplitData(ticket_time_validity_line);

    return split_ticket_time_validity_line.size() == 3 ? stoi(split_ticket_time_validity_line[2]) : 0;
}

void RequestManager::sendMessage(char code, string message) {
    string response = code + message;
    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &remote, len);
}

string RequestManager::getAuthData(string buf) {
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

vector<string> RequestManager::getSplitData(string data) {
    vector<string> split_data;

    stringstream ss(data);
    string token;

    while (getline(ss, token, DELIMITER))
        split_data.push_back(token);

    return split_data;
}

//this function prepares a string ready to be used by a method from Ticket class which encrypts given string
//parameters - self explanatory
string RequestManager::prepareTicketToEncryption(string ip, string serverNr, string serviceNr, int ticketValidityTime){
	return ip + DELIMITER + serverNr + DELIMITER + serviceNr + DELIMITER + to_string(ticketValidityTime);
}
