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

    if (splitTicketData.size() < 4) {
        log.info("Ticket %s invalid", ticket.c_str());
        return TICKET_INVALID;
    }
    if (splitTicketData[0] != senderIP) {
        log.info("Ticket wrong IP. Got %s. Expected %s", splitTicketData[0].c_str(), senderIP.c_str());
        return TICKET_WRONG_IP;
    }

    if (splitTicketData[1] != serverID || splitTicketData[2] != serviceID) {
        log.info(
                "Ticket wrong Server or Service. Got <%s %s>. Expected <%s %s>",
                splitTicketData[1].c_str(), splitTicketData[2].c_str(), serverID, serverID
        );
        return TICKET_WRONG_SN;
    }
    if (stoi(splitTicketData[3]) < currentTime) {
        log.info("Ticket expired. Got %s. Expected value greater than %d", splitTicketData[3], currentTime);
        return TICKET_EXPIRED;
    }

    log.info("Ticket %s is correct", ticket);
    return TICKET_CORRECT;

}

string TicketCorrectnessTester::getDecryptedTicket(string ticket) {
    string decryptedTicket = "";

    try {
        decryptedTicket = decryptor.decryptTicket(ticket);
        log.info("Ticket decrypted");
    }
    catch (CryptoPP::InvalidCiphertext e) {
        log.error("Invalid key");
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
int TicketCorrectnessTester::CheckTicket(char *buf) {
    return 0;
}