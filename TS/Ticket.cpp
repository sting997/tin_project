//
// Created by monspid on 06.05.17.
//

#include "Ticket.h"
void Ticket::createTicket(char *ticketBuffer, char * privilegeInfo){
	memcpy(ticketBuffer, privilegeInfo, sizeof(privilegeInfo)+1);
}