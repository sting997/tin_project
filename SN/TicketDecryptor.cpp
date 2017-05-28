//
// Created by monspid on 06.05.17.
//

#include <cstring>
#include "TicketDecryptor.h"

TicketDecryptor::TicketDecryptor(){
	memset( key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );
}

std::string TicketDecryptor::decryptTicket(std::string ticket){
	std::string ciphertext = ticket;
	std::string decryptedtext;

	CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
    stfDecryptor.MessageEnd();
	return decryptedtext;
}