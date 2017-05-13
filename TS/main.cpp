//
// Created by monspid on 13.05.17.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>


int main()
{
    int sock, length;
    struct sockaddr_in name;
    char buf[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    /* Create name with wildcards. */
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = INADDR_ANY;
    name.sin_port = htons(9000);

    if (bind(sock,(struct sockaddr *)&name, sizeof name) == -1) {
        perror("binding datagram socket");
        exit(1);
    }

    /* Wydrukuj na konsoli numer portu */
    length = sizeof(name);

    if (getsockname(sock,(struct sockaddr *) &name, (socklen_t *) &length)
        == -1) {
        perror("getting socket name");
        exit(1);
    }

    printf("Socket port #%d\n", ntohs(name.sin_port));

    /* Read from the socket. */
    if ( read(sock, buf, 1024) == -1 )
        perror("receiving datagram packet");
    printf("-->%s\n", buf);
    close(sock);
    exit(0);
}
