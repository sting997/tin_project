#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#define DATA "The sea is calm, the tide is full . . ."
#define PORT 9000
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in broadcastAddr;

    /* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    broadcastAddr.sin_port = htons(PORT);

    /* Send message. */
    if (sendto(sock, DATA, sizeof DATA ,0, (struct sockaddr *) &broadcastAddr,sizeof broadcastAddr) == -1)
        perror("sending datagram message");

    close(sock);
    exit(0);
}