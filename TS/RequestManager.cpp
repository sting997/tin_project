//
// Created by monspid on 06.05.17.
//
#include "RequestManager.h"

char RequestManager::getRequestCode() {
    return strlen(buf) > 0 ? buf[0] : ERROR;
}

void RequestManager::requestIP() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    sendMessage(sock, TS_IP, "");
}

void RequestManager::requestTicket() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    PrivilegeManager privilege_manager = PrivilegeManager();
    std::string auth_data = getAuthData(buf);

    int privilege_status = privilege_manager.getPrivilegeInfo(inet_ntoa(remote.sin_addr), auth_data);
    char grant_status;
    std::string message;

    if (privilege_status == 3) {
        grant_status = TS_GRANTED;
        Ticket ticket;
        message = ticket.createTicket("oto jest ticket");//change the argument to a string based on privilege info
    } else {
        grant_status = TS_REFUSED;
        message = std::to_string(privilege_status);
    }

    sendMessage(sock, grant_status, message);
}

void RequestManager::sendMessage(int sock, char code, std::string message) {
    std::string response = code + message;
    printf("%s", response.c_str());
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
    while(true) {
        bzero(buf, 1024);
        n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);

        switch(getRequestCode()) {
            case TS_REQ_IP:
                requestIP();
                break;

            case TS_REQ_TICKET:
                requestTicket();
                break;

            case ERROR:
                sendMessage(sock, ERROR, "Invalid code format.");
        };
    }
}