//
// Created by monspid on 06.05.17.
//
#include "RequestManager.h"

char RequestManager::getRequestCode(char *message) {
    return strlen(message) > 0 ? message[0] : ERROR;
}

void RequestManager::requestIP() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    if (shouldPerform(TS_REQ_IP)) {
        sendMessage(sock, TS_IP, "");
    }
}

void RequestManager::requestTicket() {
    printf("Got a datagram from %s port %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

    if (shouldPerform(TS_REQ_TICKET)) {
        PrivilegeManager privilege_manager = PrivilegeManager();
        std::string auth_data = getAuthData(buf);

        int status = privilege_manager.getPrivilegeInfo(inet_ntoa(remote.sin_addr), auth_data);
        std::string message;
        if (status == 3) {
            Ticket ticket;
            message = ticket.createTicket("oto jest ticket");//change the argument to a string based on privilege info
        } else
            message = std::to_string(status);

        sendMessage(sock, TS_GRANTED, message);
    }
}

bool RequestManager::shouldPerform(char code) {
    return n >= 0 and getRequestCode(buf) == code;
}

void RequestManager::sendMessage(int sock, char code, std::string message) {
    std::string response = code + message;

    sendto(sock, response.c_str(), strlen(response.c_str()), 0, (struct sockaddr *) &remote, len);
}

std::string RequestManager::getAuthData(std::string buf) {
    buf.erase(0, 1);

    return buf;
}

RequestManager::RequestManager(int socket) {
    sock = socket;
    n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &remote, &len);
}
