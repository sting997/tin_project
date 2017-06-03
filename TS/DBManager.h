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
#include <log4cpp/Category.hh>
#include "config.h"
using namespace std;

class DBManager {
    log4cpp::Category &log = log4cpp::Category::getInstance(LOGGER_NAME);
    bool doesLineExist(string file_name, regex select);

    string getDBLine(string file_name, regex select);

public:
    bool isUserInDB(string username, string password);

    bool canIPPerformService(string IP, string server_name, string service_name, string username);

    string getTicketTimeValidityLine(string server_name, string service_name);
};


#endif //TIN_DBMANAGER_H
