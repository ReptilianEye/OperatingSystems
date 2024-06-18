#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

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
    if (argc != 4)
    {
        printf("Usage: %s <name> <ip> <port>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, sig_handler);

    char *client_id = argv[1];
    uint32_t ip = inet_addr(argv[2]);
    uint16_t port = (uint16_t)strtol(argv[3], NULL, 10);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = ip,
    };

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    request_message_t alive_message = {
        .type = ALIVE,
    };

    strncpy(alive_message.client_name, client_id, MAX_CLIENT_NAME);
    send(socket_fd, &alive_message, sizeof(alive_message), MSG_DONTWAIT);

    pid_t listener_pid = fork();
    if (listener_pid == 0)
    {
        while (!finish)
        {
            request_message_t message;
            recv(socket_fd, &message, sizeof(message), MSG_WAITALL);
            switch (message.type)
            {
            case LIST:
                printf("Connected clients:\n");
                for (int i = 0; i < message.data.list.client_count; i++)
                {
                    printf("%s\n", message.data.list.client_names[i]);
                }
                break;
            case BROADCAST:
                printf("%s: %s\n", message.client_name, message.data.broadcast_message);
                break;
            case UNICAST:
                printf("%s: %s\n", message.client_name, message.data.unicast_message.message);
                break;
            case ALIVE:
                send(socket_fd, &alive_message, sizeof(alive_message), MSG_DONTWAIT);
                break;
            default:
                printf("Unknown message type\n");
                break;
            }
        }
    }
    else
    {
        char *request_type_input_buffer = NULL;
        while (!finish)
        {
            scanf("%ms", &request_type_input_buffer);
            request_message_t message;
            if (strcmp(request_type_input_buffer, "list") == 0)
            {
                message.type = LIST;
                if (send(socket_fd, &message, sizeof(message), MSG_DONTWAIT) < 0)
                {
                    perror("send");
                    return 1;
                }
            }
            else if (strncmp(request_type_input_buffer, "broadcast", 9) == 0)
            {
                message.type = BROADCAST;
                scanf("%s", message.data.broadcast_message);
                send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
            }
            else if (strncmp(request_type_input_buffer, "unicast", 7) == 0)
            {
                message.type = UNICAST;
                scanf("%s", message.data.unicast_message.target_client);
                scanf("%s", message.data.unicast_message.message);
                send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
            }
            else
            {
                printf("Unknown command\n");
            }
            free(request_type_input_buffer);
        }

        request_message_t stop_message = {
            .type = STOP,
        };

        strncpy(stop_message.client_name, client_id, MAX_CLIENT_NAME);
        send(socket_fd, &stop_message, sizeof(stop_message), MSG_DONTWAIT);
    }
    close(socket_fd);
}