//
// Created by monspid on 06.05.17.
//

#include "Ticket.h"

Ticket::Ticket() {
    memset(key, 0x00, (size_t) CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(iv, 0x00, (size_t) CryptoPP::AES::BLOCKSIZE);
}

std::string Ticket::createTicket(std::string privilegeInfo) {
    std::string plaintext = privilegeInfo;
    std::string ciphertext;

    CryptoPP::AES::Encryption aesEncryption(key, (size_t) CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char *>( plaintext.c_str()), plaintext.length() + 1);
    stfEncryptor.MessageEnd();
    return ciphertext;
}