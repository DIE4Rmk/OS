#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER 256 

int main(int argc, char *argv[]) {
    int client_fd; 
    // serv address
    struct sockaddr_in serv_addr;
    //buffer
    char buffer[MAX_BUFFER];
    //ip
    char *serv_ip; 
    // port
    int serv_port; 
    //client id
    int client_id; 
    //comp 
    char client_comp[MAX_BUFFER]; 
    int n; 

    if (argc != 5) {
        printf("Error_input: %s ip +  port +  id + comp \n", argv[0]);
        exit(1);
    }
  
    serv_ip = argv[1];
    serv_port = atoi(argv[2]);
    client_id = atoi(argv[3]);
    strcpy(client_comp, argv[4]);

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Error_sock");
        exit(1);
    }

    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip);

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Error_connection");
        exit(1);
    }

    printf("Client %d: is on %s:%d\n", client_id, serv_ip, serv_port);

    sprintf(buffer, "%d %s", client_id, client_comp);
    n = send(client_fd, buffer, strlen(buffer), 0);
    if (n == -1) {
        perror("Error_sending is broken");
        exit(1);
    }

    n = recv(client_fd, buffer, MAX_BUFFER - 1, 0);
    if (n == -1) {
        perror("Error_read is broken");
        exit(1);
    }
    buffer[n] = '\0'; 

    printf("%s", buffer); 

    
    while (1) {
        n = recv(client_fd, buffer, MAX_BUFFER - 1, 0);
        if (n == -1) {
            perror("Error_read is broken");
            exit(1);
        }
        buffer[n] = '\0'; 

        
        printf("%s", buffer);
    }

    close(client_fd);

    return 0;
}
