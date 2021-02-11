#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 5000

int main()
{
    int sock = 0, recv_len;
    struct sockaddr_in server_address;
    char message[256];
    char buffer[1024] = {0};
    char name[20];

    printf("Enter name - ");
    scanf("%[^\n]%*c", name);
    printf("\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection failed!\n");
        return -1;
    }

    send(sock, name, strlen(name), 0);

    printf("Connected to server\nUse /exit to exit\n");

    while (1)
    {
        printf("> ");
        scanf("%[^\n]%*c", message);
        send(sock, message, strlen(message), 0);
        if (strcmp(message, "/exit") == 0)
            break;
    }
    close(sock);
    return 0;
}