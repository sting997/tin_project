//
// Created by monspid on 29.05.17.
//

#include "DBManager.h"


bool DBManager::isUserInDB(string username, string password) {
    string file_name = "user.txt";
    regex select(username + DELIMITER + password);

    return doesLineExist(file_name, select);
}

bool DBManager::canIPPerformService(string IP, string server_name, string service_name, string username) {
    string file_name = "service.txt";
    regex select(IP + DELIMITER + server_name + DELIMITER + service_name + DELIMITER + username);

    return doesLineExist(file_name, select);
}

bool DBManager::doesLineExist(string file_name, regex select) {
    return getDBLine(file_name, select) != "";
}

string DBManager::getTicketTimeValidityLine(string server_name, string service_name) {
    string file_name = "time.txt";
    regex select(server_name + DELIMITER + service_name);

    return getDBLine(file_name, select);
}

string DBManager::getDBLine(string file_name, regex select) {
    ifstream inFile(file_name);

    string file_line;
    string return_line = "";

    while (inFile >> file_line)
        if (regex_search(file_line, select)) {
            return_line = file_line;
            break;
        }

    inFile.close();

    return return_line;
}