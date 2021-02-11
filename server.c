#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define PORT 5000

#define BUFFER_SIZE 1024
#define MAX_CONN 100

typedef struct Client
{
    int sock_fd;
    char name[20];
} Client;

typedef struct Connection
{
    pthread_t id;
    Client *client;
} Connection;

void *handle_client(void *vargp)
{
    int recv_len;
    char buffer[BUFFER_SIZE];
    Client *client = (Client *)vargp;
    recv_len = read(client->sock_fd, buffer, BUFFER_SIZE);
    strcpy(client->name, buffer);

    printf("%s joined the chat\n", client->name);

    while (1)
    {
        recv_len = read(client->sock_fd, buffer, BUFFER_SIZE);
        buffer[recv_len] = '\0';

        if (strcmp(buffer, "/exit") == 0)
        {
            break;
        }
        printf("%s: %s\n", client->name, buffer);
    }

    printf("%s left chat\n", client->name);

    close(client->sock_fd);
}

int main()
{
    int server_fd, conn_socket, recv_len;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    Connection connections[MAX_CONN];
    int curr = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nError creating socket\n");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("couldn't bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((conn_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        Client *client = (Client *)malloc(sizeof(Client));

        client->sock_fd = conn_socket;
        pthread_create(&connections[curr].id, NULL, handle_client, (void *)client);
    }
    return 0;
}