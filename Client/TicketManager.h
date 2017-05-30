//
// Created by monspid on 06.05.17.
//

#ifndef TIN_TICKETMANAGER_H
#define TIN_TICKETMANAGER_H
#include <utility>
#include <string>
#include <map>

class TicketManager {
	//this is a map containing all the tickets our client posessed
	//the key is a pair of strings, where first - server, second - service
	//these informations are assigned to the ticket
	//the value in this map is the encrypted ticket himself
	std::map<std::pair<std::string, std::string>, std::string> ticketMap;
public:
	TicketManager();
	void addTicket(std::pair<std::string, std::string>, std::string);
	std::string getTicket(std::pair<std::string, std::string>);
	bool contains(std::pair<std::string, std::string>);
};


#endif //TIN_TICKETMANAGER_H
