//
// Created by monspid on 06.05.17.
//

#ifndef TIN_REQUESTMANAGER_H
#define TIN_REQUESTMANAGER_H

#include <iostream>

class RequestManager {
    int startTcpCon(char const *ip, int port);

    int checkIfEnd(char const *buf, char const *seq);

public:
    void tcpEchoTest();

    void tcpTimeTest();
};


#endif //TIN_REQUESTMANAGER_H
