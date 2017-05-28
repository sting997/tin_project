//
// Created by monspid on 06.05.17.
//

#include "PrivilegeManager.h"

int PrivilegeManager::getPrivilegeInfo(std::string IP, std::string auth_data) {
    int privilege_info = 0;
    std::vector<std::string> split_auth_data = getSplitAuthData(auth_data);

    if (isValidAuthData(IP, split_auth_data))
        privilege_info = getStatusFromDB(IP, split_auth_data);

    return privilege_info;
}

bool PrivilegeManager::isValidAuthData(std::string IP, std::vector<std::string> split_auth_data) {
    return split_auth_data.size() == 4 and isValidIP(IP) and
           isValidServerName(split_auth_data) and isValidServiceName(split_auth_data) and
           isValidUsername(split_auth_data) and isValidPassword(split_auth_data);
}

std::vector<std::string> PrivilegeManager::getSplitAuthData(std::string auth_data) {
    std::vector<std::string> split_auth_data;

    std::stringstream ss(auth_data);
    std::string token;

    while (std::getline(ss, token, DELIMITER))
        split_auth_data.push_back(token);

    return split_auth_data;
}

bool PrivilegeManager::isValidIP(std::string IP) {
    size_t size = IP.size();
    std::regex IP_regex("^\\d+.\\d+.\\d+.\\d+$");
    return size >= 7 and size <= 15 and std::regex_match(IP, IP_regex);
}

bool PrivilegeManager::isValidServerName(std::vector<std::string> split_auth_data) {
    size_t size = (split_auth_data[0]).size();

    return size > 0 and size <= 2;
}

bool PrivilegeManager::isValidServiceName(std::vector<std::string> split_auth_data) {
    size_t size = (split_auth_data[1]).size();

    return size > 0 and size <= 2;
}

bool PrivilegeManager::isValidUsername(std::vector<std::string> split_auth_data) {
    size_t size = (split_auth_data[2]).size();

    return size > 0 and size <= 30;
}

bool PrivilegeManager::isValidPassword(std::vector<std::string> split_auth_data) {
    size_t size = (split_auth_data[3]).size();

    return size > 0 and size <= 16;
}

int PrivilegeManager::getStatusFromDB(std::string IP, std::vector<std::string> split_auth_data) {
    if (not isUserInDB(split_auth_data[2], split_auth_data[3]))
        return 1;

    if (not canIPPerformService(IP, split_auth_data[0], split_auth_data[1]))
        return 2;

    return 3;
}

bool PrivilegeManager::isUserInDB(std::string username, std::string password) {
    std::string file_name = "user.txt";
    std::string line = username + DELIMITER + password;

    return doesLineExist(file_name, line);
}

bool PrivilegeManager::canIPPerformService(std::string IP, std::string server_name, std::string service_name) {
    std::string file_name = "service.txt";
    std::string line = IP + DELIMITER + server_name + DELIMITER + service_name;

    return doesLineExist(file_name, line);
}

bool PrivilegeManager::doesLineExist(std::string name, std::string line) {
    std::ifstream inFile(name);

    std::string file_line;
    bool status = false;

    while (inFile >> file_line)
        if (line == file_line) {
            status = true;
            break;
        }

    inFile.close();
    return status;
}
