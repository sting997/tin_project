//
// Created by monspid on 06.05.17.
//

#include "TicketDecryptor.h"

TicketDecryptor::TicketDecryptor() {
    memset(key, 0x00, (size_t) CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(iv, 0x00, (size_t) CryptoPP::AES::BLOCKSIZE);
}

std::string TicketDecryptor::decryptTicket(std::string ticket){
    log.info("Started decryption");
    std::string ciphertext = ticket;
    std::string decryptedtext;

    CryptoPP::AES::Decryption aesDecryption(key, (size_t) CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
    stfDecryptor.Put(reinterpret_cast<const unsigned char *>( ciphertext.c_str()), ciphertext.size());
    stfDecryptor.MessageEnd();
    log.info("Finished decryption");
    return decryptedtext;
}
