//
// Created by monspid on 06.05.17.
//

#include "PrivilegeManager.h"
#include "DBManager.h"

int PrivilegeManager::getPrivilegeInfo(std::string IP, std::vector<std::string> split_auth_data) {
    int privilege_info = AUTH_INVALID;

    if (isValidAuthData(split_auth_data)) {
        log.info("Got valid auth_data");
        privilege_info = getStatusFromDB(IP, split_auth_data);
    } else
        log.info("Got invalid auth_data");

    return privilege_info;
}

bool PrivilegeManager::isValidAuthData(std::vector<std::string> split_auth_data) {
    return split_auth_data.size() == 4 and
           isValidServerName(split_auth_data) and isValidServiceName(split_auth_data) and
           isValidUsername(split_auth_data) and isValidPassword(split_auth_data);
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

    return size > 0 and size <= 100;
}

int PrivilegeManager::getStatusFromDB(std::string IP, std::vector<std::string> split_auth_data) {
    DBManager DB_manager = DBManager();
    if (not DB_manager.isUserInDB(split_auth_data[2], split_auth_data[3])) {
        log.info("There is no %s:%s in DB", split_auth_data[2].c_str(), split_auth_data[3].c_str());
        return AUTH_WRONG_USERNAME;
    }

    if (not DB_manager.canIPPerformService(IP, split_auth_data[0], split_auth_data[1], split_auth_data[2])) {
        log.info(
                "<%s %s> cannot perform <%s %s>",
                IP.c_str(), split_auth_data[2].c_str(), split_auth_data[0].c_str(), split_auth_data[1].c_str()
        );
        return AUTH_WRONG_IP;
    }

    return AUTH_CORRECT;
}
