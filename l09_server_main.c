#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MY_PORT       9990
#define MAXBUF        1024

int main(void) {
    int sockfd;                 // int to hold the socket file descriptor
    struct sockaddr_in self;    // do a google search on this to see
                                // what the struct contain and is used for
    char buffer[MAXBUF];

    // create a typical TCP socket - man socket will get you
    // the defined constants and what they mean
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(errno);
    }
    // init the address struct for use
    bzero(&self, sizeof(self));     // clear the struct
    self.sin_family = AF_INET;      // set up for ipv4
    self.sin_port = htons(MY_PORT); // set the port
    self.sin_addr.s_addr = INADDR_ANY;  // allow connect from any IP

    // bind the socket to the port
    if (bind(sockfd, (struct sockaddr *) &self, sizeof(self)) != 0) {
        perror("socket--bind");
        exit(errno);
    }
    // socket is all ready, now tell os ready to accept connections
    if (listen(sockfd, 20) != 0) {
        perror("socket--listen");
        exit(errno);
    }

    int clientfd;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);

    printf("Listening for a connection...\n");

    // accept an incoming connect and bind it to a new file descriptor
    // inet_ntoa() and ntohs() require some investigation on your part
    clientfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *) &addrlen);
    printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // blocking recv of message from client
    recv(clientfd, buffer, MAXBUF, 0);
    printf("from client-> %s\n", buffer);

    // send "echo" the message back to the client
    char *newmsg = strdup(strcat(buffer, "-- back at you!"));
    send(clientfd, newmsg, MAXBUF, 0);

    // cleanup and exit
    close(clientfd);
    close(sockfd);
    return (0);
}