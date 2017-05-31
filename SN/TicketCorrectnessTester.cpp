//
// Created by monspid on 06.05.17.
//

#include "TicketCorrectnessTester.h"
#define DELIMITER ';'

//method used to validate the ticket
//returns: codes desribed in the method below
//arguments: ticket - encrypted ticket, senderIP - the ip from which the package was received
//serverID - the id of the server Sn which performs the service == "my id" (this is the way the server "thinks" :)
int TicketCorrectnessTester::checkTicket(std::string ticket, std::string senderIP, 
										std::string serverID, std::string serviceID) {
	try{
		std::string decryptedTicket = decryptor.decryptTicket(ticket);
		std::vector<std::string> splitTicketData = getSplitData(decryptedTicket);
		time_t currentTime = time(nullptr);
		if (splitTicketData[1] != serverID || splitTicketData[2] != serviceID)
			return 1; //wrong ticket - it is supposed for another Sn or another service!
		if (splitTicketData[0] != senderIP)
			return 2; //the sender does not match owner or the ticket - stolen ticket? ;)
		if (std::stoi(splitTicketData[3]) < currentTime)
			return 3; //the validity period has passed

		// 0 means ticket is correct
		return 0;
	}
	catch(CryptoPP::InvalidCiphertext e){
		return 4; //invalid data
	}
}

std::vector<std::string> TicketCorrectnessTester::getSplitData(std::string data) {
    std::vector<std::string> split_data;

    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, DELIMITER))
        split_data.push_back(token);

    return split_data;
}

//LEGACY METHOD, DELETE IT ASAP
int TicketCorrectnessTester::CheckTicket(char *buf){
	return 0;
}