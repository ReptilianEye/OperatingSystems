#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>

const char *SERVER_QUEUE_NAME = "/server_queue";
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

    struct mq_attr attr =
        {
            .mq_flags = 0,
            .mq_maxmsg = 10,
            .mq_msgsize = sizeof(message),
        };

    int seed = getpid();
    char queue_name[100] = {0};

    sprintf(queue_name, "/client_queue_%d", seed);
    printf("Queue name: %s\n", queue_name);
    mqd_t client_mq = mq_open(queue_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if (client_mq == -1)
    {
        perror("mq_open");
    }

    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if (server_queue == -1)
    {
        perror("mq_open");
    }

    message init_message = {
        .id = -1,
        .type = INIT};

    memcpy(init_message.text, queue_name, strlen(queue_name));

    if (mq_send(server_queue, (char *)&init_message, sizeof(queue_name), 5) == -1)
    {
        perror("mq_send");
        return -1;
    }

    int id_reader[2];
    if (pipe(id_reader) < 0)
    {
        perror("id_reader pipe");
    }

    signal(SIGKILL, handler);

    pid_t listener = fork();
    if (listener < 0)
    {
        perror("fork");
    }
    else if (listener == 0)
    {
        close(id_reader[0]);

        message received;
        while (!finish)
        {
            mq_receive(client_mq, (char *)&received, sizeof(received), NULL);
            switch (received.type)
            {
            case TEXT:
                printf("Received TEXT message: %s\n", received.text);
                break;
            case INIT:
                printf("Received INIT message with id=%d\n", received.id);
                write(id_reader[1], &received.id, sizeof(received.id));
                break;
            case IDENTIFIER:
                printf("Received IDENTIFIER message with id=%d\n", received.id);
                write(id_reader[1], &received.id, sizeof(received.id));
                break;
            default:
                printf("Received unknown message type\n");
                break;
            }
        }
        printf("Listener finished\n");
        exit(0);
    }
    close(id_reader[1]);
    int client_mq_id_on_server;
    if (read(id_reader[0], &client_mq_id_on_server, sizeof(client_mq_id_on_server)) < 0)
    {
        perror("read server id");
    }
    printf("Server id: %d\n", client_mq_id_on_server);

    char *user_mess;
    while (!finish)
    {
        mq_getattr(server_queue, &attr);

        if (attr.mq_curmsgs >= attr.mq_maxmsg)
        {
            printf("Server is busy\n");
            continue;
        }

        scanf("%ms", &user_mess);

        message msg = {
            .type = TEXT,
            .id = client_mq_id_on_server};

        memcpy(msg.text, user_mess, strlen(user_mess));

        mq_send(server_queue, (char *)&msg, sizeof(msg), 5);
        free(user_mess);
        user_mess = NULL;
    }

    if (client_mq_id_on_server != -1)
    {
        message stop_message = {
            .type = STOP,
            .id = client_mq_id_on_server};

        mq_send(server_queue, (char *)&stop_message, sizeof(stop_message), 5);
    }

    mq_close(client_mq);
    mq_close(server_queue);
    mq_unlink(queue_name);
}