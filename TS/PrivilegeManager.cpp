//
// Created by monspid on 06.05.17.
//

#include "PrivilegeManager.h"

char* PrivilegeManager::getPrivilegeInfo(char* auth_data) {
    char* privilege_info = nullptr;
    std::vector<char *> split_auth_data = getSplitAuthData(auth_data);
    if(isValidAuthData(split_auth_data)) {
        // get it from DB
        privilege_info = (char *) "Ok";
    }

    return privilege_info;
}

bool PrivilegeManager::isValidAuthData(std::vector<char *> split_auth_data) {
    return split_auth_data.size() == 5 and isValidIP(split_auth_data) and
            isValidServerName(split_auth_data) and isValidServiceName(split_auth_data) and
            isValidUsername(split_auth_data) and isValidPassword(split_auth_data);
}

std::vector<char *> PrivilegeManager::getSplitAuthData(char auth_data[]) {
    std::vector<char *> split_auth_data;

    char * pch = strtok(auth_data, ";");

    while (pch != NULL) {
        split_auth_data.push_back(pch);
        pch = strtok (NULL, ";");
    }

    return split_auth_data;
}

bool PrivilegeManager::isValidIP(std::vector<char *> split_auth_data) {
    return split_auth_data[0] != "";
}

bool PrivilegeManager::isValidServerName(std::vector<char *> split_auth_data) {
    return split_auth_data[1] != "";
}

bool PrivilegeManager::isValidServiceName(std::vector<char *> split_auth_data) {
    return split_auth_data[2] != "";
}

bool PrivilegeManager::isValidUsername(std::vector<char *> split_auth_data) {
    return split_auth_data[3] != "";
}

bool PrivilegeManager::isValidPassword(std::vector<char *> split_auth_data) {
    return split_auth_data[4] != "";
}



