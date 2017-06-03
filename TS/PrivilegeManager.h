//
// Created by monspid on 06.05.17.
//

#ifndef TIN_PRIVILEGEMANAGER_H
#define TIN_PRIVILEGEMANAGER_H

#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <regex>
#include "config.h"
#include <log4cpp/Category.hh>

class PrivilegeManager {
    log4cpp::Category &log = log4cpp::Category::getInstance(LOGGER_NAME);

    bool isValidAuthData(std::vector<std::string> split_auth_data);

    bool isValidServerName(std::vector<std::string> split_auth_data);

    bool isValidServiceName(std::vector<std::string> split_auth_data);

    bool isValidUsername(std::vector<std::string> split_auth_data);

    bool isValidPassword(std::vector<std::string> split_auth_data);

    int getStatusFromDB(std::string IP, std::vector<std::string> split_auth_data);

public:
    int getPrivilegeInfo(std::string IP, std::vector<std::string> split_auth_data);
};

#endif //TIN_PRIVILEGEMANAGER_H
