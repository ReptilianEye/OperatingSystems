#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define MAX_CLIENTS 50
#define MAX_CLIENT_NAME 64
#define MAX_MESSAGE_LENGTH 256

typedef enum
{
    LIST,
    BROADCAST,
    UNICAST,
    STOP,
    ALIVE
} request_type_t;

typedef struct
{
    request_type_t type;
    char client_name[MAX_CLIENT_NAME];
    union
    {
        struct
        {
            char target_client[MAX_CLIENT_NAME];
            char message[MAX_MESSAGE_LENGTH];
        } unicast_message;
        char broadcast_message[MAX_MESSAGE_LENGTH];
        struct
        {
            char client_names[MAX_CLIENTS][MAX_CLIENT_NAME];
            int client_count;
        } list;
    } data;
} request_message_t;

int finish = 0;

void sig_handler(int signo)
{
    finish = 1;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, sig_handler);

    uint32_t ip = inet_addr(argv[1]);
    uint16_t port = atoi(argv[2]);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = ip,
    };

    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    int t = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }
    if (listen(socket_fd, MAX_CLIENTS) < 0)
    {
        perror("listen");
        return 1;
    }

    int clients_fds[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients_fds[i] = -1;
    }

    bool clients_id_set[MAX_CLIENTS] = {0};
    char clients[MAX_CLIENTS][MAX_CLIENT_NAME] = {0};
    clock_t clients_last_seen[MAX_CLIENTS] = {0};

    clock_t ping_time = clock();

    while (!finish)
    {
        int client_fd;
        if ((client_fd = accept(socket_fd, NULL, 0)) > 0)
        {
            int i = 0;
            while (i < MAX_CLIENTS)
            {
                if (clients_fds[i] == -1)
                {
                    clients_fds[i] = client_fd;
                    clients_last_seen[i] = clock();

                    printf("Client connected at index %d\n", i);
                    break;
                }
                i++;
            }
            if (i == MAX_CLIENTS)
            {
                printf("Too many clients\n");
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients_fds[i] != -1)
            {
                request_message_t message;
                if (recv(clients_fds[i], &message, sizeof(message), MSG_DONTWAIT) > 0)
                {
                    switch (message.type)
                    {
                    case LIST:
                        printf("LIST FROM %s\n", message.client_name);
                        int length = 0;
                        for (int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if (clients_fds[j] != -1)
                            {
                                length++;
                                strncpy(message.data.list.client_names[j], clients[j], MAX_CLIENT_NAME);
                            }
                        }
                        message.data.list.client_count = length;

                        send(clients_fds[i], &message, sizeof(message), MSG_DONTWAIT);
                        break;
                    case BROADCAST:
                        printf("BROADCAST FROM %s message: %s\n", message.client_name, message.data.broadcast_message);
                        for (int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if (clients_fds[j] != -1 && j != i)
                            {
                                send(clients_fds[j], &message, sizeof(message), MSG_DONTWAIT);
                            }
                        }
                        break;
                    case UNICAST:
                        printf("UNICAST FROM %s TO %s message: %s\n", message.client_name, message.data.unicast_message.target_client, message.data.unicast_message.message);
                        for (int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if (clients_fds[j] != -1 && strcmp(clients[j], message.data.unicast_message.target_client) == 0)
                            {
                                send(clients_fds[j], &message, sizeof(message), MSG_DONTWAIT);
                            }
                        }
                        break;
                    case ALIVE:
                        printf("ALIVE FROM %s\n", message.client_name);
                        clients_last_seen[i] = clock();
                        if (clients_id_set[i] == 0)
                        {
                            clients_id_set[i] = 1;
                            strncpy(clients[i], message.client_name, MAX_CLIENT_NAME);
                        }
                        break;
                    case STOP:
                        printf("STOP FROM %s\n", message.client_name);
                        clients_fds[i] = -1;
                        clients_id_set[i] = 0;
                        break;
                    }
                    fflush(stdout);
                }
            }
        }
        if ((clock() - ping_time) / CLOCKS_PER_SEC > 1)
        {
            request_message_t keep_alive_message = {
                .type = ALIVE,
            };
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients_fds[i] != -1)
                {
                    send(clients_fds[i], &keep_alive_message, sizeof(keep_alive_message), MSG_DONTWAIT);
                }
            }
            ping_time = clock();
        }
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients_fds[i] != -1 && (clock() - clients_last_seen[i]) / CLOCKS_PER_SEC > 5)
            {
                printf("Client %s timed out\n", clients[i]);
                close(clients_fds[i]);
                clients_fds[i] = -1;
                clients_id_set[i] = 0;
            }
        }
    }
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients_fds[i] != -1)
            close(clients_fds[i]);
}