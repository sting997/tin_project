//
// Created by monspid on 06.05.17.
//

#ifndef TIN_REQUESTMANAGER_H
#define TIN_REQUESTMANAGER_H
#include <cstring>
#include "../protocol_codes.h"
#include "Ticket.h"

class RequestManager {
public:
	static char getRequestCode(char *message);
	static void createRequest(char * requestBuffer, char requestCode);
};


#endif //TIN_REQUESTMANAGER_H
