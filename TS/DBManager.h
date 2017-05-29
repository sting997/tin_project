//
// Created by monspid on 29.05.17.
//

#ifndef TIN_DBMANAGER_H
#define TIN_DBMANAGER_H

#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <regex>
#include "config.h"


class DBManager {
    bool doesLineExist(std::string file_name, std::regex select);

    std::string getDBLine(std::string file_name, std::regex select);

public:
    bool isUserInDB(std::string username, std::string password);

    bool canIPPerformService(std::string IP, std::string server_name, std::string service_name, std::string username);

    std::string getTicketTimeValidityLine(std::string server_name, std::string service_name);
};


#endif //TIN_DBMANAGER_H
