
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>
#include "../SN/TicketDecryptor.h"//only for debug

#include "RequestManager.h"
#include "ConsoleMenu.h"

void exitClient() {
    printf("Exiting client program...\n");
    exit(0);
};

log4cpp::Category &log = log4cpp::Category::getInstance(LOGGER_NAME);

int main() {
    log4cpp::PropertyConfigurator::configure(LOGGER_CONFIG);
    log.info("<=== STARTED LISTENING ===>");

    RequestManager requestManager;
    ConsoleMenu menu;

    if (!requestManager.RequestIP())
        exit(0);

    std::function<void()> action[7];
    action[0] = std::bind(&RequestManager::RequestTicket, &requestManager);
    action[1] = std::bind(&RequestManager::RequestUDPEcho, &requestManager);
    action[2] = std::bind(&RequestManager::RequestUDPTime, &requestManager);
    action[3] = std::bind(&RequestManager::RequestTCPEcho, &requestManager);
    action[4] = std::bind(&RequestManager::RequestTCPTime, &requestManager);
    action[5] = std::bind(&RequestManager::RequestNewData, &requestManager);
    action[6] = std::bind(exitClient);

    menu.add(0, "0) Get ticket from TS service");
    menu.add(1, "1) Request UDP ECHO service");
    menu.add(2, "2) Request UDP TIME service");
    menu.add(3, "3) Request TCP ECHO service");
    menu.add(4, "4) Request TCP TIME service");
    menu.add(5, "5) Enter new user data");
    menu.add(6, "6) Exit");

    log.info("Menu created");

    while (1) {
        menu.display();

        if (menu.selection())
            action[menu.opt()]();
        else
            std::cerr << "\nInvalid option\n" << std::endl;
    }
}