//
// Created by monspid on 06.05.17.
//
#include "TicketManager.h"

TicketManager::TicketManager(){};

//returns a ticket for a service identified by a std::pair
//first - server id, second - service id
std::string TicketManager::getTicket(std::pair<std::string, std::string> ticketKey){
	return ticketMap[ticketKey];
}

//stores a ticket to a service identified by a std::pair
//first - server id, second - service (eg 1 for udp echo)
void TicketManager::addTicket(std::pair<std::string, std::string> key, std::string value){
	ticketMap[key] = value;
}

//method which returns true if a ticket for a service is stored
//can be used in a method which requests service from Sn
//instead of asking Ts for a ticket check with this method
//if you have one previously saved
bool TicketManager::contains(std::pair<std::string, std::string> key){
	return ticketMap.find(key) != ticketMap.end();
}

