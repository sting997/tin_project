//
// Created by monspid on 06.05.17.
//

#ifndef TIN_TICKETDECRYPTOR_H
#define TIN_TICKETDECRYPTOR_H

#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include <cryptopp/rijndael.h>
#include <log4cpp/Category.hh>
#include "config.h"

class TicketDecryptor {
    log4cpp::Category &log = log4cpp::Category::getInstance(LOGGER_NAME);
    byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
    byte iv[CryptoPP::AES::BLOCKSIZE];

public:
    TicketDecryptor();
    std::string decryptTicket(std::string ticket);
};


#endif //TIN_TICKETDECRYPTOR_H
