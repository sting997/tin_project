//
// Created by monspid on 06.05.17.
//

#include "TicketCorrectnessTester.h"

//method used to validate the ticket
//returns: codes described in the method below
//arguments: ticket - encrypted ticket, senderIP - the ip from which the package was received
//serverID - the id of the server Sn which performs the service == "my id" (this is the way the server "thinks" :)
int TicketCorrectnessTester::checkTicket(string ticket, string senderIP, string serverID, string serviceID) {
    time_t currentTime = time(nullptr);
    string decryptedTicket;
    decryptedTicket = getDecryptedTicket(ticket);
    vector<string> splitTicketData = getSplitData(decryptedTicket);

    if (splitTicketData.size() < 4)
        return TICKET_INVALID;
    if (splitTicketData[0] != senderIP)
        return TICKET_WRONG_IP;
    if (splitTicketData[1] != serverID || splitTicketData[2] != serviceID)
        return TICKET_WRONG_SN;
    if (stoi(splitTicketData[3]) < currentTime)
        return TICKET_EXPIRED;

    return TICKET_CORRECT;

}

string TicketCorrectnessTester::getDecryptedTicket(string ticket) {
    string decryptedTicket = "";

    try {
        decryptedTicket = decryptor.decryptTicket(ticket);
    }
    catch (CryptoPP::InvalidCiphertext e) {
        perror("Invalid key");
    }

    return decryptedTicket;
}


vector<string> TicketCorrectnessTester::getSplitData(string data) {
    vector<string> split_data;

    stringstream ss(data);
    string token;

    while (getline(ss, token, DELIMITER))
        split_data.push_back(token);

    return split_data;
}

//LEGACY METHOD, DELETE IT ASAP
int TicketCorrectnessTester::CheckTicket(char *buf){
	return 0;
}