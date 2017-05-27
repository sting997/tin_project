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

class PrivilegeManager {
    std::vector<std::string> getSplitAuthData(std::string auth_data);

    bool isValidAuthData(std::string IP, std::vector<std::string> split_auth_data);

    bool isValidIP(std::string IP);

    bool isValidServerName(std::vector<std::string> split_auth_data);

    bool isValidServiceName(std::vector<std::string> split_auth_data);

    bool isValidUsername(std::vector<std::string> split_auth_data);

    bool isValidPassword(std::vector<std::string> split_auth_data);

    int getStatusFromDB(std::string IP, std::vector<std::string> split_auth_data);

    bool isUserInDB(std::string username, std::string password);

    bool canIPPerformService(std::string IP, std::string server_name, std::string service_name);

    bool doesLineExist(std::string name, std::string line);

public:
    int getPrivilegeInfo(std::string IP, std::string auth_data);
};

#endif //TIN_PRIVILEGEMANAGER_H
