//
// Created by monspid on 06.05.17.
//

#ifndef TIN_TICKET_H
#define TIN_TICKET_H
#include <cstring>
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"

class Ticket {
	byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ];
	byte iv[ CryptoPP::AES::BLOCKSIZE ];

public:
	Ticket();
	std::string createTicket(std::string privilegeInfo);
};


#endif //TIN_TICKET_H
