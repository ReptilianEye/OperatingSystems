#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include "socketutil.h"

#define MAX_BUFFER_SIZE 1024

void start_listening_thread(void *fd);
void *listen_and_print(void *socketFD);
void collect_and_send_entries(int socketFD, struct sockaddr *address);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <name> <ip> <port>\n", argv[0]);
        exit(1);
    }

    char *name = argv[1];
    char *ipAddress = argv[2];
    int port = atoi(argv[3]);

    if (strcmp(ipAddress, "def") == 0)
        ipAddress = "";

    int socketFD = createUDPSocket();
    struct sockaddr_in *address = createIPv4Address(ipAddress, port);

    sendto(socketFD, "OK", 2, 0, (struct sockaddr *)address, sizeof(*address));
    sendto(socketFD, name, strlen(name), 0, (struct sockaddr *)address, sizeof(*address));

    start_listening_thread(&socketFD);

    collect_and_send_entries(socketFD, (struct sockaddr *)address);

    close(socketFD);

    return 0;
}

void collect_and_send_entries(int socketFD, struct sockaddr *address)
{
    char *line = NULL;
    size_t lineSize = 0;

    char buffer[MAX_BUFFER_SIZE];

    while (true)
    {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        if (charCount > 0)
        {
            line[charCount - 1] = 0; // Remove newline

            sprintf(buffer, "%s", line);
            sendto(socketFD, "OK", 2, 0, address, sizeof(*address));
            sendto(socketFD, buffer, strlen(buffer), 0, address, sizeof(*address));

            if (strcmp(line, "STOP") == 0)
                break;
        }
    }

    free(line);
}

void start_listening_thread(void *socketFD)
{
    pthread_t id;
    pthread_create(&id, NULL, listen_and_print, socketFD);
}

void *listen_and_print(void *socket)
{
    int socketFD = *(int *)socket;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in senderAddress;
    socklen_t senderAddressLen = sizeof(senderAddress);

    while (true)
    {
        ssize_t amountReceived = recvfrom(socketFD, buffer, sizeof(buffer) - 1, 0,
                                          (struct sockaddr *)&senderAddress, &senderAddressLen);

        if (amountReceived > 0)
        {
            buffer[amountReceived] = '\0';
            printf("%s\n", buffer);
        }
        else if (amountReceived == 0)
        {
            break;
        }
        else
        {
            perror("recvfrom failed");
            break;
        }
    }

    close(socketFD);
    return NULL;
}
