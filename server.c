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
    char message[BUFFER_SIZE];
    int status;
    char mode[5];
    struct Connection *connections;
    int *num;
    pthread_mutex_t mut;
} Client;

typedef struct Connection
{
    pthread_t id;
    Client *client;
} Connection;

void broadcast(char *message, Connection *conn, int num)
{
    printf("\033[1;31mNew message - %s\033[0m\n", message);
    for (int i = 0; i < num; i++)
    {
        Client *other = (conn[i]).client;

        pthread_mutex_lock(&(other->mut));
        if (strcmp(other->mode, "RECV") == 0)
        {
            strcpy(other->message, message);
            other->status = 1;
        }
        pthread_mutex_unlock(&(other->mut));
    }
}

void *handle_client(void *vargp)
{
    int recv_len;
    char buffer[BUFFER_SIZE];
    char mode[5];

    char message[BUFFER_SIZE];

    Client *client = (Client *)vargp;
    recv_len = read(client->sock_fd, buffer, BUFFER_SIZE);
    buffer[recv_len] = '\0';
    strcpy(client->name, buffer);

    recv_len = read(client->sock_fd, buffer, BUFFER_SIZE);
    buffer[recv_len] = '\0';

    strcpy(mode, buffer);
    strcpy(client->mode, mode);
    printf("User %s connected with mode - %s\n", client->name, client->mode);
    if (strcmp(mode, "SEND") == 0)
    {
        sprintf(message, "%s joined the chat", client->name);
        broadcast(message, client->connections, *(client->num));

        while (1)
        {
            recv_len = read(client->sock_fd, buffer, BUFFER_SIZE);
            buffer[recv_len] = '\0';

            if (recv_len == 0)
            {
                break;
            }

            if (strcmp(buffer, "/exit") == 0)
            {
                break;
            }
            sprintf(message, "%s: %s", client->name, buffer);

            broadcast(message, client->connections, *(client->num));
        }

        Client *other;
        for (int i = 0; i < *(client->num); i++)
        {
            Connection *conn = client->connections;
            other = conn[i].client;
            pthread_mutex_lock(&(other->mut));
            if (other != client)
            {
                if (strcmp(other->name, client->name) == 0)
                {
                    other->status = 2;
                }
            }
            pthread_mutex_unlock(&(other->mut));
        }

        sprintf(message, "%s left the chat", client->name);
        broadcast(message, client->connections, *(client->num));
    }
    else if (strcmp(mode, "RECV") == 0)
    {
        while (1)
        {
            while (1)
            {
                pthread_mutex_lock(&(client->mut));
                if (client->status == 1 || client->status == 2)
                    break;
                pthread_mutex_unlock(&(client->mut));
            }
            if (client->status == 2)
            {
                pthread_mutex_unlock(&(client->mut));
                break;
            }
            send(client->sock_fd, client->message, strlen(client->message), 0);
            client->status = 0;
            pthread_mutex_unlock(&(client->mut));
        }
    }

    printf("Closing connection for %s with mode %s\n", client->name, client->mode);

    close(client->sock_fd);
}

int main()
{
    int server_fd, conn_socket, recv_len;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    int seq = 0;

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
            printf("Error accepting connection!!!\n");
            fflush(stdout);
            perror("accept");
        }

        Client *client = (Client *)malloc(sizeof(Client));

        client->sock_fd = conn_socket;
        client->status = 0;
        client->connections = connections;
        connections[curr].client = client;

        if (pthread_mutex_init(&(client->mut), NULL) != 0)
        {
            printf("Mutex initialization failed!!\n");
        }
        else
        {
            pthread_create(&connections[curr].id, NULL, handle_client, (void *)client);
            printf("New thread created - %d\n", connections[curr].id);
            curr++;
            client->num = &curr;
        }
    }

    printf("Loop broken\n");
    return 0;
}