#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 3
#define MAX_BUFFER 256


void send_mess(int sockfd, struct sockaddr_in *server_addr, char *message) {
    int len = sizeof(*server_addr);
    int n = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)server_addr, len);
    if (n < 0) {
        perror("sendto");
        exit(1);
    }
}


void receive_mess(int sockfd, struct sockaddr_in *server_addr, char *buffer) {
    int len = sizeof(*server_addr);
    int n = recvfrom(sockfd, buffer, MAX_BUFFER, 0, (struct sockaddr *)server_addr, &len);
    if (n < 0) {
        perror("recvfrom");
        exit(1);
    }
    buffer[n] = '\0';
}


void display_info(int id, int component) {
    char *names[] = {"tobacco", "paper", "matches"};
    printf("Display %d: The mediator generated %s and %s.\n", id, names[(component % 3)], names[((component + 1) % 3)]);
    printf("Display %d: Smoker %d has %s.\n", id, component, names[component - 1]);
    printf("Display %d: Smoker %d takes %s and %s from the table.\n", id, component, names[(component % 3)], names[((component + 1) % 3)]);
    printf("Display %d: Smoker %d rolls a cigarette with %s, %s and %s.\n", id, component, names[component - 1], names[(component % 3)], names[((component + 1) % 3)]);
    printf("Display %d: Smoker %d smokes the cigarette.\n", id, component);
}


int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    int id;
    int component;


    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <client_ip> <client_id>\n", argv[0]);
        exit(1);
    }

    id = atoi(argv[3]);

    if (id < 4 || id > 6) {
        fprintf(stderr, "err: %d\n", id);
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


    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    client_addr.sin_addr.s_addr = inet_addr(argv[2]);

    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Client started on %s:%d\n", argv[2], ntohs(client_addr.sin_port));


    sprintf(buffer, "display %d!", id);
    send_mess(sockfd, &server_addr, buffer);
    printf("Sent: %s\n", buffer);


    receive_mess(sockfd, &server_addr, buffer);
    printf("Received: %s\n", buffer);


    while (1) {
        receive_mess(sockfd, &server_addr, buffer);
        printf("Received: %s\n", buffer);
        component = atoi(buffer);
        display_info(id, component);
        sleep(5);
    }


    close(sockfd);
    return 0;
}