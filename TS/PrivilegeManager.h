//
// Created by monspid on 06.05.17.
//

#ifndef TIN_PRIVILEGEMANAGER_H
#define TIN_PRIVILEGEMANAGER_H
#include <cstring>
#include <vector>

class PrivilegeManager {
    std::vector<char *> getSplitAuthData(char auth_data[]);
    bool isValidAuthData(std::vector<char *> split_auth_data);
    bool isValidIP(std::vector<char *> split_auth_data);
    bool isValidServerName(std::vector<char *> split_auth_data);
    bool isValidServiceName(std::vector<char *> split_auth_data);
    bool isValidUsername(std::vector<char *> split_auth_data);
    bool isValidPassword(std::vector<char *> split_auth_data);

public:
    char* getPrivilegeInfo(char* auth_data);
};



#endif //TIN_PRIVILEGEMANAGER_H
