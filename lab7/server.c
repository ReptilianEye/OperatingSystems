#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

const char *SERVER_QUEUE_NAME = "/server_queue";
const int MAX_CLIENTS = 10;
typedef enum
{
    INIT,
    TEXT,
    STOP,
    IDENTIFIER
} message_type;
typedef struct
{
    message_type type;
    char text[100];
    int id;
} message;

int finish = 0;
void handler(int sig)
{
    finish = 1;
}

int main(int argc, char *argv[])
{

    signal(SIGINT, handler);

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(message),
    };

    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if (server_queue == -1)
    {
        perror("mq_open");
    }

    mqd_t client_queues[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_queues[i] = -1;
    }

    message received;

    while (!finish)
    {
        mq_receive(server_queue, (char *)&received, sizeof(received), NULL);

        switch (received.type)
        {
        case INIT:
        {
            printf("Got INIT signal\n");
            int i = 0;
            while (client_queues[i] != -1 && i < MAX_CLIENTS)
                i++;
            if (i == MAX_CLIENTS)
            {
                printf("Server is full\n");
                continue;
            }
            client_queues[i] = mq_open(received.text, O_RDWR, S_IRUSR | S_IWUSR, NULL);
            if (client_queues[i] == -1)
            {
                perror("mq_open");
            }
            message response = {
                .id = i,
                .type = IDENTIFIER};
            mq_send(client_queues[i], (char *)&response, sizeof(message), 5);
            printf("Client %d has connected\n", i);
            break;
        }
        case TEXT:
        {
            printf("Got TEXT\n");
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_queues[i] != -1 && i != received.id)
                {
                    mq_send(client_queues[i], (char *)&received, sizeof(message), 5);
                }
            }
            break;
        }
        case STOP:
        {
            mq_close(client_queues[received.id]);
            client_queues[received.id] = -1;

            printf("Client %d has disconnected\n", received.id);
            break;
        }
        default:
            printf("Unknown message type %d\n", received.type);
            break;
        }
    }
    printf("Server is closing\n");

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_queues[i] != -1)
        {
            mq_close(client_queues[i]);
        }
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
}
