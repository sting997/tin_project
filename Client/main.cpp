#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#define DATA "The sea is calm, the tide is full . . ."
#define URL "localhost"
#define PORT 9000
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in name;
    struct hostent *hp;

    /* Create socket on which to send. */
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    hp = gethostbyname(URL);

    if (hp == (struct hostent *) 0) {
        fprintf(stderr, "%s: unknown host\n", URL);
        exit(2);
    }

    memcpy((char *) &name.sin_addr, hp->h_addr, (size_t) hp->h_length);

    name.sin_family = AF_INET;
    name.sin_port = htons(PORT);

    /* Send message. */
    if (sendto(sock, DATA, sizeof DATA ,0, (struct sockaddr *) &name,sizeof name) == -1)
        perror("sending datagram message");

    close(sock);
    exit(0);
}