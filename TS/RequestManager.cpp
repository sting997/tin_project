//
// Created by monspid on 06.05.17.
//

#include "RequestManager.h"
char RequestManager::getRequestCode(char *message){
	if (strlen(message) > 0)
		return message[0];
	else return '9'; //error code
}

void RequestManager::createRequest(char * requestBuffer, char requestCode){
	requestBuffer[0] = requestCode;
	if (requestCode == TS_GRANTED)
		Ticket::createTicket(&requestBuffer[2], "Tyket");
}