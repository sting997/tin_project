#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include "../protocol_codes.h"
#include "../TS/RequestManager.h"

#define DATA "The sea is calm, the tide is full . . ."
#define PORT 9000


//TODO
//refactor this code, divide into functions


int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in broadcastAddr;
	struct sockaddr_in remote;
    char buf[1024];
    

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

	//create simple request for server
	char req[1];
	RequestManager::createRequest(req, TS_REQ_IP);
	/* Send message. */
    if (sendto(sock, req, sizeof req ,0, (struct sockaddr *) &broadcastAddr,sizeof broadcastAddr) == -1)
        perror("sending datagram message");

	//receive package from server
	socklen_t len = sizeof(remote);
	int n;
	while (true){
		n = recvfrom(sock,buf,1024,0,(struct sockaddr *) &remote, &len);
		if (n<0) 
			perror("Error receiving data");
		else {
			if(buf[0] == TS_IP){
				printf("Received package from TS: %s\n", inet_ntoa(remote.sin_addr));
				break;
			}
			else
				printf("Received roaming package, didn't want it though!\n");
		}	
	}
	
	/* Create socket on which to send. */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1) {
        perror("opening datagram socket");
        exit(1);
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = remote.sin_addr.s_addr;
    broadcastAddr.sin_port = htons(8000);

	//create simple request for server
	RequestManager::createRequest(req, TS_REQ_TICKET);
    /* Send message. */
    if (sendto(sock, req, sizeof req ,0, (struct sockaddr *) &broadcastAddr,sizeof broadcastAddr) == -1)
        perror("sending datagram message");
	
	n = recvfrom(sock,buf,1024,0,(struct sockaddr *) &remote, &len);
	if (n<0) 
		perror("Error receiving data");
	else {
		if(buf[0] == TS_GRANTED){
			printf("I just received my ticket, whoooaaa!\n%s\n", buf);
		}
		else
			printf("Received roaming package, didn't want it though!\n");
	}	
	
    close(sock);
    exit(0);
}