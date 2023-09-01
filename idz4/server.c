#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 3
#define MAX_BUFFER 256


void send_mess(int sockfd, struct sockaddr_in *client_addr, char *message) {
    int len = sizeof(*client_addr);
    int n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)client_addr, len);
    if (n < 0) {
        perror("sendto");
        exit(1);
    }
}


void receive_mess(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    int len = sizeof(*client_addr);
    int n = recvfrom(sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *)client_addr, &len);
    if (n < 0) {
        perror("error");
        exit(1);
    }
    buffer[n] = '\0';
}


int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr[MAX_CLIENTS];
    char buffer[MAX_BUFFER];
    int components;


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(1);
    }


    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }


    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);


    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Servr started on %s:%d\n", argv[1], PORT);


    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("Waiting for client %d...\n", i + 1);
        receive_mess(sockfd, &client_addr[i], buffer);
        printf("Received: %s\n", buffer);
        sprintf(buffer, "Welcome, smoker %d!", i + 1);
        send_mess(sockfd, &client_addr[i], buffer);

    }


    while (1) {
        components = rand() % 3 + 1;;
        printf("Gen components: %d\n", components);
        sprintf(buffer, "%d", components);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            send_mess(sockfd, &client_addr[i], buffer);
            printf("Sent to smoker %d: %s\n", i + 1, buffer);
        }
        receive_mess(sockfd, &client_addr[components - 1], buffer);
        printf("Received from smoker %d: %s\n", components, buffer);
        sleep(5);
    }

    close(sockfd);
    return 0;
}