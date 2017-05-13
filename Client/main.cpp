//
// Created by monspid on 13.05.17.
//

#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

#define SRV_IP "192.168.0.23"

void diep(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in si_other;
    int s, i;
    unsigned slen =sizeof(si_other);
    char buf[BUFLEN];
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        diep((char *) "socket");

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    for (i=0; i<NPACK; i++) {
        printf("Sending packet %d\n", i);
        sprintf(buf, "This is packet %d\n", i);
        if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, slen)==-1)
            diep((char *) "sendto()");
    }

    close(s);
    return 0;
}
