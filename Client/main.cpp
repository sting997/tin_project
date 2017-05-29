
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include "../SN/TicketDecryptor.h"//only for debug

#include "RequestManager.h"
#include "ConsoleMenu.h"

void exitClient() {
    printf("Exiting client program...\n");
    exit(0);
};

int main(int argc, char *argv[]) {
    RequestManager requestManager;
    ConsoleMenu menu;

    std::function<void()> action[8];
    action[0] = std::bind(&RequestManager::RequestIP, &requestManager);
    action[1] = std::bind(&RequestManager::RequestTicket, &requestManager);
    action[2] = std::bind(&RequestManager::RequestUDPEcho, &requestManager);
    action[3] = std::bind(&RequestManager::RequestUDPTime, &requestManager);
    action[4] = std::bind(&RequestManager::RequestTCPEcho, &requestManager);
    action[5] = std::bind(&RequestManager::RequestTCPTime, &requestManager);
    action[6] = std::bind(&RequestManager::RequestNewData, &requestManager);
    action[7] = std::bind(exitClient);

    menu.add(1, "1) Request IP with broadcast message");
    menu.add(2, "2) Get ticket from TS service");
    menu.add(3, "3) Request UDP ECHO service");
    menu.add(4, "4) Request UDP TIME service");
    menu.add(5, "5) Request TCP ECHO service");
    menu.add(6, "6) Request TCP TIME service");
    menu.add(7, "7) Enter new user data");
    menu.add(8, "8) Exit");

    while (1) {
        menu.display();

        if (menu.selection())
            action[menu.opt() - 1]();
        else
            std::cerr << "\nInvalid option\n" << std::endl;
    }
}