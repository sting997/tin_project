//
// Created by monspid on 06.05.17.
//

#ifndef TIN_TICKETCORRECTNESSTESTER_H
#define TIN_TICKETCORRECTNESSTESTER_H
#include "TicketDecryptor.h"
#include <string>
#include <sstream>
class TicketCorrectnessTester {
	TicketDecryptor decryptor;
public:
	static int CheckTicket(char *buf); //legacy method, has to be deleted after reimplementing Sn services!!!
    int checkTicket(std::string ticket, std::string senderIP, std::string serverID, std::string serviceID);
	std::vector<std::string> getSplitData(std::string data);
};


#endif //TIN_TICKETCORECTNESSTESTER_H
