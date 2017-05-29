//
// Created by monspid on 29.05.17.
//

#include "DBManager.h"


bool DBManager::isUserInDB(std::string username, std::string password) {
    std::string file_name = "user.txt";
    std::regex select(username + DELIMITER + password);

    return doesLineExist(file_name, select);
}

bool DBManager::canIPPerformService(std::string IP, std::string server_name, std::string service_name,
                                    std::string username) {
    std::string file_name = "service.txt";
    std::regex select(IP + DELIMITER + server_name + DELIMITER + service_name + DELIMITER + username);

    return doesLineExist(file_name, select);
}

bool DBManager::doesLineExist(std::string file_name, std::regex select) {
    return getDBLine(file_name, select) != "";
}

std::string DBManager::getTicketTimeValidityLine(std::string server_name, std::string service_name) {
    std::string file_name = "time.txt";
    std::regex select(server_name + DELIMITER + service_name);

    return getDBLine(file_name, select);
}

std::string DBManager::getDBLine(std::string file_name, std::regex select) {
    std::ifstream inFile(file_name);

    std::string file_line;
    std::string return_line = "";

    while (inFile >> file_line)
        if (std::regex_search(file_line, select)) {
            return_line = file_line;
            break;
        }

    inFile.close();

    return return_line;
}