//
// Created by monspid on 06.05.17.
//

#ifndef TIN_TICKETCORRECTNESSTESTER_H
#define TIN_TICKETCORRECTNESSTESTER_H

#include "TicketDecryptor.h"
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <log4cpp/Category.hh>
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "config.h"

using namespace std;

class TicketCorrectnessTester {
    log4cpp::Category &log = log4cpp::Category::getInstance(LOGGER_NAME);
    TicketDecryptor decryptor;

    string getDecryptedTicket(string ticket);

public:
    static int CheckTicket(char *buf); //legacy method, has to be deleted after reimplementing Sn services!!!
    int checkTicket(string ticket, string senderIP, string serverID, string serviceID);

    vector<string> getSplitData(string data);
};


#endif //TIN_TICKETCORECTNESSTESTER_H
