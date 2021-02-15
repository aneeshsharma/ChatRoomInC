#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define PORT 5000
#define BUFFER_SIZE 1024

void *listen_messages(void *arg)
{
    int sock_fd = 0, recv_len;
    char *name = (char *)arg;
    struct sockaddr_in server_address;
    char message[256];
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return NULL;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return NULL;
    }

    printf("Connecting to message stream...\n");

    if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection failed!\n");
        return NULL;
    }

    send(sock_fd, name, strlen(name), 0);
    sleep(1);
    send(sock_fd, "RECV", strlen("RECV"), 0);

    while (1)
    {
        recv_len = recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (recv_len == 0)
        {
            close(sock_fd);
            return NULL;
        }
        buffer[recv_len] = '\0';
        printf("%s\n", buffer);
        fflush(stdout);
    }
}

int main()
{
    int sock_fd = 0, recv_len;
    struct sockaddr_in server_address;
    char message[256];
    char buffer[1024] = {0};
    char name[20];

    pthread_t listener;

    printf("Enter name - ");
    scanf("%[^\n]%*c", name);
    printf("\n");

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return -1;
    }

    printf("Connecting...\n");

    if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection failed!\n");
        return -1;
    }

    pthread_create(&listener, NULL, listen_messages, name);

    send(sock_fd, name, strlen(name), 0);
    sleep(1);
    send(sock_fd, "SEND", strlen("SEND"), 0);

    printf("Connected to server\nUse /exit to exit\n");

    while (1)
    {
        message[0] = 0;
        scanf("%[^\n]%*c", message);
        if (strlen(message) == 0)
        {
            getchar();
            continue;
        }
        send(sock_fd, message, strlen(message), 0);
        if (strcmp(message, "/exit") == 0)
            break;
    }

    printf("Closing connection...\n");
    close(sock_fd);
    printf("Disconnected\n");
    return 0;
}