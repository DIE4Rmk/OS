// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <stdbool.h>

#define MAX_CLIENTS 3  
#define MAX_BUFFER 256

enum component
{
    weed = 1,
    paper = 2,
    light = 3
};

struct client
{
    int id;              
    int sock;            
    enum component comp; 
};

// make a component
void make_comp(enum component *comp1, enum component *comp2)
{
    int r = rand() % 3 + 1;
    
    if(r == 1) {
        *comp1 = paper;
        *comp2 = light;
    } else if(r == 2) {
        *comp1 = weed;
        *comp2 = light;
    } else {
        *comp1 = weed;
        *comp2 = paper;
    }
    
}


// id third comps exist
int has_third_component(struct client *cl, enum component c1, enum component c2)
{
    return (cl->comp != c1 && cl->comp != c2);
}

void send_mess(int sock, char *msg)
{
    if (send(sock, msg, strlen(msg), 0) < 0)
    {
        perror("send error");
        exit(1);
    }
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    srand(time(NULL));

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    int new_socket;

    int port = atoi(argv[1]);       
    struct sockaddr_in serv_addr;
    int opt = 1;
    pthread_t thread_id;

    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;         
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(port);       

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error_socket");
        exit(1);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen error");
        exit(1);
    }

    printf("Server - port is  %d\n", port);

    struct client clients[MAX_CLIENTS]; 
    int num_clients = 0;            

    while (num_clients < MAX_CLIENTS)
    {
        
        struct sockaddr_in client_addr;             
        socklen_t client_len = sizeof(client_addr); 

        int client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0)
        {
            perror("Error_accept");
            exit(1);
        }

        printf("Serv - get mess from  %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        
        char buffer[MAX_BUFFER];       
        memset(buffer, 0, MAX_BUFFER); 

        if (recv(client_sock, buffer, MAX_BUFFER - 1, 0) < 0)
        {
            perror("Error_recieve");
            exit(1);
        }

        printf("Serv got mes %s\n", buffer);

        int id;              
        enum component comp; 

        if (sscanf(buffer, "%d %u", &id, &comp) != 2 || comp < weed || comp > light || id < 1 || id > MAX_CLIENTS)
        {
            fprintf(stderr, "Error - wrong mes format\n");
            close(client_sock); 
            continue;           
        }

        printf("Serv got comp %d \n", id);

        clients[id - 1].id = id;
        clients[id - 1].sock = client_sock;
        clients[id - 1].comp = comp;

        num_clients++;

        printf("Serv added a client %d \n", id);
    }

    printf("all Clients are on\n");
    
    // process of task
    while (1)
    {

        enum component comp1, comp2;         
        make_comp(&comp1, &comp2);

        printf("Serv - comps maded\n");

        char buffer[MAX_BUFFER];    
        memset(buffer, 0, MAX_BUFFER);    
        sprintf(buffer, "%d %d", comp1, comp2); 

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            send_mess(clients[i].sock, buffer); 
        }

        printf("Serv - comps sended \n");

        int flag = 0; 
        int smoker_id;      

        while (!flag)
        {

            for (int i = 0; i < MAX_CLIENTS; i++)
            {

                memset(buffer, 0, MAX_BUFFER);

                if (recv(clients[i].sock, buffer, MAX_BUFFER - 1, 0) < 0)
                {
                    perror("recv error");
                    exit(1);
                }

                printf("Serv - got client %d: %s\n", clients[i].id, buffer);

                if (strcmp(buffer, "smoking weed") == 0)
                {                              
                    flag = 1;          
                    smoker_id = clients[i].id; 
                    break;                     
                }
            }
            if (flag)
            {
                break; 
            }
        }

        printf("Serv - got client with third comp: %d\n", smoker_id);

        sleep(5);

        printf("Server - reboot process\n");
        flag = 0;
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        close(clients[i].sock);
    }
    close(server_fd);

    return 0;
}

