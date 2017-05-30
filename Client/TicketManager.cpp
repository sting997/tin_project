//
// Created by monspid on 06.05.17.
//
#include "TicketManager.h"

TicketManager::TicketManager(){};

std::string TicketManager::getTicket(std::pair<std::string, std::string> ticketKey){
	return ticketMap[ticketKey];
}

void TicketManager::addTicket(std::pair<std::string, std::string> key, std::string value){
	ticketMap[key] = value;
}

