#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>


#define MAX_BUFFER 256 


void send_mess(int sock, char *str) {

    if (send(sock, str, strlen(str), 0) < 0) {

        perror("Error_sm");

        exit(1);

    }

}

enum component {

    weed = 1,

    paper = 2,

    light = 3

};

int main(int argc, char *argv[]) {


    if (argc != 5) {

        fprintf(stderr, "Error_input. Using: %s serv_ip + serv_port + id + comp\n", argv[0]);

        exit(1);
    }

    // ip
    char *serv_ip = argv[1]; 
    // port
    int serv_port = atoi(argv[2]);
    //client aka id
    int id = atoi(argv[3]); 
    //comp
    enum component comp = atoi(argv[4]); 

    printf(" cliet %d: get component \n", id);

    int client_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (client_sock < 0) {

        perror("Error_sock");

        exit(1);

    }

    struct sockaddr_in serv_addr; 

    memset(&serv_addr, 0, sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET; 

    serv_addr.sin_addr.s_addr = inet_addr(serv_ip);

    serv_addr.sin_port = htons(serv_port);

    if (connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {

        perror("Error_connection");

        exit(1);

    }


    printf("Client %d: entered %s:%d\n", id, serv_ip, serv_port);


    char buffer[MAX_BUFFER]; 

    memset(buffer, 0, MAX_BUFFER); 

    sprintf(buffer, "%d %d", id, comp); 

    send_mess(client_sock, buffer); 

    printf("Client %d: sended\n", id);

    while (1) {
        
        // buff clear
        memset(buffer, 0, MAX_BUFFER); 


        if (recv(client_sock, buffer, MAX_BUFFER - 1, 0) < 0) {

            perror("Error_receive");

            exit(1);

        }


        printf("CLient %d: got mess: %s\n", id, buffer);

        enum component comp1, comp2; 

        // if third comp is exist

        if (comp != comp1 && comp != comp2) {

            printf("CLient %d: has third comp \n", id);
            
            
            send_mess(client_sock, "smoking weed"); 

            printf("Client %d: sent mess: took comps and smoking \n", id);

            sleep(5);

            printf("Client  %d: finished smoking \n", id);

        } else {

            printf("Client %d: there isn't a third comp, I can't smoke! \n", id);


        }

    }

    close(client_sock);
    return 0;

}
